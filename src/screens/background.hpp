#pragma once

#include "../director.hpp"
#include "../tetrominos.hpp"

#include <vector>

class BackgroundTetromino : public Tetromino {
public:
	void reset();
	float scale = 1.0;
	float speed = 0.0;
};

class BackgroundScreen : public Screen {
public:
	BackgroundScreen();
	~BackgroundScreen();

	void update();
	void render();

private:
	void createObject();

	int count = 100;
	float base_size = 1;
	std::vector<BackgroundTetromino *> tetrominos;
};
