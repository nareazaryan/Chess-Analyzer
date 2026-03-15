#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <algorithm>
#include "Board.h"
#include "Pieces.h"
#include "GameAnalyzer.h"


static int countPieces(const Board& board, Color color, char pieceTypeLower) {
    int count = 0;
    for (int r = 0; r < Board::SIZE; ++r) {
        for (int c = 0; c < Board::SIZE; ++c) {
            const Piece* p = board.getElement(r, c);
            if (p && p->getColor() == color) {
                char sym = static_cast<char>(std::tolower(
                               static_cast<unsigned char>(p->symbol())));
                if (sym == pieceTypeLower) ++count;
            }
        }
    }
    return count;
}


static bool placePiece(Board& board, const std::string& token) {
    if (token.size() != 3) {
        std::cout << "  [!] Invalid token \"" << token
                  << "\" — expected format: <piece><file><rank>  (e.g. ke1, Ka8)\n";
        return false;
    }

    char pieceChar = token[0];
    char fileChar  = static_cast<char>(std::tolower(token[1]));
    char rankChar  = token[2];

    if (fileChar < 'a' || fileChar > 'h') {
        std::cout << "  [!] Invalid file '" << token[1] << "' — must be a–h\n";
        return false;
    }
    if (rankChar < '1' || rankChar > '8') {
        std::cout << "  [!] Invalid rank '" << rankChar << "' — must be 1–8\n";
        return false;
    }

    Color color = std::islower(static_cast<unsigned char>(pieceChar))
                  ? Color::White
                  : Color::Black;

    int col = fileChar - 'a';
    int row = rankChar - '1';

    char p = static_cast<char>(std::tolower(static_cast<unsigned char>(pieceChar)));
    if (p == 'p' && (row == 0 || row == 7)) {
        std::cout << "  [!] Pawns cannot be placed on rank 1 or rank 8.\n";
        return false;
    }

    std::unique_ptr<Piece> piece;
    switch (p) {
        case 'k': piece = std::make_unique<King>  (color, row, col); break;
        case 'q': piece = std::make_unique<Queen> (color, row, col); break;
        case 'r': piece = std::make_unique<Rook>  (color, row, col); break;
        case 'b': piece = std::make_unique<Bishop>(color, row, col); break;
        case 'n': piece = std::make_unique<Knight>(color, row, col); break;
        case 'p': piece = std::make_unique<Pawn>  (color, row, col); break;
        default:
            std::cout << "  [!] Unknown piece '" << pieceChar
                      << "' — use k q r b n p  (case sets color)\n";
            return false;
    }


    {
        int maxAllowed = 0;
        const char* pieceName2 = "";
        switch (p) {
            case 'k': maxAllowed = 1;  pieceName2 = "King";   break;
            case 'q': maxAllowed = 9;  pieceName2 = "Queen";  break;
            case 'r': maxAllowed = 10; pieceName2 = "Rook";   break;
            case 'b': maxAllowed = 10; pieceName2 = "Bishop"; break;
            case 'n': maxAllowed = 10; pieceName2 = "Knight"; break;
            case 'p': maxAllowed = 8;  pieceName2 = "Pawn";   break;
        }
        int current = countPieces(board, color, p);
        if (current >= maxAllowed) {
            const char* colorName2 = (color == Color::White) ? "White" : "Black";
            std::cout << "  [!] Cannot place " << colorName2 << " " << pieceName2
                      << " — limit of " << maxAllowed
                      << (maxAllowed == 1 ? " already" : " already")
                      << " reached (" << current << " on board).\n";
            if (p == 'k')
                std::cout << "      Each side may have only 1 King.\n";
            else if (p == 'p')
                std::cout << "      Each side may have at most 8 Pawns.\n";
            else
                std::cout << "      Includes up to 8 extras from pawn promotion.\n";
            return false;
        }
    }

    // Warn if overwriting an existing piece
    if (board.isOccupied(row, col)) {
        const Piece* existing = board.getElement(row, col);
        const char* exColor = (existing->getColor() == Color::White) ? "White" : "Black";
        std::cout << "  [!] Square " << fileChar << rankChar
                  << " is already occupied by " << exColor
                  << " " << existing->symbol() << " — overwriting.\n";
    }

    board.setElement(row, col, std::move(piece));

    const char* colorName = (color == Color::White) ? "White" : "Black";
    std::string pieceName;
    switch (p) {
        case 'k': pieceName = "King";   break;
        case 'q': pieceName = "Queen";  break;
        case 'r': pieceName = "Rook";   break;
        case 'b': pieceName = "Bishop"; break;
        case 'n': pieceName = "Knight"; break;
        case 'p': pieceName = "Pawn";   break;
    }
    std::cout << "  [+] " << colorName << " " << pieceName
              << " placed on " << fileChar << rankChar << "\n";
    return true;
}


static void runAnalysis(const Board& board) {
    board.print();

    std::cout << "\nAnalyze which color? (w = White / b = Black): ";
    std::string ans;
    std::getline(std::cin, ans);
    // trim
    ans.erase(std::remove_if(ans.begin(), ans.end(), ::isspace), ans.end());

    Color side;
    if (!ans.empty() && (ans[0] == 'w' || ans[0] == 'W')) {
        side = Color::White;
        std::cout << "\nAnalysis for White:\n";
    } else if (!ans.empty() && (ans[0] == 'b' || ans[0] == 'B')) {
        side = Color::Black;
        std::cout << "\nAnalysis for Black:\n";
    } else {
        std::cout << "  [!] Unrecognized — defaulting to White.\n";
        side = Color::White;
        std::cout << "\nAnalysis for White:\n";
    }

    GameAnalyzer(board).evaluate(side);
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║     Chess State Analyzer — Interactive Setup     ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "  Notation:  <piece><file><rank>\n";
    std::cout << "    lowercase piece letter = White  (k q r b n p)\n";
    std::cout << "    UPPERCASE piece letter = Black  (K Q R B N P)\n";
    std::cout << "\n";
    std::cout << "  Examples:  ke1  Ka8  qd1  Rd8  nb3\n";
    std::cout << "\n";
    std::cout << "  Commands:  done  clear  quit\n";
    std::cout << "─────────────────────────────────────────────────────\n\n";

    Board board;

    while (true) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line)) break; 

        // Parse space-separated tokens on the same line
        std::istringstream ss(line);
        std::string token;
        bool gotCommand = false;

        while (ss >> token) {
            std::string lower = token;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

            if (lower == "quit" || lower == "exit") {
                std::cout << "Goodbye!\n";
                return 0;
            }
            if (lower == "clear") {
                board.clear();
                std::cout << "  Board cleared.\n";
                gotCommand = true;
                break;
            }
            if (lower == "done") {
                // Require exactly one king per side before analysis
                int whiteKings = countPieces(board, Color::White, 'k');
                int blackKings = countPieces(board, Color::Black, 'k');
                if (whiteKings != 1 || blackKings != 1) {
                    if (whiteKings == 0)
                        std::cout << "  [!] Missing White King — place it with e.g. ke1\n";
                    if (blackKings == 0)
                        std::cout << "  [!] Missing Black King — place it with e.g. Ke8\n";
                    std::cout << "      Both kings must be on the board before analysis.\n";
                    gotCommand = true;
                    break;
                }
                // Reject positions where the two kings are adjacent
                {
                    int wkr = -1, wkc = -1, bkr = -1, bkc = -1;
                    for (int r = 0; r < Board::SIZE; ++r)
                        for (int c = 0; c < Board::SIZE; ++c) {
                            const Piece* pc = board.getElement(r, c);
                            if (!pc) continue;
                            char sym = static_cast<char>(std::tolower(
                                           static_cast<unsigned char>(pc->symbol())));
                            if (sym == 'k') {
                                if (pc->getColor() == Color::White) { wkr = r; wkc = c; }
                                else                                 { bkr = r; bkc = c; }
                            }
                        }
                    if (std::abs(wkr - bkr) <= 1 && std::abs(wkc - bkc) <= 1) {
                        std::cout << "  [!] The two kings are adjacent — illegal position.\n";
                        std::cout << "      Move one king before running analysis.\n";
                        gotCommand = true;
                        break;
                    }
                }
                runAnalysis(board);
                board.clear();  
                std::cout << "\n─────────────────────────────────────────────────────\n";
                std::cout << "  Board reset. Enter pieces for a new position, or type quit.\n\n";
                gotCommand = true;
                break;
            }

            // Otherwise treat as a piece token
            placePiece(board, token);
        }
        (void)gotCommand;
    }

    return 0;
}

