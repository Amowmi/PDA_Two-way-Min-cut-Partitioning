#include <algorithm>
#include <cassert>
#include <fstream>  // file stream
#include <iostream>
#include <sstream>  //class object streams the string into different variables
#include <string>
#include <vector>

#include "BucketList.hpp"
#include "DataMgr.hpp"
#include "Parser.hpp"
#include "FM.hpp"
#include "Writer.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Wrong format of input/output" << endl;
        exit(1);
    }
    string input_path = argv[1];
    string output_path = argv[2];
    
    DataMgr::ptr dataMgrPtr = Parser(input_path).parse();
    FM fm(move(dataMgrPtr));
    dataMgrPtr = fm.solve();
    Writer writer(output_path, move(dataMgrPtr));
    writer.write();

    return 0;
}