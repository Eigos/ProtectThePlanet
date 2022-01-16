#include "SpaceShip.h"
#include <math.h>

const float SpaceShip::MaxSpeed = 360.0f; // def: 3.0f
const float SpaceShip::Acceleration = 0.96f; // hardcoded part!!! def: 0.008f
const float SpaceShip::DeAcceleration = Acceleration / 2; // hardcoded part!!! def: 0.004f 
const float SpaceShip::TurnSpeed = 180.0f; // def: 1.5f


SpaceShip::SpaceShip(const SpaceShipOptions& ShipOptions)
{
	Movement = ShipOptions.objectVariables.Movement;

	Position = ShipOptions.objectVariables.Position;

	Size = ShipOptions.objectVariables.Size;

	if (ShipOptions.objectVariables.Size.widht == 0) {
		Size.widht = DefWidth;
	}

	if (ShipOptions.objectVariables.Size.height == 0) {
		Size.height = DefHeight;
	}

	texture = ShipOptions.drawable;

	CollMask.origin = Point{ Position.x, Position.y };

	CollMask.AddNewMask(AABB({ Position.x - Size.widht / 4, Position.y }, {Size.widht/2, Size.height}));
	CollMask.AddNewMask(AABB({ Position.x + Size.widht / 4,Position.y }, { Size.widht / 2, Size.height / 3 }));

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

//	Movement.direction = Clip(Movement.direction, 0.0f, 2 * PI);
	
	CollMask.RotateMask(rotation);
}

void SpaceShip::TurnRight(float dt)
{


	float rotation = DegreeToRadianF(TurnSpeed) * dt;

	Movement.direction += rotation;

	if (Movement.direction >= 0) {
		Movement.direction = Movement.direction - 2.0f * PI;
	}
	//Movement.direction = Clip(Movement.direction, 0.0f, 2 * PI);

	CollMask.RotateMask(rotation);

}

void SpaceShip::Stop() {
	
	Movement.velocity -= DeAcceleration;
	Movement.velocity = Clip(Movement.velocity, 0.0f, MaxSpeed);

}


Object* SpaceShip::Shoot()
{
	Object* bullet = new Object;
	bullet->Movement.velocity = 960.f; // def 8.0f
	bullet->Movement.direction = this->Movement.direction;
	bullet->Position.x = this->Position.x + (texture->dest.width / 2) * cosf(bullet->Movement.direction);
	bullet->Position.y = this->Position.y + (texture->dest.height / 2) * sinf(bullet->Movement.direction);
	bullet->Size.widht = this->getTexture()->dest.width;
	bullet->Size.height = this->getTexture()->dest.height / 2;
	bullet->CollMask.origin = bullet->Position;
	bullet->CollMask.AddNewMask(AABB{ bullet->Position, SizeInfo{bullet->Size.widht, bullet->Size.height / 2}, bullet->Movement.direction});

	return bullet;
}

void SpaceShip::setTexture(Drawable* newTexture)
{
	texture = newTexture;
}

Drawable* SpaceShip::getTexture()
{
	return texture;
}

const SizeInfo SpaceShip::getSize()
{
	return Size;
}
