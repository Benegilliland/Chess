#pragma once
#include "engine.h"
#include <execution>

class AI {
	int counter;
	char _nSteps;
	std::vector<Move> bMove;
	bool _side;
	Engine* _engine;
	const int pieceValue[13] = { 0, -100, -300, -300, -500, -900, -10000, 100, 300, 300, 500, 900, 10000 };
	Board board_copy;

public:
	int evaluateBoard(Board& board);
	int evaluateMove(Board& board, Move& move);
	void doMove();
	bool canMove();
	int findMoveRecursive(char step, Board& board);

	AI(bool side, Engine* engine, const char nSteps) {
		_nSteps = nSteps;
		for (int i = 0; i < _nSteps; i++) {
			bMove.push_back({ 0,0 });
		}
		_side = side;
		_engine = engine;
	}
};