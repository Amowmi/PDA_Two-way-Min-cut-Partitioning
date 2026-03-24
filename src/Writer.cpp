#include "Writer.hpp"
using namespace std;

unique_ptr<DataMgr> Writer::Writer::write() {
    ofstream output_file(output_path, ios::out); 

    if (output_file.fail()) {
        cout << "output file opening failed" << endl;
        exit(1);
    }
    computeCutsize();
    readDieInfo();
    writeOutput();
    return move(dataMgr);
}

inline void Writer::computeCutsize(){
    cut_size = 0;
    for (auto& n : dataMgr->AllNets) {
        int num_a = 0, num_b = 0;
        for (auto& c : n->cells) {
            if (c->in_dieA)
                ++num_a;
            else
                ++num_b;
        }
        n->num_cell_in[0] = num_a;
        n->num_cell_in[1] = num_b;
        if (num_a > 0 && num_b > 0) cut_size += n->weight;
    }
    // cout << "final cut : " << cut_size << endl;
}

inline void Writer::readDieInfo(){
    num_cellA = 0;
    num_cellB = 0;

    for (auto& c : dataMgr->AllCells) {
        if (c->in_dieA){
            ++num_cellA;
            cell_idx_in_A.push_back(c->idx + 1);
        }
        else{
            ++num_cellB;
            cell_idx_in_B.push_back(c->idx + 1);
        }
    }
}

inline void Writer::writeOutput(){
    ofstream output_file;
    output_file.open(output_path);

    output_file << "CutSize " << cut_size << '\n';

    output_file << "DieA " << num_cellA << '\n';
    for(auto& id : cell_idx_in_A){
        output_file << "C" << id << '\n';
    }

    output_file << "DieB " << num_cellB << '\n';
    for(auto& id : cell_idx_in_B){
        output_file << "C" << id << '\n';
    }
    // output_file.flush();
    output_file.close();
}