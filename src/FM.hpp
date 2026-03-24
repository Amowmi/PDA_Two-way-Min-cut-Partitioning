#pragma once
#include <algorithm>
#include <memory>
#include <omp.h>

#include "BucketList.hpp"
#include "DataMgr.hpp"
using namespace std;

class FM {
   public:
    long long cut_size;             // FM
    BucketList *bucket_A, *bucket_B;  // FM
    DataMgr::ptr dataMgr;
    FM(DataMgr::ptr dataMgr) : dataMgr(move(dataMgr)) {}
    inline bool keepThrow2DieB(Cell*);
    inline bool keepThrow2DieA(Cell*);
    unique_ptr<DataMgr> solve();
    bool initPartitionA2B();
    void initPartitionB2A();
    bool initPartition_v2();
    void FM_main();
    void initDistribution();
    void initCellGain();
    inline void moveCell(Cell* c);
    inline bool moveValid(Cell* c);
    inline void singleGainUpdate(Cell* c, int updated_gain);
    void updateCellGain(Cell* c);
    Cell* findBase();
    int pass();
};