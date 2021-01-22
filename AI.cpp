#include "AI.h"

bool AI::shouldMove() {
	return (_engine->getTurn() == _side);
}

int AI::evaluateBoard() {
	Piece* board = _engine->getBoard();
	int score = 0;
	for (int i = 0; i < 64; i++) {
		score += pieceValue[board[i]];
	}
	return score;
}

int AI::evaluateBoard(Piece* board) {
	int score = 0;
	for (int i = 0; i < 64; i++) {
		score += pieceValue[board[i]];
	}
	if (_engine->detectCheck(white, board)) {
		std::cout << "Detected check on white king\n";
		score -= 50000;
	}
	if (_engine->detectCheck(black, board)) {
		std::cout << "Detected check on black king\n";
		score += 50000;
	}
	return score;
}

int AI::evaluateMove(int oldSq, int newSq) {
	Piece* board = _engine->getBoard();
	Piece board2[64];
	for (int i = 0; i < 64; i++) {
		board2[i] = board[i];
	}
	board2[newSq] = board2[oldSq];
	board2[oldSq] = empty_square;
	return evaluateBoard(board2);
}

void AI::doMove() {
	int move[2] = { 0, 0 };
	findMove(move);
	_engine->makeMove(move[0], move[1]);
	_engine->switchTurn();
}

void AI::findMove(int* move) {
	Piece* board = _engine->getBoard();
	std::vector<int> avail_moves;
	int curMove = 0;
	int bestMove = 200000 * (_side ? -2 : 1);
	int bestI = 0;
	int bestJ = 0;
	avail_moves.clear();
	for (int i = 0; i < 64; i++) {
		if (board[i] >= (1 + 6 * _side) && board[i] <= (6 + 6 * _side)) {
			_engine->calcAvailMoves(i, avail_moves, board); // Get available moves for every piece on AI's side
			for (int j = 0; j < avail_moves.size(); j++) { // Loop through available moves
				curMove = evaluateMove(i, avail_moves[j]); // Evaluate move value
				std::cout << "curMove = " << curMove << " at i = " << i << ", j = " << avail_moves[j] << '\n';
				if ((_side == white && curMove >= bestMove) || (_side == black && curMove <= bestMove)) {
					bestMove = curMove;
					bestI = i;
					bestJ = avail_moves[j];
				}
			}
			avail_moves.clear();
		}
	}
	std::cout << "Detected best move with score = " << bestMove << " at i = " << bestI << ", j = " << bestJ << '\n';
	move[0] = bestI;
	move[1] = bestJ;
}