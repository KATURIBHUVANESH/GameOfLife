#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H
#include <mpi.h>
#include <Eigen/Dense>
#include <memory>
#include <string>

class GameOfLifeUI;
class LocalGameOfLife;

class GameOfLife {
public:
    GameOfLife(int global_rows, int global_cols);
    ~GameOfLife();

    void randomize(double alive_prob = 0.3);
    void step();
    void output_to_file(const std::string &filename);
    int nprocs() const;
    int procno() const;
    bool is_alive(int row, int col) const;
    int get_proc(int row, int col) const;
    void gather_full_grid();

private:
    void initialize_mpi();
    void exchange_halo();
    MPI_Comm cart_comm_;
    int global_rows_, global_cols_;
    int halo_size_ = 1;
    int mpi_rank_, mpi_size_;
    int dims_[2], coords_[2];
    int local_rows_, local_cols_;
    int up_, down_, left_, right_;
    Eigen::MatrixXi global_grid_;
    Eigen::MatrixXi process_grid_;
    std::unique_ptr<LocalGameOfLife> local_game_;
};

#endif  
