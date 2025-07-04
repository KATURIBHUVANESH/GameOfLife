#include "GameOfLife.h"
#include "LocalGameOfLife.h"
#include <mpi.h>
#include <random>
#include <iostream>

GameOfLife::GameOfLife(int global_rows, int global_cols)
    : global_rows_(global_rows), global_cols_(global_cols) {
    initialize_mpi();
    local_game_ = std::make_unique<LocalGameOfLife>(local_rows_, local_cols_);
}

GameOfLife::~GameOfLife() {}

void GameOfLife::initialize_mpi() {
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size_);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank_);
    dims_[0] = dims_[1] = 0;
    MPI_Dims_create(mpi_size_, 2, dims_);

    int periods[2] = {0, 0};  
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims_, periods, 0, &cart_comm_);
    MPI_Comm_rank(cart_comm_, &mpi_rank_);
    MPI_Cart_coords(cart_comm_, mpi_rank_, 2, coords_);
    MPI_Cart_shift(cart_comm_, 0, 1, &up_, &down_);
    MPI_Cart_shift(cart_comm_, 1, 1, &left_, &right_);

    if (mpi_rank_ == 0)
        std::cout << "Dims: " << dims_[0] << "x" << dims_[1] << "\n";

    std::cout << "Rank " << mpi_rank_
              << ": up=" << up_ << ", down=" << down_
              << ", left=" << left_ << ", right=" << right_ << "\n";

    local_rows_ = global_rows_ / dims_[0];
    local_cols_ = global_cols_ / dims_[1];
}

void GameOfLife::randomize(double alive_prob) {
    local_game_->initialize(true, alive_prob);
}

void GameOfLife::step() {
    exchange_halo();
    local_game_->step();
}

void GameOfLife::exchange_halo() {
    auto& grid = local_game_->mutable_grid();
    int rows = grid.rows();
    int cols = grid.cols();

    std::vector<int> top_send(cols), bottom_send(cols);
    std::vector<int> top_recv(cols), bottom_recv(cols);

    for (int j = 0; j < cols; ++j) {
        top_send[j] = grid(1, j);                 
        bottom_send[j] = grid(rows - 2, j);      
    }

    MPI_Status status;

    MPI_Sendrecv(top_send.data(), cols, MPI_INT,
                 (up_ != MPI_PROC_NULL ? up_ : MPI_PROC_NULL), 0,
                 bottom_recv.data(), cols, MPI_INT,
                 (down_ != MPI_PROC_NULL ? down_ : MPI_PROC_NULL), 0,
                 cart_comm_, &status);

    MPI_Sendrecv(bottom_send.data(), cols, MPI_INT,
                 (down_ != MPI_PROC_NULL ? down_ : MPI_PROC_NULL), 1,
                 top_recv.data(), cols, MPI_INT,
                 (up_ != MPI_PROC_NULL ? up_ : MPI_PROC_NULL), 1,
                 cart_comm_, &status);

    for (int j = 0; j < cols; ++j) {
        if (up_ != MPI_PROC_NULL) grid(0, j) = top_recv[j];
        if (down_ != MPI_PROC_NULL) grid(rows - 1, j) = bottom_recv[j];
    }

    std::vector<int> left_send(rows), right_send(rows);
    std::vector<int> left_recv(rows), right_recv(rows);

    for (int i = 0; i < rows; ++i) {
        left_send[i] = grid(i, 1);                 
        right_send[i] = grid(i, cols - 2);         
    }

    MPI_Sendrecv(left_send.data(), rows, MPI_INT,
                 (left_ != MPI_PROC_NULL ? left_ : MPI_PROC_NULL), 2,
                 right_recv.data(), rows, MPI_INT,
                 (right_ != MPI_PROC_NULL ? right_ : MPI_PROC_NULL), 2,
                 cart_comm_, &status);

    MPI_Sendrecv(right_send.data(), rows, MPI_INT,
                 (right_ != MPI_PROC_NULL ? right_ : MPI_PROC_NULL), 3,
                 left_recv.data(), rows, MPI_INT,
                 (left_ != MPI_PROC_NULL ? left_ : MPI_PROC_NULL), 3,
                 cart_comm_, &status);

    for (int i = 0; i < rows; ++i) {
        if (left_ != MPI_PROC_NULL) grid(i, 0) = left_recv[i];
        if (right_ != MPI_PROC_NULL) grid(i, cols - 1) = right_recv[i];
    }
}

int GameOfLife::nprocs() const {
    return mpi_size_;
}

int GameOfLife::procno() const {
    return mpi_rank_;
}

bool GameOfLife::is_alive(int row, int col) const {
    return global_grid_(row, col) == 1;
}

int GameOfLife::get_proc(int row, int col) const {
    return process_grid_(row, col);
}
void GameOfLife::gather_full_grid() {
    Eigen::MatrixXi local = local_game_->get_grid();
    std::vector<int> local_data(local.data(), local.data() + local.size());
    std::vector<int> recv_data;

    if (mpi_rank_ == 0)
        recv_data.resize(global_rows_ * global_cols_);

    MPI_Gather(
        local_data.data(),
        local_rows_ * local_cols_,
        MPI_INT,
        recv_data.data(),
        local_rows_ * local_cols_,
        MPI_INT,
        0,
        cart_comm_
    );

    if (mpi_rank_ == 0) {
        global_grid_ = Eigen::MatrixXi::Zero(global_rows_, global_cols_);
        process_grid_ = Eigen::MatrixXi::Zero(global_rows_, global_cols_);

        for (int rank = 0; rank < mpi_size_; ++rank) {
            int coords[2];
            MPI_Cart_coords(cart_comm_, rank, 2, coords);
            int start_i = coords[0] * local_rows_;
            int start_j = coords[1] * local_cols_;

            for (int i = 0; i < local_rows_; ++i) {
                for (int j = 0; j < local_cols_; ++j) {
                    int global_i = start_i + i;
                    int global_j = start_j + j;
                    int idx = rank * local_rows_ * local_cols_ + i * local_cols_ + j;

                    global_grid_(global_i, global_j) = recv_data[idx];
                    process_grid_(global_i, global_j) = rank;
                }
            }
        }
    }
}

void GameOfLife::output_to_file(const std::string &filename) {
     
}
