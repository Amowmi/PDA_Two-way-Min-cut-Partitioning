#include "FM.hpp"

unique_ptr<DataMgr> FM::solve() {
    if (!initPartition_v2()) {
        if (!initPartitionA2B()) {
            initPartitionB2A();
        }
    }
    FM_main();

    return move(dataMgr);
}


inline bool FM::keepThrow2DieB(Cell* c) {
    int cb = c->sizein_B;
    double ratioA = (double)dataMgr->sizeA / dataMgr->maxSize_A;
    double ratioB = (double)dataMgr->sizeB / dataMgr->maxSize_B;
    return ratioA > 1 or
           (ratioA > ratioB and
            dataMgr->maxSize_B >= dataMgr->sizeB + cb);
}

inline bool FM::keepThrow2DieA(Cell* c) {
    int ca = c->sizein_A;
    double ratioA = (double)dataMgr->sizeA / dataMgr->maxSize_A;
    double ratioB = (double)dataMgr->sizeB / dataMgr->maxSize_B;
    return ratioB > 1 or
           (ratioB > ratioA and
            dataMgr->maxSize_A >= dataMgr->sizeA + ca);
}

bool FM::initPartition_v2() {
    vector<Net*> sorted_nets;
    vector<Cell*> cell_list;

    dataMgr->sizeA = 0;
    dataMgr->sizeB = 0;
    for (auto& n : dataMgr->AllNets) {
        sorted_nets.push_back(n);
    }
    sort(sorted_nets.begin(), sorted_nets.end(), [](Net* a, Net* b) {
        return a->weight > b->weight;
    });
    for (auto& n : sorted_nets) {
        for (auto& c : n->cells) {
            if (!c->is_put) {
                cell_list.push_back(c);
                c->is_put = true;
                c->in_dieA = true;
                dataMgr->sizeA += c->sizein_A;
            }
        }
    }
    for (auto& c : cell_list) {
        if (!keepThrow2DieB(c)) break;
        dataMgr->sizeA -= c->sizein_A;
        dataMgr->sizeB += c->sizein_B;
        c->in_dieA = false;
    }

    return (dataMgr->sizeA <= dataMgr->maxSize_A && dataMgr->sizeB <= dataMgr->maxSize_B);
}

bool FM::initPartitionA2B() {
    vector<Cell*> sorted_cells;
    dataMgr->sizeA = 0;
    dataMgr->sizeB = 0;
    for (auto& c : dataMgr->AllCells) {
        sorted_cells.push_back(c);
        // put all into A first
        dataMgr->sizeA += c->sizein_A;
        c->in_dieA = true;
    }
    sort(sorted_cells.begin(), sorted_cells.end(), [](Cell* a, Cell* b) {
        return a->sizein_B < b->sizein_B;
    });

    for (auto& c : sorted_cells) {
        if (!keepThrow2DieB(c)) break;
        dataMgr->sizeA -= c->sizein_A;
        dataMgr->sizeB += c->sizein_B;
        c->in_dieA = false;
    }

    return (dataMgr->sizeA <= dataMgr->maxSize_A && dataMgr->sizeB <= dataMgr->maxSize_B);
}

void FM::initPartitionB2A() {
    vector<Cell*> sorted_cells;
    dataMgr->sizeA = 0;
    dataMgr->sizeB = 0;
    for (auto& c : dataMgr->AllCells) {
        sorted_cells.push_back(c);
        // put all into A first
        dataMgr->sizeB += c->sizein_B;
        c->in_dieA = false;
    }
    sort(sorted_cells.begin(), sorted_cells.end(), [](Cell* a, Cell* b) {
        return a->sizein_A < b->sizein_A;
    });

    for (auto& c : sorted_cells) {
        if (!keepThrow2DieA(c)) break;
        dataMgr->sizeA += c->sizein_A;
        dataMgr->sizeB -= c->sizein_B;
        c->in_dieA = true;
    }

    
}

void FM::FM_main() {
    // initialize the BucketLists
    long long Pmax = 0;
    for (auto& c : dataMgr->AllCells) {
        Pmax = max(Pmax, c->max_gain_of_cell);
    }
    bucket_A = new BucketList(Pmax, dataMgr->AllCells.size());
    bucket_B = new BucketList(Pmax, dataMgr->AllCells.size());
    int round = 1;

    while (true) {
        int best_gain = pass();

        if (best_gain <= 0) {
            break;
        }

        for (auto& c : dataMgr->AllCells) {
            c->locked = false;
        }
        ++round;
    }
}

void FM::initDistribution() {
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
    }
}

void FM::initCellGain() {
    dataMgr->sizeA = 0;
    dataMgr->sizeB = 0;
    bucket_A->clear();
    bucket_B->clear();

    for (auto& c : dataMgr->AllCells) {
        int gain = 0;
        for (auto& n : c->nets) {
            if (c->in_dieA) {
                if (n->num_cell_in[0] == 1) gain += n->weight;
                if (n->num_cell_in[1] == 0) gain -= n->weight;
            } else {
                if (n->num_cell_in[1] == 1) gain += n->weight;
                if (n->num_cell_in[0] == 0) gain -= n->weight;
            }
        }
        c->gain = gain;
        if (c->in_dieA) {
            bucket_A->insertCell(c);
            dataMgr->sizeA += c->sizein_A;
        } else {
            bucket_B->insertCell(c);
            dataMgr->sizeB += c->sizein_B;
        }
    }
}

inline void FM::moveCell(Cell* c) {
    if (c->in_dieA) {
        bucket_A->removeCell(c);
        c->in_dieA = false;
        dataMgr->sizeA -= c->sizein_A;
        dataMgr->sizeB += c->sizein_B;
    } else {
        bucket_B->removeCell(c);
        c->in_dieA = true;
        dataMgr->sizeA += c->sizein_A;
        dataMgr->sizeB -= c->sizein_B;
    }
    c->locked = true;
}

inline bool FM::moveValid(Cell* c) {
    long long sizein_A = c->sizein_A;
    long long sizein_B = c->sizein_B;

    if (c->in_dieA) {
        return (!c->locked && dataMgr->sizeB + sizein_B <= dataMgr->maxSize_B);
    } else {
        return (!c->locked && dataMgr->sizeA + sizein_A <= dataMgr->maxSize_A);
    }
}

inline void FM::singleGainUpdate(Cell* c, int updated_gain) {
    if (c->in_dieA) {
        bucket_A->removeCell(c);
        c->gain = updated_gain;
        bucket_A->insertCell(c);
    } else {
        bucket_B->removeCell(c);
        c->gain = updated_gain;
        bucket_B->insertCell(c);
    }
}

void FM::updateCellGain(Cell* c) {
    int F = c->in_dieA ? 0 : 1;  // 0
    int T = 1 - F;               // 1
    moveCell(c);

    for (auto& net : c->nets) {
        if (net->num_cell_in[T] == 0) {
            for (auto& cell_on_net : net->cells) {
                if (!cell_on_net->locked) {
                    singleGainUpdate(cell_on_net, cell_on_net->gain + net->weight);
                }
            }
        }
        if (net->num_cell_in[T] == 1) {
            for (auto& cell_on_net : net->cells) {
                if (((!cell_on_net->in_dieA) == T) && !cell_on_net->locked) {
                    singleGainUpdate(cell_on_net, cell_on_net->gain - net->weight);
                }
            }
        }
        --net->num_cell_in[F];
        ++net->num_cell_in[T];
        if (net->num_cell_in[F] == 0) {
            for (auto& cell_on_net : net->cells) {
                if (!cell_on_net->locked) {
                    singleGainUpdate(cell_on_net, cell_on_net->gain - net->weight);
                }
            }
        }
        if (net->num_cell_in[F] == 1) {
            for (auto& cell_on_net : net->cells) {
                if (((!cell_on_net->in_dieA) == F) && !cell_on_net->locked) {
                    singleGainUpdate(cell_on_net, cell_on_net->gain + net->weight);
                }
            }
        }
    }
}

Cell* FM::findBase() {
    int idx_A = 2 * bucket_A->Pmax;
    int idx_B = 2 * bucket_B->Pmax;

    while (idx_A >= 0 || idx_B >= 0) {
        while (idx_A >= 0 && bucket_A->list[idx_A].empty()) --idx_A;
        while (idx_B >= 0 && bucket_B->list[idx_B].empty()) --idx_B;

        if (idx_A < 0 && idx_B < 0) break;

        if (idx_B < 0 || (idx_A >= 0 && idx_A > idx_B)) {
            for (auto& cell : bucket_A->list[idx_A]) {
                if (moveValid(cell)) return cell;
            }
            --idx_A;
        } else {
            for (auto& cell : bucket_B->list[idx_B]) {
                if (moveValid(cell)) return cell;
            }
            --idx_B;
        }
    }
    return nullptr;
}

int FM::pass() {
    initDistribution();
    initCellGain();

    vector<Cell*> move_sequence;
    int max_gain = -1;
    int best_move_index = -1;
    int cumulative_gain = 0;

    while (true) {
        Cell* basecell = findBase();
        if (!basecell) break;

        // cumulative_gain = the gain improved by moving basecell
        cumulative_gain += basecell->gain;
        move_sequence.push_back(basecell);

        if (cumulative_gain >= max_gain) {
            max_gain = cumulative_gain;
            // move_sequence from index 0 to index best_move_index gives the best gain
            best_move_index = move_sequence.size() - 1;
        }
        updateCellGain(basecell);
    }

    // for all move_seq with index < best_move_index, roll back
    for (int i = move_sequence.size() - 1; i > best_move_index; --i) {
        auto& record = move_sequence[i];
        if (record->in_dieA) {
            dataMgr->sizeA -= record->sizein_A;
            dataMgr->sizeB += record->sizein_B;
        } else {
            dataMgr->sizeA += record->sizein_A;
            dataMgr->sizeB -= record->sizein_B;
        }
        record->in_dieA = !record->in_dieA;
    }

    move_sequence.clear();
    return max_gain;
}