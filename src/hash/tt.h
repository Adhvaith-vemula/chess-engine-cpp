#pragma once

#include "board/board.h"
#include <cstdint>

enum TTFlag {
    EXACT,
    LOWER_BOUND,
    UPPER_BOUND
};

struct TTEntry {
    uint64_t key = 0;
    int score = 0;
    int depth = -1;
    TTFlag flag = EXACT;
    Move best = 0;
};

TTEntry* ttGet(uint64_t key);
void ttStore(uint64_t key, int depth, int score, TTFlag flag, Move best);
void ttClear();
