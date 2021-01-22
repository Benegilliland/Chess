#pragma once
#include "engine.h"

class AI {
	int nSteps = 3;
	Turn _side;
	Engine* _engine;
	const int pieceValue[13] = { 0, -100, -300, -300, -500, -900, -10000, 100, 300, 300, 500, 900, 10000 };

public:
	int evaluateBoard();
	int evaluateBoard(Piece* board);
	int evaluateMove(int oldSq, int newSq);
	void doMove(Piece* board);
	void doMove(Piece* board, int step);
	bool shouldMove();

	AI(Turn side, Engine* engine) {
		_side = side;
		_engine = engine;
	}
};