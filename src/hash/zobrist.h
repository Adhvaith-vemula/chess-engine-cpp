#pragma once

#include <cstdint>

struct Board;

extern uint64_t ZP[2][6][64];
extern uint64_t ZEP[8];
extern uint64_t ZCastle[16];
extern uint64_t ZSide;

void initZobrist();
uint64_t computeZobrist(const Board& b);
