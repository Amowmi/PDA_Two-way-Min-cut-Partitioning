#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "DataMgr.hpp"
using namespace std;

class Writer {
    string output_path;
    ifstream output_file;
    DataMgr::ptr dataMgr;
    int cut_size, num_cellA, num_cellB;
    vector<int> cell_idx_in_A, cell_idx_in_B;

   public:
    Writer(const string& output_path, DataMgr::ptr dataMgr)
        : output_path(output_path), dataMgr(move(dataMgr)) {}

    unique_ptr<DataMgr> write();
    
    inline void computeCutsize();
    inline void readDieInfo();
    inline void writeOutput();
};