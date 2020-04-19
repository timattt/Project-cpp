#ifndef WORLD
#define WORLD

using namespace tStorage;

class Universe {
	tVector<LSObject*> objects;
	list<LSObject*> particles;

	// Objects
	Beaker *beaker;
	Background *background;
	Ruler *ruler;
	Glass *glass;
	Rag *rag;

public:

	Universe() {
		beaker = new Beaker();
		background = new Background();
		ruler = new Ruler();
		glass = new Glass();
		rag = new Rag();

		objects.tPush_back(beaker);
		objects.tPush_back(glass);
		objects.tPush_back(ruler);
		objects.tPush_back(rag);
	}

	void addObject(LSObject *ent) {
		objects.tPush_back(ent);
	}

	void update(double time_delta) {
		for (unsigned i = 0, sz = objects.getSize(); i < sz; i++) {
			objects[i]->update(time_delta, &objects, &particles);
			if (sz - 1 == objects.getSize()) {
				sz--;
				i--;
			}
		}
		for (std::list<LSObject*>::iterator it = particles.begin();
				it != particles.end(); it++) {
			(*it)->update(time_delta, &objects, &particles);
		}
	}

	void update_control(input_info inf, double dt) {
		int dx = inf.mx - inf.prev_x;
		int dy = inf.my - inf.prev_y;

		RectangleShape mouse;
		mouse.setPosition(inf.mx, inf.my);
		mouse.setSize( { 10, 10 });

		for (unsigned i = 0, sz = objects.getSize(); i < sz; i++) {
			if (isChildren<Grabable>(objects[i])
					&& collision::areColliding(objects[i]->box, mouse)) {
				if (inf.left_mouse_button_pressed) {
					objects[i]->move(dx, dy);
					objects[i]->freeze();
				}
				if (inf.right_mouse_button_pressed) {
					objects[i]->rotate(dt * 50.0);
					objects[i]->freeze();
				}
				if (inf.mid_mouse_button_pressed) {
					objects[i]->rotate(-dt * 50.0);
					objects[i]->freeze();
				}
				break;
			}
		}

	}

	~Universe() {

	}

	void render(RenderWindow *wnd) {
		wnd->draw(background->getSprite());
		for (std::list<LSObject*>::iterator it = particles.begin();
				it != particles.end(); it++) {
			wnd->draw((*it)->getSprite());
		}
		for (unsigned i = 0, sz = objects.getSize(); i < sz; i++) {
			wnd->draw(objects[i]->getSprite());
		}

	}

};

#endif
