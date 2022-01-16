#pragma once
#include "Utility.hpp"


class Bullet : Object {

private:
	Drawable* texture;

public: 

	void setTexture(Drawable* newTexture);

	Drawable* getTexture();
};

