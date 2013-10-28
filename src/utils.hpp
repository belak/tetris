#pragma once

#include <allegro5/allegro.h>

class Vect {
public:
	Vect() {
		Vect(0, 0);
	}

	Vect(float in_x, float in_y) {
		x = in_x;
		y = in_y;
	}

	float x;
	float y;
};


bool al_config_has_section(ALLEGRO_CONFIG *c, const char *s);
void al_set_config_value_f(ALLEGRO_CONFIG *c, const char *s, const char *v, const char *fmt, ...);
