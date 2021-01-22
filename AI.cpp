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
	if (_engine->detectCheck(white, board)) {
		score -= 50000;
	}
	if (_engine->detectCheck(black, board)) {
		score += 50000;
	}
	counter++;
	return score;
}

int AI::evaluateMoves(std::vector<Move> moves) {
	Piece* board = _engine->getBoard();
	int score;
	std::vector<Piece> storage;
	for (int i = 0; i < moves.size(); i++) {
		storage.push_back(board[moves[i].sq[0]]);
		storage.push_back(board[moves[i].sq[1]]);
		board[moves[i].sq[1]] = board[moves[i].sq[0]];
		board[moves[i].sq[0]] = empty_square;
	}
	score = evaluateBoard();
	for (int i = moves.size() - 1; i >= 0; i--) {
		board[moves[i].sq[0]] = storage[2 * i];
		board[moves[i].sq[1]] = storage[2 * i + 1];
	}
	return score;
}

int AI::findMoveRecursive(char step, std::vector<Move> preMoves) {
	std::vector<Move> moves;
	int bestScore;
	int curScore;
	//bMove.push_back({ 0,0 });
	_engine->findAvailableMoves((_side+step)%2?black:white, preMoves, moves);
	if (step == nSteps) {
		bestScore = 200000 * (((int)_side + step) % 2 ? 1 : -1);
		for (auto& i : moves) {
			preMoves.push_back(i);
			curScore = evaluateMoves(preMoves);
			preMoves.pop_back();
			if (curScore * (((int)_side + step) % 2 ? -1 : 1) >= (bestScore * (((int)_side + step) % 2 ? -1 : 1))) {
				bestScore = curScore;
				bMove[step-1] = i;
			}
		}
	}
	else {
		int curScore;
		 bestScore = 200000 * (((int)_side + step) % 2 ? 1 : -1);
		for (auto& i : moves) {
			preMoves.push_back(i);
			curScore = findMoveRecursive(step + 1, preMoves);
			if (curScore * (((int)_side + step) % 2 ? -1 : 1) >= (bestScore * (((int)_side + step) % 2 ? -1 : 1))) {
				bestScore = curScore;
				bMove[step-1] = preMoves[0];
			}
			preMoves.pop_back();
		}
	}
	return bestScore;
}

void AI::doMove() {
	std::vector<Move> moves;
	counter = 0;
    findMoveRecursive(1, moves);
	std::cout << "Checked " << counter << " moves\n";
	moves.clear();
	_engine->makeMove(bMove[0].sq[0], bMove[0].sq[1]);
	_engine->switchTurn();
}

/* Recursive algorithm
* If nsteps % 2 = 1, want to maximise score
* If nsteps % 2 = 0, want to minimise score
* If nsteps = 1, calculate move with max score
* If nsteps = 2, calculate move where enemy's best move is lowest
* If nsteps = 3, calculate move where e
*/