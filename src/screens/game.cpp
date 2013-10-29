#include "game.hpp"
#include "menu.hpp"

#include <allegro5/allegro_primitives.h>

#include <iostream>

using namespace std;

#define FALL_LENGTH 0.25
#define QUEUE_SIZE 5

// TODO:
// Display held piece
// Display upcoming
// Fix rotations
// Separate stabalize timer
// Make it SRS standard

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
}

GameScreen::~GameScreen() {
	cout << "Destroying Game Screen" << endl;
}

void GameScreen::fillQueue() {
	while (upcoming.size() < QUEUE_SIZE) {
		auto temp = Tetromino();
		temp.loc = Vect(width / 2, 0);

		// TODO: Does this work?
		auto bound = temp.bound();
		temp.loc.y = -bound.first.y;

		upcoming.push_back(temp);
	}
}

void GameScreen::generateTetromino() {
	fillQueue();

	current = upcoming.front();
	upcoming.pop_front();
	sanitizeCurrent();

	fillQueue();
}

void GameScreen::update() {
	if (running) {
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
							// Bring out the old one and reset the coord
							auto temp = store;
							store = current;
							current = temp;

							// TODO: Move this into another method
							current.loc = Vect(width / 2, 0);
							// TODO: Does this work?
							auto bound = temp.bound();
							current.loc.y = -bound.first.y;

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

					for (int i = 0; i < current.matrix.size(); i++) {
						for (int j = 0; j < current.matrix.size(); j++) {
							int x = (int)(current.loc.x + i);
							int y = (int)(current.loc.y + j);

							// Not allowed to rotate if x or y aren't valid or there's a block there
							if (current.matrix[j][i].on && (y < 0 || y >= height || x < 0 || x >= width || grid[y][x].on)) {
								stabalize = true;
								break;
							}
						}
					}

					if (stabalize) {
						current = copy;
					} else {
						sanitizeCurrent();
					}
					
					return;
				case ALLEGRO_KEY_LEFT:
					for (int i = 0; i < current.matrix.size(); i++) {
						for (int j = 0; j < current.matrix.size(); j++) {
							int x = (int)(current.loc.x + i - 1);
							int y = (int)(current.loc.y + j);
							if (y > 0 && current.matrix[j][i].on && grid[y][x].on) {
								can_move = false;
								break;
							}
						}
					}
					if (can_move) {
						current.loc.x -= 1;
						sanitizeCurrent();
					}
					break;
				case ALLEGRO_KEY_RIGHT:
					for (int i = 0; i < current.matrix.size(); i++) {
						for (int j = 0; j < current.matrix.size(); j++) {
							int x = (int)(current.loc.x + i + 1);
							int y = (int)(current.loc.y + j);
							if (y > 0 && current.matrix[j][i].on && grid[y][x].on) {
								can_move = false;
								break;
							}
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
		for (int count = ghost.loc.y; !found && count < height; count++) {
			ghost.loc.y += 1;
			for (int i = 0; i < current.matrix.size(); i++) {
				for (int j = 0; j < current.matrix.size(); j++) {
					auto block = current.matrix[j][i];
					int x = (int)(ghost.loc.x + i);
					int y = (int)(ghost.loc.y + j);
					if (block.on && (y >= height || (y >= 0 && grid[y][x].on))) {
						// Move back up and save location
						ghost.loc.y -= 1;
						found = true;
						break;
					}
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
			for (int i = 0; i < current.matrix.size(); i++) {
				for (int j = 0; j < current.matrix.size(); j++) {
					auto block = current.matrix[j][i];
					int x = (int)(current.loc.x + i);
					int y = (int)(current.loc.y + j);
					if (block.on && (y >= height || (y >= 0 && grid[y][x].on))) {
						stabalize = true;
						break;
					}
				}
			}

			if (stabalize) {
				current.loc.y -= 1;
				
				running = false;
				for (int i = 0; i < current.matrix.size(); i++) {
					for (int j = 0; j < current.matrix.size(); j++) {
						auto block = current.matrix[j][i];
						int x = (int)(current.loc.x + i);
						int y = (int)(current.loc.y + j);
						if (block.on && y >= 0) {
							grid[y][x] = block;
							running = true;
						}
					}
				}

				if (!running) {
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
	} else {
		// If not running
		
		// Drain input
		while(!al_is_event_queue_empty(Director::input)) {
			ALLEGRO_EVENT iev;
			al_wait_for_event(Director::input, &iev);
			if (iev.type == ALLEGRO_EVENT_KEY_CHAR) {
				switch (iev.keyboard.keycode) {
				case ALLEGRO_KEY_ESCAPE:
					// TODO: display a confirmation screen
					Director::pop();
					return;
				}
			}
		}
	}
}

void GameScreen::render() {
	// Draw border
	al_draw_rectangle(start_x - line_size, start_y - line_size,
			start_x + cell_size * width + line_size, start_y + cell_size * height + line_size,
			al_map_rgb(255, 255, 255), line_size);

	// Draw held piece
	// TODO: Don't hard code 5
	al_draw_rectangle(start_x - line_size - cell_size * 5, start_y + line_size + cell_size * 5,
			start_x - line_size, start_y - line_size,
			al_map_rgb(255, 255, 255), line_size);

	if (has_stored) {
		auto bound = store.bound();
		float cx = bound.second.x - bound.first.x;
		float cy = bound.second.y - bound.first.y;
		for (int i = 0; i < current.matrix.size(); i++) {
			for (int j = 0; j < current.matrix.size(); j++) {
				auto block = current.matrix[j][i];
				al_draw_rectangle(start_x - line_size - (cell_size * 5 / 2) - cx, start_y + line_size + (cell_size * 5 / 2) + cy,
						start_x - line_size - cx, start_y - line_size - cy,
						block.color, line_size);
			}
		}
	}


	// Draw the grid. A digital frontier. I tried to picture clusters of information as they moved
	// through the computer. What did they look like? Ships? Motorcycles? Were the circuits like freeways?
	// I kept dreaming of a world I thought I'd never see. And then one day...
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

	if (running) {
		// Draw current ghost
		for (int i = 0; i < current.matrix.size(); i++) {
			for (int j = 0; j < current.matrix.size(); j++) {
				auto block = current.matrix[j][i];
				if (block.on && ghost.loc.y + j >= 0 && ghost.loc.y + j < height) {
					al_draw_rectangle(
							start_x + (ghost.loc.x + i) * cell_size + line_size,
							start_y + (ghost.loc.y + j) * cell_size + line_size,
							start_x + (ghost.loc.x + i + 1) * cell_size - line_size,
							start_y + (ghost.loc.y + j + 1) * cell_size - line_size,
							ghost.color, line_size);
				}
			}
		}

		// Draw current block
		for (int i = 0; i < current.matrix.size(); i++) {
			for (int j = 0; j < current.matrix.size(); j++) {
				auto block = current.matrix[j][i];
				if (block.on && current.loc.y + j >= 0 && current.loc.y + j < height) {
					al_draw_rectangle(
							start_x + (current.loc.x + i) * cell_size + line_size,
							start_y + (current.loc.y + j) * cell_size + line_size,
							start_x + (current.loc.x + i + 1) * cell_size - line_size,
							start_y + (current.loc.y + j + 1) * cell_size - line_size,
							block.color, line_size);
				}
			}
		}
	} else {
		// If not running
	}
}

void GameScreen::sanitizeCurrent() {
	for (int i = 0; i < current.matrix.size(); i++) {
		for (int j = 0; j < current.matrix.size(); j++) {
			auto block = current.matrix[j][i];

			// TODO: Infinite loop possibility if block is way too wide
			// TODO: Make so this won't allow rotating into blocks
			if (block.on) {
				while (current.loc.x + i < 0) {
					current.loc.x += 1;
				}

				while (current.loc.x + i >= width) {
					current.loc.x -= 1;
				}
			}
		}
	}
}
