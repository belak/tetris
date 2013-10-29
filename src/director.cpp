#include "director.hpp"
#include "utils.hpp"

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>

#include <list>
#include <iostream>

#define CONFIG_FILENAME "settings.ini"
#define DEFAULT_WIDTH 500
#define DEFAULT_HEIGHT 600

using namespace std;

namespace Director {
	bool running = false;

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *input = NULL;
	ALLEGRO_EVENT_QUEUE *events = NULL; // This should not be exposed
	ALLEGRO_TIMER *timer = NULL; // This should not be exposed
	ALLEGRO_CONFIG *config = NULL;

	float targetFPS = 60.0;
	float tickLength = 1.0 / targetFPS;

	int width = 0;
	int height = 0;

	list<screen_ptr> screens;

	void init() {
		running = true;
		if (!al_init()) {
			cout << "Allegro failed to start" << endl;
			running = false;
			return;
		}

		// Load up extra modules
		if (!al_install_keyboard()) {
			cout << "Keyboard error" << endl;
			running = false;
			return;
		}

		if (!al_install_mouse()) {
			cout << "Mouse error" << endl;
			running = false;
			return;
		}

		if (!al_install_audio()) {
			cout << "Audio error" << endl;
			running = false;
			return;
		}

		al_init_image_addon();
		al_init_font_addon();
		al_init_ttf_addon();
		al_init_primitives_addon();
		al_init_acodec_addon();

		if (!(events = al_create_event_queue())) {
			cout << "Failed to create event queue" << endl;
			running = false;
			return;
		}

		if (!(input = al_create_event_queue())) {
			cout << "Failed to create input queue" << endl;
			running = false;
			return;
		}

		if (!(timer = al_create_timer(tickLength))) {
			cout << "Failed to create event timer" << endl;
			running = false;
			return;
		}

		al_register_event_source(events, al_get_timer_event_source(timer));

		// Register keyboard and mouse as input
		al_register_event_source(input, al_get_keyboard_event_source());
		al_register_event_source(input, al_get_mouse_event_source());

		// Config stuff
		ALLEGRO_PATH *config_path = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);

		al_set_path_filename(config_path, CONFIG_FILENAME);

		config = al_load_config_file(al_path_cstr(config_path, ALLEGRO_NATIVE_PATH_SEP));
		if (!config) {
			config = al_create_config();
		}

		if (!al_config_has_section(config, "display")) {
			cout << "Config missing display section" << endl;
			// TODO: Add config comments
			width = DEFAULT_WIDTH;
			height = DEFAULT_HEIGHT;
		} else {
			// TODO: Make sure width and height aren't too big either
			width = atoi(al_get_config_value(config, "display", "width"));
			height = atoi(al_get_config_value(config, "display", "height"));
			if (width <= 0) {
				width = DEFAULT_WIDTH;
			}
			if (height <= 0) {
				height = DEFAULT_HEIGHT;
			}
		}

		// Write our values out to the config file
		al_set_config_value_f(config, "display", "width", "%d", width);
		al_set_config_value_f(config, "display", "height", "%d", height);

		if (!flushConfig()) {
			cout << "Failed to save config file" << endl;
			running = false;
			return;
		}

		al_destroy_path(config_path);

		// Make sure we have a screen to play with
		resetDisplay();
	}

	void run() {
		if (timer != nullptr) {
			al_start_timer(timer);
		}

		ALLEGRO_EVENT ev;
		while (running) {
			al_wait_for_event(events, &ev);

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				running = false;
				break;
			}

			screens.back()->update();

			// Simple frame skipping
			// TODO: If game is running too slow, this won't display anything
			if (running && al_is_event_queue_empty(events)) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
				screens.back()->render();
				al_flip_display();
			}
		}

		if (timer != nullptr) {
			al_stop_timer(timer);
		}

	}

	void cleanup() {
		while (!screens.empty()) {
			pop();
		}
	}

	bool flushConfig() {
		ALLEGRO_PATH *config_path = al_get_standard_path(ALLEGRO_USER_SETTINGS_PATH);
		if (!al_make_directory(al_path_cstr(config_path, ALLEGRO_NATIVE_PATH_SEP))) {
			al_destroy_path(config_path);
			return false;
		}

		al_set_path_filename(config_path, CONFIG_FILENAME);

		if (!al_save_config_file(al_path_cstr(config_path, ALLEGRO_NATIVE_PATH_SEP), config)) {
			cout << "Failed to save config file" << endl;
			al_destroy_path(config_path);
			return false;
		}

		al_destroy_path(config_path);
		return true;
	}

	void resetDisplay() {
		if (display != nullptr) {
			// TODO: Make sure we don't need to deregister the window's event source
			al_destroy_display(display);
		}

		if (!(display = al_create_display(width, height))) {
			cout << "Failed to create allegro display" << endl;
			running = false;
			return;
		}

		al_register_event_source(events, al_get_display_event_source(display));
	}

	// Screen Management
	void push(Screen *s) {
		push(screen_ptr(s));
	}

	void push(screen_ptr s) {
		screens.push_back(s);
	}

	screen_ptr pop() {
		auto tmp = screens.back();
		screens.pop_back();
		return tmp;
	}

	screen_ptr replace(Screen *s) {
		return replace(screen_ptr(s));
	}

	screen_ptr replace(screen_ptr s) {
		auto tmp = pop();
		push(s);
		return tmp;
	}
};
