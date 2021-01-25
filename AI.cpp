#include "AI.h"

bool AI::canMove() {
	return (_engine->getTurn() == _side);
}

int AI::evaluateBoard() {
	char* board = _engine->getBoard();
	int score = 0;
	for (int i = 0; i < 64; i++) {
		score += pieceValue[board[i]];
	}
	if (_engine->detectCheck(true)) score -= 50000;
	if (_engine->detectCheck(false)) score += 50000;
	/*if (_engine->detectMate(true)) score -= 100000;
	if (_engine->detectMate(false)) score += 100000;*/
	counter++;
	return score;
}

int AI::evaluateMoves(std::vector<Move> moves) {
	char* board = _engine->getBoard();
	int score;
	std::vector<char> storage;
	for (int i = 0; i < moves.size(); i++) {
		storage.push_back(board[moves[i].newSq]);
		_engine->movePiece(moves[i]);
	}
	score = evaluateBoard();
	for (int i = moves.size() - 1; i >= 0; i--) {
		_engine->movePiece({ moves[i].newSq, moves[i].oldSq });
		board[moves[i].newSq] = storage[i];
	}
	return score;
}

int AI::findMoveRecursive(char step, std::vector<Move> preMoves) {
	std::vector<Move> moves;
	int bestScore, curScore;
	_engine->findAvailableMoves((_side+step)%2?false:true, preMoves, moves);
	bool curTurn = !((_side + step) % 2);
	if (step == nSteps) {
		bestScore = 200000 * (curTurn ? -1 : 1);
		for (auto& i : moves) {
			preMoves.push_back(i);
			curScore = evaluateMoves(preMoves);
			//std::cout << "Step = " << (int)step << ", move = " << (int)i.oldSq << ", " << (int)i.newSq << ". Score = " << curScore << '\n';
			preMoves.pop_back();
			if ((curTurn && curScore >= bestScore) || (!curTurn && curScore <= bestScore)) {
				bestScore = curScore;
				bMove[step-1] = i;
			}
		}
	}
	else {
		int curScore;
		bestScore = 200000 * (curTurn ? -1 : 1);
		for (auto& i : moves) {
			preMoves.push_back(i);
			curScore = findMoveRecursive(step + 1, preMoves);
			//std::cout << "Step = " << (int)step << ", move = " << (int)i.oldSq << ", " << (int)i.newSq << ". Score = " << curScore << '\n';
			if ((curTurn && curScore >= bestScore) || (!curTurn && curScore <= bestScore)) {
				bestScore = curScore;
				bMove[step-1] = preMoves[0];
			}
			preMoves.pop_back();
		}
	}
	//std::cout << "Step = " << (int)step << ", move = " << (int)bMove[step-1].oldSq << ", " << (int)bMove[step-1].newSq << ". Best score = " << bestScore << '\n';
	return bestScore;
}

void AI::doMove() {
	std::vector<Move> moves;
	counter = 0;
    findMoveRecursive(1, moves);
	std::cout << "Checked " << counter << " moves\n";
	moves.clear();
	_engine->movePiece(bMove[0]);
	_engine->switchTurn();
}

/* Recursive algorithm
* If nsteps % 2 = 1, want to maximise score
* If nsteps % 2 = 0, want to minimise score
* If nsteps = 1, calculate move with max score
* If nsteps = 2, on step 2 calculate the highest score, on step 1 calculate the lowest score
* If nsteps = 3, calculate move where e
*/