#include "zobrist.h"
#include <random>
#include "../board/board.h"

uint64_t ZP[2][6][64]; // piece/color/square
uint64_t ZEP[8];        // en passant file
uint64_t ZCastle[16];   // castling rights bitmask
uint64_t ZSide;          // XOR when Black to move

void initZobrist(){
    std::mt19937_64 rng(0xDEADBEEFCAFEBABEULL);
    for(auto&c:ZP) for(auto&p:c) for(auto&s:p) s=rng();
    for(auto&s:ZEP) s=rng();
    for(auto&s:ZCastle) s=rng();
    ZSide=rng();
}
uint64_t computeZobrist(const Board& b){
    uint64_t h=0;
    for(int c=0;c<2;c++) for(int p=0;p<6;p++){
        Bitboard bb=b.pieces[c][p];
        while(bb){int sq=pop(bb); h^=ZP[c][p][sq];}
    }
    if(b.side==BLACK) h^=ZSide;
    if(b.epSquare!=-1) h^=ZEP[b.epSquare%8];
    h^=ZCastle[b.castling];
    return h;
}