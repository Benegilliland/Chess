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

struct Move {
	int sq[2];
};

class Engine {

	// GUI
	const char* WINDOW_TITLE = "Chess";
	const int SQUARE_WIDTH = 100;
	const int SQUARE_HEIGHT = 100;
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Rect white_squares[32];
	SDL_Rect black_squares[32];
	SDL_Rect prevMove[2];
	std::vector<SDL_Rect> highlighted_squares;
	SDL_Texture* image;
	SDL_Rect pieces[12];
	SDL_Rect dstrect[64];
	char dragSq;
	SDL_Rect pawnPromotionWindow[4];
	bool promotingPawn;
	char promotingSquare;

	// Engine
	Piece board[64];
	bool quit;
	std::vector<int> avail_moves;
	bool turn; // True => white's turn, false => black's turn
	bool wChk, bChk, wMt, bMt, stlMt; // White check, black check, white checkmate, black checkmate, stalemate
	Move wEnPass[2], bEnPass[2]; // Stores the available en passant move(s), a maximum of 2 per side
	bool enPassMove; // Whether the player is moving en passant
	bool wCanCstl[2], bCanCstl[2]; // Whether each player can castle, [0] = queenside and [1] = kingside

public:

	// GUI
	bool initWindow();
	void quitWindow();
	void initBoard();
	void drawGame();
	void pollEvents();
	void openPromotionWindow(int square);
	void closePromotionWindow();

	// Engine
	void resetGame();
	bool detectCheck(bool turn);
	bool detectCheck(bool turn, Move move);
	void detectMate(bool turn);
	void calcPawnMoves(int square, std::vector<int>& moves, bool white);
	void calcKnightMoves(int square, std::vector<int>& moves, bool white);
	void calcBishMoves(int square, std::vector<int>& moves, bool white);
	void calcRookMoves(int square, std::vector<int>& moves, bool white);
	void calcQueenMoves(int square, std::vector<int>& moves, bool white);
	void calcKingMoves(int square, std::vector<int>& moves, bool white);
	void addCastlingMoves(int square, std::vector<int>& moves);
	void calcAvailMoves(int square, std::vector<int>& moves);
	void showAvailMoves();
	bool validateMove(int oldSquare, int newSquare);
	void movePiece(int oldSquare, int newSquare);
	void unmovePiece(int oldSquare, int newSquare, Piece p);
	void mDownEvent(SDL_Event& event);
	void mUpEvent(SDL_Event& event);
	void mMotionEvent(SDL_Event& event);
	void keyDownEvent(SDL_Event& event);
	int findKing(bool turn, Piece* board);
	bool gameOver();
	bool getTurn();
	void switchTurn();
	Piece* getBoard();
	void pawnPromotion(int square, int choice);
	void printBoard(Piece* board);
	void findAvailableMoves(bool turn, std::vector<Move> preMoves, std::vector<Move>& moves);
	void findAvailableMoves(bool turn, std::vector<Move>& moves);
	bool canCastle(bool turn, bool side);
	void makePlayerMove(int oldSquare, int newSquare);

};