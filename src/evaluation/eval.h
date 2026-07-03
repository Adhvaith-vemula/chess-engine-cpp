#pragma once

struct Board;

inline constexpr int PIECE_VAL[6] = {100, 320, 330, 500, 900, 20000};

// Returns a centipawn score from the side-to-move's perspective.
// Positive means the player to move is better.
int evaluate(const Board& b);
