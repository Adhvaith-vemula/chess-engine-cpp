#pragma once
#include <cstdint>
#include <string>

using Bitboard = uint64_t;
using Move     = uint32_t;

// Piece types — index into pieces[color][type]
enum PieceType : uint8_t {
    PAWN=0, KNIGHT=1, BISHOP=2,
    ROOK=3, QUEEN=4, KING=5, NONE=6
};
enum Color : uint8_t { WHITE=0, BLACK=1 };

// Square indices: a1=0, b1=1, ..., h8=63
enum Square : int {
    A1=0,B1=1,C1=2,D1=3,E1=4,F1=5,G1=6,H1=7,
    A2=8,B2=9,C2=10,D2=11,E2=12,F2=13,G2=14,H2=15,
    A3=16,B3=17,C3=18,D3=19,E3=20,F3=21,G3=22,H3=23,
    A4=24,B4=25,C4=26,D4=27,E4=28,F4=29,G4=30,H4=31,
    A5=32,B5=33,C5=34,D5=35,E5=36,F5=37,G5=38,H5=39,
    A6=40,B6=41,C6=42,D6=43,E6=44,F6=45,G6=46,H6=47,
    A7=48,B7=49,C7=50,D7=51,E7=52,F7=53,G7=54,H7=55,
    A8=56,B8=57,C8=58,D8=59,E8=60,F8=61,G8=62,H8=63
};

// Move flags (bits 12-15 of Move)
constexpr int FL_NONE=0, FL_CAPTURE=1, FL_EP=2,
               FL_CASTLE=4, FL_PROMO=8;

// Castling rights bitmask
constexpr int CASTLE_WK=1, CASTLE_WQ=2,
               CASTLE_BK=4, CASTLE_BQ=8;

struct BoardState {    // saved before makeMove, restored by unmakeMove
    uint8_t   castling;
    int8_t    epSquare;
    int       halfClock;
    uint64_t  zobrist;
    PieceType captured;
};

class Board;

BoardState saveState(const Board& b, Move m);

struct Board {
    Bitboard pieces[2][6] = {};  // [Color][PieceType]
    Color    side      = WHITE;
    uint8_t  castling  = 0b1111;
    int8_t   epSquare  = -1;
    int      halfClock = 0;
    int      fullMove  = 1;
    uint64_t zobrist   = 0;

    Bitboard  occupied()          const;
    Bitboard  byColor(Color c)    const;
    PieceType pieceAt(int sq)      const;
    Color     colorAt(int sq)      const;
    BoardState save()              const;
    void      loadFEN(const std::string& fen);
    void      print()              const;
};

// Move pack/unpack helpers
inline Move mkMove(int f,int t,int fl=0,int pr=0){
    return f|(t<<6)|(fl<<12)|(pr<<16);
}
inline int mFrom (Move m){return m&0x3F;}
inline int mTo   (Move m){return (m>>6)&0x3F;}
inline int mFlags(Move m){return (m>>12)&0xF;}
inline int mPromo(Move m){return (m>>16)&0x7;}

// Bitboard helpers
inline int      lsb(Bitboard b){
    if(b==0){
        return -1;
    }
    return __builtin_ctzll(b);}
inline int      popcount(Bitboard b){return __builtin_popcountll(b);}
inline int      pop(Bitboard& b){int s=lsb(b);b&=b-1;return s;}
inline void    set(Bitboard& b,int s){b|=1ULL<<s;}
inline void    clr(Bitboard& b,int s){b&=~(1ULL<<s);}

extern void makeMove(Board&,Move);
extern void unmakeMove(Board&,Move,const BoardState&);
