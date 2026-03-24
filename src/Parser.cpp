#include "Parser.hpp"
using namespace std;

unique_ptr<DataMgr> Parser::parse() {
    string line;
    input_file.open(input_path);
    if (input_file.fail()) {
        cout << "input file opening failed" << endl;
        exit(1);
    }
    dataMgr = make_unique<DataMgr>();
    readLib();
    readDie();
    readCell();
    readNet();
    return move(dataMgr);
}

void Parser::readLib() {
    istringstream iss;
    string line, s1, s2, s3;  // sX for redundent info
    string lib_cell_name;
    int num_lib_cell;
    long long w, h;

    // NumTech 2
    eatNewLine(iss, line);
    iss >> s1 >> num_tech;

    // Tech TA 3
    eatNewLine(iss, line);
    iss >> s1 >> s2 >> num_lib_cell;
    dataMgr->TA.resize(num_lib_cell);
    dataMgr->TB.resize(num_lib_cell);

    // LibCell MC1 7 10
    for (int i = 0; i < num_lib_cell; ++i) {
        eatNewLine(iss, line);
        iss >> s1 >> lib_cell_name >> w >> h;
        if (lib_cell_name.size() < 3 || !isdigit(lib_cell_name[2])) {
            cerr << "Error: Invalid lib_cell_name format: " << lib_cell_name << endl;
            exit(1);
        }
        int lib_cell_idx = stoi(lib_cell_name.substr(2)) - 1;

        dataMgr->TA[lib_cell_idx] = w * h;
    }

    // if there is TB
    if (num_tech > 1) {
        // Tech TB 3
        eatNewLine(iss, line);
        iss >> s1 >> s2 >> num_lib_cell;
        for (int i = 0; i < num_lib_cell; ++i) {
            eatNewLine(iss, line);
            iss >> s1 >> lib_cell_name >> w >> h;
            int lib_cell_idx = stoi(lib_cell_name.substr(2)) - 1;
            dataMgr->TB[lib_cell_idx] = w * h;

            // max_cell_size = max(max_cell_size, w * h);
        }
    } else
        dataMgr->TB = dataMgr->TA;
}

void Parser::readDie() {
    istringstream iss;
    string line, s1, s2;  // sX for redundent info
    int uti;
    long long w, h;

    // DieSize 40 30
    eatNewLine(iss, line);
    iss >> s1 >> w >> h;
    dataMgr->size_die = w * h;

    // Die TA 80
    eatNewLine(iss, line);
    iss >> s1 >> s2 >> uti;
    dataMgr->maxSize_A = dataMgr->size_die * (uti / 100.);

    if (num_tech > 1) {
        // Die TB 90
        eatNewLine(iss, line);
        iss >> s1 >> s2 >> uti;
        dataMgr->maxSize_B = dataMgr->size_die * (uti / 100.);
    }
}

void Parser::readCell() {
    istringstream iss;
    string line, s1, s2;  // sX for redundent info
    string cell_name, lib_cell_name;
    int idx, lib_cell_idx, num_cell;

    // NumCells 8
    eatNewLine(iss, line);
    iss >> s1 >> num_cell;
    dataMgr->AllCells.resize(num_cell);

    for (int i = 0; i < num_cell; ++i) {
        // Cell C1 MC1
        eatNewLine(iss, line);
        iss >> s1 >> cell_name >> lib_cell_name;
        idx = stoi(cell_name.substr(1)) - 1;
        lib_cell_idx = stoi(lib_cell_name.substr(2)) - 1;
        Cell* new_cell = new Cell(lib_cell_idx,
                                  idx,
                                  dataMgr->TA[lib_cell_idx],
                                  dataMgr->TB[lib_cell_idx]);
        dataMgr->AllCells[idx] = new_cell;
    }
}

void Parser::readNet() {
    istringstream iss;
    string line, s1, s2;  // sX for redundent info
    int num_cell_net, net_weight;
    string net_name, cell_name;
    int net_idx, idx, num_net;

    // NumNets 6
    eatNewLine(iss, line);
    iss >> s1 >> num_net;
    dataMgr->AllNets.resize(num_net);

    for (int i = 0; i < num_net; ++i) {
        // Net N1 2 3
        eatNewLine(iss, line);
        iss >> s1 >> net_name >> num_cell_net >> net_weight;
        net_idx = stoi(net_name.substr(1)) - 1;
        Net* new_net = new Net(net_idx, net_weight);
        dataMgr->AllNets[net_idx] = new_net;

        // num_cell_net cells in this net;
        for (int j = 0; j < num_cell_net; ++j) {
            // Cell C1
            eatNewLine(iss, line);
            iss >> s1 >> cell_name;
            idx = stoi(cell_name.substr(1)) - 1;
            new_net->cells.emplace_back(dataMgr->AllCells[idx]);
            dataMgr->AllCells[idx]->nets.emplace_back(new_net);
            dataMgr->AllCells[idx]->max_gain_of_cell += new_net->weight;
        }
    }
}

inline void Parser::eatNewLine(istringstream& iss, string& line) {
    do {
        iss.clear();
        iss.str("");
        getline(input_file, line);
        iss.str(line);

    } while (readEmptyLine(line) and input_file);
}

inline bool Parser::readEmptyLine(string& line) {
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
    return line.size() == 0;
}