#include "GameOfLifeUI.h"

GameOfLifeUI::GameOfLifeUI(int rows, int cols, unsigned int cell_size,
                           bool show_process_colors, const std::string& title)
    : rows_(rows), cols_(cols), cell_size_(cell_size), show_process_colors_(show_process_colors),
      window_(sf::VideoMode({cols * cell_size, rows * cell_size}), title),
      cell_shape_(sf::Vector2f(cell_size - 1, cell_size - 1)) {}

bool GameOfLifeUI::poll_events() {
    while (auto event = window_.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window_.close();
        } else if (auto key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->scancode == sf::Keyboard::Scan::Escape) {
                window_.close();
            }
        }
    }
    return window_.isOpen();
}

sf::Color GameOfLifeUI::get_color(int rank) {
    static const std::vector<sf::Color> colors = {
        sf::Color::Blue, sf::Color::Red, sf::Color::Green, sf::Color::Yellow,
        sf::Color::Cyan, sf::Color::Magenta, sf::Color(255, 165, 0), sf::Color(128, 0, 128)
    };
    return colors[rank % colors.size()];
}

void GameOfLifeUI::draw_grid(const GridAccessorFunc& accessor, const GridRankFunc& rank_func) {
    window_.clear();
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            if (accessor(i, j)) {
                if (show_process_colors_)
                    cell_shape_.setFillColor(get_color(rank_func(i, j)));
                else
                    cell_shape_.setFillColor(sf::Color::White);
                cell_shape_.setPosition(sf::Vector2f(j * cell_size_, i * cell_size_));
                window_.draw(cell_shape_);
            }
        }
    }
    window_.display();
}
