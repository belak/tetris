#include "menu.hpp"
#include "game.hpp"

#include <iostream>
#include <sstream>

using namespace std;

MenuScreen::MenuScreen() : Screen(), background(), items() {
	cout << "Creating Menu Screen" << endl;

	title_font = al_load_font("assets/fonts/emulogic.ttf", 32, 0);
	font = al_load_font("assets/fonts/emulogic.ttf", 24, 0);

	items.push_back("New Game");
	items.push_back("Exit");
}

MenuScreen::~MenuScreen() {
	cout << "Destroying Menu Screen" << endl;

	al_destroy_font(title_font);
	al_destroy_font(font);
}

void MenuScreen::update() {
	// On each tick, we run through all the input
	while (!al_is_event_queue_empty(Director::input)) {
		ALLEGRO_EVENT iev;
		al_wait_for_event(Director::input, &iev);
		if (iev.type == ALLEGRO_EVENT_KEY_CHAR) {
			switch (iev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				selected = (selected == 0) ? items.size() - 1 : selected - 1;
				break;
			case ALLEGRO_KEY_DOWN:
				selected = (selected + 1) % items.size();
				break;
			case ALLEGRO_KEY_ENTER:
			case ALLEGRO_KEY_SPACE:
				switch (selected) {
					case 0:
						Director::push(new GameScreen());
						break;
					case 1:
						Director::running = false;
					default:
						break;
				}
				break;
			case ALLEGRO_KEY_ESCAPE:
				Director::running = false;
				break;
			}

			// Reset the fade on keypress
			selected_weight = 255;
			selected_shrinking = true;
		}
	}

	if (selected_shrinking) {
		selected_weight -= 5;
		if (selected_weight < 20) {
			selected_weight = 20;
			selected_shrinking = false;
		}
	} else {
		selected_weight += 5;
		if (selected_weight > 255) {
			selected_weight = 255;
			selected_shrinking = true;
		}
	}

	background.update();
}

void MenuScreen::render() {
	background.render();

	al_draw_text(title_font, al_map_rgb(255,255,255), 0, 0, ALLEGRO_ALIGN_LEFT, "Tetris");
	
	ALLEGRO_COLOR color;

	for (uint i = 0; i < items.size(); i++) {
		if (selected == i) {
			color = al_map_rgb(selected_weight, selected_weight, selected_weight);
		} else {
			color = al_map_rgb(255, 255, 255);
		}

		al_draw_text(font, color, Director::width / 2, Director::height * 4 / 7 + al_get_font_line_height(font) * i, ALLEGRO_ALIGN_CENTRE, items[i].c_str());
	}	
}
