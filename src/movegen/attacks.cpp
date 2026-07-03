#include "attacks.h"
#include "../board/board.h"

Bitboard KNIGHT_ATK[64];
Bitboard KING_ATK[64];
Bitboard PAWN_ATK[2][64];  // [color][square]
Bitboard FILE_A = 0x0101010101010101ULL;
Bitboard FILE_H = 0x8080808080808080ULL;

void initAttacks() {
    for(int sq=0;sq<64;sq++){
        int r=sq/8, f=sq%8;
        KNIGHT_ATK[sq]=0; KING_ATK[sq]=0;

        // Knight: 8 offsets, check bounds to avoid wrap-around
        int kn[][2]={{2,1},{2,-1},{-2,1},{-2,-1},
                     {1,2},{1,-2},{-1,2},{-1,-2}};
        for(auto&d:kn){
            int tr=r+d[0],tf=f+d[1];
            if(tr>=0&&tr<8&&tf>=0&&tf<8)
                set(KNIGHT_ATK[sq], tr*8+tf);
        }
        // King: 8 directions
        int ki[][2]={{1,0},{-1,0},{0,1},{0,-1},
                     {1,1},{1,-1},{-1,1},{-1,-1}};
        for(auto&d:ki){
            int tr=r+d[0],tf=f+d[1];
            if(tr>=0&&tr<8&&tf>=0&&tf<8)
                set(KING_ATK[sq], tr*8+tf);
        }
        // Pawn attacks
        Bitboard b=1ULL<<sq;
        PAWN_ATK[WHITE][sq] = ((b&~FILE_A)<<7)|((b&~FILE_H)<<9);
        PAWN_ATK[BLACK][sq] = ((b&~FILE_A)>>9)|((b&~FILE_H)>>7);
    }
}

// Sliding attacks — call every time (upgrade to magic bitboards later)
Bitboard rookAttacks(int sq, Bitboard occ){
    Bitboard atk=0;
    int r=sq/8,f=sq%8;
    for(int i=f+1;i<8;i++){atk|=1ULL<<(r*8+i);if(occ>>(r*8+i)&1)break;}
    for(int i=f-1;i>=0;i--){atk|=1ULL<<(r*8+i);if(occ>>(r*8+i)&1)break;}
    for(int i=r+1;i<8;i++){atk|=1ULL<<(i*8+f);if(occ>>(i*8+f)&1)break;}
    for(int i=r-1;i>=0;i--){atk|=1ULL<<(i*8+f);if(occ>>(i*8+f)&1)break;}
    return atk;
}
Bitboard bishopAttacks(int sq, Bitboard occ){
    Bitboard atk=0; int r=sq/8,f=sq%8;
    int d[4][2]={{1,1},{1,-1},{-1,1},{-1,-1}};
    for(auto&dr:d){
        int tr=r+dr[0],tf=f+dr[1];
        while(tr>=0&&tr<8&&tf>=0&&tf<8){
            atk|=1ULL<<(tr*8+tf);
            if(occ>>(tr*8+tf)&1) break;
            tr+=dr[0]; tf+=dr[1];
        }
    }
    return atk;
}
Bitboard queenAttacks(int sq,Bitboard occ){
    return rookAttacks(sq,occ)|bishopAttacks(sq,occ);
}
