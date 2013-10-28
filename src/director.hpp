#pragma once

#include <allegro5/allegro.h>

#include <memory>

class Screen {
public:
	virtual ~Screen() {};

	// Logic
	virtual void update() = 0;

	// Render
	virtual void render() = 0;
};

typedef std::shared_ptr<Screen> screen_ptr;

namespace Director {
	// Set up the window and game space
	void init();

	// Main game loop
	void run();

	// Clean up any assets
	void cleanup();

	// Save the config file to disk
	bool flushConfig();

	// Used when we change the screen size
	void resetDisplay();

	// Screen management
	void push(Screen *s);
	void push(screen_ptr s);
	screen_ptr pop();
	screen_ptr replace(Screen *s);
	screen_ptr replace(screen_ptr s);

	// Useful variables
	extern ALLEGRO_DISPLAY *display;
	extern ALLEGRO_EVENT_QUEUE *input;
	extern ALLEGRO_CONFIG *config;
	extern int width;
	extern int height;
	extern float targetFPS;
	extern float tickLength;
	extern bool running;
};
