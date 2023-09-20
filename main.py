import pygame
import copy
import random

class TicTacToe:
    def __init__(self):
        self.reset()

    def advance(self, r, c):
        if self.board[r][c] != None:
            return 
        self.board[r][c] = self.turn[0]
        self.turn[0], self.turn[1] = self.turn[1], self.turn[0]
        return self.turn[1]

    X = 'X'
    O = 'O'

    def reset(self):
        self.board = [[None for i in range(3)] for j in range(3)]
        self.turn = [TicTacToe.X, TicTacToe.O]
    
    def __isComplete(self):
        return not True in [True in [e == None for e in row] for row in self.board]
    
    def get_open_squares(self):
        r = []
        [[r.append((row, col)) if self.board[row][col] == None else None for col in range(len(self.board[row]))] for row in range(len(self.board))]
        return r

    def isTie(self):
        return self.__isComplete() and not self.isWon()
        
    def isWon(self):
        b = self.board
        if (b[0][0] == b[0][1] and b[0][1] == b[0][2] and b[0][0] != None or 
            b[1][0] == b[1][1] and b[1][1] == b[1][2] and b[1][0] != None or
            b[2][0] == b[2][1] and b[2][1] == b[2][2] and b[2][0] != None or
            b[0][0] == b[1][0] and b[1][0] == b[2][0] and b[0][0] != None or
            b[0][1] == b[1][1] and b[1][1] == b[2][1] and b[0][1] != None or
            b[0][2] == b[1][2] and b[1][2] == b[2][2] and b[0][2] != None or
            b[0][0] == b[1][1] and b[1][1] == b[2][2] and b[0][0] != None or
            b[2][0] == b[1][1] and b[1][1] == b[0][2] and b[2][0] != None):
            return True
        return False
    
    def isOver(self):
        return self.__isComplete() or self.isWon()
    
class Renderer():
    WIDTH = 500
    HEIGHT = 300
    # x_image = pygame.transform.scale(pygame.image.load('x.png'), (WIDTH // 3 - 10, HEIGHT // 3 - 10)) 
    # o_image = pygame.transform.scale(pygame.image.load('o.png'), (WIDTH // 3 - 10, HEIGHT // 3 - 10))

    def draw_grid(self):
        window = self.window
        # window.fill(pygame.Color(255, 255, 255))
        window.blit(self.background_image, (0, 0))
        # pygame.draw.line(window, (0, 0, 0), (0, Renderer.HEIGHT // 3), (Renderer.WIDTH, Renderer.HEIGHT // 3), 5)
        # pygame.draw.line(window, (0, 0, 0), (0, Renderer.HEIGHT // 3 * 2), (Renderer.WIDTH, Renderer.HEIGHT // 3 * 2), 5)
        # pygame.draw.line(window, (0, 0, 0), (Renderer.WIDTH // 3, 0), (Renderer.WIDTH // 3, Renderer.HEIGHT), 5)
        # pygame.draw.line(window, (0, 0, 0), (Renderer.WIDTH // 3 * 2, 0), (Renderer.WIDTH // 3 * 2, Renderer.HEIGHT), 5)
        pygame.display.update()


    def __init__(self):
        pygame.init()
        self.font = pygame.font.Font(pygame.font.get_default_font(), 70)
        self.window = pygame.display.set_mode((Renderer.WIDTH, Renderer.HEIGHT))
        Renderer.x_image = pygame.font.Font(pygame.font.get_default_font(), Renderer.HEIGHT // 3 - 10).render('X', True, (100, 100, 100))
        Renderer.o_image = pygame.font.Font(pygame.font.get_default_font(), Renderer.HEIGHT // 3 - 10).render('O', True, (100, 100, 100))
        Renderer.x_image = pygame.transform.scale(Renderer.x_image, (Renderer.WIDTH // 3 - 10, Renderer.x_image.get_size()[1]))
        Renderer.o_image = pygame.transform.scale(Renderer.o_image, (Renderer.WIDTH // 3 - 10, Renderer.o_image.get_size()[1]))
        self.background_image = pygame.transform.scale(pygame.image.load('wood.jpg'), (Renderer.WIDTH, Renderer.HEIGHT))
        black_texture = pygame.transform.scale(pygame.image.load('black.jpg'), (Renderer.WIDTH // 3 - 10, Renderer.HEIGHT // 3 - 10))
        Renderer.x_image.blit(black_texture, (0, 0), special_flags=pygame.BLEND_RGB_MULT)
        Renderer.o_image.blit(black_texture, (0, 0), special_flags=pygame.BLEND_RGB_MULT)
        self.draw_grid()    

    def getLocation(self):
        x, y = pygame.mouse.get_pos()
        r, c = y // (Renderer.HEIGHT // 3), x // (Renderer.WIDTH // 3)
        return r, c

    def mark(self, symbol, r, c):
            rect = Renderer.x_image.get_rect() if symbol == 'X' else Renderer.o_image.get_rect()
            rect.center = (c * (Renderer.WIDTH // 3) + Renderer.WIDTH // 6, r * (Renderer.HEIGHT // 3) + Renderer.HEIGHT // 6)
            self.window.blit(Renderer.x_image if symbol == 'X' else Renderer.o_image , rect)
            pygame.display.update()

    def message(self, s):
        self.window.fill((0, 0, 0))
        # self.window.blit(self.background_image, (0, 0))
        text = self.font.render(s, True, (70, 0, 180))
        r = text.get_rect()
        r.center = (Renderer.WIDTH // 2, Renderer.HEIGHT // 2)
        self.window.blit(text, r)
        pygame.display.update()

class Bot():
    def play(game):
        moves = game.get_open_squares()
        if len(moves) >= 8:
            move = (1, 1) if (1, 1) in moves else (0, 0) 
        else:
            move = Bot.__minimax(game)
        symbol = game.advance(*move)
        return move, symbol

    def __minimax(game, isFriendly = True, depth = 0):
        moves = {move: 0 for move in game.get_open_squares()}
        for move, _ in moves.items():
            newgame = copy.deepcopy(game)
            newgame.advance(*move)
            if newgame.isWon():
                moves[move] = 10 - depth if isFriendly else -10 + depth
            elif newgame.isTie():
                moves[move] = 0
            else:
                moves[move] = Bot.__minimax(newgame, not isFriendly, depth + 1)
        if depth == 0:
            values = list(moves.values())
            count = values.count(max(values))
            target = random.randint(1, count)
            while target:
                index = values.index(max(values))
                move = list(moves.keys())[index]
                moves.pop(move)
                values.pop(index)
                target -= 1
            return move
        return max(moves.values()) if isFriendly else min(moves.values())

if __name__ == '__main__':
    renderer = Renderer()
    game = TicTacToe()
    while True:    
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
            if event.type == pygame.MOUSEBUTTONDOWN:
                if game.isOver():
                    game.reset()
                    renderer.draw_grid()
                else:
                    r, c = renderer.getLocation()
                    symbol = game.advance(r, c)
                    if symbol != None:
                        renderer.mark(symbol, r, c)
                        if not game.isOver():
                            botmove, botsymbol = Bot.play(game)
                            renderer.mark(botsymbol, *botmove)
                    if game.isOver():
                        if game.isTie():
                            renderer.message('Tie')
                        else:
                            renderer.message(('X' if game.turn[1] == TicTacToe.X else 'O') + ' won!')
