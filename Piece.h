#pragma once
#include <vector>
#include <utility>
#include <memory>
#include <string>

// Forward declaration so Piece can reference Board without circular includes
class Board;

// ─────────────────────────────────────────────
// Color enum — identifies which side a piece belongs to
// ─────────────────────────────────────────────
enum class Color { White, Black };

// Returns the opponent's color
inline Color opponent(Color c) {
    return (c == Color::White) ? Color::Black : Color::White;
}

// Convenience alias for a board square (row, col)
using Square = std::pair<int, int>;

// ═════════════════════════════════════════════
// Abstract base class: Piece
// Every chess piece inherits from this class.
// Encapsulates color and position; declares the
// pure-virtual interface all pieces must fulfil.
// ═════════════════════════════════════════════
class Piece {
protected:
    Color color_;
    int   row_;
    int   col_;

public:
    Piece(Color color, int row, int col)
        : color_(color), row_(row), col_(col) {}

    virtual ~Piece() = default;

    // ── Accessors ──────────────────────────────
    Color getColor() const { return color_; }
    int   getRow()   const { return row_; }
    int   getCol()   const { return col_; }

    // Update internal position (used after simulated moves)
    void setPosition(int r, int c) { row_ = r; col_ = c; }

    // ── Pure-virtual interface ─────────────────

    /// Returns all squares this piece can move to on the given board.
    /// Moves that leave own king in check are NOT filtered here —
    /// that is the GameAnalyzer's responsibility.
    virtual std::vector<Square> getPossibleMoves(const Board& board) const = 0;

    /// Returns true if this piece can attack (capture) square (r, c).
    /// Used by GameAnalyzer to quickly check if a king is threatened.
    virtual bool canAttack(int r, int c, const Board& board) const = 0;

    /// Deep-copies the piece (required for board cloning / simulation).
    virtual std::unique_ptr<Piece> clone() const = 0;

    /// Single-character symbol for debug display (e.g. 'K', 'q').
    virtual char symbol() const = 0;
};
