#pragma once
#include "Piece.h"
#include "Board.h"
#include <vector>
#include <memory>

// ─────────────────────────────────────────────
// Helper: sliding-piece move generator.
// Walks in a given direction (dr, dc) from (startR, startC)
// until it hits the board edge, a friendly piece, or a capturable
// enemy piece (which is included, then stopped).
// ─────────────────────────────────────────────
inline std::vector<Square> slidingMoves(
    int startR, int startC, Color color,
    const Board& board,
    const std::vector<std::pair<int,int>>& directions)
{
    std::vector<Square> moves;
    for (auto [dr, dc] : directions) {
        int r = startR + dr;
        int c = startC + dc;
        while (board.inBounds(r, c)) {
            if (board.isFriendly(r, c, color)) break;        // blocked by own piece
            moves.emplace_back(r, c);
            if (board.isEnemy(r, c, color)) break;           // capture; stop sliding
            r += dr;
            c += dc;
        }
    }
    return moves;
}

// ═════════════════════════════════════════════
// King
// ═════════════════════════════════════════════
class King : public Piece {
public:
    King(Color color, int row = 0, int col = 0)
        : Piece(color, row, col) {}

    std::vector<Square> getPossibleMoves(const Board& board) const override;
    bool canAttack(int r, int c, const Board& board) const override;
    std::unique_ptr<Piece> clone() const override {
        return std::make_unique<King>(color_, row_, col_);
    }
    char symbol() const override {
        return (color_ == Color::White) ? 'K' : 'k';
    }
};

// ═════════════════════════════════════════════
// Queen
// ═════════════════════════════════════════════
class Queen : public Piece {
public:
    Queen(Color color, int row = 0, int col = 0)
        : Piece(color, row, col) {}

    std::vector<Square> getPossibleMoves(const Board& board) const override;
    bool canAttack(int r, int c, const Board& board) const override;
    std::unique_ptr<Piece> clone() const override {
        return std::make_unique<Queen>(color_, row_, col_);
    }
    char symbol() const override {
        return (color_ == Color::White) ? 'Q' : 'q';
    }
};

// ═════════════════════════════════════════════
// Rook
// ═════════════════════════════════════════════
class Rook : public Piece {
public:
    Rook(Color color, int row = 0, int col = 0)
        : Piece(color, row, col) {}

    std::vector<Square> getPossibleMoves(const Board& board) const override;
    bool canAttack(int r, int c, const Board& board) const override;
    std::unique_ptr<Piece> clone() const override {
        return std::make_unique<Rook>(color_, row_, col_);
    }
    char symbol() const override {
        return (color_ == Color::White) ? 'R' : 'r';
    }
};

// ═════════════════════════════════════════════
// Bishop
// ═════════════════════════════════════════════
class Bishop : public Piece {
public:
    Bishop(Color color, int row = 0, int col = 0)
        : Piece(color, row, col) {}

    std::vector<Square> getPossibleMoves(const Board& board) const override;
    bool canAttack(int r, int c, const Board& board) const override;
    std::unique_ptr<Piece> clone() const override {
        return std::make_unique<Bishop>(color_, row_, col_);
    }
    char symbol() const override {
        return (color_ == Color::White) ? 'B' : 'b';
    }
};

// ═════════════════════════════════════════════
// Knight
// ═════════════════════════════════════════════
class Knight : public Piece {
public:
    Knight(Color color, int row = 0, int col = 0)
        : Piece(color, row, col) {}

    std::vector<Square> getPossibleMoves(const Board& board) const override;
    bool canAttack(int r, int c, const Board& board) const override;
    std::unique_ptr<Piece> clone() const override {
        return std::make_unique<Knight>(color_, row_, col_);
    }
    char symbol() const override {
        return (color_ == Color::White) ? 'N' : 'n';
    }
};

// ═════════════════════════════════════════════
// Pawn
// Note: no en passant or promotion (per spec).
// White pawns move from row 1→7; Black from row 6→0.
// ═════════════════════════════════════════════
class Pawn : public Piece {
public:
    Pawn(Color color, int row = 0, int col = 0)
        : Piece(color, row, col) {}

    std::vector<Square> getPossibleMoves(const Board& board) const override;
    bool canAttack(int r, int c, const Board& board) const override;
    std::unique_ptr<Piece> clone() const override {
        return std::make_unique<Pawn>(color_, row_, col_);
    }
    char symbol() const override {
        return (color_ == Color::White) ? 'P' : 'p';
    }
};
