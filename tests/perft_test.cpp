#include "../src/board/board.h"
#include "../src/movegen/movegen.h"
#include "../src/movegen/attacks.h"
#include <iostream>
#include <chrono>
#include <cstring>

uint64_t perft(Board& b, int depth) {
    if(depth == 0) return 1;
    MoveList ml = legalMoves(b);
    if(depth == 1) return ml.n;  // optimization: skip last recursion
    uint64_t nodes = 0;
    for(int i=0; i<ml.n; i++){
        Board before=b;
        BoardState st = saveState(b,ml.m[i]);
        makeMove(b, ml.m[i]);
        nodes += perft(b, depth-1);
        unmakeMove(b, ml.m[i], st);
for(int c=0;c<2;c++)
{
    for(int p=0;p<6;p++)
    {
        if(before.pieces[c][p] != b.pieces[c][p])
        {
            std::cout << "Piece mismatch c=" << c
                      << " p=" << p << "\n";

            std::cout << "before="
                      << before.pieces[c][p] << "\n";

            std::cout << "after="
                      << b.pieces[c][p] << "\n";

            std::cout
                << "from=" << mFrom(ml.m[i])
                << " to=" << mTo(ml.m[i])
                << " flags=" << mFlags(ml.m[i])
                << "\n";

            return 0; // or exit(1)
        }
    }
}
if(before.side != b.side)
{
    std::cout << "side mismatch\n";
    exit(1);
}

if(before.castling != b.castling)
{
    std::cout << "castling mismatch\n";
    exit(1);
}

if(before.epSquare != b.epSquare)
{
    std::cout << "ep mismatch\n";
    exit(1);
}
    }
    return nodes;
}

// Perft divide: shows per-root-move count for debugging
void divide(Board& b, int depth){
    MoveList ml=legalMoves(b); uint64_t total=0;
    for(int i=0;i<ml.n;i++){
        BoardState st=saveState(b,ml.m[i]); makeMove(b,ml.m[i]);
        uint64_t n=perft(b,depth-1);
        unmakeMove(b,ml.m[i],st);
        // print move in UCI format + node count
        std::cout << /*moveStr(ml.m[i])*/ "??" << ": " << n << '\n';
        total+=n;
    }
    std::cout << "Total: " << total << '\n';
}

struct PerftTest{ const char* fen; int depth; uint64_t expected; };

PerftTest TESTS[] = {
    // Starting position
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1,      20},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2,     400},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3,    8902},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4,  197281},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609},
    // Position 2 — tests pins, checks
    {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 1, 48},
    {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 2, 2039},
    // Position 3 — tests en passant
    {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 1, 14},
    {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", 3, 2812},
};

int main() {
    initAttacks();
    int pass=0, fail=0;
    for(auto& t : TESTS){
        Board b; b.loadFEN(t.fen);
        auto t0=std::chrono::high_resolution_clock::now();
        uint64_t got=perft(b,t.depth);
        auto ms=std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::high_resolution_clock::now()-t0).count();
        if(got==t.expected){
            std::cout<<"PASS depth="<<t.depth<<" nodes="<<got<<" ("<<ms<<"ms)\n";
            pass++;
        } else {
            std::cout<<"FAIL depth="<<t.depth<<" got="<<got<<" expected="<<t.expected<<"\n";
            fail++;
        }
    }
    std::cout<<pass<<" passed, "<<fail<<" failed\n";
}
