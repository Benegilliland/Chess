#pragma once
#include "engine.h"

class AI {
	int counter;
	const int nSteps = 2;
	std::vector<Move> bMove;
	bool _side;
	Engine* _engine;
	const int pieceValue[13] = { 0, -100, -300, -300, -500, -900, -10000, 100, 300, 300, 500, 900, 10000 };

public:
	int evaluateBoard();
	int evaluateMoves(std::vector<Move> moves);
	void doMove();
	bool shouldMove();
	int findMoveRecursive(char step, std::vector<Move> preMoves);

	AI(bool side, Engine* engine) {
		for (int i = 0; i < nSteps; i++) {
			bMove.push_back({ 0,0 });
		}
		_side = side;
		_engine = engine;
	}
};