#pragma once

#include "../director.hpp"
#include "../tetrominos.hpp"

#include <vector>
#include <unordered_map>
#include <queue>

class GameScreen : public Screen {
public:
	GameScreen();
	~GameScreen();

	void update();
	void render();

private:
	void generateTetromino();
	void sanitizeCurrent();

	int width = 10;
	int height = 20;
	int height_buffer = 0;
	int margin = 2;
	int cell_size = 0;
	int start_x = 0;
	int start_y = 0;
	float line_size = 3.0;
	bool speed_run = false;
	bool has_stored = false;
	float move_timer = 0.0;
	bool running = true;

	std::vector<std::vector<Block>> grid;
	Tetromino current;
	Tetromino store;
	Tetromino ghost;
	std::queue<Tetromino> upcoming;
};
