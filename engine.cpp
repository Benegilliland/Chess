/*
To add:
- add better highlighting, with different colours
- improve code
- significantly optimise code
	the AI calls each of these functions millions of times per game, so even a fraction of a second will make a huge difference

 - Validate whether move is legal (e.g. doesn't put you in check) before giving it to AI , thus hugely saving on computation time

 - Recode the board to use my own 4-bit (or 5-bit) data type, with one bit representing black or white
 - This gives 16 possible values when we need 13. We can use the other 3 in clever ways e.g. storing an en passant pawn, castling or pawn promotion etc.
Bugs:
*/

#include "engine.h"

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
	board[56] = board[63] = white_rook;
	board[57] = board[62] = white_knight;
	board[58] = board[61] = white_bishop;
	board[59] = white_queen;
	board[60] = white_king;
	turn = true;
	wChk = bChk = false;
	prevMove[0] = prevMove[1] = {};
	wEnPass[0] = bEnPass[0] = {};
	wEnPass[1] = bEnPass[1] = {};
	enPassMove = false;
	promotingPawn = false;
	wCanCstl[0] = wCanCstl[1] = bCanCstl[0] = bCanCstl[1] = true;
}

void Engine::movePiece(int oldSquare, int newSquare) {
	board[newSquare] = board[oldSquare];
	board[oldSquare] = empty_square;
}

void Engine::unmovePiece(int oldSquare, int newSquare, Piece p) {
	board[newSquare] = board[oldSquare];
	board[oldSquare] = p;
}

bool Engine::validateMove(int oldSquare, int newSquare) {
	bool canMove = false;
	bool white = board[oldSquare] > black_king;
	for (int i = 0; i < avail_moves.size(); i++) {
		if (avail_moves[i] == newSquare) {
			canMove = true;
			break;
		}
	}
	if (canMove) {
		canMove = !detectCheck(turn, { oldSquare, newSquare });
	}
	return canMove;
}

void Engine::calcRookMoves(int square, std::vector<int>& moves, bool white) {
		int x = square % 8;
		int y = (int)(square / 8);
		for (int i = 1; i < 8 - x; i++) {
			if (board[square + i] != empty_square) {
				if ((white && board[square + i] <= black_king) || (!white && board[square + i] > black_king)) moves.push_back(square + i);
				break;
			}
			else {
				moves.push_back(square + i);
			}
		}
		for (int i = 1; i < (x + 1); i++) {
			if (board[square - i] != empty_square) {
				if ((white && board[square - i] <= black_king) || (!white && board[square - i] > black_king)) moves.push_back(square - i);
				break;
			}
			else {
				moves.push_back(square - i);
			}
		}
		for (int i = 1; i < 8 - y; i++) {
			if (board[square + 8*i] != empty_square) {
				if ((white && board[square + 8*i] <= black_king) || (!white && board[square + 8*i] > black_king)) moves.push_back(square + 8*i);
				break;
			}
			else {
				moves.push_back(square + 8*i);
			}
		}
		for (int i = 1; i < (y + 1); i++) {
			if (board[square - 8*i] != empty_square) {
				if ((white && board[square - 8*i] <= black_king) || (!white && board[square - 8*i] > black_king)) moves.push_back(square - 8*i);
				break;
			}
			else {
				moves.push_back(square - 8*i);
			}
		}
}

void Engine::calcBishMoves(int square, std::vector<int>& moves, bool white) {
		int x = square % 8;
		int y = (int)(square / 8);
			// Up-left
			for (int i = 1; i < std::min(x,y)+1; i++) {
				if (board[square - 9 * i] != empty_square) {
					if ((white && board[square - 9 * i] <= black_king) || (!white && (board[square - 9*i] == empty_square || board[square - 9*i] > black_king))) moves.push_back(square - 9 * i);
					break;
				}
				else {
					moves.push_back(square - 9 * i);
				}
			}
			// Up-right
			for (int i = 1; i < std::min((7-x), y) + 1; i++) {
				if (board[square - 7 * i] != empty_square) {
					if ((white && board[square - 7 * i] <= black_king) || (!white && (board[square - 7 * i] == empty_square || board[square - 7 * i] > black_king))) moves.push_back(square - 7 * i);
					break;
				}
				else {
					moves.push_back(square - 7 * i);
				}
			}
			// Down left
			for (int i = 1; i < std::min(x, (7-y)) + 1; i++) {
				if (board[square + 7 * i] > empty_square) {
					if ((white && board[square + 7 * i] <= black_king) || (!white && (board[square + 7 * i] == empty_square || board[square + 7 * i] > black_king))) moves.push_back(square + 7 * i);
					break;
				}
				else {
					moves.push_back(square + 7 * i);
				}
			}
			// Down right
			for (int i = 1; i < std::min((7-x), (7 - y)) + 1; i++) {
				if (board[square + 9 * i] > empty_square) {
					if ((white && board[square + 9 * i] <= black_king) || (!white && (board[square + 9 * i] == empty_square || board[square + 9 * i] > black_king))) moves.push_back(square + 9 * i);
					break;
				}
				else {
					moves.push_back(square + 9 * i);
				}
			}
}

void Engine::calcPawnMoves(int square, std::vector<int>& moves, bool white) {
	if (board[square + (white?-8:8)] == empty_square) moves.push_back(square + (white?-8:8));
	if ((int)(square / 8) == (white?6:1) && board[square + (white?-8:8)] == empty_square && board[square + (white ? -16 : 16)] == empty_square) moves.push_back(square + (white ? -16 : 16));
	if ((square % 8 != 7) && (board[square + (white ? -7 : 9)] <= (white?6:12) && board[square + (white ? -7 : 9)] > (white?0:6))) moves.push_back(square + (white ? -7 : 9));
	if ((square % 8 != 0) && (board[square + (white ? -9 : 7)] <= (white ? 6 : 12) && board[square + (white ? -9 : 7)] > (white ? 0 : 6))) moves.push_back(square + (white ? -9: 7));
	if (white) { // En passant moves
		if (wEnPass[0].sq[0] == square) moves.push_back(wEnPass[0].sq[1]);
		if (wEnPass[1].sq[0] == square) moves.push_back(wEnPass[1].sq[1]);
	}
	else {
		if (bEnPass[0].sq[0] == square) moves.push_back(bEnPass[0].sq[1]);
		if (bEnPass[1].sq[0] == square) moves.push_back(bEnPass[1].sq[1]);
	}
}

void Engine::calcKnightMoves(int square, std::vector<int>& moves, bool white) {
	if (square % 8 != 7 && square > 15 && !(board[square - 15] > (white?6:0) && board[square - 15] <= (white ? 12 : 6))) moves.push_back(square - 15);
	if (square % 8 < 6 && square > 6 && !(board[square - 6] > (white ? 6 : 0) && board[square - 6] <= (white ? 12 : 6))) moves.push_back(square - 6);
	if (square % 8 != 0 && square > 16 && !(board[square - 17] > (white ? 6 : 0) && board[square - 17] <= (white ? 12 : 6))) moves.push_back(square - 17);
	if (square % 8 > 1 && square > 8 && !(board[square - 10] > (white ? 6 : 0) && board[square - 10] <= (white ? 12 : 6))) moves.push_back(square - 10);
	if (square % 8 != 7 && square < 46 && !(board[square + 17] > (white ? 6 : 0) && board[square + 17] <= (white ? 12 : 6))) moves.push_back(square + 17);
	if (square % 8 < 6 && square < 53 && !(board[square + 10] > (white ? 6 : 0) && board[square + 10] <= (white ? 12 : 6))) moves.push_back(square + 10);
	if (square % 8 != 0 && square < 48 && !(board[square + 15] > (white ? 6 : 0) && board[square + 15] <= (white ? 12 : 6))) moves.push_back(square + 15);
	if (square % 8 > 1 && square < 57 && !(board[square + 6] > (white ? 6 :0) && board[square + 6] <= (white ? 12 : 6))) moves.push_back(square + 6);
}

void Engine::calcQueenMoves(int square, std::vector<int>& moves, bool white) {
	calcRookMoves(square, moves, white);
	calcBishMoves(square, moves, white);
}

void Engine::calcKingMoves(int square, std::vector<int>& moves, bool white) {
	int x = square % 8;
	int y = square / 8;
	if (x != 7 && !(board[square + 1] > (white ? 6 : 0) && board[square + 1] <= (white ? 12 : 6))) moves.push_back(square + 1);
	if (x != 0 && !(board[square - 1] > (white ? 6 : 0) && board[square - 1] <= (white ? 12 : 6))) moves.push_back(square - 1);
	if (y < 7 && !(board[square + 8] > (white ? 6 : 0) && board[square + 8] <= (white ? 12 : 6))) moves.push_back(square + 8);
	if (y > 0 && !(board[square - 8] > (white ? 6 : 0) && board[square - 8] <= (white ? 12 : 6))) moves.push_back(square - 8);
	if (x != 0 && y != 0 && !(board[square - 9] > (white ? 6 : 0) && board[square - 9] <= (white ? 12 : 6))) moves.push_back(square - 9);
	if (x != 7 && y != 0 && !(board[square - 7] > (white ? 6 : 0) && board[square - 7] <= (white ? 12 : 6))) moves.push_back(square - 7);
	if (x != 0 && y != 7 && !(board[square + 7] > (white ? 6 : 0) && board[square + 7] <= (white ? 12 : 6))) moves.push_back(square + 7);
	if (x != 7 && y != 7 && !(board[square + 9] > (white ? 6 : 0) && board[square + 9] <= (white ? 12 : 6))) moves.push_back(square + 9);
}

void Engine::addCastlingMoves(int square, std::vector<int>& moves) {
	if (board[square] == white_king) {
		if (canCastle(true, false)) moves.push_back(58);
		if (canCastle(true, true)) moves.push_back(62);
	}
	else {
		if (canCastle(false, true)) moves.push_back(6);
		if (canCastle(false, false)) moves.push_back(2);
	}
}

void Engine::calcAvailMoves(int square, std::vector<int>& moves) {
	switch (board[square]) {
	case empty_square:
		break;
	case black_pawn:
		calcPawnMoves(square, moves, false);
		break;
	case white_pawn:
		calcPawnMoves(square, moves, true);
		break;
	case black_knight:
		calcKnightMoves(square, moves, false);
		break;
	case white_knight:
		calcKnightMoves(square, moves, true);
		break;
	case black_bishop:
		calcBishMoves(square, moves, false);
		break;
	case white_bishop:
		calcBishMoves(square, moves, true);
		break;
	case black_rook:
		calcRookMoves(square, moves, false);
		break;
	case white_rook:
		calcRookMoves(square, moves, true);
		break;
	case black_queen:
		calcQueenMoves(square, moves, false);
		break;
	case white_queen:
		calcQueenMoves(square, moves, true);
		break;
	case black_king:
		calcKingMoves(square, moves, false);
		break;
	case white_king:
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
	std::vector<int> avail_moves;
	for (int i = 0; i < 64; i++) {
		if (board[i] >= (7 - 6 * turn) && board[i] <= (13 - 6 * turn)) {
			calcAvailMoves(i, avail_moves);
			for (int j = 0; j < avail_moves.size(); j++) {
				if (avail_moves[j] == findKing(turn, board)) {
					std::cout << "King is in check at i = " << i << ", j = " << avail_moves[j] << '\n';
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
	Piece piece = board[move.sq[1]];
	movePiece(move.sq[0], move.sq[1]);
	detectedCheck = detectCheck(turn);
	movePiece(move.sq[1], move.sq[0]);
	board[move.sq[1]] = piece;
	return detectedCheck;
}

void Engine::detectMate(bool turn) {
	std::vector<Move> moves;
	findAvailableMoves(turn, moves);
	int counter = 0;
	for (auto& i : moves) {
		if (!detectCheck(turn, i)) {
			counter++;
		}
	}
	std::cout << "Found " << counter <<  " valid moves\n";
	if (counter == 0) {
		if ((turn ? wChk : bChk)) {
			std::cout << "Checkmate on " << (turn ? "white" : "black") << '\n';
		}
		else {
			std::cout << "Stalemate\n";
		}
	}
}

bool Engine::gameOver() {
	return quit;
}

bool Engine::getTurn() {
	return turn;
}

Piece* Engine::getBoard() {
	return board;
}

void Engine::switchTurn() {
	turn = !turn;
}

int Engine::findKing(bool turn, Piece* board) { // Going back to original idea of king pointers would be much more efficient
	for (int i = 0; i < 64; i++) {
		if ((turn && board[i] == white_king) || (!turn && board[i] == black_king)) {
			return i;
		}
	}
	return -1;
}

void Engine::printBoard(Piece* board) {
	std::cout << '\n';
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			std::cout << (Piece)board[i * 8 + j] << "\t";
		}
		std::cout << '\n';
	}
}

void Engine::findAvailableMoves(bool turn, std::vector<Move>& moves) {
	std::vector<int> avail_moves;
	for (int i = 0; i < 64; i++) {
		if (board[i] >= (1 + 6 * turn) && board[i] <= (6 + 6 * turn)) {
			calcAvailMoves(i, avail_moves);
			for (int j = 0; j < avail_moves.size(); j++) {
				moves.push_back({ i, avail_moves[j] });
			}
			avail_moves.clear();
		}
	}
}

void Engine::findAvailableMoves(bool turn, std::vector<Move> preMoves, std::vector<Move>& moves) {
	std::vector<Piece> storage;
	if (preMoves.size() >= 1) {
		for (int i = 0; i < preMoves.size(); i++) {
			storage.push_back(board[preMoves[i].sq[0]]);
			storage.push_back(board[preMoves[i].sq[1]]);
			board[preMoves[i].sq[1]] = board[preMoves[i].sq[0]];
			board[preMoves[i].sq[0]] = empty_square;
		}
	}
	findAvailableMoves(turn, moves);
	if (preMoves.size() >= 1) {
		for (int i = preMoves.size() - 1; i >= 0; i--) {
			board[preMoves[i].sq[0]] = storage[2 * i];
			board[preMoves[i].sq[1]] = storage[2 * i + 1];
		}
	}
}

void Engine::pawnPromotion(int square, int choice) { // 1 = queen, 2 = bishop, 3 = knight
	choice *= ((board[square] == white_pawn) ? 1 : -1); // Multiplies choice by 1 if we're promoting a white pawn, otherwise by -1
	switch (choice) {
	case 1:
		board[square] = white_queen;
		break;
	case 2:
		board[square] = white_bishop;
		break;
	case 3:
		board[square] = white_knight;
		break;
	case -1:
		board[square] = black_queen;
		break;
	case -2:
		board[square] = black_bishop;
		break;
	case -3:
		board[square] = black_knight;
		break;
	}
	detectCheck((board[square] > black_king));
}

bool Engine::canCastle(bool turn, bool side) {
	// Check that the space between them is empty
	//std::cout << "asd\n";
	if (detectCheck(turn)) return false; // If player is in check
	//std::cout << "cancastle\n";
	if (turn) {
		if (wCanCstl[side?1:0] == false) return false;
		if (side) { // White kingside
			if (board[62] != empty_square || board[61] != empty_square) return false;
			if (detectCheck(turn, { 60, 61 }) || detectCheck(turn, { 60, 62 })) return false;
		}
		else { // White queenside
			if (board[57] != empty_square || board[58] != empty_square || board[59] != empty_square) return false;
			if (detectCheck(turn, { 60, 59 }) || detectCheck(turn, { 60, 58 })) return false;
		}
	}
	else {
		if (bCanCstl[side ? 1 : 0] == false) return false;
		if (side) { // Black kingside
			if (board[5] != empty_square || board[6] != empty_square) return false;
			if (detectCheck(turn, { 4, 5 }) || detectCheck(turn, { 4, 6 })) return false;
		}
		else {
			if (board[1] != empty_square || board[2] != empty_square || board[3] != empty_square) return false;
			if (detectCheck(turn, { 4, 3 }) || detectCheck(turn, { 4, 2 })) return false;
		}
	}
	return true;
}

void Engine::makePlayerMove(int oldSquare, int newSquare) {
	movePiece(oldSquare, newSquare);

	// En passant rule
	if (board[newSquare] == black_pawn && newSquare == (oldSquare + 16)) {
		if (board[oldSquare + 15] == white_pawn) wEnPass[0] = { oldSquare + 15, oldSquare + 8 };
		if (board[oldSquare + 17] == white_pawn) wEnPass[1] = { oldSquare + 17, oldSquare + 8 };
	}
	if (board[newSquare] == white_pawn && newSquare == (oldSquare - 16)) {
		if (board[oldSquare - 15] == black_pawn) bEnPass[0] = { oldSquare - 15, oldSquare - 8 };
		if (board[oldSquare - 17] == black_pawn) bEnPass[1] = { oldSquare - 17, oldSquare - 8 };
	}

	if (board[newSquare] > black_king) {
		wEnPass[0] = wEnPass[1] = {};
	}
	else {
		bEnPass[0] = bEnPass[1] = {};
	}

	if (board[newSquare] == black_pawn && ((newSquare - oldSquare) % 8 != 0)) { // If moving en passant
		board[newSquare - 8] = empty_square;
	}
	if (board[newSquare] == white_pawn && ((oldSquare - newSquare) % 8 != 0)) { // If moving en passant
		board[newSquare + 8] = empty_square;
	}

	// Check for pawn promotion
	if (board[newSquare] == white_pawn && (int)(newSquare / 8) == 0) openPromotionWindow(newSquare);
	if (board[newSquare] == black_pawn && (int)(newSquare / 8) == 7) openPromotionWindow(newSquare - 24);

	// Check whether it invalidates castling
	if (board[newSquare] == white_king) {
		wCanCstl[0] = wCanCstl[1] = false;
	}
	else if (board[newSquare] == white_rook) {
		if (oldSquare == 63) wCanCstl[1] = false;
		if (oldSquare == 55) wCanCstl[0] = false;
	}

	if (board[newSquare] == black_king) {
		bCanCstl[0] = bCanCstl[1] = false;
	}
	else if (board[newSquare] == black_rook) {
		if (oldSquare == 0) bCanCstl[0] = false;
		if (oldSquare == 7) bCanCstl[1] = false;
	}

	// Check whether we're castling
	if ((board[newSquare] == white_king || board[newSquare] == black_king) && abs(newSquare - oldSquare) == 2) {
		if (newSquare == 2) movePiece(0, 3);
		if (newSquare == 6) movePiece(7, 5);
		if (newSquare == 62) movePiece(63, 61);
		if (newSquare == 58) movePiece(56, 59);
	}

	switchTurn();
	if (turn) wChk = detectCheck(turn);
	else bChk = detectCheck(turn);
	detectMate(turn);
}