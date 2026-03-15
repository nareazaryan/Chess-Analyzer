#include "GameAnalyzer.h"
#include "Pieces.h"
#include <iostream>
#include <algorithm>

GameAnalyzer::GameAnalyzer(const Board& board)
    : board_(board.clone())     
{}

void GameAnalyzer::evaluate(Color side) {
    const std::string sideStr = (side == Color::White) ? "White" : "Black";
    Color attacker = opponent(side);

    AnalysisResult result;

    if (isCheckmate(side, board_)) {
        result = AnalysisResult::Checkmate;
    } else if (isInCheck(side, board_)) {
        result = AnalysisResult::Check;
    } else if (hasNoLegalMoves(side, board_)) {
        result = AnalysisResult::Stalemate;
    } else if (isMateInOne(attacker, board_)) {
        result = AnalysisResult::MateInOne;
    } else if (isMateInTwo(attacker, board_)) {
        result = AnalysisResult::MateInTwo;
    } else {
        result = AnalysisResult::Safe;
    }

    switch (result) {
        case AnalysisResult::Checkmate:
            std::cout << sideStr << " is in CHECKMATE\n";
            break;
        case AnalysisResult::Stalemate:
            std::cout << sideStr << " is in STALEMATE (draw)\n";
            break;
        case AnalysisResult::Check:
            std::cout << sideStr << " is in CHECK\n";
            break;
        case AnalysisResult::MateInOne: {
            const std::string atkStr = (attacker == Color::White) ? "White" : "Black";
            std::cout << atkStr << " has MATE IN 1 against " << sideStr << "\n";
            break;
        }
        case AnalysisResult::MateInTwo: {
            const std::string atkStr = (attacker == Color::White) ? "White" : "Black";
            std::cout << atkStr << " has MATE IN 2 against " << sideStr << "\n";
            break;
        }
        case AnalysisResult::Safe:
            std::cout << sideStr << " is SAFE\n";
            break;
    }
}

void GameAnalyzer::evaluateBoth() {
    evaluate(Color::White);
    evaluate(Color::Black);
}

std::pair<int,int> GameAnalyzer::findKing(Color color, const Board& board) {
    for (int r = 0; r < Board::SIZE; ++r)
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (p && p->getColor() == color && dynamic_cast<King*>(p))
                return {r, c};
        }
    return {-1, -1};  
}


Board GameAnalyzer::applyMove(const Board& board,
                              int fromR, int fromC,
                              int toR,   int toC)
{
    Board next = board.clone();
    Piece* p = next.getElement(fromR, fromC);
    if (!p) return next;

    auto movedPiece = p->clone();
    movedPiece->setPosition(toR, toC);
    next.clearSquare(fromR, fromC);
    next.setElement(toR, toC, std::move(movedPiece));
    return next;
}


bool GameAnalyzer::isInCheck(Color color, const Board& board) {
    auto [kr, kc] = findKing(color, board);
    if (kr == -1) return false;   

    Color enemy = opponent(color);
    for (int r = 0; r < Board::SIZE; ++r)
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (p && p->getColor() == enemy && p->canAttack(kr, kc, board))
                return true;
        }
    return false;
}


std::vector<Square> GameAnalyzer::legalMoves(const Piece* piece, const Board& board) {
    std::vector<Square> legal;
    for (auto [toR, toC] : piece->getPossibleMoves(board)) {
        Board simulated = applyMove(board,
                                    piece->getRow(), piece->getCol(),
                                    toR, toC);
        if (!isInCheck(piece->getColor(), simulated))
            legal.emplace_back(toR, toC);
    }
    return legal;
}

bool GameAnalyzer::hasNoLegalMoves(Color color, const Board& board) {
    for (int r = 0; r < Board::SIZE; ++r)
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (p && p->getColor() == color && !legalMoves(p, board).empty())
                return false;
        }
    return true;
}

bool GameAnalyzer::isCheckmate(Color color, const Board& board) {
    return isInCheck(color, board) && hasNoLegalMoves(color, board);
}


bool GameAnalyzer::isMateInOne(Color attacker, const Board& board) {
    if (isCheckmate(opponent(attacker), board)) return false;

    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (!p || p->getColor() != attacker) continue;

            for (auto [toR, toC] : legalMoves(p, board)) {
                Board after = applyMove(board, r, c, toR, toC);
                if (isCheckmate(opponent(attacker), after))
                    return true;           
            }
        }
    }
    return false;
}


bool GameAnalyzer::isMateInTwo(Color attacker, const Board& board) {
    if (isCheckmate(opponent(attacker), board)) return false;
    if (isMateInOne(attacker, board))           return false;

    Color defender = opponent(attacker);

    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (!p || p->getColor() != attacker) continue;

            for (auto [m1R, m1C] : legalMoves(p, board)) {
                Board after1 = applyMove(board, r, c, m1R, m1C);

                if (isCheckmate(defender, after1)) continue;

                bool allLeadToMate = true;
                bool defenderHasMoves = false;

                for (int dr = 0; dr < Board::SIZE && allLeadToMate; ++dr) {
                    for (int dc = 0; dc < Board::SIZE && allLeadToMate; ++dc) {
                        Piece* dp = after1.getElement(dr, dc);
                        if (!dp || dp->getColor() != defender) continue;

                        auto defMoves = legalMoves(dp, after1);
                        for (auto [m2R, m2C] : defMoves) {
                            defenderHasMoves = true;
                            Board after2 = applyMove(after1, dr, dc, m2R, m2C);
                            if (!isMateInOne(attacker, after2)) {
                                allLeadToMate = false;
                                break;
                            }
                        }
                    }
                }

                if (!defenderHasMoves) continue;

                if (allLeadToMate)
                    return true;  
            }
        }
    }
    return false;
}
