#include "engine.h"
#include "AI.h"

int main(int argc, char** argv) {
	Engine chess;
	AI gameAI(black, &chess);
	if (chess.initWindow()) {

		chess.initBoard();
		chess.loadImages();
		chess.resetGame();
		while (!chess.gameOver()) {
			chess.drawGame();
			chess.pollEvents();
			if (gameAI.shouldMove()) {
				gameAI.doMove(chess.getBoard(), 1);
			}
		}
	}

	chess.quitWindow();
	return 0;
}