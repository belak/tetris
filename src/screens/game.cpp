#include "game.hpp"
#include "menu.hpp"

#include <allegro5/allegro_primitives.h>

#include <iostream>
#include <cmath>

using namespace std;

#define FALL_LENGTH 0.5
#define QUEUE_SIZE 5

// TODO:
// Display upcoming
// Make sure move locks are done in right places
// 2 piece buffer above grid (and change death to come with this if something spawns on top)
// Options for lock style, etc

GameScreen::GameScreen() {
	cout << "Creating Game Screen" << endl;

	// Make sure the grid is large enough
	cleanGrid();

	cell_size = (Director::height - 2 * margin) / height;
	
	// This recalculates the margin based on the cell size so we can center it
	margin = (Director::height - cell_size * height) / 2;

	start_x = ((float)Director::width / 2.0) - (width * cell_size / 2);
	start_y = margin;

	generateTetromino();
	calculateGhost();
}

GameScreen::~GameScreen() {
	cout << "Destroying Game Screen" << endl;
}

bool GameScreen::validPosition() {
	for (int i = 0; i < current.matrix.size(); i++) {
		for (int j = 0; j < current.matrix.size(); j++) {
			int x = (int)(current.loc.x + i);
			int y = (int)(current.loc.y + j);

			// Not allowed to rotate if x or y aren't valid or there's a block there
			// Note that we ARE allowed to rotate above the grid
			if (current.matrix[j][i].on && (y >= height || x < 0 || x >= width || (y >= 0 && grid[y][x].on))) {
				return false;
			}
		}
	}
	return true;
}

void GameScreen::cleanGrid() {
	// Remove lines if they're full
	// NOTE: we do this in reverse order
	// so we can do it all in one swoop without
	// the ids changing on us
	int num_cleared = 0;
	for (int i = grid.size() - 1; i >= 0; i--) {
		bool on = true;
		for (int j = 0; j < grid[i].size(); j++) {
			// If we find one that isn't on, we can't remove it
			if (!grid[i][j].on) {
				on = false;
				break;
			}
		}

		if (on) {
			cout << "Removing line" << endl;
			num_cleared += 1;
			grid.erase(grid.begin() + i);

			// Update variables
			int tmp = level;
			lines += 1;
			level = ceil((float) lines / 10);
			if (level > 10) {
				level = 10;
			}
			if (level != tmp) {
				cout << "Now on level " << level << endl;
			}
		}
	}

	if (num_cleared == 4) {
		cout << "Tetris!" << endl;
	}

	// Push new lines onto the beginning until we're at the right size
	while (grid.size() < height) {
		grid.insert(grid.begin(), vector<Block>(width));
	}

}

void GameScreen::resetCurrent() {
	auto bound = current.bound();
	current.loc = Vect(
			width / 2 - (bound.second.x - bound.first.x) / 2 - 1,
			-(bound.second.y - bound.first.y) - 1);
	sanitizeCurrent();
}

void GameScreen::fillQueue() {
	// Make sure we have enough tetrominos for the queue
	while (upcoming.size() < QUEUE_SIZE) {
		upcoming.push_back(Tetromino());
	}
}

void GameScreen::generateTetromino() {
	// Make sure we have something to pop
	fillQueue();

	// Grab the current and put it in the right place
	current = upcoming.front();
	upcoming.pop_front();
	resetCurrent();

	fillQueue();
}

void GameScreen::calculateGhost() {
	// Copy ghost from the current
	ghost = current;

	// Make sure the color is right
	ghost.color = al_map_rgb(128, 128, 128);

	// Loop through from the current position
	bool found = false;
	for (int count = ghost.loc.y; !found && count < height; count++) {
		// Move down a line
		ghost.loc.y += 1;

		// If one or more blocks are invalid, move back up and mark that spot as final
		for (int i = 0; !found && i < ghost.matrix.size(); i++) {
			for (int j = 0; !found && j < ghost.matrix.size(); j++) {
				auto block = ghost.matrix[j][i];
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
}

void GameScreen::update() {
	if (running) {
		bool hyper_speed = false;
		float timer_freq = (11 - level) * 0.05;

		auto copy = current;
		while(!al_is_event_queue_empty(Director::input)) {
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

							resetCurrent();
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
				bool found = false;

				switch (iev.keyboard.keycode) {
				case ALLEGRO_KEY_ESCAPE:
					// TODO: display a confirmation screen
					// Maybe just pause the game
					Director::pop();
					return;
				case ALLEGRO_KEY_Z:
					// A counter clockwise rotation is just rotating right 3 times
					current.rotate();
					current.rotate();
				case ALLEGRO_KEY_UP:
					current.rotate();

					// Check all the wall kicks
					// Take the first one that's valid
					for (auto kick : current.kicks[current.current_kicks]) {
						// Note that because the board is 0 at the top in stead of the bottom
						// we subtract the current loc
						current.loc.x += kick.first;
						current.loc.y -= kick.second;
						if (validPosition()) {
							found = true;
							break;
						} else {
							current.loc = copy.loc;
						}
					}

					// If we don't have a valid kick, reset the tetromino
					if (found) {
						sanitizeCurrent();
						lock_timer = 0.0;
					} else {
						current = copy;
					}
					break;
				case ALLEGRO_KEY_LEFT:
					current.loc.x -= 1;
					if (validPosition()) {
						sanitizeCurrent();
						lock_timer = 0.0;
					} else {
						current.loc.x += 1;
					}
					break;
				case ALLEGRO_KEY_RIGHT:
					current.loc.x += 1;
					if (validPosition()) {
						sanitizeCurrent();
						lock_timer = 0.0;
					} else {
						current.loc.x -= 1;
					}
					break;
				case ALLEGRO_KEY_SPACE:
					hyper_speed = true;
					break;
				}
			}
		}

		// If holding down, shrink the time before moving down
		if (speed_run) {
			timer_freq /= 4;
		}

		// Now that input is done, change the ghost
		calculateGhost();

		// If jumping down, replace current's loc with the ghost's
		// and force a stabilize
		if (hyper_speed) {
			move_timer = FALL_LENGTH;
			current.loc = ghost.loc;
			lock_timer = timer_freq;
		}

		// Move the timers forward
		move_timer += Director::tickLength;
		lock_timer += Director::tickLength;

		// If it's time for current to move down, do it
		if (move_timer > timer_freq) {
			// Conveniently enough, we already know where this needs to go
			// The ghost will be in the final location
			if (current.loc == ghost.loc) {
				// SRS has a separate lock timer, so if a movement happens, we can't stabilize
				if (lock_timer > timer_freq) {
					running = false;

					// If all of the positions are above the top, we just lost
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

					// If we just lost, we don't need to run any more calculations
					if (!running) {
						return;
					}

					// Make sure we don't have anything to remove
					cleanGrid();

					// Pop from the queue and recalculate the ghost
					generateTetromino();
					calculateGhost();

					// Block is now stabilized
					lock_timer = 0.0;
					move_timer = 0.0;
				}
			} else {
				// Reset the movement timer every time we move
				move_timer = 0.0;
				current.loc.y += 1;
			}
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
	al_draw_rectangle(
			start_x - line_size,
			start_y - line_size,
			start_x + cell_size * width + line_size,
			start_y + cell_size * height + line_size,
			al_map_rgb(255, 255, 255), line_size);

	// Draw held piece
	// TODO: Don't hard code 5
	int cell_width = 5;
	al_draw_rectangle(
			start_x - line_size - cell_size * cell_width,
			start_y - line_size,
			start_x - line_size,
			start_y + line_size + cell_size * cell_width,
			al_map_rgb(255, 255, 255), line_size);

	if (has_stored) {
		auto bound = store.bound();
		float cx = bound.second.x - bound.first.x;
		float cy = bound.second.y - bound.first.y;

		for (int i = 0; i < store.matrix.size(); i++) {
			for (int j = 0; j < store.matrix.size(); j++) {
				auto block = store.matrix[j][i];
				if (block.on) {
					// This is all drawn using black magic.
					al_draw_rectangle(
							start_x - line_size + cell_size * (i - cx / 2 - 3 - bound.first.x),
							start_y - line_size + cell_size * (j - cy / 2 + 2 - bound.first.y),
							start_x - line_size + cell_size * (i - cx / 2 - 3 - bound.first.x + 1),
							start_y - line_size + cell_size * (j - cy / 2 + 2 - bound.first.y + 1),
							block.color, line_size);
				}
			}
		}
	}

	// Draw upcoming pieces
	int k = 0;
	for (auto store : upcoming) {
		al_draw_rectangle(
				start_x + cell_size * width + line_size,
				start_y - line_size + k * (cell_size * cell_width + line_size),
				start_x + cell_size * width + line_size + cell_size * cell_width,
				start_y /*- line_size*/ + cell_size * cell_width + k * (cell_size * cell_width + line_size),
				al_map_rgb(255, 255, 255), line_size);

		auto bound = store.bound();
		float cx = bound.second.x - bound.first.x;
		float cy = bound.second.y - bound.first.y;

		for (int i = 0; i < store.matrix.size(); i++) {
			for (int j = 0; j < store.matrix.size(); j++) {
				auto block = store.matrix[j][i];
				if (block.on) {
					// This is all drawn using black magic.
					// This one even more so than the stored piece
					al_draw_rectangle(
							start_x + cell_size * width + 0.8 * line_size + cell_size * (i - cx / 2 - 3 - bound.first.x) + cell_size * cell_width,
							start_y - line_size + cell_size * (j - cy / 2 + 2 - bound.first.y) + k * (cell_size * cell_width + line_size),
							start_x + cell_size * width + 0.8 * line_size + cell_size * (i - cx / 2 - 3 - bound.first.x + 1) + cell_size * cell_width,
							start_y - line_size + cell_size * (j - cy / 2 + 2 - bound.first.y + 1) + k * (cell_size * cell_width + line_size),
							block.color, line_size);
				}
			}
		}

		k += 1;
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
				if (block.on && (int) ghost.loc.y + j >= 0 && (int) ghost.loc.y + j < height) {
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
				if (block.on && (int) current.loc.y + j >= 0 && (int) current.loc.y + j < height) {
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
