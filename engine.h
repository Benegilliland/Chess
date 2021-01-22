#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>
#include <algorithm>

enum Piece {
	empty_square, black_pawn, black_knight, black_bishop, black_rook, black_queen, black_king,
	white_pawn, white_knight, white_bishop, white_rook, white_queen, white_king
};

enum Checkstate {
	no_check, black_check, white_check, black_checkmate, white_checkmate
};

enum Turn {
	black = false, white = true
};

struct Move {
	int sq[2];
};

class Engine {
	const char* WINDOW_TITLE = "Chess";
	const int SQUARE_WIDTH = 80;
	const int SQUARE_HEIGHT = 80;
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Rect white_squares[32];
	SDL_Rect black_squares[32];
	SDL_Rect prevMove[2];
	std::vector<SDL_Rect> highlighted_squares;
	SDL_Texture* image;
	SDL_Rect pieces[12];
	Piece board[64];
	bool quit;
	std::vector<int> avail_moves;
	Turn turn;
	bool dragging = false;
	int dragx, dragy, dragsq;
	Checkstate checkstate;

public:
	void runGameLoop();
	bool initWindow();
	void quitWindow();
	void initBoard();
	void loadImages();
	void drawGame();
	void pollEvents();
	void resetGame();
	bool detectCheck(bool turn, Piece* board);
	bool detectCheckmate(bool turn);
	void calcAvailMoves(int square, std::vector<int>& moves, Piece* board);
	void showAvailMoves();
	void makeMove(int oldSquare, int newSquare);
	bool validateMove(int oldSquare, int newSquare);
	void calcDiagMoves(int square, std::vector<int>& moves, Piece* board);
	void calcStrMoves(int square, std::vector<int>& moves, Piece* board);
	void mDownEvent(SDL_Event& event);
	void mUpEvent(SDL_Event& event);
	void mMotionEvent(SDL_Event& event);
	void keyDownEvent(SDL_Event& event);
	void bPawnMoves(int square, std::vector<int>& moves, Piece* board);
	void wPawnMoves(int square, std::vector<int>& moves, Piece* board);
	void knightMoves(int square, std::vector<int>& moves, Piece* board);
	int findKing(bool turn, Piece* board);
	bool gameOver();
	bool getTurn();
	void switchTurn();
	Piece* getBoard();
	void promoteQueen(int square);
	void promoteBishop(int square);
	void promoteKnight(int square);
	void printBoard(Piece* board);
	void findAvailableMoves(Turn turn, std::vector<Move> preMoves, std::vector<Move>& moves);
};