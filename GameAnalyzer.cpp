#include "GameAnalyzer.h"
#include "Pieces.h"
#include <iostream>
#include <algorithm>

// ── Constructor ────────────────────────────────────────────────────────
GameAnalyzer::GameAnalyzer(const Board& board)
    : board_(board.clone())     // store private deep copy; original never touched
{}

// ── Public evaluate ────────────────────────────────────────────────────
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

    // ── Print result ──────────────────────────────────────────────────
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

// ── findKing ───────────────────────────────────────────────────────────
std::pair<int,int> GameAnalyzer::findKing(Color color, const Board& board) {
    for (int r = 0; r < Board::SIZE; ++r)
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            // dynamic_cast allows polymorphic identification of King
            if (p && p->getColor() == color && dynamic_cast<King*>(p))
                return {r, c};
        }
    return {-1, -1};   // king not found (invalid position)
}

// ── applyMove — returns a new board with the move applied ──────────────
// The source square is cleared; the destination gets the moved piece.
Board GameAnalyzer::applyMove(const Board& board,
                              int fromR, int fromC,
                              int toR,   int toC)
{
    Board next = board.clone();
    Piece* p = next.getElement(fromR, fromC);
    if (!p) return next;

    // Clone the piece to place it at the new position
    auto movedPiece = p->clone();
    movedPiece->setPosition(toR, toC);
    next.clearSquare(fromR, fromC);
    next.setElement(toR, toC, std::move(movedPiece));
    return next;
}

// ── isInCheck ─────────────────────────────────────────────────────────
// Scans all enemy pieces; returns true if any can attack our king.
bool GameAnalyzer::isInCheck(Color color, const Board& board) {
    auto [kr, kc] = findKing(color, board);
    if (kr == -1) return false;   // no king — treat as not in check

    Color enemy = opponent(color);
    for (int r = 0; r < Board::SIZE; ++r)
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (p && p->getColor() == enemy && p->canAttack(kr, kc, board))
                return true;
        }
    return false;
}

// ── legalMoves ────────────────────────────────────────────────────────
// Returns all moves for `piece` that do NOT leave own king in check.
// Uses Board::clone() internally — real board is never mutated.
std::vector<Square> GameAnalyzer::legalMoves(const Piece* piece, const Board& board) {
    std::vector<Square> legal;
    for (auto [toR, toC] : piece->getPossibleMoves(board)) {
        Board simulated = applyMove(board,
                                    piece->getRow(), piece->getCol(),
                                    toR, toC);
        // Only keep the move if our own king is not in check afterwards
        if (!isInCheck(piece->getColor(), simulated))
            legal.emplace_back(toR, toC);
    }
    return legal;
}

// ── hasNoLegalMoves ───────────────────────────────────────────────────
bool GameAnalyzer::hasNoLegalMoves(Color color, const Board& board) {
    for (int r = 0; r < Board::SIZE; ++r)
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (p && p->getColor() == color && !legalMoves(p, board).empty())
                return false;
        }
    return true;
}

// ── isCheckmate ───────────────────────────────────────────────────────
// Checkmate = king in check AND every friendly piece has no legal moves.
bool GameAnalyzer::isCheckmate(Color color, const Board& board) {
    return isInCheck(color, board) && hasNoLegalMoves(color, board);
}

// ── isMateInOne ───────────────────────────────────────────────────────
// For every legal move by `attacker`, check if the resulting position
// is checkmate for the defender.
bool GameAnalyzer::isMateInOne(Color attacker, const Board& board) {
    // Already checkmate? No need for mate-in-one.
    if (isCheckmate(opponent(attacker), board)) return false;

    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (!p || p->getColor() != attacker) continue;

            for (auto [toR, toC] : legalMoves(p, board)) {
                Board after = applyMove(board, r, c, toR, toC);
                if (isCheckmate(opponent(attacker), after))
                    return true;           // found a one-move win
            }
        }
    }
    return false;
}

// ── isMateInTwo ───────────────────────────────────────────────────────
// Two-ply search:
//   For every attacker move m1:
//     For every defender response m2 (all legal defender moves):
//       Check if attacker has a mating reply (mate in one).
//   If EVERY defender response to m1 allows attacker to mate in one,
//   then m1 is a "mate-in-two" first move.
bool GameAnalyzer::isMateInTwo(Color attacker, const Board& board) {
    if (isCheckmate(opponent(attacker), board)) return false;
    if (isMateInOne(attacker, board))           return false;

    Color defender = opponent(attacker);

    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            Piece* p = board.getElement(r, c);
            if (!p || p->getColor() != attacker) continue;

            for (auto [m1R, m1C] : legalMoves(p, board)) {
                // Apply attacker's first move
                Board after1 = applyMove(board, r, c, m1R, m1C);

                // Attacker may have already delivered mate on move 1 — skip
                if (isCheckmate(defender, after1)) continue;

                // Collect all defender's legal responses
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
                            // After defender's reply, can attacker mate in one?
                            if (!isMateInOne(attacker, after2)) {
                                allLeadToMate = false;
                                break;
                            }
                        }
                    }
                }

                // If defender is in stalemate / no moves after m1, skip this line
                if (!defenderHasMoves) continue;

                if (allLeadToMate)
                    return true;    // m1 forces mate in two
            }
        }
    }
    return false;
}
