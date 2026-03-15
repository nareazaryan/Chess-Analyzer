#pragma once
#include "Piece.h"
#include <array>
#include <memory>

// ═════════════════════════════════════════════
// Board — owns the 8×8 grid of pieces.
// Responsible for piece storage, bounds checking,
// and deep-copy cloning (used during move simulation).
// ═════════════════════════════════════════════
class Board {
public:
    static constexpr int SIZE = 8;

    Board();

    // ── Piece placement ────────────────────────

    /// Place a piece at (row, col). Takes ownership via unique_ptr.
    void setElement(int row, int col, std::unique_ptr<Piece> piece);

    /// Remove any piece at (row, col).
    void clearSquare(int row, int col);

    /// Remove every piece from the board.
    void clear();

    // ── Queries ────────────────────────────────

    /// Returns a raw (non-owning) pointer to the piece at (row, col),
    /// or nullptr if the square is empty or out of bounds.
    Piece* getElement(int row, int col) const;

    /// Returns true if (row, col) is within the 8×8 grid.
    bool inBounds(int row, int col) const;

    /// Returns true if the square is occupied by any piece.
    bool isOccupied(int row, int col) const;

    /// Returns true if the square is occupied by a piece of the given color.
    bool isFriendly(int row, int col, Color color) const;

    /// Returns true if the square is occupied by an opponent piece.
    bool isEnemy(int row, int col, Color color) const;

    // ── Simulation support ─────────────────────

    /// Creates a fully independent deep copy of the board.
    /// Used by GameAnalyzer to test hypothetical moves without
    /// mutating the real board state.
    Board clone() const;

    // ── Debug ──────────────────────────────────

    /// Prints an ASCII representation to stdout.
    void print() const;

private:
    // grid_[row][col] — row 0 is rank 1 (bottom), row 7 is rank 8 (top)
    std::array<std::array<std::unique_ptr<Piece>, SIZE>, SIZE> grid_;
};
