#include "SDLEngine.h"


int main( int argc, char* argv[] )
{	
	SDLEngine engine;
	
	engine.init();

	while(true) {
		bool isMenuComplete = false;
		while(!isMenuComplete) {
			isMenuComplete = engine.runMenu();
		}
		
		bool isGameComplete = false;
		while (!isGameComplete) {
			isGameComplete = engine.run();
		}
	}

	return 0;
}

