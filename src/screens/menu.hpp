#pragma once

#include "background.hpp"

#include <allegro5/allegro_font.h>

#include <vector>

class MenuScreen : public Screen {
public:
	MenuScreen();
	~MenuScreen();

	void update();
	void render();

private:
	ALLEGRO_FONT *title_font = NULL;
	ALLEGRO_FONT *font = NULL;

	int selected_weight = 255;
	bool selected_shrinking = true;

	BackgroundScreen background;

	uint selected = 0;

	std::vector<std::string> items;
};
