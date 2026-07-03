#include "board.h"
#include "hash/zobrist.h"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>

Bitboard Board::occupied() const {
    return pieces[WHITE][PAWN]|pieces[WHITE][KNIGHT]|pieces[WHITE][BISHOP]|
           pieces[WHITE][ROOK]|pieces[WHITE][QUEEN]|pieces[WHITE][KING]|
           pieces[BLACK][PAWN]|pieces[BLACK][KNIGHT]|pieces[BLACK][BISHOP]|
           pieces[BLACK][ROOK]|pieces[BLACK][QUEEN]|pieces[BLACK][KING];
}
Bitboard Board::byColor(Color c) const {
    return pieces[c][PAWN]|pieces[c][KNIGHT]|pieces[c][BISHOP]|
           pieces[c][ROOK]|pieces[c][QUEEN]|pieces[c][KING];
}
PieceType Board::pieceAt(int sq) const {
    Bitboard b = 1ULL<<sq;
    for(int c=0;c<2;c++)
        for(int p=0;p<6;p++)
            if(pieces[c][p]&b) return (PieceType)p;
    return NONE;
}
Color Board::colorAt(int sq) const {
    Bitboard b = 1ULL<<sq;
    if(byColor(WHITE)&b) return WHITE;
    return BLACK;
}

void Board::loadFEN(const std::string& fen) {
    // Reset
    for(auto& c:pieces) for(auto& p:c) p=0;
    std::istringstream ss(fen);
    std::string board_str, side_str, castle_str, ep_str;
    ss >> board_str >> side_str >> castle_str >> ep_str
       >> halfClock >> fullMove;

    int rank=7, file=0;
    for(char c : board_str) {
        if(c=='/'){rank--;file=0;}
        else if(isdigit(c)) file+=c-'0';
        else {
            int sq=rank*8+file;
            Color col=isupper(c)?WHITE:BLACK;
            char lc=tolower(c);
            PieceType pt=lc=='p'?PAWN:lc=='n'?KNIGHT:lc=='b'?BISHOP:
                          lc=='r'?ROOK:lc=='q'?QUEEN:KING;
            set(pieces[col][pt], sq); file++;
        }
    }
    side = (side_str=="w") ? WHITE : BLACK;
    castling=0;
    if(castle_str.find('K')!=std::string::npos) castling|=CASTLE_WK;
    if(castle_str.find('Q')!=std::string::npos) castling|=CASTLE_WQ;
    if(castle_str.find('k')!=std::string::npos) castling|=CASTLE_BK;
    if(castle_str.find('q')!=std::string::npos) castling|=CASTLE_BQ;
    epSquare = (ep_str=="-") ? -1 : (ep_str[0]-'a')+(ep_str[1]-'1')*8;
    zobrist = computeZobrist(*this);
}

void Board::print() const {
    const char* sym = "PNBRQKpnbrqk.";
    for(int r=7;r>=0;r--){
        std::cout << r+1 << " ";
        for(int f=0;f<8;f++){
            int sq=r*8+f; char ch='.';
            for(int c=0;c<2;c++)
                for(int p=0;p<6;p++)
                    if(pieces[c][p]>>sq&1) ch=sym[c*6+p];
            std::cout << ch << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "  a b c d e f g h\n";
}
BoardState Board::save() const {
    return {
        castling,
        epSquare,
        halfClock,
        zobrist,
        NONE
    };
}
void makeMove(Board& b, Move m) {
    int from=mFrom(m), to=mTo(m), fl=mFlags(m);
    Color us=b.side, them=(Color)!us;

    // 1. Save irreversible state (caller saves BoardState before calling)
    // 2. Move the piece
    PieceType pt = b.pieceAt(from);
    
    // 3. Remove captured piece
    if((fl & FL_CAPTURE) && !(fl & FL_EP)) {
        if(to==H1) b.castling &= ~CASTLE_WK;
        if(to==A1) b.castling &= ~CASTLE_WQ;
        if(to==H8) b.castling &= ~CASTLE_BK;
        if(to==A8) b.castling &= ~CASTLE_BQ;
        PieceType cap = b.pieceAt(to);
        clr(b.pieces[them][cap], to);
    }
    clr(b.pieces[us][pt], from);
    set(b.pieces[us][pt], to);
    // 4. En passant: remove pawn not on 'to' square
    if(fl & FL_EP) {
        int capSq = to + (us==WHITE ? -8 : 8);
        clr(b.pieces[them][PAWN], capSq);
    }
    // 5. Promotion: replace pawn with promo piece
    if(fl & FL_PROMO) {
        clr(b.pieces[us][PAWN], to);
        set(b.pieces[us][(PieceType)mPromo(m)], to);
    }
    // 6. Castling: move the rook too
    if(fl & FL_CASTLE) {
        if(to==G1){clr(b.pieces[WHITE][ROOK],H1);set(b.pieces[WHITE][ROOK],F1);}
        if(to==C1){clr(b.pieces[WHITE][ROOK],A1);set(b.pieces[WHITE][ROOK],D1);}
        if(to==G8){clr(b.pieces[BLACK][ROOK],H8);set(b.pieces[BLACK][ROOK],F8);}
        if(to==C8){clr(b.pieces[BLACK][ROOK],A8);set(b.pieces[BLACK][ROOK],D8);}
    }
    // 7. Update castling rights, en passant, half-clock, side
    b.epSquare = -1;
    if(pt==PAWN && abs(to-from)==16)
        b.epSquare = from + (us==WHITE ? 8 : -8);
    // castling rights update omitted for brevity — clear relevant bits
    if(pt == KING){
        if(us == WHITE)
            b.castling &= ~(CASTLE_WK | CASTLE_WQ);
        else
            b.castling &= ~(CASTLE_BK | CASTLE_BQ);
    }
    if(pt == ROOK){

        if(from == H1) b.castling &= ~CASTLE_WK;
        if(from == A1) b.castling &= ~CASTLE_WQ;

        if(from == H8) b.castling &= ~CASTLE_BK;
        if(from == A8) b.castling &= ~CASTLE_BQ;
    }

    // Halfmove clock
    if(pt == PAWN || (fl & FL_CAPTURE) || (fl & FL_EP))
        b.halfClock = 0;
    else
        b.halfClock++;

    b.side = them;
    b.zobrist = computeZobrist(b);
}


void unmakeMove(Board& b, Move m, const BoardState& st) {
    // if(__builtin_popcountll(b.pieces[WHITE][KING]) != 1){
    //     std::cout << "WHITE KING BAD BEFORE UNDO\n";
    //     exit(1);
    // }

    // if(__builtin_popcountll(b.pieces[BLACK][KING]) != 1){
    //     std::cout << "BLACK KING BAD BEFORE UNDO\n";
    //     exit(1);
    // }
    // ── Step 1: flip side back (do this first — 'us' is who just moved) ────────
    b.side = (Color)!b.side;
    Color us   = b.side;          // the player whose move we are undoing
    Color them = (Color)!us;     // the opponent

    int from  = mFrom(m);
    int to    = mTo(m);
    int flags = mFlags(m);

    // ── Step 2: move the piece back from 'to' → 'from' ─────────────────────────
    // If this was a promotion, the piece on 'to' is the promoted piece (queen/rook/etc),
    // but what goes back on 'from' must be a PAWN.

    if (flags & FL_PROMO) {
        PieceType promoPiece = (PieceType)mPromo(m);
        clr(b.pieces[us][promoPiece], to);   // remove promoted piece from 'to'
        set(b.pieces[us][PAWN],      from);  // place pawn back on 'from'
    } else {
        Bitboard mask = 1ULL << to;

bool found = false;

for(int p=0; p<6; p++)
{
    if(b.pieces[us][p] & mask)
    {
        found = true;
        break;
    }
}

if(!found)
{
    std::cout
        << "TO SQUARE DOES NOT CONTAIN MOVING SIDE PIECE\n"
        << "from=" << from
        << " to=" << to
        << " flags=" << flags
        << "\n";

    exit(1);
}
        PieceType pt = b.pieceAt(to);       // whatever piece is sitting on 'to' now
        clr(b.pieces[us][pt], to);
        set(b.pieces[us][pt], from);
    }

    // ── Step 3: restore captured piece (if any) ─────────────────────────────────
    // st.captured was set by makeMove before it cleared the victim.
    // En passant capture is NOT on square 'to' — handled separately below.

    if ((flags & FL_CAPTURE) && !(flags & FL_EP)) {
        // if(st.captured == KING){
        //     std::cout << "RESTORING CAPTURED KING!\n";
        //     std::cout << "from=" << from
        //       << " to=" << to
        //       << " flags=" << flags
        //       << "\n";
        //     exit(1);
        // }
        set(b.pieces[them][st.captured], to);   // put victim back on 'to'
    }

    // ── Step 4: restore en passant captured pawn ────────────────────────────────
    // In en passant the captured pawn is NOT on 'to'.
    // It is one square behind 'to' relative to us:
    //   White captures en passant → captured Black pawn is at (to - 8)
    //   Black captures en passant → captured White pawn is at (to + 8)

    if (flags & FL_EP) {
        int capSq = to + (us == WHITE ? -8 : 8);
        set(b.pieces[them][PAWN], capSq);       // restore opponent pawn
        // Note: 'to' square stays empty — no piece was placed there in makeMove
    }

    // ── Step 5: restore rook for castling ───────────────────────────────────────
    // makeMove moved the rook alongside the king. We must move it back.
    //   White kingside:  rook went H1→F1  →  move it back F1→H1
    //   White queenside: rook went A1→D1  →  move it back D1→A1
    //   Black kingside:  rook went H8→F8  →  move it back F8→H8
    //   Black queenside: rook went A8→D8  →  move it back D8→A8

    if (flags & FL_CASTLE) {
        if      (to == G1) { clr(b.pieces[WHITE][ROOK], F1); set(b.pieces[WHITE][ROOK], H1); }
        else if (to == C1) { clr(b.pieces[WHITE][ROOK], D1); set(b.pieces[WHITE][ROOK], A1); }
        else if (to == G8) { clr(b.pieces[BLACK][ROOK], F8); set(b.pieces[BLACK][ROOK], H8); }
        else if (to == C8) { clr(b.pieces[BLACK][ROOK], D8); set(b.pieces[BLACK][ROOK], A8); }
    }

    // ── Step 6: restore all irreversible state fields from BoardState ────────────
    // These cannot be recomputed — they must have been saved before makeMove.

    b.castling  = st.castling;    // rights that were revoked by the move
    b.epSquare  = st.epSquare;    // en passant square that was valid before the move
    b.halfClock = st.halfClock;   // 50-move counter before the move
    b.zobrist   = st.zobrist;     // hash before the move — no recompute needed
    // fullMove is restored separately only if you track it (decrement if black just moved)
    // if(__builtin_popcountll(b.pieces[WHITE][KING]) != 1){
    //     std::cout << "WHITE KING BAD AFTER UNDO\n";
    //     exit(1);
    // }

    // if(__builtin_popcountll(b.pieces[BLACK][KING]) != 1){
    //     std::cout << "BLACK KING BAD AFTER UNDO\n";
    //     exit(1);
    // }
}

BoardState saveState(const Board& b, Move m)
{
    BoardState st;

    st.castling = b.castling;
    st.epSquare = b.epSquare;
    st.halfClock = b.halfClock;
    st.zobrist = b.zobrist;
    st.captured = NONE;
    if(st.captured == KING){
        std::cout<< "KING CAPTURE GENERATED\n";
    }
    int fl = mFlags(m);
    int to = mTo(m);

    if(fl & FL_EP)
        st.captured = PAWN;
    else if(fl & FL_CAPTURE)
        st.captured = b.pieceAt(to);

    return st;
}

bool boardConsistent(const Board& b)
{
    for(int c=0;c<2;c++)
    {
        Bitboard unionBB = 0;

        for(int p=0;p<6;p++)
        {
            // same square occupied by two piece types
            if(unionBB & b.pieces[c][p])
                return false;

            unionBB |= b.pieces[c][p];
        }
    }

    return true;
}
