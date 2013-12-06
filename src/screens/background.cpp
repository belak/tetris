#include "background.hpp"

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <cmath>
#include <iostream>

using namespace std;

void BackgroundTetromino::reset() {
	loc.x = rand() % Director::width;
	loc.y = -20;

	// http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
	float hue;
	modff(rand() + 0.618033988749895f, &hue);

	color = al_color_hsv(hue, 0.5, 0.95);

	scale = 5.0 + 10 * (float) rand() / (float) RAND_MAX;
	speed = scale / 10.0;
}

BackgroundScreen::BackgroundScreen() {
	cout << "Creating Background Screen" << endl;

	// Create "particles" for each shape
	for (int i = 0; i < count; i++) {
		tetrominos.push_back(new BackgroundTetromino());
		tetrominos.back()->reset();
		tetrominos.back()->loc.y = rand() % Director::height;
	}
}

BackgroundScreen::~BackgroundScreen() {
	cout << "Destroying Background Screen" << endl;

	while (!tetrominos.empty()) {
		auto tetromino = tetrominos.back();
		tetrominos.pop_back();
		delete tetromino;
	}
}

void BackgroundScreen::update() {
	for (auto tetromino : tetrominos) {
		tetromino->loc.y += tetromino->speed;

		// TODO: take into account the size
		if (tetromino->loc.y > Director::height) {
			tetromino->reset();
		}
	}
}

void BackgroundScreen::render() {
	for (auto t : tetrominos) {
		for (int i = 0; i < t->matrix.size(); i++) {
			for (int j = 0; j < t->matrix.size(); j++) {
				auto block = t->matrix[j][i];
				if (block.on) {
					al_draw_rectangle(
							t->loc.x + i * base_size * t->scale,
							t->loc.y + j * base_size * t->scale,
							t->loc.x + (i + 1) * base_size * t->scale,
							t->loc.y + (j + 1) * base_size * t->scale,
							t->color, 2);
				}
			}
		}
	}
}
