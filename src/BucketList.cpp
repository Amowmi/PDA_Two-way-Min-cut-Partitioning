#include "BucketList.hpp"

void BucketList::insertCell(Cell* cell) {
    int gain = cell->gain;
    int bucket_idx = gain + Pmax;
    list[bucket_idx].push_back(cell);
    cell_info[cell->idx][0] = bucket_idx;
    cell_info[cell->idx][1] = list[bucket_idx].size() - 1;
}

void BucketList::removeCell(Cell* cell) {
    int bucket_idx = cell_info[cell->idx][0];
    int idx_in_bucket = cell_info[cell->idx][1];

    // move the last element in the bucket to the current cell's position
    Cell* last_cell = list[bucket_idx].back();
    list[bucket_idx][idx_in_bucket] = last_cell;
    cell_info[last_cell->idx][1] = idx_in_bucket;
    
    // remove the last cell;
    list[bucket_idx].pop_back();
}

void BucketList::clear() {
    for (auto& bucket : list)
        bucket.clear();
}
