#include "movegen/attacks.h"
#include "hash/zobrist.h"
#include "uci/uci.h"

int main() {
    initAttacks();   // must be first — all move gen depends on this
    initZobrist();   // must be before any board is created
    uciLoop();       // blocks until "quit" received
    return 0;
}