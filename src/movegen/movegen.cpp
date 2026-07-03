#include "movegen.h"
#include "attacks.h"
#include "../board/board.h"
#include <cassert>
#include <bits/stdc++.h>

// struct MoveList{
//     Move m[218]; int n=0;
//     void add(Move mv){m[n++]=mv;}
// };

bool isAttacked(const Board& b, int sq, Color by){
    Bitboard occ=b.occupied();
    if(KNIGHT_ATK[sq]&b.pieces[by][KNIGHT]) return true;
    if(KING_ATK[sq]&b.pieces[by][KING])     return true;
    if(PAWN_ATK[!by][sq]&b.pieces[by][PAWN]) return true;
    if(rookAttacks(sq,occ)&(b.pieces[by][ROOK]|b.pieces[by][QUEEN])) return true;
    if(bishopAttacks(sq,occ)&(b.pieces[by][BISHOP]|b.pieces[by][QUEEN])) return true;
    return false;
}

MoveList generateMoves(const Board& b){
    MoveList ml; Color us=b.side, them=(Color)!us;
    Bitboard ours=b.byColor(us), occ=b.occupied(), enemy=b.byColor(them);
    Bitboard enemyNoKing=enemy&~b.pieces[them][KING];
    // Pawns 
    Bitboard pw=b.pieces[us][PAWN];

    if(us==WHITE){

        Bitboard push1=(pw<<8)&~occ;
        Bitboard push2=((push1&0xFF0000ULL)<<8)&~occ;

        Bitboard capL=((pw&~FILE_A)<<7)&enemyNoKing;
        Bitboard capR=((pw&~FILE_H)<<9)&enemyNoKing;

        Bitboard tmp=push1;

        while(tmp){
            int sq=pop(tmp);

            if(sq>=56){
                ml.add(mkMove(sq-8,sq,FL_PROMO,QUEEN));
                ml.add(mkMove(sq-8,sq,FL_PROMO,ROOK));
                ml.add(mkMove(sq-8,sq,FL_PROMO,BISHOP));
                ml.add(mkMove(sq-8,sq,FL_PROMO,KNIGHT));
            }
            else{
                ml.add(mkMove(sq-8,sq));
            }
        }

        tmp=push2;

        while(tmp){
            int sq=pop(tmp);
            ml.add(mkMove(sq-16,sq));
        }

        tmp=capL;

        while(tmp){
            int sq=pop(tmp);

            if(sq>=56){
                ml.add(mkMove(sq-7,sq,FL_PROMO|FL_CAPTURE,QUEEN));
                ml.add(mkMove(sq-7,sq,FL_PROMO|FL_CAPTURE,ROOK));
                ml.add(mkMove(sq-7,sq,FL_PROMO|FL_CAPTURE,BISHOP));
                ml.add(mkMove(sq-7,sq,FL_PROMO|FL_CAPTURE,KNIGHT));
            }
            else{
                ml.add(mkMove(sq-7,sq,FL_CAPTURE));
            }
        }

        tmp=capR;

        while(tmp){
            int sq=pop(tmp);

            if(sq>=56){
                ml.add(mkMove(sq-9,sq,FL_PROMO|FL_CAPTURE,QUEEN));
                ml.add(mkMove(sq-9,sq,FL_PROMO|FL_CAPTURE,ROOK));
                ml.add(mkMove(sq-9,sq,FL_PROMO|FL_CAPTURE,BISHOP));
                ml.add(mkMove(sq-9,sq,FL_PROMO|FL_CAPTURE,KNIGHT));
            }
            else{
                ml.add(mkMove(sq-9,sq,FL_CAPTURE));
            }
        }

        if(b.epSquare!=-1){
            Bitboard ep=PAWN_ATK[them][b.epSquare]&pw;

            while(ep){
                int sq=pop(ep);
                ml.add(mkMove(sq,b.epSquare,FL_EP));
            }
        }
    }
    else{

        Bitboard push1=(pw>>8)&~occ;
        Bitboard push2=((push1&0xFF0000000000ULL)>>8)&~occ;

        Bitboard capL=((pw&~FILE_A)>>9)&enemyNoKing;
        Bitboard capR=((pw&~FILE_H)>>7)&enemyNoKing;

        Bitboard tmp=push1;

        while(tmp){
            int sq=pop(tmp);

            if(sq<8){
                ml.add(mkMove(sq+8,sq,FL_PROMO,QUEEN));
                ml.add(mkMove(sq+8,sq,FL_PROMO,ROOK));
                ml.add(mkMove(sq+8,sq,FL_PROMO,BISHOP));
                ml.add(mkMove(sq+8,sq,FL_PROMO,KNIGHT));
            }
            else{
                ml.add(mkMove(sq+8,sq));
            }
        }

        tmp=push2;

        while(tmp){
            int sq=pop(tmp);
            ml.add(mkMove(sq+16,sq));
        }

        tmp=capL;

        while(tmp){
            int sq=pop(tmp);

            if(sq<8){
                ml.add(mkMove(sq+9,sq,FL_PROMO|FL_CAPTURE,QUEEN));
                ml.add(mkMove(sq+9,sq,FL_PROMO|FL_CAPTURE,ROOK));
                ml.add(mkMove(sq+9,sq,FL_PROMO|FL_CAPTURE,BISHOP));
                ml.add(mkMove(sq+9,sq,FL_PROMO|FL_CAPTURE,KNIGHT));
            }
            else{
                ml.add(mkMove(sq+9,sq,FL_CAPTURE));
            }
        }

        tmp=capR;

        while(tmp){
            int sq=pop(tmp);

            if(sq<8){
                ml.add(mkMove(sq+7,sq,FL_PROMO|FL_CAPTURE,QUEEN));
                ml.add(mkMove(sq+7,sq,FL_PROMO|FL_CAPTURE,ROOK));
                ml.add(mkMove(sq+7,sq,FL_PROMO|FL_CAPTURE,BISHOP));
                ml.add(mkMove(sq+7,sq,FL_PROMO|FL_CAPTURE,KNIGHT));
            }
            else{
                ml.add(mkMove(sq+7,sq,FL_CAPTURE));
            }
        }

        if(b.epSquare!=-1){
            Bitboard ep=PAWN_ATK[them][b.epSquare]&pw;

            while(ep){
                int sq=pop(ep);
                ml.add(mkMove(sq,b.epSquare,FL_EP));
            }
        }
    }
    // Knights
    Bitboard kn=b.pieces[us][KNIGHT];
    while(kn){int sq=pop(kn);
        Bitboard tg=(KNIGHT_ATK[sq]&~ours & ~b.pieces[them][KING]);
        while(tg){int t=pop(tg);
            ml.add(mkMove(sq,t,(enemy>>t&1)?FL_CAPTURE:FL_NONE));
        }
    }
    // Bishops, Rooks, Queens — same pattern with sliding attack fns
    // Bishops
    Bitboard bi=b.pieces[us][BISHOP];
    while(bi){
        int sq=pop(bi);
        Bitboard tg=bishopAttacks(sq,occ)&~ours&~b.pieces[them][KING];
        while(tg){
            int t=pop(tg);
            ml.add(mkMove(sq,t,(enemy>>t&1)?FL_CAPTURE:FL_NONE));
        }
    }

    // Rooks
    Bitboard ro=b.pieces[us][ROOK];
    while(ro){
        int sq=pop(ro);
        Bitboard tg=rookAttacks(sq,occ)&~ours&~b.pieces[them][KING];
        while(tg){
            int t=pop(tg);
            ml.add(mkMove(sq,t,(enemy>>t&1)?FL_CAPTURE:FL_NONE));
        }
    }

    // Queens
    Bitboard qu=b.pieces[us][QUEEN];
    while(qu){
        int sq=pop(qu);
        Bitboard tg=queenAttacks(sq,occ)&~ours&~b.pieces[them][KING];
        while(tg){
            int t=pop(tg);
            ml.add(mkMove(sq,t,(enemy>>t&1)?FL_CAPTURE:FL_NONE));
        }
    }

    // King + castling
    int ksq=lsb(b.pieces[us][KING]);
    Bitboard ktg=KING_ATK[ksq]&~ours&~b.pieces[them][KING];
    while(ktg){int t=pop(ktg);
        ml.add(mkMove(ksq,t,(enemy>>t&1)?FL_CAPTURE:FL_NONE));
    }
    if(us == WHITE){

    // O-O
        if(b.castling & CASTLE_WK){
            if(!(occ & ((1ULL<<F1) | (1ULL<<G1)))){
                if(!isAttacked(b,E1,BLACK) &&
                    !isAttacked(b,F1,BLACK) &&
                    !isAttacked(b,G1,BLACK))
                {
                    ml.add(mkMove(E1,G1,FL_CASTLE));
                }
            }
        }

        // O-O-O
        if(b.castling & CASTLE_WQ){
            if(!(occ & ((1ULL<<B1)|(1ULL<<C1)|(1ULL<<D1)))){
                if(!isAttacked(b,E1,BLACK) &&
                    !isAttacked(b,D1,BLACK) &&
                    !isAttacked(b,C1,BLACK))
                {
                    ml.add(mkMove(E1,C1,FL_CASTLE));
                }
            }
        }
    }
    if(us == BLACK){

        // O-O
        if(b.castling & CASTLE_BK){
            if(!(occ & ((1ULL<<F8)|(1ULL<<G8)))){
                if(!isAttacked(b,E8,WHITE) &&
                    !isAttacked(b,F8,WHITE) &&
                    !isAttacked(b,G8,WHITE))
                {
                    ml.add(mkMove(E8,G8,FL_CASTLE));
                }
            }
        }

        // O-O-O
        if(b.castling & CASTLE_BQ){
            if(!(occ & ((1ULL<<B8)|(1ULL<<C8)|(1ULL<<D8)))){
                if(!isAttacked(b,E8,WHITE) &&
                !isAttacked(b,D8,WHITE) &&
                !isAttacked(b,C8,WHITE))
                {
                    ml.add(mkMove(E8,C8,FL_CASTLE));
                }
            }
        }
    }
    // Castling rights checks omitted for brevity
    return ml;
}

// Filter: only return moves that don't leave king in check
MoveList legalMoves(Board& b){
    MoveList pseudo=generateMoves(b), legal;
    for(int i=0;i<pseudo.n;i++){
        BoardState st=saveState(b,pseudo.m[i]); 
        makeMove(b,pseudo.m[i]);
        assert(b.pieces[WHITE][KING]);
        assert(b.pieces[BLACK][KING]);
        // if(__builtin_popcountll(b.pieces[WHITE][KING]) != 1){
        //     std::cout << "White king corruption\n";
        //     exit(1);
        // }

        // if(__builtin_popcountll(b.pieces[BLACK][KING]) != 1){
        //     std::cout << "Black king corruption\n";
        //     exit(1);
        // }
        int ksq=lsb(b.pieces[!b.side][KING]);
        if(!isAttacked(b,ksq,b.side)) legal.add(pseudo.m[i]);
        unmakeMove(b,pseudo.m[i],st);
    }
    return legal;
}
