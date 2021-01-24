#include "engine.h"
#include "AI.h"

int main(int argc, char** argv) {
	Engine chess;
	AI gameAI(false, &chess);
	if (chess.initWindow()) {
		chess.initBoard();
		chess.resetGame();
		while (!chess.gameOver()) {
			chess.drawGame();
			chess.pollEvents();
			/*if (gameAI.shouldMove()) {
				gameAI.doMove();
			}*/
		}
	}
	chess.quitWindow();
	return 0;
}