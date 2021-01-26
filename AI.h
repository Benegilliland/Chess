#pragma once
#include "engine.h"
#include <execution>

class AI {
	int counter;
	const char nSteps = 3;
	std::vector<Move> bMove;
	bool _side;
	Engine* _engine;
	const int pieceValue[13] = { 0, -100, -300, -300, -500, -900, -10000, 100, 300, 300, 500, 900, 10000 };
	Board board_copy;

public:
	int evaluateBoard();
	int evaluateMoves(std::vector<Move> moves);
	void doMove();
	bool canMove();
	int findMoveRecursive(char step, std::vector<Move> preMoves);

	AI(bool side, Engine* engine) {
		for (int i = 0; i < nSteps; i++) {
			bMove.push_back({ 0,0 });
		}
		_side = side;
		_engine = engine;
	}
};