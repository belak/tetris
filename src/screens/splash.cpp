#include "splash.hpp"
#include "menu.hpp"

#include <iostream>

using namespace std;

SplashScreen::SplashScreen() {
	cout << "Creating Splash Screen" << endl;

}

SplashScreen::~SplashScreen() {
	cout << "Destroying Splash Screen" << endl;
}

void SplashScreen::update() {
	while(!al_is_event_queue_empty(Director::input)) {
		ALLEGRO_EVENT iev;
		al_wait_for_event(Director::input, &iev);
		if (iev.type == ALLEGRO_EVENT_KEY_CHAR) {
			switch (iev.keyboard.keycode) {
			case ALLEGRO_KEY_SPACE:
			case ALLEGRO_KEY_ENTER:
				Director::replace(new MenuScreen());
				return;
			}
		}
	}

	timer += Director::tickLength;

	if (timer > 1.0) {
		Director::replace(new MenuScreen());
		return;
	}
}

void SplashScreen::render() {
	
}
