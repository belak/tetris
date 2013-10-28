#pragma once

#include "../director.hpp"

class SplashScreen : public Screen {
public:
	SplashScreen();
	~SplashScreen();

	void update();
	void render();

private:
	float timer = 0.0;
};
