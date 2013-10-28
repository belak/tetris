#include "tetrominos.hpp"

#include <allegro5/allegro_color.h>

#include <cmath>

using namespace std;

Tetromino::Tetromino() {
	loc = Vect(0, 0);
	type = rand() % 6;

	// http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
	float hue;
	modff(rand() + 0.618033988749895f, &hue);
	color = al_color_hsv(hue, 0.5, 0.95);

	switch (type) {
		case 0:
			// L
			blocks.push_back(Block(0, 0));
			blocks.push_back(Block(-1, 0));
			blocks.push_back(Block(1, 0));
			blocks.push_back(Block(1, -1));
			break;
		case 1:
			// L reversed
			blocks.push_back(Block(0, 0));
			blocks.push_back(Block(-1, 0));
			blocks.push_back(Block(1, 0));
			blocks.push_back(Block(-1, -1));
			break;
		case 2:
			// S
			blocks.push_back(Block(0, 0));
			blocks.push_back(Block(-1, 0));
			blocks.push_back(Block(-1, 1));
			blocks.push_back(Block(0, -1));
			break;
		case 3:
			// S reversed
			blocks.push_back(Block(0, 0));
			blocks.push_back(Block(1, 0));
			blocks.push_back(Block(1, 1));
			blocks.push_back(Block(0, -1));
			break;
		case 4:
			// Cube
			blocks.push_back(Block(0, 0));
			blocks.push_back(Block(1, 1));
			blocks.push_back(Block(1, 0));
			blocks.push_back(Block(0, 1));
			break;
		case 5:
			// Line
			blocks.push_back(Block(0, 0));
			blocks.push_back(Block(-1, 0));
			blocks.push_back(Block(1, 0));
			blocks.push_back(Block(2, 0));
			break;

		default:
			break;
	}

	for (int i = 0; i < blocks.size(); i++) {
		blocks[i].color = color;
	}
}

Tetromino::~Tetromino() {
	
}

void Tetromino::rotate() {
	// Don't rotate cubes
	if (type == 4) {
		return;
	}

	for (int i = 0; i < blocks.size(); i++) {
		int x = -blocks[i].offset.y;
		int y = blocks[i].offset.x;

		blocks[i].offset.x = x;
		blocks[i].offset.y = y;
	}
}
