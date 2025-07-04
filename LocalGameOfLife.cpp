#include "LocalGameOfLife.h"
#include <cstdlib>
#include <ctime>
#include <mpi.h>

LocalGameOfLife::LocalGameOfLife(int local_rows, int local_cols)
    : local_rows_(local_rows), local_cols_(local_cols) {
    grid_ = Eigen::MatrixXi::Zero(local_rows_ + 2, local_cols_ + 2);
    next_grid_ = Eigen::MatrixXi::Zero(local_rows_ + 2, local_cols_ + 2);
}

void LocalGameOfLife::initialize(bool random_init, double alive_prob) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 1; i <= local_rows_; ++i) {
    for (int j = 1; j <= local_cols_; ++j) {
        grid_(i, j) = (random_init && ((std::rand() / (double)RAND_MAX) < alive_prob)) ? 1 : 0;
    }
}
}

void LocalGameOfLife::step() {
    for (int i = 1; i <= local_rows_; ++i) {
        for (int j = 1; j <= local_cols_; ++j) {
            int live_neighbors = count_neighbors(i, j);
            if (grid_(i, j) == 1)
                next_grid_(i, j) = (live_neighbors == 2 || live_neighbors == 3) ? 1 : 0;
            else
                next_grid_(i, j) = (live_neighbors == 3) ? 1 : 0;
        }
    }
    grid_.swap(next_grid_);
}

bool LocalGameOfLife::is_alive(int r, int c) const {
    return grid_(r, c) == 1;
}

int LocalGameOfLife::get_rank(int, int) const {
    return 0;   
}

int LocalGameOfLife::count_neighbors(int r, int c) const {
    int count = 0;
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < local_rows_ + 2 && nc >= 0 && nc < local_cols_ + 2) {
                count += grid_(nr, nc);
            }
        }
    }
    return count;
}

void LocalGameOfLife::exchange_ghost_rows() {
  
}
Eigen::MatrixXi& LocalGameOfLife::mutable_grid()
{
return grid_;
}
