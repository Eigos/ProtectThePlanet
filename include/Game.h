#pragma once

#include <vector>
#include <iostream>
#include <string>

#include "raylib.hpp"
#include "SpaceShip.h"
#include "Asteroid.h"
#include "Planet.h"
#include "bullet.h"

extern ResourceManager ResManager;

class Game {

private:

	const int32_t RandomValueSeedTest = 0xaabbccff;

	const int32_t AsteroidCountMax = 7; // def: 7
	int32_t AsteroidCount = 0;
	uint32_t Score = 0;
	uint16_t ScoreIncrement = 5;

	bool showHitbox = false;
	bool logEverything = false;
	bool enableShaders = true;
	
	bool running = true;

	bool playMusic = true;
	bool playEffects = true;

	SpaceShip* activeShip;

	Planet planet;

	bool isInsideTheMap(const Object& obj) const;

	Asteroid* SpawnNewAsteroid();

	void ProcessCommand(std::string& command);
	
	float deltaTime = 0;

	Music* BackgroundMusicPlaying;

	struct BackgroundShaderLocation {

		int32_t UniformCoordinateLocation;
		int32_t UniformMapSizeLocation;
	}BgShaderLocations;

	std::vector<Bullet*> BulletList; //Linked list yapýlacak
	std::vector<Asteroid*> AsteroidList; // Linked list yapýlacak 


public:

	uint16_t GameWidth = 1280;
	uint16_t GameHeight = 720;

	SpaceShip spaceShip;

	Drawable* AsteroidTextures;
	Drawable* ShipTextures;

	Drawable* PlanetTexture;


	Sound SpaceShipShootSound;

	std::vector<Music*>	BackgroundMusicList;

	Shader* BackgroundShader = nullptr;

	//False to turn off the music
	void shouldPlayMusics(bool shouldPlay);

	void shouldPlayEffects(bool shouldPlay);

	void NextTick(float dt = 1.0f);

	void Draw();

	void Input();

	void Run();

	bool isRunning();

	~Game();
};


