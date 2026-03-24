#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include "Cell.hpp"

using namespace std;

class BucketList {
    public:
        int Pmax;
        vector<vector<Cell*>> list;
        vector<array<int, 2>> cell_info;
        // cell_info[cell][0] = the bucket index of the cell;
        // cell_info[cell][1] = the index of the cell within the bucket;
        BucketList(){}
        BucketList(const int pmax, const int num_cell) : Pmax(pmax){
            list.resize(Pmax * 2 + 1);
            cell_info.resize(num_cell);
        }    
        void insertCell(Cell* cell);
        void removeCell(Cell* cell);
        void clear();
    };