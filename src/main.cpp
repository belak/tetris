#include "director.hpp"
#include "screens/splash.hpp"

#include <allegro5/allegro.h>

#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	cout << "Starting Tetris" << endl;

	// Start up
	Director::init();

	// Push the initial screen
	Director::push(new SplashScreen());

	// Main loop
	Director::run();

	// Cleanup when done
	Director::cleanup();

	return 0;
}
