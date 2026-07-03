#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <cstdint>
#include "../board/board.h"

// =====================================================
// Move flags
// =====================================================

// enum MoveFlags : uint8_t {
//     FL_NONE    = 0,
//     FL_CAPTURE = 1 << 0,
//     FL_EP      = 1 << 1,
//     FL_CASTLE  = 1 << 2,
//     FL_PROMO   = 1 << 3
// };

// =====================================================
// Move type
// =====================================================

using Move = uint32_t;

// =====================================================
// Move encoding
//
// bits  0-5   : from square
// bits  6-11  : to square
// bits 12-15  : flags
// bits 16-18  : promotion piece
// =====================================================

// inline Move mkMove(
//     int from,
//     int to,
//     int flags = FL_NONE,
//     int promo = 0
// ){
//     return from |
//            (to << 6) |
//            (flags << 12) |
//            (promo << 16);
// }

// inline int mFrom(Move m){
//     return m & 63;
// }

// inline int mTo(Move m){
//     return (m >> 6) & 63;
// }

// inline int mFlags(Move m){
//     return (m >> 12) & 15;
// }

// inline int mPromo(Move m){
//     return (m >> 16) & 7;
// }

// =====================================================
// Move list
// =====================================================
// struct MoveList;
struct MoveList{
    Move m[218];
    int n = 0;

    void add(Move mv){
        m[n++] = mv;
    }
};

// =====================================================
// Attack / move generation
// =====================================================

bool isAttacked(
    const Board& b,
    int sq,
    Color by
);

MoveList generateMoves(const Board& b);

MoveList legalMoves(Board& b);

// =====================================================
// Make / unmake
// =====================================================

void makeMove(
    Board& b,
    Move m
);

void unmakeMove(
    Board& b,
    Move m,
    const BoardState& st
);

#endif