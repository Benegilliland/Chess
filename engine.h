#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>
#include <algorithm>

#define EMPTY 0
#define BLACK_PAWN 1
#define BLACK_KNIGHT 2
#define BLACK_BISHOP 3
#define BLACK_ROOK 4
#define BLACK_QUEEN 5
#define BLACK_KING 6
#define WHITE_PAWN 7
#define WHITE_KNIGHT 8
#define WHITE_BISHOP 9
#define WHITE_ROOK 10
#define WHITE_QUEEN 11
#define WHITE_KING 12

#define NORMAL_MOVE 0
#define CASTLING 1
#define EN_PASSANT 2
#define PAWN_PROMOTION 3

struct Move {
	char oldSq;
	char newSq;
};

struct Board {
	char b[64];
	bool turn; // True => white's turn, false => black's turn
	bool wChk, bChk, wMt, bMt, stlMt; // White check, black check, white checkmate, black checkmate, stalemate
	Move wEnPass[2], bEnPass[2]; // Stores the available en passant move(s), a maximum of 2 per side
	bool enPassMove; // Whether the player is moving en passant
	bool wCanCstl[2], bCanCstl[2]; // Whether each player can castle, [0] = queenside and [1] = kingside
	char wKingPnt, bKingPnt; // Holds the position of the king

	void copyTo(Board& b) {
		b.turn = this->turn;
		b.wChk = this->wChk;
		b.bChk = this->bChk;
		b.wMt = this->wMt;
		b.bMt = this->bMt;
		b.stlMt = this->stlMt;
		b.wEnPass[0] = this->wEnPass[0];
		b.wEnPass[1] = this->wEnPass[1];
		b.bEnPass[0] = this->bEnPass[0];
		b.bEnPass[1] = this->bEnPass[1];
		b.enPassMove = this->enPassMove;
		b.wCanCstl[0] = this->wCanCstl[0];
		b.wCanCstl[1] = this->wCanCstl[1];
		b.bCanCstl[0] = this->bCanCstl[0];
		b.bCanCstl[1] = this->bCanCstl[1];
		b.wKingPnt = this->wKingPnt;
		b.bKingPnt = this->bKingPnt;
		for (int i = 0; i < 64; i++) {
			b.b[i] = this->b[i];
		}
	}
};

class Engine {

	// GUI
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
	SDL_Rect dstrect[64];
	char dragSq;
	SDL_Rect pawnPromotionWindow[4];
	bool promotingPawn;
	char promotingSquare;

	// Engine
	Board board;
	bool quit;
	std::vector<char> avail_moves;

public:

	int findCheckCounter = 0;
	int findMovesCounter = 0;

	// GUI
	bool initWindow();
	void quitWindow();
	void initBoard();
	void drawGame();
	void pollEvents();
	void openPromotionWindow(char square);
	void closePromotionWindow();
	void mDownEvent(SDL_Event& event);
	void mUpEvent(SDL_Event& event);
	void mMotionEvent(SDL_Event& event);
	void keyDownEvent(SDL_Event& event);
	void showAvailMoves();

	// Engine
	void resetGame();
	bool detectCheck(bool turn);
	bool detectCheck(bool turn, Move move);
	bool detectMate(bool turn);
	void calcPawnMoves(char square, std::vector<char>& moves, bool white);
	void calcKnightMoves(char square, std::vector<char>& moves, bool white);
	void calcBishMoves(char square, std::vector<char>& moves, bool white);
	void calcRookMoves(char square, std::vector<char>& moves, bool white);
	void calcQueenMoves(char square, std::vector<char>& moves, bool white);
	void calcKingMoves(char square, std::vector<char>& moves, bool white);
	void addCastlingMoves(char square, std::vector<char>& moves);
	void calcAvailMoves(char square, std::vector<char>& moves);
	bool validateMove(Move move);
	void movePiece(Move move);
	//void movePieces(std::vector<Move>& moves);
	//void undoMoves(Board* board);
	void makePlayerMove(Move move, char moveType = 0);
	void switchTurn();
	void pawnPromotion(char square, char choice);
	void findAvailableMoves(bool turn, std::vector<Move> preMoves, std::vector<Move>& moves);
	void findAvailableMoves(bool turn, std::vector<Move>& moves);
	bool canCastle(bool turn, bool side);

	// AI
	bool gameOver();
	bool getTurn();
	char* getBoard();

	Engine() {
		quit = false;
		if (initWindow()) {
			initBoard();
			resetGame();
		}
	}
};