#ifndef ENTITY
#define ENTITY

using namespace tStorage;

const double FLOOR_Y = 450;

int puddle_count = 0;

class Grabable {

};

class LSObject {
public:

	// Speed and acceleration
	double speed_y = 0;
	double acceleration_y = 0;

	RectangleShape box;
	Texture *mainTexture;

	LSObject(float w, float h, Texture *__mainTexture) {
		box.setSize( { w, h });
		box.setOrigin(w / 2, h / 2);
		mainTexture = __mainTexture;
		box.setTexture(mainTexture);
	}

	virtual ~LSObject() {

	}

	Drawable& getSprite() {
		return box;
	}

	virtual void update(double dt, tVector<LSObject*> *objects,
			list<LSObject*> *part) {
		speed_y += acceleration_y * dt;
		acceleration_y += speed_y * dt;
		box.move(0, speed_y * dt);
	}

	void freeze() {
		speed_y = 0;
		acceleration_y = 0;
	}

	void move(int dx, int dy) {
		box.move(dx, dy);
	}

	void rotate(double angle) {
		box.rotate(angle);
	}

};

class Drop: public LSObject {
public:
	long long creationTime = 0;

	Drop(double x, double y) :
			LSObject(20.0, 20.0, &DROP) {
		box.setPosition(x, y);
		creationTime = currTime();
	}

	void update(double dt, tVector<LSObject*> *objects, list<LSObject*> *part) {
		LSObject::update(dt, objects, part);
		if (creationTime + 100 < currTime()) {
			part->remove(this);
		}
	}
};

class Puddle: public LSObject {
public:
	Puddle(double x, double y) :
			LSObject(600, 200, &PUDDLE) {
		box.setPosition(x, y);
	}
};

class Rag: public LSObject, public Grabable {
public:
	Rag() :
			LSObject(250, 250, &RAG) {
		move(1000, 300);
	}

	void update(double dt, tVector<LSObject*> *objects, list<LSObject*> *part) {
		LSObject::update(dt, objects, part);
		for (auto it = part->begin(); it != part->end(); it++) {
			if (collision::areColliding(box, (*it)->box)) {
				box.rotate((rand() % 3) - 1);
				if (rand() % 1000 == 1) {
					part->remove(*it);
				}
			}
		}
	}
};

template<typename T> T* isChildren(LSObject *obj) {
	try {
		T *res = NULL;
		if ((res = dynamic_cast<T*>(obj)) != NULL) {
			return res;
		}
		return 0;
	} catch (...) {
		return 0;
	}
}

class LSGravityObject: public LSObject {

public:
	// Speed
	double crashSpeed = 3000;
	bool crashed = 0;

	double floor_y = 0;

	// Crashes
	Texture *crashedTexture;

	LSGravityObject(float w, float h, Texture *mainT, Texture *__crashedTexture) :
			LSObject(w, h, mainT) {
		crashedTexture = __crashedTexture;
	}

	void setCrashSpeed(double sp) {
		crashSpeed = sp;
	}

	virtual void update(double dt, tVector<LSObject*> *objects,
			list<LSObject*> *part) {
		LSObject::update(dt, objects, part);
		RectangleShape floor;
		floor.setPosition(-1000, FLOOR_Y + floor_y);
		floor.setSize( { 2000, 10 });

		if (collision::areColliding(floor, box)) {
			if (speed_y > crashSpeed && speed_y > 0) {
				this->crash(dt, objects, part);
			}
			speed_y = 0;
			acceleration_y = 0;
		} else {
			acceleration_y = 9800 / 4;
		}
	}

	void setFloorY(double val) {
		floor_y = val;
	}

	virtual void crash(double dt, tVector<LSObject*> *objects,
			list<LSObject*> *part) {
		crashed = 1;
		box.setTexture(crashedTexture);
	}

};

class LSGravityWithLiquidObject: public LSGravityObject {
public:
	// Liquid
	int totalLiquidLevels;
	Texture *liquidLevelsTextures;
	bool textureReversed = 0;

	// Physics
	double currentWaterLevel = 1; // [0, 1]
	double expirationFactor = 1; // скорость истечения
	double capacity = 14; // милилитр

	LSGravityWithLiquidObject(float w, float h, int total_ll,
			Texture *liquid_level, Texture *crashedTexture) :
			LSGravityObject(w, h, liquid_level, crashedTexture) {
		totalLiquidLevels = total_ll;
		liquidLevelsTextures = liquid_level;
	}

	void crash(double dt, tVector<LSObject*> *objects, list<LSObject*> *part) {
		LSGravityObject::crash(dt, objects, part);
		if (currentWaterLevel > 0) {
			makePuddle(dt, objects, part);
			currentWaterLevel = 0;
		}
	}

	void update(double dt, tVector<LSObject*> *objects, list<LSObject*> *part) {
		LSGravityObject::update(dt, objects, part);

		if (box.getRotation() > 270) {
			box.setRotation(0);
		}
		if (box.getRotation() > 135) {
			box.setRotation(135);
		}

		if (crashed == 1) {
			return;
		}

		updateExpiration(dt);
		updateTransfusion(dt, objects, part);

		box.setTexture(getCurrentTexture());
	}

private:

	void makePuddle(double dt, tVector<LSObject*> *objects,
			list<LSObject*> *part, double x = -10000, double y = -10000) {
		if (x == -10000) {
			part->push_back(
					new Puddle(box.getPosition().x, box.getPosition().y));
		} else {
			part->push_back(new Puddle(x, y));
		}
	}

	void transfuse(double ml) {
		currentWaterLevel += (ml / capacity);
		currentWaterLevel = max(0.0, min(1.0, currentWaterLevel));
	}

	void updateTransfusion(double dt, tVector<LSObject*> *objects,
			list<LSObject*> *part) {
		float angle = box.getRotation();
		// right corner
		if (angle > 45.0 && angle <= 180.0) {
			double dw = min(dt * capacity * (angle - 45.0) / 90.0,
					currentWaterLevel * capacity);
			if (dw == 0) {
				return;
			}
			transfuse(-dw);
			Vector2f p1 = box.getTransform().transformPoint(box.getPoint(1));
			RectangleShape p( { 10, 10 });
			p.setPosition(p1);
			for (double y = 0; y < 3000; y += 10) {
				if (FLOOR_Y - 100 < p.getPosition().y) {
					if (rand() % 100 == 6) {
						makePuddle(dt, objects, part, p.getPosition().x,
								p.getPosition().y);
					}
					break;
				}
				p.move(0, 10);
				if (std::rand() % 70 == 1) {
					part->push_back(
							new Drop(p.getPosition().x, p.getPosition().y));
				}
				for (unsigned i = 0; i < objects->getSize(); i++) {
					LSGravityWithLiquidObject *o = isChildren<
							LSGravityWithLiquidObject>((*objects)[i]);
					if (o == NULL || o == this) {
						continue;
					}
					if (collision::areColliding((*objects)[i]->box, p)) {
						o->transfuse(dw);
						return;
					}
				}
			}
		}
	}

	double expir_dw(double dt) {
		double R = 1.6 / 100.0;
		double r = 0.4 / 100.0;
		double nu = 1.06;
		double l = 0.01;
		double ro = 1000;
		double g = 9.8;
		cout << ( r * r * r * r * ro * g / (8 * l * R * R)  ) << "\n";
		return - r * r * g * r * r * ro * currentWaterLevel * dt
				/ (R * R * 8 * nu * l);
	}

	void updateExpiration(double dt) {
		double dw = expir_dw(dt) * expirationFactor;
		currentWaterLevel += dw;
		currentWaterLevel = max(0.0, currentWaterLevel);
	}

	Texture* getCurrentTexture() {
		if (textureReversed == 1) {
			return liquidLevelsTextures
					+ min(
							(int) (totalLiquidLevels
									- totalLiquidLevels * currentWaterLevel),
							totalLiquidLevels - 1);
		} else {
			return liquidLevelsTextures
					+ min((int) (currentWaterLevel * totalLiquidLevels),
							totalLiquidLevels - 1);
		}
	}

protected:

	void setCapacity(double cap) {
		capacity = cap;
	}

	void setLiquidTexturesReversed(bool val) {
		textureReversed = val;
	}

	void setExpirationFactor(double val) {
		expirationFactor = val;
	}

};

class Background: public LSObject {
public:
	Background() :
			LSObject(3000, 1800, &BACKGROUND) {

	}
};

class Ruler: public LSGravityObject, public Grabable {
public:
	Ruler() :
			LSGravityObject(50, 360, &RULER, &RULER) {
		box.rotate(-90);
		move(-500, 0);
	}
};

class Beaker: public LSGravityWithLiquidObject {
public:
	Beaker() :
			LSGravityWithLiquidObject(500.0, 900.0, 71, BEAKER_LIQ,
					&BEAKER_CRASHED) {
		setLiquidTexturesReversed(1);
	}
};

class Glass: public LSGravityWithLiquidObject, public Grabable {
public:
	Glass() :
			LSGravityWithLiquidObject(100, 200, 45, GLASS_LIQ, &GLASS_BROKEN) {
		setCapacity(60);
		setExpirationFactor(0);
		setCrashSpeed(1300);
		move(500, -0);
		setFloorY(-100);
	}
};

#endif
