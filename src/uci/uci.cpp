#include "uci.h"
#include "board/board.h"
#include "movegen/movegen.h"
#include "search/search.h"
#include "hash/zobrist.h"
#include "hash/tt.h"
#include "movegen/attacks.h"
#include <iostream>
#include <sstream>

// Convert Move to UCI string: e2e4, a7a8q (promotion)
std::string moveToStr(Move m){
    int f=mFrom(m),t=mTo(m);
    std::string s;
    s+=(char)('a'+f%8); s+=(char)('1'+f/8);
    s+=(char)('a'+t%8); s+=(char)('1'+t/8);
    if(mFlags(m)&FL_PROMO){
        const char* promo="pnbrq";
        s+=promo[mPromo(m)];
    }
    return s;
}

// Parse UCI move string "e2e4" into Move
Move parseMove(const Board& b, const std::string& s){
    int f=(s[0]-'a')+(s[1]-'1')*8;
    int t=(s[2]-'a')+(s[3]-'1')*8;
    // Find matching legal move
    Board tmp=b; MoveList ml=legalMoves(tmp);
    for(int i=0;i<ml.n;i++)
        if(mFrom(ml.m[i])==f && mTo(ml.m[i])==t) return ml.m[i];
    return 0;
}

void uciLoop(){
    Board board;
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    std::string line, token;

    while(std::getline(std::cin, line)){
        std::istringstream ss(line);
        ss >> token;

        if(token=="uci"){
            std::cout << "id name MyEngine\n"
                      << "id author YourName\n"
                      << "uciok\n";
        }
        else if(token=="isready"){
            std::cout << "readyok\n";
        }
        else if(token=="ucinewgame"){
            ttClear();
        }
        else if(token=="position"){
            std::string type; ss >> type;
            if(type=="startpos")
                board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
            else if(type=="fen"){
                std::string fen_str;
                for(int i=0;i<6;i++){
                    std::string p; ss>>p; fen_str+=(i?" ":"")+p;
                }
                board.loadFEN(fen_str);
            }
            std::string moves_kw; ss >> moves_kw;
            if(moves_kw=="moves"){
                std::string mv;
                while(ss >> mv){
                    Move m=parseMove(board,mv);
                    if(m) makeMove(board,m);
                }
            }
        }
        else if(token=="go"){
            int depth=6;
            std::string t; while(ss>>t) if(t=="depth") ss>>depth;
            Move best=iterativeDeepening(board,depth);
            std::cout << "bestmove " << moveToStr(best) << "\n";
        }
        else if(token=="quit") break;

        std::cout.flush();
    }
}