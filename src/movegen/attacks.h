#ifndef ATTACKS_H
#define ATTACKS_H

#include <cstdint>

// Forward declarations from board module
typedef uint64_t Bitboard;

// enum Color {
//     WHITE,
//     BLACK
// };

// ==============================
// Global attack lookup tables
// ==============================

extern Bitboard KNIGHT_ATK[64];
extern Bitboard KING_ATK[64];
extern Bitboard PAWN_ATK[2][64];

// ==============================
// File masks
// ==============================

extern Bitboard FILE_A;
extern Bitboard FILE_H;

// ==============================
// Initialization
// ==============================

void initAttacks();

// ==============================
// Sliding attack generators
// ==============================

Bitboard rookAttacks(int sq, Bitboard occ);

Bitboard bishopAttacks(int sq, Bitboard occ);

Bitboard queenAttacks(int sq, Bitboard occ);

#endif