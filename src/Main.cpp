#include <iostream>
#include "Engine/Engine.h"

int main(int argc, char* argv[]) {
	Engine engine;

	engine.Initialise();

	engine.Run();

	engine.Shutdown();

	return 0;
}