#pragma once

#include <allegro5/allegro.h>

class Vect {
public:
	Vect() {
		Vect(0, 0);
	}

	Vect(int in_x, int in_y) {
		x = in_x;
		y = in_y;
	}

	bool operator==(const Vect &v) {
		return v.x == x && v.y == y;
	}

	int x;
	int y;
};


bool al_config_has_section(ALLEGRO_CONFIG *c, const char *s);
void al_set_config_value_f(ALLEGRO_CONFIG *c, const char *s, const char *v, const char *fmt, ...);
