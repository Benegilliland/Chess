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

void AI::doMove(Piece* board) {
	std::vector<int> avail_moves;
	int curMove = 0;
	int bestMove = 10000 * (_side ? -2 : 1);
	int bestI = 0;
	int bestJ = 0;
	avail_moves.clear();
	for (int i = 0; i < 64; i++) {
		if (board[i] >= (1 + 6 * _side) && board[i] <= (6 + 6 * _side)) {
			std::cout << "i = " << i << '\n';
			_engine->calcAvailMoves(i, avail_moves); // Get available moves for every piece on AI's side
			for (int j = 0; j < avail_moves.size(); j++) { // Loop through available moves
				//if (i != avail_moves[j]) { // This shouldn't need to be here, but for some reason the AI keeps deleting its own rook by moving it from square 0 to square 0;
					curMove = evaluateMove(i, avail_moves[j]); // Evaluate move value
					if ((_side == white && curMove >= bestMove) || (_side == black && curMove <= bestMove)) {
						bestMove = curMove;
						bestI = i;
						bestJ = avail_moves[j];
					}
				//}
			}
			avail_moves.clear();
		}
	}
	std::cout << "Side = " << _side << '\n';
	std::cout << "Detected best move with score " << bestMove << " at i = " << bestI << ", j = " << bestJ << '\n';
	_engine->makeMove(bestI, bestJ);
	_engine->switchTurn();
}

void AI::doMove(Piece* board, int step) {

}