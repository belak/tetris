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
	void fillQueue();
	void generateTetromino();
	void sanitizeCurrent();
	void calculateGhost();
	void cleanGrid();
	void resetCurrent();
	bool validPosition();

	int level = 1;
	int lines = 0;
	int tetris_in_a_row = 0;
	int points = 0;

	int width = 10;
	int height = 20;
	int margin = 2;
	int cell_size = 0;
	int start_x = 0;
	int start_y = 0;
	float line_size = 3.0;
	bool speed_run = false;
	bool has_stored = false;
	float move_timer = 0.0;
	float lock_timer = 0.0;
	bool running = true;

	std::vector<std::vector<Block>> grid;
	Tetromino current;
	Tetromino store;
	Tetromino ghost;
	std::deque<Tetromino> upcoming;
};
