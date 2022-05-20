#pragma once
#include "Utility.hpp"
#include "SpaceShip.h"

enum WeaponType;

typedef WeaponType BulletType;

class Bullet : public Object {

public: 
	BulletType bulletType;
	Drawable* drawable;

};

