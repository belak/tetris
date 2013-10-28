#pragma once

#include "utils.hpp"

#include <vector>

class Block {
public:
	Block() {
		Block(0, 0);
	}
	Block(float x, float y) {
		offset = Vect(x, y);
	}

	bool on = false;
	Vect offset;
	ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);
};

class Tetromino {
public:
	Tetromino();
	~Tetromino();
	void rotate();

	std::vector<Block> blocks;
	Vect loc;
	int type;
	ALLEGRO_COLOR color;
};
