#include "engine.h"
#include "AI.h"

int main(int argc, char** argv) {
	Engine chess;
	AI gameAI(false, &chess);
	while (!chess.gameOver()) {
		chess.drawGame();
		chess.pollEvents();
		if (gameAI.canMove()) {
			gameAI.doMove();
		}
	}
	chess.quitWindow();
	return 0;
}

/*
Optimisations:
- only calculate detectcheck and calcavailmoves once then store it in memory, possibly do it after movePiece function
- store copy of the board and "move" this new board so that we don't have to undo the move, we can simply release the board copy from memory (this might not actually be more efficient because we have to copy the data over each time
- store data in arrays instead of vectors, because static memory is faster
- multithreading, which probably needs to be done with point 2 above

To add:
- add better highlighting, with different colours
- improve code
- significantly optimise code
	the AI calls each of these functions millions of times per game, so even a fraction of a second will make a huge difference

 - Validate whether move is legal (e.g. doesn't put you in check) before giving it to AI , thus hugely saving on computation time

 - Recode the board to use my own 4-bit (or 5-bit) data type, with one bit representing black or white
 - This gives 16 possible values when we need 13. We can use the other 3 in clever ways e.g. storing an en passant pawn, castling or pawn promotion etc.
 
 - recode makeMove function to use moveType variable

 - to significantly cut down on CPU usage, we can store each piece's available moves and only recalculate them after each move
 
Bugs:
*/


// Interpret board as cstring to check for equality, to prevent calling multiple detectCheck functions
// hash map