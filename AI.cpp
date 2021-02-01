#include "AI.h"
#include <chrono>

bool AI::canMove() {
	return (_engine->getTurn() == _side);
}

int AI::evaluateBoard(Board& board) {
	int score = 0;
	for (int i = 0; i < 64; i++) {
		score += pieceValue[board.b[i]];
	}
	if (board.chk[1]) score -= 50000;
	if (board.chk[0]) score += 50000;
	/*if (_engine->detectMate(true)) score -= 100000;
	if (_engine->detectMate(false)) score += 100000;*/
	counter++;
	return score;
}

int AI::evaluateMove(Board& board, Move& move) {
	Board newBoard(board);
	_engine->movePiece(move, newBoard);
	return evaluateBoard(newBoard);
}

int AI::findMoveRecursive(char step, Board& board) {
	std::vector<Move> moves;
	int bestScore, curScore;
	_engine->findAvailableMoves((_side+step)%2?false:true, moves, board);
	bool curTurn = !((_side + step) % 2);
	if (step == _nSteps) {
		bestScore = 200000 * (curTurn ? -1 : 1);
		for (auto& i : moves) {
		//std::for_each(std::execution::par_unseq, std::begin(moves), std::end(moves), [&](Move& i) {
		curScore = evaluateMove(board, i);
		//std::cout << "Step = " << (int)step << ", move = " << (int)i.oldSq << ", " << (int)i.newSq << ". Score = " << curScore << '\n';
		if ((curTurn && curScore >= bestScore) || (!curTurn && curScore <= bestScore)) {
			bestScore = curScore;
			bMove[step - 1] = i;
		}
		//});
		}
	}
	else {
		int curScore;
		bestScore = 200000 * (curTurn ? -1 : 1);
		Board newBoard(board);
		_engine->findAvailableMoves((_side + step)%2 ? false:true, moves, newBoard);
		for (auto& i : moves) {
			newBoard = board;
			_engine->movePiece(i, newBoard);
			curScore = findMoveRecursive(step + 1, newBoard);
			//std::cout << "Step = " << (int)step << ", move = " << (int)i.oldSq << ", " << (int)i.newSq << ". Score = " << curScore << '\n';
			if ((curTurn && curScore >= bestScore) || (!curTurn && curScore <= bestScore)) {
				bestScore = curScore;
				bMove[step-1] = i;
			}
		}
	}
	//std::cout << "Step = " << (int)step << ", move = " << (int)bMove[step-1].oldSq << ", " << (int)bMove[step-1].newSq << ". Best score = " << bestScore << '\n';
	return bestScore;
}

void AI::doMove() {
	std::vector<Move> moves;
	counter = 0;
	_engine->findCheckCounter = 0;
	_engine->findMovesCounter = 0;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	findMoveRecursive(1, *_engine->getBoard());
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
	std::cout << "Calculate n = " << (int)_nSteps << " in " << time_span.count() << " seconds\n";
	std::cout << "Checked " << _engine->findMovesCounter << " calcAvailMoves\n";
	std::cout << "Checked " << _engine->findCheckCounter << " detectCheck\n";
	std::cout << "Checked " << counter << " moves\n";
	moves.clear();
	_engine->movePiece(bMove[0], *_engine->getBoard());
	_engine->switchTurn();
}

/* Recursive algorithm
* If nsteps % 2 = 1, want to maximise score
* If nsteps % 2 = 0, want to minimise score
* If nsteps = 1, calculate move with max score
* If nsteps = 2, on step 2 calculate the highest score, on step 1 calculate the lowest score
* If nsteps = 3, calculate move where e
*/