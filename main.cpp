#include <iostream>
#include <vector>
#include <array>
#include <utility>
#include <string>

using namespace std::string_literals;

class Game {
public:
    static constexpr char SYMBOLS[] = {'X','O'};
    static constexpr char EMPTY_CHARACTER = ' ';

    Game() {
        board.fill({EMPTY_CHARACTER, EMPTY_CHARACTER, EMPTY_CHARACTER});
    }

    auto isOver() const -> bool {
        return (hasWinner().first || boardIsComplete());
    }

    auto isTie() const -> bool {
        return (boardIsComplete() && !hasWinner().first);
    }

    auto hasWinner() const -> std::pair<bool, char> {
        return hasWinner(board);                       
    } 

    static auto hasWinner(const std::array<std::array<char, 3>, 3>& state) -> std::pair<bool, char> {
    
        auto isLineCompleted = [](char a, char b, char c) -> bool {
            return (a == b && b == c && a != EMPTY_CHARACTER);
        };
        
        if (isLineCompleted(state[0][0], state[0][1], state[0][2])) 
            return {true, state[0][0]};
        else if (isLineCompleted(state[1][0], state[1][1], state[1][2]))
            return {true, state[1][0]};
        else if (isLineCompleted(state[2][0], state[2][1], state[2][2]))
            return {true, state[2][0]};
        else if (isLineCompleted(state[0][0], state[1][0], state[2][0]))
            return {true, state[0][0]};
        else if (isLineCompleted(state[0][1], state[1][1], state[2][1]))
            return {true, state[0][1]};
        else if (isLineCompleted(state[0][2], state[1][2], state[2][2]))
            return {true, state[0][2]};
        else if (isLineCompleted(state[0][0], state[1][1], state[2][2]))
            return {true, state[0][0]};
        else if (isLineCompleted(state[2][0], state[1][1], state[0][2]))
            return {true, state[2][0]};
        return {false, ' '};                        
    }

    auto advance(const std::pair<int, int>& location) -> void {
        const auto& [row, col] = location;
        if (row < 0 || row > 2 || col < 0 || col > 2)
            throw "Location is out of bounds"s;
        if (board[row][col] != EMPTY_CHARACTER)
            throw "Location already marked"s;
        board[row][col] = (*current.first);
        std::swap(current.first, current.second);
    }

    auto getBoard() const -> const std::array<std::array<char, 3>, 3>& { return board; } 

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

private:
    std::array<std::array<char, 3>, 3> board;
    std::pair<const char*, const char*> current{&SYMBOLS[0], &SYMBOLS[1]};

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
    Player() {
        if (count == 2)
            throw "Too many players"s;
        symbol = &Game::SYMBOLS[count];
        count++;   
    } 

    ~Player() {
        count--;
    }

    virtual auto getTargetLocation(const Game& game) -> std::pair<int, int> {
        game.printBoard();
        auto targetRow = -1;
        auto targetCol = -1;
        std::cout << "input location: ";
        std::cin >> targetRow >> targetCol;
        return {targetRow, targetCol};  
    } 

    auto getSymbol() const -> char { return (*symbol);  }

private:
    static int count; 
    const char* symbol; 
};

class Bot : public Player {
public:
    Bot() : enemySymbol{Game::SYMBOLS[0] == getSymbol() ? Game::SYMBOLS[1] : Game::SYMBOLS[0]} {
    }

    auto getTargetLocation(const Game& game) -> std::pair<int, int>  override {
        const auto& board = game.getBoard();
        auto possibleMoves = getPossibleMoves(board);

        // first bot move
        if (possibleMoves.size() == 9) {
            srand(time(0));
            auto botMarkedCenter = (bool)std::round(static_cast<float>(rand()) / RAND_MAX);
            return (botMarkedCenter ? std::pair<int, int>{1, 1} : std::pair<int, int>{0, 0});
        } else if (possibleMoves.size() == 8) {
            return ((board[1][1] != Game::EMPTY_CHARACTER) ? std::pair<int, int>{0, 0} : std::pair<int, int>{1, 1});
        }

        auto result = minimax(board, true);
        return {result[1], result[2]};
    }

private:
    const char enemySymbol;

    auto minimax(const std::array<std::array<char, 3>, 3>& board, bool isFriendly, int depth = 0) -> std::array<int, 3> {
        auto moves = getPossibleMoves(board);
        auto results = std::vector<int>(moves.size(), 0);

        if (moves.size() == 0)
            return {0, -1, -1};

        auto max = std::numeric_limits<int>::min();
        auto min = std::numeric_limits<int>::max();
        auto maxIndex = 0;
        auto minIndex = 0;

        for (auto i = 0; i < moves.size(); ++i) {
            auto futureBoard = board;
            auto& move = moves[i];
            futureBoard[move.first][move.second] = (isFriendly ? getSymbol() : enemySymbol);
            auto [isGameOver, winner] = Game::hasWinner(futureBoard);
            if (isGameOver)
                results[i] += (isFriendly ? 10 - depth : -10 + depth);
            else 
                results[i] += minimax(futureBoard, !isFriendly, depth + 1)[0];

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
            return{results[minIndex], moves[minIndex].first, moves[minIndex].second};
    }

    auto getPossibleMoves(const std::array<std::array<char, 3>, 3>& board) -> std::vector<std::pair<int, int>> {
        auto moves = std::vector<std::pair<int, int>>{};
        for (auto row = 0; row < 3; row++) {
            for (auto col = 0; col < 3; ++col) {
                if (board[row][col] == Game::EMPTY_CHARACTER)
                    moves.push_back({row, col});
            }
        }
        return moves;
    }

};

int Player::count = 0;

void playAgainstBot() {

    Game game;
    Player human;
    Bot bot; 

    Player* currentPlayer = &human;
    Player* otherPlayer = &bot;

    while(!game.isOver()) {
        auto location = currentPlayer->getTargetLocation(game);            
        try {
            game.advance(location);
        } catch(std::string& e) {
            std::cout << e << std::endl;
            return;
        }
        std::swap(currentPlayer, otherPlayer);
    }

    game.printBoard();
    auto [hasWinner, winner] = game.hasWinner();
    if (hasWinner)
        std::cout << winner << " wins!" << std::endl;
    else 
        std::cout << "Tie!" << std::endl;
    
}

void battleOfTheBots(int iterations = 1000) {
    
    for (auto i = 0; i < iterations; ++i) {

        Game game;
        Bot bot1;
        Bot bot2; 

        Player* currentPlayer = &bot1;
        Player* otherPlayer = &bot2;

        while(!game.isOver()) {
            auto location = currentPlayer->getTargetLocation(game);            
            try {
                game.advance(location);
            } catch(std::string& e) {
                std::cout << e << std::endl;
                return;
            }
            std::swap(currentPlayer, otherPlayer);
        }

        auto [hasWinner, winner] = game.hasWinner();
        if (hasWinner)
            std::cout << winner << " wins!" << std::endl;
    }
}

int main() {

    battleOfTheBots();

    return 0;
}
