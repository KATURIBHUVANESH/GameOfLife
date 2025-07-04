#ifndef GAME_OF_LIFE_UI_H
#define GAME_OF_LIFE_UI_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class GameOfLifeUI {
public:
    using GridAccessorFunc = std::function<bool(int, int)>;
    using GridRankFunc = std::function<int(int, int)>;

    GameOfLifeUI(int rows, int cols, unsigned int cell_size,
                 bool show_process_colors = false,
                 const std::string& window_title = "Game of Life");

    bool poll_events();
    void draw_grid(const GridAccessorFunc& accessor,
                   const GridRankFunc& rank_func);

private:
    int rows_, cols_;
    unsigned int cell_size_;
    bool show_process_colors_;
    sf::RenderWindow window_;
    sf::RectangleShape cell_shape_;

    sf::Color get_color(int rank);
};

#endif  
