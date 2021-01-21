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

class Engine {
	const char* WINDOW_TITLE = "Chess";
	const int SQUARE_WIDTH = 80;
	const int SQUARE_HEIGHT = 80;
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Rect white_squares[32];
	SDL_Rect black_squares[32];
	std::vector<SDL_Rect> highlighted_squares;
	SDL_Texture* image;
	SDL_Rect pieces[12];
	Piece board[64];
	bool quit;
	std::vector<int> avail_moves;
	bool turn;
	bool dragging = false;
	int dragx, dragy, dragsq;
	int wKingPnt, bKingPnt;
	Checkstate checkstate;

	bool initWindow();
	void quitWindow();
	void initBoard();
	void loadImages();
	void drawGame();
	void pollEvents();
	void resetGame();
	bool detectCheck(bool white);
	bool detectCheckmate(bool white);
	void calcAvailMoves(int square);
	void showAvailMoves();
	void makeMove(int oldSquare, int newSquare);
	bool validateMove(int oldSquare, int newSquare);
	void calcDiagMoves(int square);
	void calcStrMoves(int square);
	void mDownEvent(SDL_Event& event);
	void mUpEvent(SDL_Event& event);
	void mMotionEvent(SDL_Event& event);
	void keyDownEvent(SDL_Event& event);
	void bPawnMoves(int square);
	void wPawnMoves(int square);
	void knightMoves(int square);

public:
	void runGameLoop();
};