#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Cell;  // Forward declaration

class Net {
   public:
    const int net_idx;
    const int weight;
    int num_cell_in[2];   // num_cell_in[0] = num cell in A, num_cell_in[0] = num cell in B
    vector<Cell*> cells;  // # cells in the net = size(cell)
    Net() = default;
    Net(const int net_idx, const int weight)
        : net_idx(net_idx), weight(weight) {
        num_cell_in[0] = 0;
        num_cell_in[1] = 0;
    }
};
