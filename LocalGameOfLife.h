#ifndef LOCAL_GAME_OF_LIFE_H
#define LOCAL_GAME_OF_LIFE_H

#include <Eigen/Dense>
#include <vector>

class LocalGameOfLife {
public:
    LocalGameOfLife(int local_rows, int local_cols);
    void initialize(bool random_init = false, double alive_prob = 0.2);
    void step();
    bool is_alive(int r, int c) const;
    int get_rank(int r, int c) const;
    const Eigen::MatrixXi& get_grid() const { return grid_; }
    Eigen::MatrixXi& mutable_grid();

private:
    int local_rows_, local_cols_;
    Eigen::MatrixXi grid_, next_grid_;
    int count_neighbors(int r, int c) const;
    void exchange_ghost_rows();
};

#endif   
