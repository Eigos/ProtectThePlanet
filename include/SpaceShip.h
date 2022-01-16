#pragma once
#include "Utility.hpp"

struct SpaceShipOptions {
	Object objectVariables;
	Drawable* drawable;
};

class SpaceShip : public Object {

private:
	const static int DefWidth = 70;
	const static int DefHeight = 46;
	
	Drawable* texture;
	
public:

	const static float MaxSpeed;
	const static float Acceleration;
	const static float DeAcceleration;
	const static float TurnSpeed;

	int HP = 4;
	bool canShoot = true;

	SpaceShip(const SpaceShipOptions& ShipOptions);

	SpaceShip() {};

	void MoveForward();

	void Stop();

	void TurnLeft(float dt = 1.0f);

	void TurnRight(float dt = 1.0f);

	Object* Shoot();

	void setTexture(Drawable* newTexture);

	Drawable* getTexture();

	const SizeInfo getSize();

};






