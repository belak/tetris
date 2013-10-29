#include "tetrominos.hpp"

#include <allegro5/allegro_color.h>

#include <cmath>

using namespace std;

Tetromino::Tetromino() {
	loc = Vect(0, 0);
	int type = rand() % 6;

	// http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
	float hue;
	modff(rand() + 0.618033988749895f, &hue);
	color = al_color_hsv(hue, 0.5, 0.95);

	switch (type) {
		case 0:
			// L
			matrix = {
				{1, 0, 0},
				{1, 1, 1},
				{0, 0, 0}
			};
			break;
		case 1:
			// L reversed
			matrix = {
				{0, 0, 1},
				{1, 1, 1},
				{0, 0, 0}
			};
			break;
		case 2:
			// S
			matrix = {
				{0, 0, 1},
				{1, 1, 1},
				{0, 0, 0}
			};
			break;
		case 3:
			// S reversed
			matrix = {
				{0, 0, 1},
				{1, 1, 1},
				{0, 0, 0}
			};
			break;
		case 4:
			// Cube
			matrix = {
				{1, 1},
				{1, 1}
			};
			break;
		case 5:
			// Line
			matrix = {
				{0, 0, 0, 0},
				{1, 1, 1, 1},
				{0, 0, 0, 0},
				{0, 0, 0, 0}
			};
			break;

		default:
			break;
	}

	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix.size(); j++) {
			matrix[j][i].color = color;
		}
	}
}

Tetromino::~Tetromino() {
	
}

pair<Vect, Vect> Tetromino::bound() {
	Vect small;
	Vect large;

	for (int i = 0; i < matrix.size(); i++) {
		for (int j = 0; j < matrix.size(); j++) {
			auto block = matrix[j][i];
			if (block.on) {
				if (small.x > i) {
					small.x = i;
				}
				if (large.x > i) {
					large.x = i;
				}
				if (small.x < j) {
					small.x = j;
				}
				if (large.x < j) {
					large.x = j;
				}
			}
		}
	 }

	 return pair<Vect, Vect>(small, large);
}

void Tetromino::rotate() {
	for (int i = 0; i < matrix.size() - 1; i++) {
		for (int j = i + 1; j < matrix.size(); j++) {
			auto temp = matrix[i][j];
			matrix[i][j] = matrix[j][i];
			matrix[j][i] = temp;
		}
	}

	for (int i = 0; i < matrix.size() / 2; i++) {
		for (int j = 0; j < matrix.size(); j++) {
			auto temp = matrix[j][i];
			matrix[j][i] = matrix[j][matrix.size() - i - 1];
			matrix[j][matrix.size() - i - 1] = temp;
		}
	}
}
