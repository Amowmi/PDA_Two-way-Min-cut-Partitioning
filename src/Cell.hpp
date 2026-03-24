#pragma once

#include <string>
#include <vector>

class Net;  // Forward declaration

class Cell {
   public:
    const int cell_lib;
    const int idx;
    std::vector<Net*> nets;
    const int sizein_A;
    const int sizein_B;
    int gain;
    long long max_gain_of_cell;  // for getting the Pmax
    bool locked;
    bool in_dieA;  // true --> A, false --> B
    bool is_put;    // used it initial partition
    Cell(const int cell_lib, const int idx, const int sizein_A, const int sizein_B)
        : cell_lib(cell_lib),
          idx(idx),
          sizein_A(sizein_A),
          sizein_B(sizein_B) {
        gain = 0;
        max_gain_of_cell = 0;
        locked = false;
        is_put = false;
    }
};
