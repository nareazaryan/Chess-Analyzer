#pragma once
#include "Piece.h"
#include <array>
#include <memory>


class Board {
public:
    static constexpr int SIZE = 8;

    Board();


    /// Takes ownership via unique_ptr.
    void setElement(int row, int col, std::unique_ptr<Piece> piece);

    void clearSquare(int row, int col);

    void clear();


    Piece* getElement(int row, int col) const;

    bool inBounds(int row, int col) const;

    bool isOccupied(int row, int col) const;

    bool isFriendly(int row, int col, Color color) const;

    bool isEnemy(int row, int col, Color color) const;


    Board clone() const;


    void print() const;

private:
    std::array<std::array<std::unique_ptr<Piece>, SIZE>, SIZE> grid_;
};
