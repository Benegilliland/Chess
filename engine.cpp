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
	board.chk[0] = board.chk[1] = false;
	prevMove[0] = prevMove[1] = {};
	board.wEnPass[0] = board.bEnPass[0] = {};
	board.wEnPass[1] = board.bEnPass[1] = {};
	board.enPassMove = false;
	promotingPawn = false;
	board.wCanCstl[0] = board.wCanCstl[1] = board.bCanCstl[0] = board.bCanCstl[1] = true;
	calcAvailMoves(board);
}

void Engine::movePiece(Move move, Board& board, int moveType) {
	board.b[move.newSq] = board.b[move.oldSq];
	board.b[move.oldSq] = EMPTY;
	if (board.b[move.newSq] == WHITE_KING) board.wKingPnt = move.newSq;
	else if (board.b[move.newSq] == BLACK_KING) board.bKingPnt = move.newSq;
	detectCheck(board);
	calcAvailMoves(board);
}

bool Engine::validateMove(Move move, Board& board) {
	bool canMove = false;
	bool white = board.b[move.oldSq] > BLACK_KING;
	for (Move& i : board.moves) {
		if (i.oldSq == move.oldSq && i.newSq == move.newSq) {
			std::cout << "ASD\n";
			canMove = true;
			break;
		}
	}
	if (canMove) {
		canMove = !detectCheck(board.turn, move, board);
	}
	return canMove;
}

void Engine::calcRookMoves(char square, bool white, Board& board) {
		int x = square % 8;
		int y = (int)(square / 8);
		for (char i = 1; i < 8 - x; i++) {
			if (board.b[square + i] != EMPTY) {
				if ((white && board.b[square + i] <= BLACK_KING) || (!white && board.b[square + i] > BLACK_KING)) board.moves.push_back( { square, square + i });
				break;
			}
			else {
				board.moves.push_back( { square, square + i });
			}
		}
		for (char i = 1; i < (x + 1); i++) {
			if (board.b[square - i] != EMPTY) {
				if ((white && board.b[square - i] <= BLACK_KING) || (!white && board.b[square - i] > BLACK_KING)) board.moves.push_back({ square, square - i });
				break;
			}
			else {
				board.moves.push_back({ square, square - i });
			}
		}
		for (char i = 1; i < 8 - y; i++) {
			if (board.b[square + 8*i] != EMPTY) {
				if ((white && board.b[square + 8 * i] <= BLACK_KING) || (!white && board.b[square + 8 * i] > BLACK_KING)) board.moves.push_back({ square, square + 8 * i });
				break;
			}
			else {
				board.moves.push_back({ square, square + 8 * i });
			}
		}
		for (char i = 1; i < (y + 1); i++) {
			if (board.b[square - 8*i] != EMPTY) {
				if ((white && board.b[square - 8 * i] <= BLACK_KING) || (!white && board.b[square - 8 * i] > BLACK_KING)) board.moves.push_back({ square, square - 8 * i });
				break;
			}
			else {
				board.moves.push_back({ square, square - 8 * i });
			}
		}
}

void Engine::calcBishMoves(char square, bool white, Board& board) {
	int x = square % 8;
		int y = (int)(square / 8);
			// Up-left
			for (char i = 1; i < std::min(x,y)+1; i++) {
				if (board.b[square - 9 * i] != EMPTY) {
					if ((white && board.b[square - 9 * i] <= BLACK_KING) || (!white && (board.b[square - 9 * i] == EMPTY || board.b[square - 9 * i] > BLACK_KING))) board.moves.push_back({ square, square - 9 * i });
					break;
				}
				else {
					board.moves.push_back({ square, square - 9 * i });
				}
			}
			// Up-right
			for (char i = 1; i < std::min((7-x), y) + 1; i++) {
				if (board.b[square - 7 * i] != EMPTY) {
					if ((white && board.b[square - 7 * i] <= BLACK_KING) || (!white && (board.b[square - 7 * i] == EMPTY || board.b[square - 7 * i] > BLACK_KING))) board.moves.push_back({ square, square - 7 * i });
					break;
				}
				else {
					board.moves.push_back({ square, square - 7 * i });
				}
			}
			// Down left
			for (char i = 1; i < std::min(x, (7-y)) + 1; i++) {
				if (board.b[square + 7 * i] > EMPTY) {
					if ((white && board.b[square + 7 * i] <= BLACK_KING) || (!white && (board.b[square + 7 * i] == EMPTY || board.b[square + 7 * i] > BLACK_KING))) board.moves.push_back({ square, square + 7 * i });
					break;
				}
				else {
					board.moves.push_back({ square, square + 7 * i });
				}
			}
			// Down right
			for (char i = 1; i < std::min((7-x), (7 - y)) + 1; i++) {
				if (board.b[square + 9 * i] > EMPTY) {
					if ((white && board.b[square + 9 * i] <= BLACK_KING) || (!white && (board.b[square + 9 * i] == EMPTY || board.b[square + 9 * i] > BLACK_KING))) board.moves.push_back({ square, square + 9 * i });
					break;
				}
				else {
					board.moves.push_back({ square, square + 9 * i });
				}
			}
}

void Engine::calcPawnMoves(char square, bool white, Board& board) {
	if (board.b[square + (white ? -8 : 8)] == EMPTY) board.moves.push_back({ square, square + (white ? -8 : 8) });
	if ((int)(square / 8) == (white ? 6 : 1) && board.b[square + (white ? -8 : 8)] == EMPTY && board.b[square + (white ? -16 : 16)] == EMPTY) board.moves.push_back({ square, square + (white ? -16 : 16) });
	if ((square % 8 != 7) && (board.b[square + (white ? -7 : 9)] <= (white ? 6 : 12) && board.b[square + (white ? -7 : 9)] > (white ? 0 : 6))) board.moves.push_back({ square, square + (white ? -7 : 9) });
	if ((square % 8 != 0) && (board.b[square + (white ? -9 : 7)] <= (white ? 6 : 12) && board.b[square + (white ? -9 : 7)] > (white ? 0 : 6))) board.moves.push_back({ square, square + (white ? -9 : 7) });
	if (white) { // En passant moves
		if (board.wEnPass[0].oldSq == square) board.moves.push_back({ square, board.wEnPass[0].newSq });
		if (board.wEnPass[1].oldSq == square) board.moves.push_back({ square, board.wEnPass[1].newSq });
	}
	else {
		if (board.bEnPass[0].oldSq == square) board.moves.push_back({ square, board.bEnPass[0].newSq });
		if (board.bEnPass[1].oldSq == square) board.moves.push_back({ square, board.bEnPass[1].newSq });
	}
}

void Engine::calcKnightMoves(char square, bool white, Board& board) {
	if (square % 8 != 7 && square > 15 && !(board.b[square - 15] > (white ? 6 : 0) && board.b[square - 15] <= (white ? 12 : 6))) board.moves.push_back({ square, square - 15 });
	if (square % 8 < 6 && square > 6 && !(board.b[square - 6] > (white ? 6 : 0) && board.b[square - 6] <= (white ? 12 : 6))) board.moves.push_back({ square, square - 6 });
	if (square % 8 != 0 && square > 16 && !(board.b[square - 17] > (white ? 6 : 0) && board.b[square - 17] <= (white ? 12 : 6))) board.moves.push_back({ square, square - 17 });
	if (square % 8 > 1 && square > 8 && !(board.b[square - 10] > (white ? 6 : 0) && board.b[square - 10] <= (white ? 12 : 6))) board.moves.push_back({ square, square - 10 });
	if (square % 8 != 7 && square < 46 && !(board.b[square + 17] > (white ? 6 : 0) && board.b[square + 17] <= (white ? 12 : 6))) board.moves.push_back({ square, square + 17 });
	if (square % 8 < 6 && square < 53 && !(board.b[square + 10] > (white ? 6 : 0) && board.b[square + 10] <= (white ? 12 : 6))) board.moves.push_back({ square, square + 10 });
	if (square % 8 != 0 && square < 48 && !(board.b[square + 15] > (white ? 6 : 0) && board.b[square + 15] <= (white ? 12 : 6))) board.moves.push_back({ square, square + 15 });
	if (square % 8 > 1 && square < 57 && !(board.b[square + 6] > (white ? 6 : 0) && board.b[square + 6] <= (white ? 12 : 6))) board.moves.push_back({ square, square + 6 });
}

void Engine::calcQueenMoves(char square, bool white, Board& board) {
	calcRookMoves(square, white, board);
	calcBishMoves(square, white, board);
}

void Engine::calcKingMoves(char square, bool white, Board& board) {
	int x = square % 8;
	int y = square / 8;
	if (x != 7 && !(board.b[square + 1] > (white ? 6 : 0) && board.b[square + 1] <= (white ? 12 : 6))) board.moves.push_back({ square, square + 1 });
	if (x != 0 && !(board.b[square - 1] > (white ? 6 : 0) && board.b[square - 1] <= (white ? 12 : 6))) board.moves.push_back({ square, square - 1 });
	if (y < 7 && !(board.b[square + 8] > (white ? 6 : 0) && board.b[square + 8] <= (white ? 12 : 6))) board.moves.push_back({ square, square + 8 });
	if (y > 0 && !(board.b[square - 8] > (white ? 6 : 0) && board.b[square - 8] <= (white ? 12 : 6))) board.moves.push_back({ square, square - 8 });
	if (x != 0 && y != 0 && !(board.b[square - 9] > (white ? 6 : 0) && board.b[square - 9] <= (white ? 12 : 6))) board.moves.push_back({ square, square - 9 });
	if (x != 7 && y != 0 && !(board.b[square - 7] > (white ? 6 : 0) && board.b[square - 7] <= (white ? 12 : 6))) board.moves.push_back({ square, square - 7 });
	if (x != 0 && y != 7 && !(board.b[square + 7] > (white ? 6 : 0) && board.b[square + 7] <= (white ? 12 : 6))) board.moves.push_back({ square, square + 7 });
	if (x != 7 && y != 7 && !(board.b[square + 9] > (white ? 6 : 0) && board.b[square + 9] <= (white ? 12 : 6))) board.moves.push_back({ square, square + 9 });
}

void Engine::addCastlingMoves(char square, Board& board) {
	if (board.b[square] == WHITE_KING) {
		if (canCastle(true, false)) board.moves.push_back({ square, 58 });
		if (canCastle(true, true)) board.moves.push_back({ square, 62 });
	}
	else {
		if (canCastle(false, true)) board.moves.push_back({ square, 6 });
		if (canCastle(false, false)) board.moves.push_back({ square, 2 });
	}
}

void Engine::calcAvailMoves(Board& board) {
	++findMovesCounter;
	board.moves.clear();
	for (int i = 0; i < 64; i++) {
		switch (board.b[i]) {
		case EMPTY:
			break;
		case BLACK_PAWN:
			calcPawnMoves(i, false, board);
			break;
		case WHITE_PAWN:
			calcPawnMoves(i, true, board);
			break;
		case BLACK_KNIGHT:
			calcKnightMoves(i, false, board);
			break;
		case WHITE_KNIGHT:
			calcKnightMoves(i, true, board);
			break;
		case BLACK_BISHOP:
			calcBishMoves(i, false, board);
			break;
		case WHITE_BISHOP:
			calcBishMoves(i, true, board);
			break;
		case BLACK_ROOK:
			calcRookMoves(i, false, board);
			break;
		case WHITE_ROOK:
			calcRookMoves(i, true, board);
			break;
		case BLACK_QUEEN:
			calcQueenMoves(i, false, board);
			break;
		case WHITE_QUEEN:
			calcQueenMoves(i, true, board);
			break;
		case BLACK_KING:
			calcKingMoves(i, false, board);
			break;
		case WHITE_KING:
			calcKingMoves(i, true, board);
			break;
		}
	}
}

void Engine::detectCheck(Board& board) {
	++findCheckCounter;
	board.chk[1] = board.chk[0] = false;
	for (Move& i : board.moves) {
		if (board.b[i.oldSq] > 0 && board.b[i.oldSq] <= 6 && i.newSq == board.wKingPnt) {
			board.chk[1] = true;
		}
		else if (board.b[i.oldSq] > 6 && board.b[i.oldSq] <= 12 && i.newSq == board.bKingPnt) {
			board.chk[0] = true;
		}
	}
}

bool Engine::detectCheck(bool turn, Move move, Board& board) {
	Board newBoard(board);
	movePiece(move, newBoard);
	//std::cout << "chk[0] = " << newBoard.chk[0] << " chk[1] = " << newBoard.chk[1] << '\n';
	return newBoard.chk[turn];
}

bool Engine::detectMate(bool turn, Board& board) {
	std::vector<Move> moves;
	findAvailableMoves(turn, moves, board);
	std::cout << "Found " << moves.size() << " valid moves\n";
	if (moves.size() == 0) {
		return true;
	}
	return false;
}

void Engine::findAvailableMoves(bool turn, std::vector<Move>& moves, Board& board) {
	//std::cout << "asd\n";
	for (Move& i : board.moves) {
		if (board.b[i.oldSq] > (turn ? 6 : 0) && board.b[i.oldSq] < (turn ? 13 : 7) && !detectCheck(turn, i, board)) {
			moves.push_back(i);
			//std::cout << "i = " << (int)i.oldSq << " j = " << (int)i.newSq << '\n';
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
	detectCheck(board);
}

bool Engine::canCastle(bool turn, bool side) {
	if (board.chk[board.turn]) return false;
	if (board.turn) {
		if (board.wCanCstl[side?1:0] == false) return false;
		if (side) { // White kingside
			if (board.b[62] != EMPTY || board.b[61] != EMPTY) return false;
			if (detectCheck(turn, { 60, 61 }, board) || detectCheck(turn, { 60, 62 }, board)) return false;
		}
		else { // White queenside
			if (board.b[57] != EMPTY || board.b[58] != EMPTY || board.b[59] != EMPTY) return false;
			if (detectCheck(turn, { 60, 59 }, board) || detectCheck(turn, { 60, 58 }, board)) return false;
		}
	}
	else {
		if (board.bCanCstl[side ? 1 : 0] == false) return false;
		if (side) { // Black kingside
			if (board.b[5] != EMPTY || board.b[6] != EMPTY) return false;
			if (detectCheck(turn, { 4, 5 }, board) || detectCheck(turn, { 4, 6 }, board)) return false;
		}
		else { // Black queenside
			if (board.b[1] != EMPTY || board.b[2] != EMPTY || board.b[3] != EMPTY) return false;
			if (detectCheck(turn, { 4, 3 }, board) || detectCheck(turn, { 4, 2 }, board)) return false;
		}
	}
	return true;
}

void Engine::makePlayerMove(Move move, char moveType) {
	movePiece(move, board);

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
		if (move.newSq == 2) movePiece({ 0, 3 }, board);
		if (move.newSq == 6) movePiece({ 7, 5 }, board);
		if (move.newSq == 62) movePiece({ 63, 61 }, board);
		if (move.newSq == 58) movePiece({ 56, 59 }, board);
	}

	switchTurn();
	if (detectMate(board.turn, board)) {
		if (board.turn ? board.chk[1] : board.chk[0]) {
			std::cout << "Mate\n";
		}
		else {
			std::cout << "Stalemate\n";
		}
	}
}

bool* Engine::getCheck() {
	return board.chk;
}

bool Engine::gameOver() {
	return quit;
}

bool Engine::getTurn() {
	return board.turn;
}

Board* Engine::getBoard() {
	return &board;
}

void Engine::switchTurn() {
	board.turn = !board.turn;
}
