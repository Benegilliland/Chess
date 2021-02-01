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
	SDL_Surface* surf = IMG_Load("images/chess_pieces.png");
	image = SDL_CreateTextureFromSurface(renderer, surf);
	for (int i = 0; i < 12; i++) {
		pieces[i] = { 128 * (i % 6), ((i > 5) ? 128 : 0), 128, 128 };
	}
	for (int i = 0; i < 64; i++) {
		dstrect[i] = { (i % 8) * SQUARE_WIDTH, (int)(i / 8) * SQUARE_HEIGHT, SQUARE_WIDTH , SQUARE_HEIGHT };
	}
	for (int i = 0; i < 32; i++) {
		white_squares[i] = SDL_Rect{ (((2 * i) % 8) + (int)(i / 4) % 2) * SQUARE_WIDTH, (int)(i / 4) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
		black_squares[i] = SDL_Rect{ (((2 * i + 1) % 8) - (int)(i / 4) % 2) * SQUARE_WIDTH, (int)(i / 4) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
	}
	dragSq = -1;
}

void Engine::openPromotionWindow(char square) {
	promotingPawn = true;
	promotingSquare = square;
	int x = square % 8;
	int y = square / 8;
	pawnPromotionWindow[0] = { x * SQUARE_WIDTH, y * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
	pawnPromotionWindow[1] = { x * SQUARE_WIDTH, (y + 1) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
	pawnPromotionWindow[2] = { x * SQUARE_WIDTH, (y + 2) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
	pawnPromotionWindow[3] = { x * SQUARE_WIDTH, (y + 3) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
}

void Engine::closePromotionWindow() {
	promotingPawn = false;
}

void Engine::keyDownEvent(SDL_Event& event) {
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
		quit = true;
		break;
	}
}

void Engine::mDownEvent(SDL_Event& event) {
	if (!promotingPawn) {
		int nSquare = (int)(event.button.x / SQUARE_WIDTH) + 8 * (int)(event.button.y / SQUARE_HEIGHT);
		if ((board.turn && board.b[nSquare] > BLACK_KING) || (!board.turn && board.b[nSquare] <= BLACK_KING && board.b[nSquare] != EMPTY)) {
			highlighted_squares.push_back({ (int)(event.button.x / SQUARE_WIDTH) * SQUARE_WIDTH,
				(int)(event.button.y / SQUARE_HEIGHT) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT });
			if (board.b[nSquare] == WHITE_KING || board.b[nSquare] == BLACK_KING) addCastlingMoves(nSquare, board);
			showAvailMoves(nSquare);
			dragSq = nSquare;
		}
	}
}

void Engine::mUpEvent(SDL_Event& event) {
	if (promotingPawn) {
		closePromotionWindow();
		SDL_Point mouse = { event.button.x, event.button.y };
		if (SDL_PointInRect(&mouse, &pawnPromotionWindow[0])) board.b[promotingSquare] = WHITE_QUEEN;
		if (SDL_PointInRect(&mouse, &pawnPromotionWindow[1])) board.b[promotingSquare] = WHITE_ROOK;
		if (SDL_PointInRect(&mouse, &pawnPromotionWindow[2])) board.b[promotingSquare] = WHITE_BISHOP;
		if (SDL_PointInRect(&mouse, &pawnPromotionWindow[3])) board.b[promotingSquare] = WHITE_KNIGHT;
	}
	else {
		if (highlighted_squares.size() > 0) {
			SDL_Rect rect = highlighted_squares[0];
			char oldSquare = rect.x / SQUARE_WIDTH + 8 * rect.y / SQUARE_HEIGHT;
			char newSquare = (int)(event.button.x / SQUARE_WIDTH) + 8 * (int)(event.button.y / SQUARE_HEIGHT);
			if (validateMove({ oldSquare, newSquare }, board)) { // This should be a function in engine.cpp
				makePlayerMove({ oldSquare, newSquare });

				// Draw
				prevMove[0] = { (oldSquare % 8) * SQUARE_WIDTH, (int)(oldSquare / 8) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
				prevMove[1] = { (newSquare % 8) * SQUARE_WIDTH, (int)(newSquare / 8) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
			}
			dstrect[oldSquare] = { (oldSquare % 8) * SQUARE_WIDTH, (int)(oldSquare / 8) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT };
		}
		dragSq = -1;
		highlighted_squares.clear();
		avail_moves.clear();
	}
}

void Engine::mMotionEvent(SDL_Event& event) {
	if (!promotingPawn) {
		if (dragSq != -1) {
			dstrect[dragSq].x += event.motion.xrel;
			dstrect[dragSq].y += event.motion.yrel;
		}
	}
}

void Engine::pollEvents() {
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			quit = true;
			break;
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

void Engine::drawGame() {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 177, 228, 185, 255);
	SDL_RenderDrawRects(renderer, white_squares, 32);
	SDL_RenderFillRects(renderer, white_squares, 32);

	SDL_SetRenderDrawColor(renderer, 112, 162, 163, 255);
	SDL_RenderDrawRects(renderer, black_squares, 32);
	SDL_RenderFillRects(renderer, black_squares, 32);

	SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	SDL_RenderDrawRects(renderer, prevMove, 2);
	SDL_RenderFillRects(renderer, prevMove, 2);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawRects(renderer, highlighted_squares.data(), highlighted_squares.size());
	SDL_RenderFillRects(renderer, highlighted_squares.data(), highlighted_squares.size());

	for (int i = 0; i < 64; i++) {
		SDL_RenderCopy(renderer, image, &pieces[board.b[i] - 1], &dstrect[i]);
	}

	if (promotingPawn) {
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderDrawRects(renderer, pawnPromotionWindow, 4);
		SDL_RenderFillRects(renderer, pawnPromotionWindow, 4);
		SDL_RenderCopy(renderer, image, &pieces[10], &pawnPromotionWindow[0]);
		SDL_RenderCopy(renderer, image, &pieces[9], &pawnPromotionWindow[1]);
		SDL_RenderCopy(renderer, image, &pieces[8], &pawnPromotionWindow[2]);
		SDL_RenderCopy(renderer, image, &pieces[7], &pawnPromotionWindow[3]);
	}

	SDL_RenderPresent(renderer);
}

void Engine::showAvailMoves(int square) {
	for (Move& i : board.moves) {
		if (i.oldSq == square) {
			highlighted_squares.push_back({ (i.newSq % 8) * SQUARE_WIDTH, (int)(i.newSq / 8) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT });
		}
	}
}