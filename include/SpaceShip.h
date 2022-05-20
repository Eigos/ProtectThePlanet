#pragma once
#include "Utility.hpp"
#include "bullet.h"

enum WeaponType {
	NoType = -1,
	Rocket = 0,
	LaserShot,
	PlasmaShot,
	LaserCutter
};

class SpaceShip : public Object {

private:
	Bullet* ShootBasic();

	BulletType bulletTypeCurrent;
	
	BulletType bulletTypeFirst;
	BulletType bulletTypeSecond;

public:

	const static int DefWidth = 70;
	const static int DefHeight = 46;

	const static float MaxSpeed;
	const static float Acceleration;
	const static float DeAcceleration;
	const static float TurnSpeed;

	int HP = 4;
	bool canShoot = true;

	Drawable* BulletTextureFirst;
	Drawable* BulletTextureSecond;

	BulletType getBulletTypeCurrent();
	BulletType getBulletTypeFirst();
	BulletType getBulletTypeSecond();
	Drawable* getBulletCurrentTexture();

	void BulletTypeSwap();
	void setBulletTypeFirst(BulletType newType);
	void setBulletTypeSecond(BulletType newType);



	SpaceShip();

	void MoveForward();

	void Stop();

	void TurnLeft(float dt = 1.0f);

	void TurnRight(float dt = 1.0f);

	Bullet* Shoot();

	const SizeInfo getSize();

};






