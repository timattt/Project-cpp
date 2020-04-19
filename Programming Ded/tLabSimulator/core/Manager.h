#ifndef MANAGER
#define MANAGER

#include <sys/time.h>

long long currTime() {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000; //get current timestamp in milliseconds
	return ms;
}

struct input_info {
	bool left_mouse_button_pressed = 0;
	bool right_mouse_button_pressed = 0;
	bool mid_mouse_button_pressed = 0;
	int prev_x;
	int prev_y;

	int mx;
	int my;
};

#include "../../tStorage/tVector.h"
#include "../Collision.h"
#include <bits/stdc++.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "../../tLabSimulator/core/Textures.h"
#include "../universe/LSObject.h"
#include "../universe/Universe.h"

using namespace std;

// Dimensions
const int width = 3000;
const int height = 1800;

class Manager {
	Universe *universe = NULL;
	RenderWindow *window = NULL;
	View *userView = NULL;

	// Mouse
	bool leftMouseButtonPressed = 0;

public:
	Manager(RenderWindow *wnd) {
		loadTextures();
		window = wnd;
		userView = new View(
				sf::FloatRect(-width / 2, -height / 2, width, height));

		universe = new Universe();
		window->setView(*userView);

	}
	input_info getInput() {
		input_info res;
		res.prev_x = res.mx;
		res.prev_y = res.my;

		res.mx = sf::Mouse::getPosition(*window).x - width / 2;
		res.my = sf::Mouse::getPosition(*window).y - height / 2;

		res.left_mouse_button_pressed = leftMouseButtonPressed;
		leftMouseButtonPressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);

		res.right_mouse_button_pressed = sf::Mouse::isButtonPressed(
				sf::Mouse::Right);
		res.mid_mouse_button_pressed = sf::Mouse::isButtonPressed(
				sf::Mouse::Middle);

		return res;
	}
	void update(double dt) {
		universe->update_control(getInput(), dt);
		universe->update(dt);

	}
	~Manager() {
		delete universe;
	}

	void render() {
		universe->render(window);
	}

	unsigned getWindowWidth() {
		return window->getSize().x;
	}

	unsigned getWindowHeight() {
		return window->getSize().y;
	}

};

void runSimulator() {
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");
	Manager *manager = new Manager(&window);

	cout << "init finished!\n";
	cout.flush();

	long long lastUpdate = currTime();

	sf::Text text;

	// Start the game loop
	while (window.isOpen()) {
		// Process events
		sf::Event event;
		while (window.pollEvent(event)) {
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();
		}
		// Clear screen
		window.clear();
		//window.draw(spr);

		manager->update((double) (currTime() - lastUpdate) / 1000.0);
		lastUpdate = currTime();
		manager->render();
		window.draw(text);

		// Update the window
		window.display();
	}
	delete manager;
}

#endif
