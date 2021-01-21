/*
To add:
- en passant rule
- checkmate
- stalemate
- add better highlighting, with different colours
*/

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "engine.h"

bool Engine::initWindow() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return false;

	if (IMG_Init(IMG_INIT_PNG) == 0) return false;

	window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 8 * SQUARE_WIDTH, 8 * SQUARE_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL) return false;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) return false;

	std::cout << "Successfully initiated game\n";
	return true;
}

void Engine::quitWindow() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}

void Engine::initBoard() {
	for (int i = 0; i < 32; i++) {
		board[i] = board[i + 32] = empty_square;
		white_squares[i] = SDL_Rect{ (((2 * i) % 8) + (int)(i / 4) % 2) * SQUARE_WIDTH, (int)(i / 4) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
		black_squares[i] = SDL_Rect{ (((2 * i + 1) % 8) - (int)(i / 4) % 2) * SQUARE_WIDTH, (int)(i / 4) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
	}
	dragging = false;
	dragsq = dragx = dragy = 0;
}

void Engine::loadImages() {
	SDL_Surface* surf = IMG_Load("images/chess_pieces.png");
	image = SDL_CreateTextureFromSurface(renderer, surf);
	for (int i = 0; i < 12; i++) {
		pieces[i] = { 128 * (i % 6), ((i > 5) ? 128 : 0), 128, 128 };
	}
}

void Engine::resetGame() {
	for (int i = 0; i < 8; i++) {
		for (int j = 2; j < 6; j++) {
			board[8 * j + i] = empty_square;
		}
		board[8 + i] = black_pawn;
		board[48 + i] = white_pawn;
	}
	board[0] = board[7] = black_rook;
	board[1] = board[6] = black_knight;
	board[2] = board[5] = black_bishop;
	board[3] = black_queen;
	board[4] = black_king;
	bKingPnt = 4;
	board[56] = board[63] = white_rook;
	board[57] = board[62] = white_knight;
	board[58] = board[61] = white_bishop;
	board[59] = white_queen;
	board[60] = white_king;
	wKingPnt = 60;
	turn = true;
	checkstate = no_check;
}

void Engine::drawGame() {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 177, 228, 185, 255);
	SDL_RenderDrawRects(renderer, white_squares, 32);
	SDL_RenderFillRects(renderer, white_squares, 32);

	SDL_SetRenderDrawColor(renderer, 112, 162, 163, 255);
	SDL_RenderDrawRects(renderer, black_squares, 32);
	SDL_RenderFillRects(renderer, black_squares, 32);

	SDL_SetRenderDrawColor(renderer, 255, 127, 0, 255);
	SDL_RenderDrawRects(renderer, highlighted_squares.data(), highlighted_squares.size());
	SDL_RenderFillRects(renderer, highlighted_squares.data(), highlighted_squares.size());

	SDL_Rect dstrect;
	for (int i = 0; i < 64; i++) {
		if (board[i] != empty_square) {
			dstrect = { (i % 8) * SQUARE_WIDTH, (int)(i / 8) * SQUARE_HEIGHT, SQUARE_WIDTH , SQUARE_HEIGHT };
			if (dragging && i == dragsq) {
				dstrect.x += dragx;
				dstrect.y += dragy;
			}
		}
		SDL_RenderCopy(renderer, image, &pieces[board[i] - 1], &dstrect);
	}

	SDL_RenderPresent(renderer);
}

void Engine::makeMove(int oldSquare, int newSquare) {
	if (board[oldSquare] == black_king) bKingPnt = newSquare;
	if (board[oldSquare] == white_king) wKingPnt = newSquare;
	board[newSquare] = board[oldSquare];
	board[oldSquare] = empty_square;
	turn = !turn;
}

bool Engine::validateMove(int oldSquare, int newSquare) { // To add: make sure your move doesn't put you in check
	bool canMove = false;
	for (int i = 0; i < avail_moves.size(); i++) {
		if (avail_moves[i] == newSquare) {
			canMove = true;
			break;
		}
	}
	if (canMove) {
		if (checkstate = no_check) {
			Piece piece = board[newSquare];
			makeMove(oldSquare, newSquare);
			canMove = !detectCheck(turn);
			makeMove(newSquare, oldSquare);
			board[newSquare] = piece;
		}
		else {
			Piece piece = board[newSquare];
			makeMove(oldSquare, newSquare);
			canMove = !detectCheck(!turn);
			makeMove(newSquare, oldSquare);
			board[newSquare] = piece;
		}
	}
	return canMove;
}

void Engine::keyDownEvent(SDL_Event& event) {
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
		quit = true;
		break;
	}
}

void Engine::mDownEvent(SDL_Event& event) {
	int nSquare = (int)(event.button.x / SQUARE_WIDTH) + 8 * (int)(event.button.y / SQUARE_HEIGHT);
	if ((turn && board[nSquare] > black_king) || (!turn && board[nSquare] <= black_king && board[nSquare] != empty_square)) {
		highlighted_squares.push_back({ (int)(event.button.x / SQUARE_WIDTH) * SQUARE_WIDTH,
			(int)(event.button.y / SQUARE_HEIGHT) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT });
		calcAvailMoves(nSquare);
		showAvailMoves();
		dragging = true;
		dragx = dragy = 0;
		dragsq = nSquare;
	}
}

void Engine::mUpEvent(SDL_Event& event) {
	if (highlighted_squares.size() > 0) {
		SDL_Rect rect = highlighted_squares[0];
		int oldSquare = rect.x / SQUARE_WIDTH + 8 * rect.y / SQUARE_HEIGHT;
		int newSquare = (int)(event.button.x / SQUARE_WIDTH) + 8 * (int)(event.button.y / SQUARE_HEIGHT);
		if (validateMove(oldSquare, newSquare)) {
			makeMove(oldSquare, newSquare);
			checkstate = detectCheck(turn) ? (turn ? white_check : black_check) : no_check;
			if (checkstate != no_check) {
				detectCheckmate(turn);
			}
		}
	}
	dragging = false;
	dragsq = dragy = dragx = 0;
	highlighted_squares.clear();
	avail_moves.clear();
}

void Engine::mMotionEvent(SDL_Event& event) {
	dragx += event.motion.xrel;
	dragy += event.motion.yrel;
}

void Engine::pollEvents() {
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_KEYDOWN:
				keyDownEvent(event);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mDownEvent(event);
				break;
			case SDL_MOUSEBUTTONUP:
				mUpEvent(event);
				break;
			case SDL_MOUSEMOTION:
				mMotionEvent(event);
				break;
		}
	}
}

void Engine::calcStrMoves(int square) {
	if (board[square] != empty_square) {
		bool white = board[square] > black_king;
		int x = square % 8;
		int y = (int)(square / 8);
		for (int i = 1; i < 8 - x; i++) {
			if (board[square + i] != empty_square) {
				if ((white && board[square + i] <= black_king) || (!white && board[square + i] > black_king)) avail_moves.push_back(square + i);
				break;
			}
			else {
				avail_moves.push_back(square + i);
			}
		}
		for (int i = 1; i < (x + 1); i++) {
			if (board[square - i] != empty_square) {
				if ((white && board[square - i] <= black_king) || (!white && board[square - i] > black_king)) avail_moves.push_back(square - i);
				break;
			}
			else {
				avail_moves.push_back(square - i);
			}
		}
		for (int i = 1; i < 8 - y; i++) {
			if (board[square + 8*i] != empty_square) {
				if ((white && board[square + 8*i] <= black_king) || (!white && board[square + 8*i] > black_king)) avail_moves.push_back(square + 8*i);
				break;
			}
			else {
				avail_moves.push_back(square + 8*i);
			}
		}
		for (int i = 1; i < (y + 1); i++) {
			if (board[square - 8*i] != empty_square) {
				if ((white && board[square - 8*i] <= black_king) || (!white && board[square - 8*i] > black_king)) avail_moves.push_back(square - 8*i);
				break;
			}
			else {
				avail_moves.push_back(square - 8*i);
			}
		}
	}
}

void Engine::calcDiagMoves(int square) {
	if (board[square] != empty_square) {
		bool white = board[square] > black_king;
		int x = square % 8;
		int y = (int)(square / 8);
			// Up-left
			for (int i = 1; i < std::min(x,y)+1; i++) {
				if (board[square - 9 * i] != empty_square) {
					if ((white && board[square - 9 * i] <= black_king) || (!white && (board[square - 9*i] == empty_square || board[square - 9*i] > black_king))) avail_moves.push_back(square - 9 * i);
					break;
				}
				else {
					avail_moves.push_back(square - 9 * i);
				}
			}
			// Up-right
			for (int i = 1; i < std::min((7-x), y) + 1; i++) {
				if (board[square - 7 * i] != empty_square) {
					if ((white && board[square - 7 * i] <= black_king) || (!white && (board[square - 7 * i] == empty_square || board[square - 7 * i] > black_king))) avail_moves.push_back(square - 7 * i);
					break;
				}
				else {
					avail_moves.push_back(square - 7 * i);
				}
			}
			// Down left
			for (int i = 1; i < std::min(x, (7-y)) + 1; i++) {
				if (board[square + 7 * i] > empty_square) {
					if ((white && board[square + 7 * i] <= black_king) || (!white && (board[square + 7 * i] == empty_square || board[square + 7 * i] > black_king))) avail_moves.push_back(square + 7 * i);
					break;
				}
				else {
					avail_moves.push_back(square + 7 * i);
				}
			}
			// Down right
			for (int i = 1; i < std::min((7-x), (7 - y)) + 1; i++) {
				if (board[square + 9 * i] > empty_square) {
					if ((white && board[square + 9 * i] <= black_king) || (!white && (board[square + 9 * i] == empty_square || board[square + 9 * i] > black_king))) avail_moves.push_back(square + 9 * i);
					break;
				}
				else {
					avail_moves.push_back(square + 9 * i);
				}
			}
	}
}

void Engine::bPawnMoves(int square) {
	if (board[square + 8] == empty_square) avail_moves.push_back(square + 8);
	if ((int)(square / 8) == 1 && board[square + 8] == empty_square && board[square + 16] == empty_square) avail_moves.push_back(square + 16);
	if ((square % 8 != 0) && (board[square + 7] > black_king)) avail_moves.push_back(square + 7);
	if ((square % 8 != 7) && (board[square + 9] > black_king)) avail_moves.push_back(square + 9);
}

void Engine::wPawnMoves(int square) {
	if (board[square - 8] == empty_square) avail_moves.push_back(square - 8);
	if ((int)(square / 8) == 6 && board[square - 8] == empty_square && board[square - 16] == empty_square) avail_moves.push_back(square - 16);
	if ((square % 8 != 0) && (board[square - 7] <= black_king && board[square - 7] != empty_square)) avail_moves.push_back(square - 7);
	if ((square % 8 != 7) && (board[square - 9] <= black_king && board[square - 9] != empty_square)) avail_moves.push_back(square - 9);
}

void Engine::calcAvailMoves(int square) {
	switch (board[square]) {
	case empty_square:
		break;
	case black_pawn:
		bPawnMoves(square);
		break;
	case white_pawn:
		wPawnMoves(square);
		break;
	case black_knight:
		if (square % 8 != 7 && square > 15 && (board[square - 15] > black_king || board[square - 15] == empty_square)) avail_moves.push_back(square - 15);
		if (square % 8 < 6 && square > 6 && (board[square - 6] > black_king || board[square - 6] == empty_square)) avail_moves.push_back(square - 6);
		if (square % 8 != 0 && square > 16 && (board[square - 17] > black_king || board[square - 17] == empty_square)) avail_moves.push_back(square - 17);
		if (square % 8 > 1 && square > 8 && (board[square - 10] > black_king || board[square - 10] == empty_square)) avail_moves.push_back(square - 10);
		if (square % 8 != 7 && square < 46 && (board[square + 17] > black_king || board[square + 17] == empty_square)) avail_moves.push_back(square + 17);
		if (square % 8 < 6 && square < 53 && (board[square + 10] > black_king || board[square + 10] == empty_square)) avail_moves.push_back(square + 10);
		if (square % 8 != 0 && square < 48 && (board[square + 15] > black_king || board[square + 15] == empty_square)) avail_moves.push_back(square + 15);
		if (square % 8 > 1 && square < 57 && (board[square + 6] > black_king || board[square + 6] == empty_square)) avail_moves.push_back(square + 6);
		break;
	case white_knight:
		if (square % 8 != 7 && square > 15 && (board[square - 15] <= black_king)) avail_moves.push_back(square - 15);
		if (square % 8 < 6 && square > 6 && (board[square - 6] <= black_king)) avail_moves.push_back(square - 6);
		if (square % 8 != 0 && square > 16 && (board[square - 17] <= black_king)) avail_moves.push_back(square - 17);
		if (square % 8 > 1 && square > 8 && (board[square - 10] <= black_king)) avail_moves.push_back(square - 10);
		if (square % 8 != 7 && square < 46 && (board[square + 17] <= black_king)) avail_moves.push_back(square + 17);
		if (square % 8 < 6 && square < 53 && (board[square + 10] <= black_king)) avail_moves.push_back(square + 10);
		if (square % 8 != 0 && square < 48 && (board[square + 15] <= black_king)) avail_moves.push_back(square + 15);
		if (square % 8 > 1 && square < 57 && (board[square + 6] <= black_king)) avail_moves.push_back(square + 6);
		break;
	case black_bishop:
	case white_bishop:
		calcDiagMoves(square);
		break;
	case black_rook:
	case white_rook:
		calcStrMoves(square);
		break;
	case black_queen:
	case white_queen:
		calcStrMoves(square);
		calcDiagMoves(square);
		break;
	case black_king:
		if (square % 8 != 7 && (board[square + 1] == empty_square || board[square + 1] > black_king)) avail_moves.push_back(square + 1);
		if (square % 8 != 0 && (board[square - 1] == empty_square || board[square - 1] > black_king)) avail_moves.push_back(square - 1);
		if (square < 56 && (board[square + 8] == empty_square || board[square + 8] > black_king)) avail_moves.push_back(square + 8);
		if (square > 7 && (board[square - 8] == empty_square || board[square - 8] > black_king)) avail_moves.push_back(square - 8);
		if (square % 8 != 0 && square > 8 && (board[square - 9] == empty_square || board[square - 9] > black_king)) avail_moves.push_back(square - 9);
		if (square % 8 != 7 && square > 8 && (board[square - 7] == empty_square || board[square - 7] > black_king)) avail_moves.push_back(square - 7);
		if (square % 8 != 0 && square < 56 && (board[square + 7] == empty_square || board[square + 7] > black_king)) avail_moves.push_back(square + 7);
		if (square % 8 != 7 && square < 56 && (board[square + 9] == empty_square || board[square + 9] > black_king)) avail_moves.push_back(square + 9);
		break;
	case white_king:
		if (square % 8 != 7 && board[square + 1] <= black_king) avail_moves.push_back(square + 1);
		if (square % 8 != 0 &&board[square - 1] <= black_king) avail_moves.push_back(square - 1);
		if (square < 56 && board[square + 8] <= black_king) avail_moves.push_back(square + 8);
		if (square > 7 && board[square - 8] <= black_king) avail_moves.push_back(square - 8);
		if (square % 8 != 0 && square > 8 && board[square - 9] <= black_king) avail_moves.push_back(square - 9);
		if (square % 8 != 7 && square > 8 && board[square - 7] <= black_king) avail_moves.push_back(square - 7);
		if (square % 8 != 0 && square < 56 && board[square + 7] <= black_king) avail_moves.push_back(square + 7);
		if (square % 8 != 7 && square < 56 && board[square + 9] <= black_king) avail_moves.push_back(square + 9);
		break;
	}
}

void Engine::showAvailMoves() {
	for (int i = 0; i < avail_moves.size(); i++) {
		highlighted_squares.push_back({ (avail_moves[i] % 8) * SQUARE_WIDTH, (int)(avail_moves[i] / 8) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT });
	}
}

bool Engine::detectCheck(bool white) {
	avail_moves.clear();
	for (int i = 0; i < 63; i++) {
		calcAvailMoves(i);
		for (int j = 0; j < avail_moves.size(); j++) {
			if (white) {
				if (avail_moves[j] == wKingPnt && board[i] <= black_king && board[i] != empty_square) {
					std::cout << "White king is in check\n";
					checkstate = white_check;
					avail_moves.clear();
					return true;
				}
			}
			else {
				if (avail_moves[j] == bKingPnt && board[i] > black_king) {
					std::cout << "Black king is in check\n";
					checkstate = black_check;
					avail_moves.clear();
					return true;
					break;
				}
			}
		}
		avail_moves.clear();
	}
	return false;
}

bool Engine::detectCheckmate(bool turn) {
	
}

void Engine::runGameLoop() {
	
	if (initWindow()) {

		initBoard();
		loadImages();
		resetGame();
		quit = false;
		while (!quit) {
			drawGame();
			pollEvents();
		}
	}

	quitWindow();

}
