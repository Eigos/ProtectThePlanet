#include "SpaceShip.h"
#include <math.h>

const float SpaceShip::MaxSpeed = 360.0f; // def: 3.0f
const float SpaceShip::Acceleration = 0.96f; // hardcoded part!!! def: 0.008f
const float SpaceShip::DeAcceleration = Acceleration / 2; // hardcoded part!!! def: 0.004f 
const float SpaceShip::TurnSpeed = 180.0f; // def: 1.5f

SpaceShip::SpaceShip() {

	Drawable* BulletTextureFirst = nullptr;
	Drawable* BulletTextureSecond= nullptr;

	BulletType bulletTypeCurrent = BulletType::NoType;
	BulletType bulletTypeFirst	 = BulletType::NoType;
	BulletType bulletTypeSecond	 = BulletType::NoType;

}

BulletType SpaceShip::getBulletTypeCurrent()
{

	return bulletTypeCurrent;
}

BulletType SpaceShip::getBulletTypeFirst()
{
	return bulletTypeFirst;
}

BulletType SpaceShip::getBulletTypeSecond()
{
	return bulletTypeSecond;
}

Drawable* SpaceShip::getBulletCurrentTexture()
{

	if (bulletTypeCurrent == bulletTypeFirst) {
		return BulletTextureFirst;
	}

	return BulletTextureSecond;

}

void SpaceShip::BulletTypeSwap()
{
	if (bulletTypeCurrent == bulletTypeFirst) {
		bulletTypeCurrent = bulletTypeSecond;
	}
	else {
		bulletTypeCurrent = bulletTypeFirst;
	}
}

void SpaceShip::setBulletTypeFirst(BulletType newType)
{
	if (bulletTypeCurrent == bulletTypeFirst) {
		bulletTypeCurrent = newType;
	}
	bulletTypeFirst = newType;
}

void SpaceShip::setBulletTypeSecond(BulletType newType)
{
	if (bulletTypeCurrent == bulletTypeSecond) {
		bulletTypeCurrent = newType;
	}
	bulletTypeSecond = newType;
}

void SpaceShip::MoveForward()
{
	//Update velocity
	if (Movement.velocity < MaxSpeed) {
		Movement.velocity += Acceleration;
	}
}

void SpaceShip::TurnLeft(float dt)
{
	float rotation = -1.0f * DegreeToRadianF(TurnSpeed) * dt;

	Movement.direction += rotation;

	if (Movement.direction <= -2.0f * PI) {
		Movement.direction = Movement.direction + 2.0f * PI;
	}


	CollMask.RotateMask(rotation);
}

void SpaceShip::TurnRight(float dt)
{


	float rotation = DegreeToRadianF(TurnSpeed) * dt;

	Movement.direction += rotation;

	if (Movement.direction >= 0) {
		Movement.direction = Movement.direction - 2.0f * PI;
	}

	CollMask.RotateMask(rotation);

}

void SpaceShip::Stop() {

	Movement.velocity -= DeAcceleration;
	Movement.velocity = Clip(Movement.velocity, 0.0f, MaxSpeed);

}

Bullet* SpaceShip::ShootBasic() {
	Bullet* bullet = new Bullet;
	bullet->Movement.direction = this->Movement.direction;
	bullet->Position.x = this->Position.x + (Size.widht / 2) * cosf(bullet->Movement.direction);
	bullet->Position.y = this->Position.y + (Size.height / 2) * sinf(bullet->Movement.direction);
	bullet->Size.widht = Size.widht;
	bullet->Size.height = Size.height / 2;
	bullet->CollMask.origin = bullet->Position;

	Drawable* currentTexture = getBulletCurrentTexture();
	
	bullet->CollMask.AddNewMask(AABB{ bullet->Position,
		SizeInfo{currentTexture->dest.width, currentTexture->dest.height},
		bullet->Movement.direction });

	bullet->bulletType = getBulletTypeCurrent();
	bullet->drawable = currentTexture;

	return bullet;
}


Bullet* SpaceShip::Shoot()
{

	Bullet* newObject = nullptr;

	switch (bulletTypeCurrent)
	{
	case WeaponType::Rocket: {

		newObject = ShootBasic();

		newObject->Movement.velocity = 960.f;

		break;
	}

	case WeaponType::LaserShot: {

		newObject = ShootBasic();

		newObject->Movement.velocity = 1440.f; // def 12.0f * 120

		break;
	}

	default:
		newObject = ShootBasic();
		newObject->bulletType = BulletType::NoType;

		break;
	}

	return newObject;
}

const SizeInfo SpaceShip::getSize()
{
	return Size;
}
