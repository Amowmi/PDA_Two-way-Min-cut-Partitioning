#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "DataMgr.hpp"
using namespace std;

class Parser {
    string input_path;
    ifstream input_file;
    DataMgr::ptr dataMgr;
    int num_tech;

   public:
    Parser(const string& input_path)
        : input_path(input_path) {}

    unique_ptr<DataMgr> parse();
    
    void readLib();
    void readDie();
    void readCell();
    void readNet();
    inline void eatNewLine(istringstream& iss, string& line);
    inline bool readEmptyLine(string& line);
};