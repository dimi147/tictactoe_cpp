// Uncomment to print timings of the bot's lookup
// #define TIMECODE

#include <array>
#ifdef TIMECODE
#include <chrono>
#endif
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class Game final {
public:
    Game() { reset(); }
    Game(const Game&) = default;
    Game(Game&&) = delete;
    ~Game() = default;

    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&) = delete;

    auto isOver() const -> bool { return (hasWinner().first || boardIsComplete()); }

    auto isTie() const -> bool { return (boardIsComplete() && !hasWinner().first); }

    auto hasWinner() const -> std::pair<bool, char> {
        auto isLineCompleted = [](char a, char b, char c) -> bool {
            return (a == b && b == c && a != EMPTY_CHARACTER);
        };

        if (isLineCompleted(board[0][0], board[0][1], board[0][2]))
            return {true, board[0][0]};
        else if (isLineCompleted(board[1][0], board[1][1], board[1][2]))
            return {true, board[1][0]};
        else if (isLineCompleted(board[2][0], board[2][1], board[2][2]))
            return {true, board[2][0]};
        else if (isLineCompleted(board[0][0], board[1][0], board[2][0]))
            return {true, board[0][0]};
        else if (isLineCompleted(board[0][1], board[1][1], board[2][1]))
            return {true, board[0][1]};
        else if (isLineCompleted(board[0][2], board[1][2], board[2][2]))
            return {true, board[0][2]};
        else if (isLineCompleted(board[0][0], board[1][1], board[2][2]))
            return {true, board[0][0]};
        else if (isLineCompleted(board[2][0], board[1][1], board[0][2]))
            return {true, board[2][0]};
        return {false, ' '};
    }

    auto advance(int row, int col) -> void {
        if (isOver())
            throw std::exception("Game is already over");
        if (row < 0 || row > 2 || col < 0 || col > 2)
            throw std::exception("Location is out of bounds");
        if (board[row][col] != EMPTY_CHARACTER)
            throw std::exception("Location already marked");
        board[row][col] = m_symbols[0];
        std::swap(m_symbols[0], m_symbols[1]);
    }

    auto printBoard() const -> void {
        std::cout << std::endl << "   0  1  2" << std::endl;
        for (auto row = 0; row < 3; row++) {
            std::cout << row << "  ";
            for (auto col = 0; col < 3; col++) {
                std::cout << board[row][col] << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    bool isEmpty(int r, int c) const { return board[r][c] == EMPTY_CHARACTER; }
    void reset() { board.fill({EMPTY_CHARACTER, EMPTY_CHARACTER, EMPTY_CHARACTER}); }

private:
    std::array<char, 2> m_symbols = {'X', 'O'};
    static constexpr char EMPTY_CHARACTER = ' ';
    std::array<std::array<char, 3>, 3> board;

    auto boardIsComplete() const -> bool {
        for (auto row = 0; row < 3; row++) {
            for (auto col = 0; col < 3; col++) {
                if (board[row][col] == EMPTY_CHARACTER)
                    return false;
            }
        }
        return true;
    }
};

class Player {
public:
    Player() = default;
    virtual ~Player() = default;

    virtual auto play(Game& game) -> void {
        if (game.isOver())
            throw std::exception("Game is already over");
        game.printBoard();
        auto targetRow = -1;
        auto targetCol = -1;
        std::cout << "input location: ";
        std::cin >> targetRow >> targetCol;
        game.advance(targetRow, targetCol);
    }
};

class Bot : public Player {
public:
    Bot() = default;
    virtual ~Bot() = default;

    auto play(Game& game) -> void override {
        if (game.isOver())
            throw std::exception("Game is already over");

#ifdef TIMECODE
        auto start = std::chrono::steady_clock::now();
#endif
        // Optional optimization to preselect the first move of the bot as it's the simplest
        // but most expensive to calculate
        auto moves = getPossibleMoves(game);
        if (moves.size() >= 8) {
            game.isEmpty(1, 1) ? game.advance(1, 1) : game.advance(0, 0);
            return;
        }

        auto result = minimax(game, true);

#ifdef TIMECODE
        auto end = std::chrono::steady_clock::now();
        std::cout << "Bot looked for " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << " milliseconds\n";
#endif
        game.advance(result[1], result[2]);
    }

private:
    auto minimax(const Game& game, bool isFriendly, int depth = 0) -> std::array<int, 3> {
        auto moves = getPossibleMoves(game);
        auto results = std::vector<int>(moves.size(), 0);

        // if (moves.size() == 0)
        //     return {0, -1, -1};

        auto max = std::numeric_limits<int>::min();
        auto min = std::numeric_limits<int>::max();
        auto maxIndex = 0;
        auto minIndex = 0;

        for (auto i = 0; i < moves.size(); ++i) {
            auto futureGame{game};
            auto& move = moves[i];
            futureGame.advance(move.first, move.second);
            auto [hasWinner, winner] = futureGame.hasWinner();
            if (hasWinner)
                results[i] = (isFriendly ? 10 - depth : -10 + depth);
            else if (moves.size() == 1)  // is a tie
                results[i] = 0;
            else
                results[i] = minimax(futureGame, !isFriendly, depth + 1)[0];

            if (results[i] > max) {
                max = results[i];
                maxIndex = i;
            }

            if (results[i] < min) {
                min = results[i];
                minIndex = i;
            }
        }

        if (isFriendly)
            return {results[maxIndex], moves[maxIndex].first, moves[maxIndex].second};
        else
            return {results[minIndex], moves[minIndex].first, moves[minIndex].second};
    }

protected:
    auto getPossibleMoves(const Game& game) -> std::vector<std::pair<int, int>> {
        auto moves = std::vector<std::pair<int, int>>{};
        for (auto row = 0; row < 3; row++) {
            for (auto col = 0; col < 3; ++col) {
                if (game.isEmpty(row, col))
                    moves.push_back({row, col});
            }
        }
        return moves;
    }
};

// #include <time.h>

// #include <cstdlib>

// class RandomBot final : public Bot {
// public:
//     RandomBot() : Bot() { srand(reinterpret_cast<unsigned>(this) + time(nullptr)); }
//     void play(Game& game) override {
//         auto moves = getPossibleMoves(game);
//         auto& move = moves[rand() % moves.size()];
//         game.advance(move.first, move.second);
//     }
// };

void playAgainstBot(bool humanPlaysFirst = true) {
    try {
        Game game;
        Player human;
        Bot bot;

        std::array<Player*, 2> turn{humanPlaysFirst ? &human : &bot, humanPlaysFirst ? &bot : &human};

        while (!game.isOver()) {
            turn[0]->play(game);
            std::swap(turn[0], turn[1]);
        }

        game.printBoard();
        auto [hasWinner, winner] = game.hasWinner();
        if (hasWinner)
            std::cout << winner << " wins!" << std::endl;
        else
            std::cout << "Tie!" << std::endl;

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return;
    }
}

void battleOfTheBots(int iterations = 100) {
    auto ties = 0;
    Bot bot1;
    Bot bot2;
    for (auto i = 0; i < iterations; ++i) {
        Game game;

        std::array<Player*, 2> turn = {&bot1, &bot2};

        while (!game.isOver()) {
            try {
                turn[0]->play(game);
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                return;
            }
            std::swap(turn[0], turn[1]);
        }

        if (game.isTie())
            ties++;
    }

    std::cout << "Finished with " << ties << "/" << iterations << " ties" << std::endl;
}

int main() {
    // playAgainstBot();
    battleOfTheBots();
    return 0;
}
