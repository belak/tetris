#include "game.hpp"
#include "menu.hpp"

#include <allegro5/allegro_primitives.h>

#include <iostream>

using namespace std;

#define FALL_LENGTH 0.25

GameScreen::GameScreen() {
	cout << "Creating Game Screen" << endl;

	for (int i = 0; i < height; i++) {
		vector<Block> temp;
		for (int j = 0; j < width; j++) {
			temp.push_back(Block());
		}
		grid.push_back(temp);
	}

	cell_size = (Director::height - 2 * margin) / height;
	
	// This recalculates the margin based on the cell size so we can center it
	margin = (Director::height - cell_size * height) / 2;

	start_x = ((float)Director::width / 2.0) - (width * cell_size / 2);
	start_y = margin;

	generateTetromino();

	//cout << cell_size << " " << start_x << " " << start_y << endl;
}

GameScreen::~GameScreen() {
	cout << "Destroying Game Screen" << endl;
}

void GameScreen::generateTetromino() {
	current = Tetromino();
	current.loc = Vect(width / 2, 0);
	for (auto block : current.blocks) {
		while (current.loc.y + block.offset.y >= 0) {
			current.loc.y -= 1;
		}
	}
	sanitizeCurrent();
}

void GameScreen::update() {
	bool hyper_speed = false;
	float timer_freq = FALL_LENGTH;

	auto copy = current;
	while(!al_is_event_queue_empty(Director::input)) {
		bool stabalize = false;

		ALLEGRO_EVENT iev;
		al_wait_for_event(Director::input, &iev);
		if (iev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (iev.keyboard.keycode) {
				case ALLEGRO_KEY_DOWN:
					speed_run = true;
					return;
				case ALLEGRO_KEY_LSHIFT:
					if (has_stored) {
						auto temp = store;
						store = current;
						current = temp;
						current.loc = Vect(rand() % width, 0);
						sanitizeCurrent();
					} else {
						has_stored = true;
						store = current;
						generateTetromino();
					}
					break;
			}
		} else if (iev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (iev.keyboard.keycode) {
				case ALLEGRO_KEY_DOWN:
					speed_run = false;
					return;
			}
		} else if (iev.type == ALLEGRO_EVENT_KEY_CHAR) {
			bool can_move = true;

			switch (iev.keyboard.keycode) {
			case ALLEGRO_KEY_ESCAPE:
				// TODO: display a confirmation screen
				Director::pop();
				return;
			case ALLEGRO_KEY_UP:
				// TODO: Fix this - push over if possible, not just disable
				current.rotate();

				for (int i = 0; i < current.blocks.size(); i++) {
					auto block = current.blocks[i];
					int x = (int)(current.loc.x + block.offset.x);
					int y = (int)(current.loc.y + block.offset.y);

					// Not allowed to rotate if x or y aren't valid or there's a block there
					if (y < 0 || y >= height || x < 0 || x >= width || grid[y][x].on) {
						stabalize = true;
						break;
					}
				}

				if (stabalize) {
					current = copy;
				} else {
					sanitizeCurrent();
				}
				
				return;
			case ALLEGRO_KEY_LEFT:
				for (int i = 0; i < current.blocks.size(); i++) {
					auto block = current.blocks[i];
					int x = (int)(current.loc.x + block.offset.x - 1);
					int y = (int)(current.loc.y + block.offset.y);
					if (y > 0 && grid[y][x].on) {
						can_move = false;
						break;
					}
				}
				if (can_move) {
					current.loc.x -= 1;
					sanitizeCurrent();
				}
				break;
			case ALLEGRO_KEY_RIGHT:
				for (int i = 0; i < current.blocks.size(); i++) {
					auto block = current.blocks[i];
					int x = (int)(current.loc.x + block.offset.x + 1);
					int y = (int)(current.loc.y + block.offset.y);
					if (y > 0 && grid[y][x].on) {
						can_move = false;
						break;
					}
				}
				if (can_move) {
					current.loc.x += 1;
					sanitizeCurrent();
				}
				break;
			case ALLEGRO_KEY_SPACE:
				hyper_speed = true;
				break;
			}
		}
	}

	if (speed_run) {
		timer_freq /= 4;
	}

	// Now that input is done, change the ghost
	ghost = current;
	ghost.color = al_map_rgb(128, 128, 128);
	bool found = false;
	for (int i = ghost.loc.y; !found && i < height; i++) {
		ghost.loc.y += 1;
		for (int j = 0; !found && j < ghost.blocks.size(); j++) {
			auto block = ghost.blocks[j];
			int x = (int)(ghost.loc.x + block.offset.x);
			int y = (int)(ghost.loc.y + block.offset.y);
			if (y >= height || (y >= 0 && grid[y][x].on)) {
				// Move back up and save location
				ghost.loc.y -= 1;
				found = true;
				break;
			}
		}
	}

	if (hyper_speed) {
		move_timer = FALL_LENGTH;
		current.loc = ghost.loc;
	}

	move_timer += Director::tickLength;
	if (move_timer > timer_freq) {
		current.loc.y += 1;
		// If any blocks will collide, move it up one and set it in stone
		bool stabalize = false;
		for (int i = 0; i < current.blocks.size(); i++) {
			auto block = current.blocks[i];
			int x = (int)(current.loc.x + block.offset.x);
			int y = (int)(current.loc.y + block.offset.y);
			if (y >= height || (y >= 0 && grid[y][x].on)) {
				stabalize = true;
				break;
			}
		}

		if (stabalize) {
			current.loc.y -= 1;
			
			running = false;
			for (int i = 0; i < current.blocks.size(); i++) {
				auto block = current.blocks[i];
				int x = (int)(current.loc.x + block.offset.x);
				int y = (int)(current.loc.y + block.offset.y);
				block.on = true;
				if (y >= 0) {
					grid[y][x] = block;
					running = true;
				}
			}

			if (!running) {
				Director::pop();
				return;
			}

			for (int i = grid.size() - 1; i >= 0; i--) {
				bool on = true;
				for (int j = 0; j < grid[i].size(); j++) {
					if (!grid[i][j].on) {
						on = false;
					}
				}

				if (on) {
					grid.erase(grid.begin() + i);
				}
			}

			while (grid.size() < height) {
				grid.insert(grid.begin(), vector<Block>(width));
			}

			generateTetromino();
			sanitizeCurrent();
		}
		
		move_timer = 0.0;
	}
}

void GameScreen::render() {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			al_draw_rectangle(
					start_x + i * cell_size, start_y + j * cell_size,
					start_x + (i + 1) * cell_size, start_y + (j + 1) * cell_size,
					al_map_rgb(50, 50, 50), line_size);

			auto block = grid[j][i];
			if (block.on) {
				al_draw_rectangle(
						start_x + i * cell_size + line_size, start_y + j * cell_size + line_size,
						start_x + (i + 1) * cell_size - line_size, start_y + (j + 1) * cell_size - line_size,
						block.color, line_size);
			}
		}
	}

	// Draw current ghost
	for (int i = 0; i < ghost.blocks.size(); i++) {
		auto block = ghost.blocks[i];
		if (ghost.loc.y + block.offset.y >= 0 && ghost.loc.y + block.offset.y < height) {
			al_draw_rectangle(
					start_x + (ghost.loc.x + block.offset.x) * cell_size + line_size,
					start_y + (ghost.loc.y + block.offset.y) * cell_size + line_size,
					start_x + (ghost.loc.x + block.offset.x + 1) * cell_size - line_size,
					start_y + (ghost.loc.y + block.offset.y + 1) * cell_size - line_size,
					ghost.color, line_size);
		}
	}

	// Draw current block
	for (int i = 0; i < current.blocks.size(); i++) {
		auto block = current.blocks[i];
		if (current.loc.y + block.offset.y >= 0 && current.loc.y + block.offset.y < height) {
			al_draw_rectangle(
					start_x + (current.loc.x + block.offset.x) * cell_size + line_size,
					start_y + (current.loc.y + block.offset.y) * cell_size + line_size,
					start_x + (current.loc.x + block.offset.x + 1) * cell_size - line_size,
					start_y + (current.loc.y + block.offset.y + 1) * cell_size - line_size,
					block.color, line_size);
		}
	}
}

void GameScreen::sanitizeCurrent() {
	for (int i = 0; i < current.blocks.size(); i++) {
		auto block = current.blocks[i];

		while (current.loc.x + block.offset.x < 0) {
			current.loc.x += 1;
		}

		while (current.loc.x + block.offset.x >= width) {
			current.loc.x -= 1;
		}
	}
}
