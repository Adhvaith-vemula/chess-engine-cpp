#include "eval.h"
#include "pst.h"
#include "board/board.h"

// Returns score in centipawns from the MOVER's perspective
int evaluate(const Board& b) {
    int score = 0;
    for(int c=0; c<2; c++){
        int sign = (c==WHITE) ? 1 : -1;
        for(int pt=0; pt<6; pt++){
            Bitboard bb = b.pieces[c][pt];
            while(bb){
                int sq = pop(bb);
                int pst_sq = (c==BLACK) ? (sq^56) : sq;
                score += sign * (PIECE_VAL[pt] + PST[pt][pst_sq]);
            }
        }
    }
    // Return from mover's perspective (positive = good for side to move)
    return (b.side==WHITE) ? score : -score;
}
