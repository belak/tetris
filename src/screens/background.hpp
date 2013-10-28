#pragma once

#include "../director.hpp"

#include <vector>

class BackgroundTetromino {
public:
	void reset(ALLEGRO_BITMAP *tile);
	void render();

	ALLEGRO_COLOR color;
	ALLEGRO_BITMAP *tile = NULL;
	int x = -1;
	int y = -1;
	float hw = 0;
	float hh = 0;
	float scale = 1.0;
	float angle = 0;
	int speed = 0;
	int flags = 0;
};

class BackgroundScreen : public Screen {
public:
	BackgroundScreen();
	~BackgroundScreen();

	void update();
	void render();

private:
	void createObject();

	int count = 30;
	std::vector<BackgroundTetromino *> tetrominos;
	std::vector<ALLEGRO_BITMAP *> shapes;
};
