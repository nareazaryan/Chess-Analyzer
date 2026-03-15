#pragma once
#include <vector>
#include <utility>
#include <memory>
#include <string>

class Board;


enum class Color { White, Black };

inline Color opponent(Color c) {
    return (c == Color::White) ? Color::Black : Color::White;
}

using Square = std::pair<int, int>;


class Piece {
protected:
    Color color_;
    int   row_;
    int   col_;

public:
    Piece(Color color, int row, int col)
        : color_(color), row_(row), col_(col) {}

    virtual ~Piece() = default;

    Color getColor() const { return color_; }
    int   getRow()   const { return row_; }
    int   getCol()   const { return col_; }

    void setPosition(int r, int c) { row_ = r; col_ = c; }

    virtual std::vector<Square> getPossibleMoves(const Board& board) const = 0;

    virtual bool canAttack(int r, int c, const Board& board) const = 0;

    /// Deep-copies the piece (required for board cloning / simulation).
    virtual std::unique_ptr<Piece> clone() const = 0;

    /// Single-character symbol for debug display (e.g. 'K', 'q').
    virtual char symbol() const = 0;
};
