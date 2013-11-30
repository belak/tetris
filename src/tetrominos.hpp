#pragma once

#include "utils.hpp"

#include <vector>

class Block {
public:
	Block() {}
	Block(int block_on) {
		on = block_on != 0;
	}

	bool on = false;
	ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);
};

class Tetromino {
public:
	Tetromino();
	~Tetromino();
	void rotate();
	std::pair<Vect, Vect> bound();

	std::vector<std::vector<Block>> matrix;
	std::vector<std::vector<std::pair<int, int>>> kicks;
	Vect loc;
	int state = 0;
	int current_kicks = 0;
	ALLEGRO_COLOR color;
};
