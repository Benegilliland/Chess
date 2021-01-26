#include "engine.h"

void Engine::resetGame() {
	for (int i = 0; i < 8; i++) {
		for (int j = 2; j < 6; j++) {
			board.b[8 * j + i] = EMPTY;
		}
		board.b[8 + i] = BLACK_PAWN;
		board.b[48 + i] = WHITE_PAWN;
	}
	board.b[0] = board.b[7] = BLACK_ROOK;
	board.b[1] = board.b[6] = BLACK_KNIGHT;
	board.b[2] = board.b[5] = BLACK_BISHOP;
	board.b[3] = BLACK_QUEEN;
	board.b[4] = BLACK_KING;
	board.bKingPnt = 4;
	board.b[56] = board.b[63] = WHITE_ROOK;
	board.b[57] = board.b[62] = WHITE_KNIGHT;
	board.b[58] = board.b[61] = WHITE_BISHOP;
	board.b[59] = WHITE_QUEEN;
	board.b[60] = WHITE_KING;
	board.wKingPnt = 60;
	board.turn = true;
	board.wChk = board.bChk = false;
	prevMove[0] = prevMove[1] = {};
	board.wEnPass[0] = board.bEnPass[0] = {};
	board.wEnPass[1] = board.bEnPass[1] = {};
	board.enPassMove = false;
	promotingPawn = false;
	board.wCanCstl[0] = board.wCanCstl[1] = board.bCanCstl[0] = board.bCanCstl[1] = true;
}

void Engine::movePiece(Move move) {
	board.b[move.newSq] = board.b[move.oldSq];
	board.b[move.oldSq] = EMPTY;
	if (board.b[move.newSq] == WHITE_KING) board.wKingPnt = move.newSq;
	else if (board.b[move.newSq] == BLACK_KING) board.bKingPnt = move.newSq;
}

bool Engine::validateMove(Move move) {
	bool canMove = false;
	bool white = board.b[move.oldSq] > BLACK_KING;
	for (int i = 0; i < avail_moves.size(); i++) {
		if (avail_moves[i] == move.newSq) {
			canMove = true;
			break;
		}
	}
	if (canMove) {
		canMove = !detectCheck(board.turn, move);
	}
	return canMove;
}

void Engine::calcRookMoves(char square, std::vector<char>& moves, bool white) {
		int x = square % 8;
		int y = (int)(square / 8);
		for (int i = 1; i < 8 - x; i++) {
			if (board.b[square + i] != EMPTY) {
				if ((white && board.b[square + i] <= BLACK_KING) || (!white && board.b[square + i] > BLACK_KING)) moves.push_back(square + i);
				break;
			}
			else {
				moves.push_back(square + i);
			}
		}
		for (int i = 1; i < (x + 1); i++) {
			if (board.b[square - i] != EMPTY) {
				if ((white && board.b[square - i] <= BLACK_KING) || (!white && board.b[square - i] > BLACK_KING)) moves.push_back(square - i);
				break;
			}
			else {
				moves.push_back(square - i);
			}
		}
		for (int i = 1; i < 8 - y; i++) {
			if (board.b[square + 8*i] != EMPTY) {
				if ((white && board.b[square + 8*i] <= BLACK_KING) || (!white && board.b[square + 8*i] > BLACK_KING)) moves.push_back(square + 8*i);
				break;
			}
			else {
				moves.push_back(square + 8*i);
			}
		}
		for (int i = 1; i < (y + 1); i++) {
			if (board.b[square - 8*i] != EMPTY) {
				if ((white && board.b[square - 8*i] <= BLACK_KING) || (!white && board.b[square - 8*i] > BLACK_KING)) moves.push_back(square - 8*i);
				break;
			}
			else {
				moves.push_back(square - 8*i);
			}
		}
}

void Engine::calcBishMoves(char square, std::vector<char>& moves, bool white) {
		int x = square % 8;
		int y = (int)(square / 8);
			// Up-left
			for (int i = 1; i < std::min(x,y)+1; i++) {
				if (board.b[square - 9 * i] != EMPTY) {
					if ((white && board.b[square - 9 * i] <= BLACK_KING) || (!white && (board.b[square - 9*i] == EMPTY || board.b[square - 9*i] > BLACK_KING))) moves.push_back(square - 9 * i);
					break;
				}
				else {
					moves.push_back(square - 9 * i);
				}
			}
			// Up-right
			for (int i = 1; i < std::min((7-x), y) + 1; i++) {
				if (board.b[square - 7 * i] != EMPTY) {
					if ((white && board.b[square - 7 * i] <= BLACK_KING) || (!white && (board.b[square - 7 * i] == EMPTY || board.b[square - 7 * i] > BLACK_KING))) moves.push_back(square - 7 * i);
					break;
				}
				else {
					moves.push_back(square - 7 * i);
				}
			}
			// Down left
			for (int i = 1; i < std::min(x, (7-y)) + 1; i++) {
				if (board.b[square + 7 * i] > EMPTY) {
					if ((white && board.b[square + 7 * i] <= BLACK_KING) || (!white && (board.b[square + 7 * i] == EMPTY || board.b[square + 7 * i] > BLACK_KING))) moves.push_back(square + 7 * i);
					break;
				}
				else {
					moves.push_back(square + 7 * i);
				}
			}
			// Down right
			for (int i = 1; i < std::min((7-x), (7 - y)) + 1; i++) {
				if (board.b[square + 9 * i] > EMPTY) {
					if ((white && board.b[square + 9 * i] <= BLACK_KING) || (!white && (board.b[square + 9 * i] == EMPTY || board.b[square + 9 * i] > BLACK_KING))) moves.push_back(square + 9 * i);
					break;
				}
				else {
					moves.push_back(square + 9 * i);
				}
			}
}

void Engine::calcPawnMoves(char square, std::vector<char>& moves, bool white) {
	if (board.b[square + (white?-8:8)] == EMPTY) moves.push_back(square + (white?-8:8));
	if ((int)(square / 8) == (white?6:1) && board.b[square + (white?-8:8)] == EMPTY && board.b[square + (white ? -16 : 16)] == EMPTY) moves.push_back(square + (white ? -16 : 16));
	if ((square % 8 != 7) && (board.b[square + (white ? -7 : 9)] <= (white?6:12) && board.b[square + (white ? -7 : 9)] > (white?0:6))) moves.push_back(square + (white ? -7 : 9));
	if ((square % 8 != 0) && (board.b[square + (white ? -9 : 7)] <= (white ? 6 : 12) && board.b[square + (white ? -9 : 7)] > (white ? 0 : 6))) moves.push_back(square + (white ? -9: 7));
	if (white) { // En passant moves
		if (board.wEnPass[0].oldSq == square) moves.push_back(board.wEnPass[0].newSq);
		if (board.wEnPass[1].oldSq == square) moves.push_back(board.wEnPass[1].newSq);
	}
	else {
		if (board.bEnPass[0].oldSq == square) moves.push_back(board.bEnPass[0].newSq);
		if (board.bEnPass[1].oldSq == square) moves.push_back(board.bEnPass[1].newSq);
	}
}

void Engine::calcKnightMoves(char square, std::vector<char>& moves, bool white) {
	if (square % 8 != 7 && square > 15 && !(board.b[square - 15] > (white?6:0) && board.b[square - 15] <= (white ? 12 : 6))) moves.push_back(square - 15);
	if (square % 8 < 6 && square > 6 && !(board.b[square - 6] > (white ? 6 : 0) && board.b[square - 6] <= (white ? 12 : 6))) moves.push_back(square - 6);
	if (square % 8 != 0 && square > 16 && !(board.b[square - 17] > (white ? 6 : 0) && board.b[square - 17] <= (white ? 12 : 6))) moves.push_back(square - 17);
	if (square % 8 > 1 && square > 8 && !(board.b[square - 10] > (white ? 6 : 0) && board.b[square - 10] <= (white ? 12 : 6))) moves.push_back(square - 10);
	if (square % 8 != 7 && square < 46 && !(board.b[square + 17] > (white ? 6 : 0) && board.b[square + 17] <= (white ? 12 : 6))) moves.push_back(square + 17);
	if (square % 8 < 6 && square < 53 && !(board.b[square + 10] > (white ? 6 : 0) && board.b[square + 10] <= (white ? 12 : 6))) moves.push_back(square + 10);
	if (square % 8 != 0 && square < 48 && !(board.b[square + 15] > (white ? 6 : 0) && board.b[square + 15] <= (white ? 12 : 6))) moves.push_back(square + 15);
	if (square % 8 > 1 && square < 57 && !(board.b[square + 6] > (white ? 6 :0) && board.b[square + 6] <= (white ? 12 : 6))) moves.push_back(square + 6);
}

void Engine::calcQueenMoves(char square, std::vector<char>& moves, bool white) {
	calcRookMoves(square, moves, white);
	calcBishMoves(square, moves, white);
}

void Engine::calcKingMoves(char square, std::vector<char>& moves, bool white) {
	int x = square % 8;
	int y = square / 8;
	if (x != 7 && !(board.b[square + 1] > (white ? 6 : 0) && board.b[square + 1] <= (white ? 12 : 6))) moves.push_back(square + 1);
	if (x != 0 && !(board.b[square - 1] > (white ? 6 : 0) && board.b[square - 1] <= (white ? 12 : 6))) moves.push_back(square - 1);
	if (y < 7 && !(board.b[square + 8] > (white ? 6 : 0) && board.b[square + 8] <= (white ? 12 : 6))) moves.push_back(square + 8);
	if (y > 0 && !(board.b[square - 8] > (white ? 6 : 0) && board.b[square - 8] <= (white ? 12 : 6))) moves.push_back(square - 8);
	if (x != 0 && y != 0 && !(board.b[square - 9] > (white ? 6 : 0) && board.b[square - 9] <= (white ? 12 : 6))) moves.push_back(square - 9);
	if (x != 7 && y != 0 && !(board.b[square - 7] > (white ? 6 : 0) && board.b[square - 7] <= (white ? 12 : 6))) moves.push_back(square - 7);
	if (x != 0 && y != 7 && !(board.b[square + 7] > (white ? 6 : 0) && board.b[square + 7] <= (white ? 12 : 6))) moves.push_back(square + 7);
	if (x != 7 && y != 7 && !(board.b[square + 9] > (white ? 6 : 0) && board.b[square + 9] <= (white ? 12 : 6))) moves.push_back(square + 9);
}

void Engine::addCastlingMoves(char square, std::vector<char>& moves) {
	if (board.b[square] == WHITE_KING) {
		if (canCastle(true, false)) moves.push_back(58);
		if (canCastle(true, true)) moves.push_back(62);
	}
	else {
		if (canCastle(false, true)) moves.push_back(6);
		if (canCastle(false, false)) moves.push_back(2);
	}
}

void Engine::calcAvailMoves(char square, std::vector<char>& moves) {
	++findMovesCounter;
	switch (board.b[square]) {
	case EMPTY:
		break;
	case BLACK_PAWN:
		calcPawnMoves(square, moves, false);
		break;
	case WHITE_PAWN:
		calcPawnMoves(square, moves, true);
		break;
	case BLACK_KNIGHT:
		calcKnightMoves(square, moves, false);
		break;
	case WHITE_KNIGHT:
		calcKnightMoves(square, moves, true);
		break;
	case BLACK_BISHOP:
		calcBishMoves(square, moves, false);
		break;
	case WHITE_BISHOP:
		calcBishMoves(square, moves, true);
		break;
	case BLACK_ROOK:
		calcRookMoves(square, moves, false);
		break;
	case WHITE_ROOK:
		calcRookMoves(square, moves, true);
		break;
	case BLACK_QUEEN:
		calcQueenMoves(square, moves, false);
		break;
	case WHITE_QUEEN:
		calcQueenMoves(square, moves, true);
		break;
	case BLACK_KING:
		calcKingMoves(square, moves, false);
		break;
	case WHITE_KING:
		calcKingMoves(square, moves, true);
		break;
	}
}

void Engine::showAvailMoves() {
	for (int i = 0; i < avail_moves.size(); i++) {
		highlighted_squares.push_back({ (avail_moves[i] % 8) * SQUARE_WIDTH, (int)(avail_moves[i] / 8) * SQUARE_HEIGHT, SQUARE_WIDTH, SQUARE_HEIGHT });
	}
}

bool Engine::detectCheck(bool turn) {
	++findCheckCounter;
	std::vector<char> avail_moves;
	for (int i = 0; i < 64; i++) {
		if (board.b[i] >= (7 - 6 * board.turn) && board.b[i] <= (12 - 6 * board.turn)) {
			calcAvailMoves(i, avail_moves);
			for (int j = 0; j < avail_moves.size(); j++) {
				if (avail_moves[j] == (turn ? board.wKingPnt : board.bKingPnt)) {
					//std::cout << "King is in check at i = " << i << ", j = " << avail_moves[j] << '\n';
					return true;
				}
			}
			avail_moves.clear();
		}
	}
	return false;
}

bool Engine::detectCheck(bool turn, Move move) {
	bool detectedCheck;
	char piece = board.b[move.newSq];
	movePiece(move);
	detectedCheck = detectCheck(turn);
	movePiece({ move.newSq, move.oldSq });
	board.b[move.newSq] = piece;
	return detectedCheck;
}

bool Engine::detectMate(bool turn) {
	std::vector<Move> moves;
	findAvailableMoves(turn, moves);
	//std::cout << "Found " << moves.size() << " valid moves\n";
	if (moves.size() == 0) {
		return true;
	}
	return false;
}

bool Engine::gameOver() {
	return quit;
}

bool Engine::getTurn() {
	return board.turn;
}

char* Engine::getBoard() {
	return board.b;
}

void Engine::switchTurn() {
	board.turn = !board.turn;
}

void Engine::findAvailableMoves(bool turn, std::vector<Move>& moves) {
	std::vector<char> avail_moves;
	for (char i = 0; i < 64; i++) {
		if (board.b[i] >= (1 + 6 * turn) && board.b[i] <= (6 + 6 * turn)) {
			calcAvailMoves(i, avail_moves);
			for (int j = 0; j < avail_moves.size(); j++) {
				if (!detectCheck(turn, { i, avail_moves[j] })) {
					moves.push_back({ i, avail_moves[j] });
				}
			}
			avail_moves.clear();
		}
	}
}

void Engine::findAvailableMoves(bool turn, std::vector<Move> preMoves, std::vector<Move>& moves) {
	std::vector<char> storage;
	if (preMoves.size() >= 1) {
		for (int i = 0; i < preMoves.size(); i++) {
			storage.push_back(board.b[preMoves[i].oldSq]);
			storage.push_back(board.b[preMoves[i].newSq]);
			movePiece(preMoves[i]);
		}
	}
	findAvailableMoves(turn, moves);
	if (preMoves.size() >= 1) {
		for (int i = preMoves.size() - 1; i >= 0; i--) {
			board.b[preMoves[i].oldSq] = storage[2 * i];
			board.b[preMoves[i].newSq] = storage[2 * i + 1];
		}
	}
}

void Engine::pawnPromotion(char square, char choice) { // 1 = queen, 2 = bishop, 3 = knight
	choice *= ((board.b[square] == WHITE_PAWN) ? 1 : -1); // Multiplies choice by 1 if we're promoting a white pawn, otherwise by -1
	switch (choice) {
	case 1:
		board.b[square] = WHITE_QUEEN;
		break;
	case 2:
		board.b[square] = WHITE_BISHOP;
		break;
	case 3:
		board.b[square] = WHITE_KNIGHT;
		break;
	case -1:
		board.b[square] = BLACK_QUEEN;
		break;
	case -2:
		board.b[square] = BLACK_BISHOP;
		break;
	case -3:
		board.b[square] = BLACK_KNIGHT;
		break;
	}
	detectCheck((board.b[square] > BLACK_KING));
}

bool Engine::canCastle(bool turn, bool side) {
	if (detectCheck(board.turn)) return false;
	if (board.turn) {
		if (board.wCanCstl[side?1:0] == false) return false;
		if (side) { // White kingside
			if (board.b[62] != EMPTY || board.b[61] != EMPTY) return false;
			if (detectCheck(turn, { 60, 61 }) || detectCheck(turn, { 60, 62 })) return false;
		}
		else { // White queenside
			if (board.b[57] != EMPTY || board.b[58] != EMPTY || board.b[59] != EMPTY) return false;
			if (detectCheck(turn, { 60, 59 }) || detectCheck(turn, { 60, 58 })) return false;
		}
	}
	else {
		if (board.bCanCstl[side ? 1 : 0] == false) return false;
		if (side) { // Black kingside
			if (board.b[5] != EMPTY || board.b[6] != EMPTY) return false;
			if (detectCheck(turn, { 4, 5 }) || detectCheck(turn, { 4, 6 })) return false;
		}
		else { // Black queenside
			if (board.b[1] != EMPTY || board.b[2] != EMPTY || board.b[3] != EMPTY) return false;
			if (detectCheck(turn, { 4, 3 }) || detectCheck(turn, { 4, 2 })) return false;
		}
	}
	return true;
}

void Engine::makePlayerMove(Move move, char moveType) {
	movePiece(move);

	// En passant rule
	if (board.b[move.newSq] == BLACK_PAWN && move.newSq == (move.oldSq + 16)) {
		if (board.b[move.oldSq + 15] == WHITE_PAWN) board.wEnPass[0] = { move.oldSq + 15, move.oldSq + 8 };
		if (board.b[move.oldSq + 17] == WHITE_PAWN) board.wEnPass[1] = { move.oldSq + 17, move.oldSq + 8 };
	}
	else if (board.b[move.newSq] == WHITE_PAWN && move.newSq == (move.oldSq - 16)) {
		if (board.b[move.oldSq - 15] == BLACK_PAWN) board.bEnPass[0] = { move.oldSq - 15, move.oldSq - 8 };
		if (board.b[move.oldSq - 17] == BLACK_PAWN) board.bEnPass[1] = { move.oldSq - 17, move.oldSq - 8 };
	}

	if (board.b[move.newSq] > BLACK_KING) {
		board.wEnPass[0] = board.wEnPass[1] = {};
	}
	else {
		board.bEnPass[0] = board.bEnPass[1] = {};
	}

	if (board.b[move.newSq] == BLACK_PAWN && ((move.newSq - move.oldSq) % 8 != 0)) { // If moving en passant
		board.b[move.newSq - 8] = EMPTY;
	}
	if (board.b[move.newSq] == WHITE_PAWN && ((move.oldSq - move.newSq) % 8 != 0)) { // If moving en passant
		board.b[move.newSq + 8] = EMPTY;
	}

	// Check for pawn promotion
	if (board.b[move.newSq] == WHITE_PAWN && (int)(move.newSq / 8) == 0) openPromotionWindow(move.newSq);
	if (board.b[move.newSq] == BLACK_PAWN && (int)(move.newSq / 8) == 7) openPromotionWindow(move.newSq - 24);

	// Check whether it invalidates castling
	if (board.b[move.newSq] == WHITE_KING) {
		board.wCanCstl[0] = board.wCanCstl[1] = false;
	}
	else if (board.b[move.newSq] == WHITE_ROOK) {
		if (move.oldSq == 63) board.wCanCstl[1] = false;
		if (move.oldSq == 55) board.wCanCstl[0] = false;
	}

	if (board.b[move.newSq] == BLACK_KING) {
		board.bCanCstl[0] = board.bCanCstl[1] = false;
	}
	else if (board.b[move.newSq] == BLACK_ROOK) {
		if (move.oldSq == 0) board.bCanCstl[0] = false;
		if (move.oldSq == 7) board.bCanCstl[1] = false;
	}

	// Check whether we're castling
	if ((board.b[move.newSq] == WHITE_KING || board.b[move.newSq] == BLACK_KING) && abs(move.newSq - move.oldSq) == 2) {
		if (move.newSq == 2) movePiece({ 0, 3 });
		if (move.newSq == 6) movePiece({ 7, 5 });
		if (move.newSq == 62) movePiece({ 63, 61 });
		if (move.newSq == 58) movePiece({ 56, 59 });
	}

	switchTurn();
	if (board.turn) board.wChk = detectCheck(board.turn);
	else board.bChk = detectCheck(board.turn);
	if (detectMate(board.turn)) {
		if (board.turn ? board.wChk : board.bChk) {
			std::cout << "Mate\n";
		}
		else {
			std::cout << "Stalemate\n";
		}
	}
}