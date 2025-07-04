#include "GameOfLife.h"
#include "GameOfLifeUI.h"
#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    if (argc < 4) {
        std::cerr << "Usage: mpirun -np <proc> ./game_of_life <rows> <cols> <initial_alive_probability>\n";
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rows = std::stoi(argv[1]);
    int cols = std::stoi(argv[2]);
    float init_prob = std::stof(argv[3]);

    GameOfLife game(rows, cols);
    game.randomize(init_prob);

    if (game.procno() == 0) {
        GameOfLifeUI ui(rows, cols, 10, true, "MPI Game of Life");
        while (ui.poll_events()) {
    game.step();  
    game.gather_full_grid();  
    ui.draw_grid(
        [&](int i, int j) { return game.is_alive(i, j); },
        [&](int i, int j) { return game.get_proc(i, j); }
    );
     
    sf::sleep(sf::milliseconds(100));  
}
    } else {
        while (true) {
            game.step();
            game.gather_full_grid();
        }
    }

    MPI_Finalize();
    return 0;
}
