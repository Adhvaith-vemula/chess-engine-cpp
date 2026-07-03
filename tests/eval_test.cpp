#include "../src/evaluation/eval.h"
#include "../src/board/board.h"
#include <iostream>

int main() {

    Board b;

    // Starting position
    b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    std::cout << "Start position eval = "
              << evaluate(b)
              << "\n";

    // White up a queen
    b.loadFEN("rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKQNR w KQkq - 0 1");

    std::cout << "White extra queen eval = "
              << evaluate(b)
              << "\n";

    // Black up a rook
    b.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBN1 b KQkq - 0 1");

    std::cout << "Black advantage eval = "
              << evaluate(b)
              << "\n";

    // Empty board except kings
    b.loadFEN("4k3/8/8/8/8/8/8/4K3 w - - 0 1");

    std::cout << "Kings only eval = "
              << evaluate(b)
              << "\n";

    return 0;
}
