#pragma once

#include "board/board.h"

constexpr int SEARCH_INF = 1000000;
constexpr int SEARCH_MATE = 99999;

int qSearch(Board& b, int alpha, int beta);
int mvvlva(const Board& b, Move m);
int alphaBeta(Board& b, int depth, int alpha, int beta);

// Searches from depth 1 through maxDepth and returns the best move found.
Move iterativeDeepening(Board& b, int maxDepth);
