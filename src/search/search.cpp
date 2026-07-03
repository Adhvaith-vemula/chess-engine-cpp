#include "search.h"
#include "board/board.h"
#include "movegen/movegen.h"
#include "evaluation/eval.h"
#include "hash/tt.h"
#include <algorithm>
#include <climits>

static uint64_t nodesSearched;

static bool isInCheck(const Board& b){
    int ksq = lsb(b.pieces[b.side][KING]);
    return ksq != -1 && isAttacked(b, ksq, (Color)!b.side);
}

// Quiescence search: extend at depth 0 until position is quiet
int qSearch(Board& b, int alpha, int beta){
    nodesSearched++;
    int standPat = evaluate(b);
    if(standPat >= beta) return beta;
    if(alpha < standPat) alpha = standPat;

    MoveList ml = generateMoves(b);
    // Only search captures
    for(int i=0;i<ml.n;i++){
        int flags = mFlags(ml.m[i]);
        if(!(flags & FL_CAPTURE) && !(flags & FL_EP)) continue;
        BoardState st=saveState(b,ml.m[i]); makeMove(b,ml.m[i]);
        int ksq=lsb(b.pieces[!b.side][KING]);
        if(isAttacked(b,ksq,b.side)){
            unmakeMove(b,ml.m[i],st);
            continue;
        }
        int score=-qSearch(b,-beta,-alpha);
        unmakeMove(b,ml.m[i],st);
        if(score>=beta) return beta;
        if(score>alpha) alpha=score;
    }
    return alpha;
}

// MVV-LVA move ordering score
int mvvlva(const Board& b, Move m){
    int flags = mFlags(m);
    if(!(flags&FL_CAPTURE) && !(flags&FL_EP)) return 0;
    int victim=(flags&FL_EP) ? PIECE_VAL[PAWN] : PIECE_VAL[b.pieceAt(mTo(m))];
    int atk=PIECE_VAL[b.pieceAt(mFrom(m))];
    return 10*victim - atk;
}

int alphaBeta(Board& b, int depth, int alpha, int beta){
    nodesSearched++;
    // TT lookup
    if(auto* e=ttGet(b.zobrist); e&&e->depth>=depth){
        if(e->flag==EXACT) return e->score;
        if(e->flag==LOWER_BOUND) alpha=std::max(alpha,e->score);
        if(e->flag==UPPER_BOUND) beta=std::min(beta,e->score);
        if(alpha>=beta) return e->score;
    }

    if(depth==0) return qSearch(b,alpha,beta);

    MoveList ml=legalMoves(b);
    if(ml.n==0) return isInCheck(b) ? -SEARCH_MATE+(100-depth) : 0;

    // Sort moves by MVV-LVA score
    std::sort(ml.m, ml.m+ml.n, [&](Move a, Move bm){
        return mvvlva(b,a) > mvvlva(b,bm);
    });

    int origAlpha=alpha; Move bestMove=0;
    for(int i=0;i<ml.n;i++){
        BoardState st=saveState(b,ml.m[i]); makeMove(b,ml.m[i]);
        int score=-alphaBeta(b,depth-1,-beta,-alpha);
        unmakeMove(b,ml.m[i],st);
        if(score>=beta){
            ttStore(b.zobrist,depth,score,LOWER_BOUND,ml.m[i]);
            return beta;
        }
        if(score>alpha){alpha=score; bestMove=ml.m[i];}
    }
    TTFlag f=(alpha>origAlpha)?EXACT:UPPER_BOUND;
    ttStore(b.zobrist,depth,alpha,f,bestMove);
    return alpha;
}

Move iterativeDeepening(Board& b, int maxDepth){
    Move best=0;
    for(int d=1; d<=maxDepth; d++){
        nodesSearched=0;
        MoveList ml=legalMoves(b);
        std::sort(ml.m,ml.m+ml.n,[&](Move a,Move bm){
            return mvvlva(b,a)>mvvlva(b,bm);
        });
        int bestScore=-SEARCH_INF;
        for(int i=0;i<ml.n;i++){
            BoardState st=saveState(b,ml.m[i]); makeMove(b,ml.m[i]);
            int s=-alphaBeta(b,d-1,-SEARCH_INF,SEARCH_INF);
            unmakeMove(b,ml.m[i],st);
            if(s>bestScore){bestScore=s;best=ml.m[i];}
        }
        // Print UCI info line: depth, score, nodes, pv
    }
    return best;
}
