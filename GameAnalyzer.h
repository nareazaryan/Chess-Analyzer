#pragma once
#include "Board.h"
#include <string>
#include <vector>


enum class AnalysisResult {
    Safe,
    Stalemate,
    Check,
    Checkmate,
    MateInOne,
    MateInTwo
};

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


class GameAnalyzer {
public:
    explicit GameAnalyzer(const Board& board);

    void evaluate(Color sideUnderThreat);

    void evaluateBoth();


    static bool isInCheck(Color color, const Board& board);

    static bool hasNoLegalMoves(Color color, const Board& board);

    static bool isCheckmate(Color color, const Board& board);

    static bool isMateInOne(Color attacker, const Board& board);

    static bool isMateInTwo(Color attacker, const Board& board);

private:
    Board board_;  

    static std::pair<int,int> findKing(Color color, const Board& board);

    static std::vector<Square> legalMoves(const Piece* piece, const Board& board);

    static Board applyMove(const Board& board, int fromR, int fromC, int toR, int toC);
};
