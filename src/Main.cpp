#include <iostream>
#include "Game.h"

int main(int argc, char* argv[]) {
	Game game;

	game.Initialise();

	game.Run();

	game.Shutdown();

	return 0;
}