#include "background.hpp"

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <cmath>
#include <iostream>

using namespace std;

void BackgroundTetromino::reset(ALLEGRO_BITMAP *input) {
	tile = input;

	hw = al_get_bitmap_width(tile) / 2.0;
	hh = al_get_bitmap_height(tile) / 2.0;

	angle = 2 * ALLEGRO_PI / 4 * (rand() % 4);

	// TODO: offset by the graphic size
	x = rand() % Director::width;
	if (y == -1) {
		y = rand() % Director::height;
	} else {
		y = -hh * scale;
	}

	// http://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
	float hue;
	modff(rand() + 0.618033988749895f, &hue);

	color = al_color_hsv(hue, 0.5, 0.95);
	scale = 0.5 + (float) rand() / (float) RAND_MAX;
	speed = 1 + rand() % 4;
}

void BackgroundTetromino::render() {
	al_draw_tinted_scaled_rotated_bitmap(tile, color, hw, hh, x, y, scale, scale, angle, flags);
}

BackgroundScreen::BackgroundScreen() {
	cout << "Creating Background Screen" << endl;

	// Load all tetromino shapes
	shapes.push_back(al_load_bitmap("assets/gfx/bg_l.png"));
	shapes.push_back(al_load_bitmap("assets/gfx/bg_s.png"));
	shapes.push_back(al_load_bitmap("assets/gfx/bg_t.png"));
	shapes.push_back(al_load_bitmap("assets/gfx/bg_cube.png"));
	shapes.push_back(al_load_bitmap("assets/gfx/bg_line.png"));

	// Create "particles" for each shape
	for (int i = 0; i < count; i++) {
		ALLEGRO_BITMAP *temp = shapes[rand() % shapes.size()];
		tetrominos.push_back(new BackgroundTetromino());
		tetrominos.back()->reset(temp);
	}
}

BackgroundScreen::~BackgroundScreen() {
	cout << "Destroying Background Screen" << endl;
	// TODO: clean out tetrominoes
	while (!shapes.empty()) {
		auto tile = shapes.back();
		shapes.pop_back();
		al_destroy_bitmap(tile);
	}

	while (!tetrominos.empty()) {
		auto tetromino = tetrominos.back();
		tetrominos.pop_back();
		delete tetromino;
	}
}

void BackgroundScreen::update() {
	for (auto tetromino : tetrominos) {
		tetromino->y += tetromino->speed;

		if (tetromino->y > Director::height + tetromino->hh) {
			ALLEGRO_BITMAP *temp = shapes[rand() % shapes.size()];
			tetromino->reset(temp);
		}
	}
}

void BackgroundScreen::render() {
	for (auto tetromino : tetrominos) {
		tetromino->render();
	}
}
