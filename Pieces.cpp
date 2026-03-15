#include "Pieces.h"
#include "Board.h"


std::vector<Square> King::getPossibleMoves(const Board& board) const {
    std::vector<Square> moves;
    static const int deltas[8][2] = {
        {-1,-1},{-1,0},{-1,1},
        { 0,-1},       { 0,1},
        { 1,-1},{ 1,0},{ 1,1}
    };
    for (auto& d : deltas) {
        int r = row_ + d[0];
        int c = col_ + d[1];
        if (board.inBounds(r, c) && !board.isFriendly(r, c, color_))
            moves.emplace_back(r, c);
    }
    return moves;
}

bool King::canAttack(int r, int c, const Board& /*board*/) const {
    return std::abs(r - row_) <= 1 && std::abs(c - col_) <= 1 &&
           !(r == row_ && c == col_);
}

// Queen — Rook + Bishop sliding
static const std::vector<std::pair<int,int>> QUEEN_DIRS = {
    {-1,-1},{-1,0},{-1,1},
    { 0,-1},       { 0,1},
    { 1,-1},{ 1,0},{ 1,1}
};

std::vector<Square> Queen::getPossibleMoves(const Board& board) const {
    return slidingMoves(row_, col_, color_, board, QUEEN_DIRS);
}

bool Queen::canAttack(int r, int c, const Board& board) const {
    int dr = (r == row_) ? 0 : (r > row_ ? 1 : -1);
    int dc = (c == col_) ? 0 : (c > col_ ? 1 : -1);
    if (r != row_ && c != col_ && std::abs(r - row_) != std::abs(c - col_))
        return false;
    int cr = row_ + dr, cc = col_ + dc;
    while (cr != r || cc != c) {
        if (board.isOccupied(cr, cc)) return false;   
        cr += dr;
        cc += dc;
    }
    return true;
}

// Rook — horizontal and vertical sliding
static const std::vector<std::pair<int,int>> ROOK_DIRS = {
    {-1,0},{1,0},{0,-1},{0,1}
};

std::vector<Square> Rook::getPossibleMoves(const Board& board) const {
    return slidingMoves(row_, col_, color_, board, ROOK_DIRS);
}

bool Rook::canAttack(int r, int c, const Board& board) const {
    if (r != row_ && c != col_) return false;   // not on same rank or file
    int dr = (r == row_) ? 0 : (r > row_ ? 1 : -1);
    int dc = (c == col_) ? 0 : (c > col_ ? 1 : -1);
    int cr = row_ + dr, cc = col_ + dc;
    while (cr != r || cc != c) {
        if (board.isOccupied(cr, cc)) return false;
        cr += dr;
        cc += dc;
    }
    return true;
}

// Bishop — diagonal sliding
static const std::vector<std::pair<int,int>> BISHOP_DIRS = {
    {-1,-1},{-1,1},{1,-1},{1,1}
};

std::vector<Square> Bishop::getPossibleMoves(const Board& board) const {
    return slidingMoves(row_, col_, color_, board, BISHOP_DIRS);
}

bool Bishop::canAttack(int r, int c, const Board& board) const {
    if (std::abs(r - row_) != std::abs(c - col_)) return false;  // not diagonal
    int dr = (r > row_) ? 1 : -1;
    int dc = (c > col_) ? 1 : -1;
    int cr = row_ + dr, cc = col_ + dc;
    while (cr != r || cc != c) {
        if (board.isOccupied(cr, cc)) return false;
        cr += dr;
        cc += dc;
    }
    return true;
}

// Knight — L-shaped jump 
static const int KNIGHT_DELTAS[8][2] = {
    {-2,-1},{-2,1},{-1,-2},{-1,2},
    { 1,-2},{ 1,2},{ 2,-1},{ 2,1}
};

std::vector<Square> Knight::getPossibleMoves(const Board& board) const {
    std::vector<Square> moves;
    for (auto& d : KNIGHT_DELTAS) {
        int r = row_ + d[0];
        int c = col_ + d[1];
        if (board.inBounds(r, c) && !board.isFriendly(r, c, color_))
            moves.emplace_back(r, c);
    }
    return moves;
}

bool Knight::canAttack(int r, int c, const Board& /*board*/) const {
    int dr = std::abs(r - row_);
    int dc = std::abs(c - col_);
    return (dr == 2 && dc == 1) || (dr == 1 && dc == 2);
}

// Pawn
// White advances toward higher row numbers (+1 per step).
// Black advances toward lower row numbers (-1 per step).
std::vector<Square> Pawn::getPossibleMoves(const Board& board) const {
    std::vector<Square> moves;
    int dir       = (color_ == Color::White) ? 1 : -1;   // direction of advance
    int startRow  = (color_ == Color::White) ? 1 : 6;    // initial rank for double-push

    int fr = row_ + dir;
    if (board.inBounds(fr, col_) && !board.isOccupied(fr, col_)) {
        moves.emplace_back(fr, col_);
        if (row_ == startRow) {
            int fr2 = row_ + 2 * dir;
            if (board.inBounds(fr2, col_) && !board.isOccupied(fr2, col_))
                moves.emplace_back(fr2, col_);
        }
    }

    for (int dc : {-1, 1}) {
        int cr = row_ + dir;
        int cc = col_ + dc;
        if (board.inBounds(cr, cc) && board.isEnemy(cr, cc, color_))
            moves.emplace_back(cr, cc);
    }

    return moves;
}

bool Pawn::canAttack(int r, int c, const Board& /*board*/) const {
    // Pawns attack diagonally forward; they do NOT capture forward
    int dir = (color_ == Color::White) ? 1 : -1;
    return (r == row_ + dir) && (std::abs(c - col_) == 1);
}
