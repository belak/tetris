#include "utils.hpp"

#include <cstdio>

bool al_config_has_section(ALLEGRO_CONFIG *c, const char *s) {
	ALLEGRO_CONFIG_SECTION *temp = NULL;
	char const *name;
	if ((name = al_get_first_config_section(c, &temp)) != nullptr && strcmp(s, name) == 0) {
		return true;
	}

	while ((name = al_get_next_config_section(&temp)) != nullptr) {
		if (strcmp(s, name) == 0) {
			return true;
		}
	}

	return false;
}

void al_set_config_value_f(ALLEGRO_CONFIG *c, const char *s, const char *v, const char *fmt, ...) {
	va_list args;
	char *str = NULL;
	va_start(args, fmt);
	vasprintf(&str, fmt, args);
	va_end(args);

	al_set_config_value(c, s, v, str);

	free(str);
}
