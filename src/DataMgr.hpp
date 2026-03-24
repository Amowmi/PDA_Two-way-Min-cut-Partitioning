#pragma once
#include <memory>
#include <vector>

#include "Cell.hpp"
#include "Net.hpp"
using namespace std;

class DataMgr {
   public:
    using ptr = std::unique_ptr<DataMgr>;
    long long size_die, sizeA, sizeB;  // Die
    long long maxSize_A, maxSize_B;    // Die

    vector<int> TA, TB;
    vector<Cell*> AllCells;
    vector<Net*> AllNets;
    DataMgr() = default;
};