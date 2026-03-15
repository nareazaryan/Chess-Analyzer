#include "Board.h"
#include <iostream>
#include <stdexcept>

Board::Board() {
    //board starts empty.
}

void Board::setElement(int row, int col, std::unique_ptr<Piece> piece) {
    if (!inBounds(row, col))
        throw std::out_of_range("Board::setElement — position out of range");
    if (piece) piece->setPosition(row, col);
    grid_[row][col] = std::move(piece);
}

void Board::clearSquare(int row, int col) {
    if (inBounds(row, col))
        grid_[row][col].reset();
}

void Board::clear() {
    for (auto& row : grid_)
        for (auto& cell : row)
            cell.reset();
}

Piece* Board::getElement(int row, int col) const {
    if (!inBounds(row, col)) return nullptr;
    return grid_[row][col].get();       
}

bool Board::inBounds(int row, int col) const {
    return row >= 0 && row < SIZE && col >= 0 && col < SIZE;
}

bool Board::isOccupied(int row, int col) const {
    return inBounds(row, col) && grid_[row][col] != nullptr;
}

bool Board::isFriendly(int row, int col, Color color) const {
    if (!isOccupied(row, col)) return false;
    return grid_[row][col]->getColor() == color;
}

bool Board::isEnemy(int row, int col, Color color) const {
    if (!isOccupied(row, col)) return false;
    return grid_[row][col]->getColor() != color;
}

Board Board::clone() const {
    Board copy;
    for (int r = 0; r < SIZE; ++r)
        for (int c = 0; c < SIZE; ++c)
            if (grid_[r][c])
                copy.grid_[r][c] = grid_[r][c]->clone();
    return copy;
}

void Board::print() const {
    std::cout << "  a b c d e f g h\n";
    for (int r = SIZE - 1; r >= 0; --r) {
        std::cout << (r + 1) << ' ';
        for (int c = 0; c < SIZE; ++c) {
            if (grid_[r][c])
                std::cout << grid_[r][c]->symbol() << ' ';
            else
                std::cout << ". ";
        }
        std::cout << (r + 1) << '\n';
    }
    std::cout << "  a b c d e f g h\n";
}
