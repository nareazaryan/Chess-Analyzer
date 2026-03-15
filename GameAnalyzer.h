#pragma once
#include "Board.h"
#include <string>
#include <vector>

// ═════════════════════════════════════════════
// AnalysisResult — structured result returned
// by GameAnalyzer for one side.
// ═════════════════════════════════════════════
enum class AnalysisResult {
    Safe,
    Stalemate,
    Check,
    Checkmate,
    MateInOne,
    MateInTwo
};

// Convert result to human-readable string
inline std::string resultToString(AnalysisResult r) {
    switch (r) {
        case AnalysisResult::Safe:      return "SAFE";
        case AnalysisResult::Stalemate: return "STALEMATE";
        case AnalysisResult::Check:     return "CHECK";
        case AnalysisResult::Checkmate: return "CHECKMATE";
        case AnalysisResult::MateInOne: return "MATE IN 1";
        case AnalysisResult::MateInTwo: return "MATE IN 2";
    }
    return "UNKNOWN";
}

// ═════════════════════════════════════════════
// GameAnalyzer
// Analyses a board position for one or both sides.
// Does NOT mutate the board; all hypothetical moves
// are tested on deep-copied Board instances.
// ═════════════════════════════════════════════
class GameAnalyzer {
public:
    /// Construct with a reference to the board to analyse.
    /// The board is cloned internally so the original is never modified.
    explicit GameAnalyzer(const Board& board);

    /// Evaluate and print the status of the given side.
    /// Outputs one of: CHECK / CHECKMATE / MATE IN 1 / MATE IN 2 / SAFE
    void evaluate(Color sideUnderThreat);

    /// Evaluate both sides and print results.
    void evaluateBoth();

    // ── Public analysis primitives (useful for testing) ──

    /// Is the king of `color` currently in check on `board`?
    static bool isInCheck(Color color, const Board& board);

    /// Does `color` have no legal moves on `board` (considering check)?
    static bool hasNoLegalMoves(Color color, const Board& board);

    /// Is `color` in checkmate on `board`?
    static bool isCheckmate(Color color, const Board& board);

    /// Can `attacker` force mate in exactly one move against defender?
    static bool isMateInOne(Color attacker, const Board& board);

    /// Can `attacker` force mate in exactly two moves against defender?
    static bool isMateInTwo(Color attacker, const Board& board);

private:
    Board board_;   // our private copy of the position

    // ── Internal helpers ──────────────────────────────────

    /// Find the king of `color`; returns {-1,-1} if not found.
    static std::pair<int,int> findKing(Color color, const Board& board);

    /// Return all squares reachable by `piece` that are also legal
    /// (i.e. do not leave own king in check).
    static std::vector<Square> legalMoves(const Piece* piece, const Board& board);

    /// Apply move (from → to) on a cloned board and return the clone.
    static Board applyMove(const Board& board, int fromR, int fromC, int toR, int toC);
};
