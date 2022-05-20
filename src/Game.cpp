#include "Game.h"

Game::~Game() {

	for (uint32_t i = 0; i < BulletList.size(); i++) {
		delete BulletList[i];
	}

	for (uint32_t i = 0; i < AsteroidList.size(); i++) {
		delete AsteroidList[i];
	}
}

void Game::NextTick(float dt)
{
	this->deltaTime = dt;

	Input();

	//Ship tick
	{
		activeShip->ApplyPhysics(planet);
		activeShip->UpdatePosition(dt);
		activeShip->CollMask.MoveMask(activeShip->Position);
	}

	if (activeShip->CollMask.CheckCollisionCircle(Circle{ planet.Position, planet.Size.radius })) {

		running = false;
		if (logEverything) {
			std::cout << "Space ship collided with the planet!" << std::endl;
		}
		return;
	}

	//Bullet tick
	for (uint32_t i = 0; i < BulletList.size(); i++) {
		Bullet* bullet = BulletList[i];
		bullet->ApplyPhysics(planet);
		bullet->UpdatePosition(dt);
		bullet->CollMask.MoveMask(bullet->Position);

		//Delete bullet if it's outside the map
		if (isInsideTheMap(*bullet) == false) {
			delete bullet;
			BulletList.erase(BulletList.begin() + i, BulletList.begin() + i + 1);
			continue;
		}

		//Collision detection bullet-asteroid
		for (uint32_t j = 0; j < AsteroidList.size(); j++) {

			//asteroid collision detection
			if (bullet->CollMask.CheckCollisionCircle(Circle{ AsteroidList[j]->Position, AsteroidList[j]->Size.radius })) {
				//delete the asteroid
				delete AsteroidList[j];

				//erase from the list of asteroids
				AsteroidList.erase(AsteroidList.begin() + j, AsteroidList.begin() + j + 1);

				j--;
				AsteroidCount--;

				//If the bullet type is explosive create an explosion and delete every asteroid collides with
				if (bullet->bulletType == BulletType::Rocket) {
					
					const float explosionRadius = 40.0f;

					for (uint32_t j = 0; j < AsteroidList.size(); j++) {
						if (CollisionCircle(Circle(bullet->Position, explosionRadius),
							Circle{ AsteroidList[j]->Position, AsteroidList[j]->Size.radius })) {

							//delete the asteroid
							delete AsteroidList[j];

							//erase from the list of asteroids
							AsteroidList.erase(AsteroidList.begin() + j, AsteroidList.begin() + j + 1);

							j--;
							AsteroidCount--;
						}
					}

				}

				if (logEverything) {
					std::cout << "A bullet collided with asteroid" << std::endl;
				}

				BulletList.erase(BulletList.begin() + i, BulletList.begin() + i + 1);

				i--;

				delete bullet;

				Score += ScoreIncrement;

				break;
			}

		}

		//Collision detection bullet-planet
		if (bullet->CollMask.CheckCollisionCircle(Circle{ planet.Position, planet.Size.radius })) {
			planet.HP--;
			if (logEverything) {
				std::cout << "A bullet collided with the planet. New planet HP: " << planet.HP << std::endl;
			}
			delete bullet;
			BulletList.erase(BulletList.begin() + i, BulletList.begin() + i + 1);
			continue;
		}

	}

	//Asteroid Tick
	for (uint32_t i = 0; i < AsteroidList.size(); i++) {
		Asteroid* ast = AsteroidList[i];
		ast->ApplyPhysics(planet);
		ast->UpdatePosition(dt);

		//delete asteroid if its outside the map
		if (isInsideTheMap(*ast) == false) {
			delete ast;
			AsteroidList.erase(AsteroidList.begin() + i, AsteroidList.begin() + i + 1);
			i--;
			AsteroidCount--;
			continue;
		}

		//Asteroid Planet collision
		if (CollisionCircle({ ast->Position, ast->Size.radius }, { planet.Position, planet.Size.widht })) {

			planet.HP--;

			delete ast;
			AsteroidList.erase(AsteroidList.begin() + i, AsteroidList.begin() + i + 1);
			i--;
			AsteroidCount--;

			if (logEverything) {
				std::cout << "An asteroid collided with the planet. New planet Hp: " << planet.HP << std::endl;
			}

			continue;
		}


		//Asteroid Space Ship collision
		if (activeShip->CollMask.CheckCollisionCircle({ ast->Position, ast->Size.radius })) {

			activeShip->HP--;

			delete ast;
			AsteroidList.erase(AsteroidList.begin() + i, AsteroidList.begin() + i + 1);
			i--;
			AsteroidCount--;

			if (logEverything) {
				std::cout << "Space ship collided with an asteroid! New Ship Hp: " << activeShip->HP << std::endl;
			}

			continue;
		}
	}

	//Spawn asteroids if needed
	while (AsteroidCountMax >= AsteroidCount) {
		AsteroidList.push_back(SpawnNewAsteroid());
	}

	//Background music control
	if (playMusic == true) {
		UpdateMusicStream(*BackgroundMusicPlaying);

		float timePlayed = GetMusicTimePlayed(*BackgroundMusicPlaying);
		float timeLenght = GetMusicTimeLength(*BackgroundMusicPlaying);
		if (timePlayed == 0) {
			for (size_t i = 0; i < BackgroundMusicList.size(); i++) {

				if (BackgroundMusicList[i] == BackgroundMusicPlaying) {

					StopMusicStream(*BackgroundMusicPlaying);


					if (i + 1 == BackgroundMusicList.size()) {
						BackgroundMusicPlaying = BackgroundMusicList[0];
					}
					else {
						BackgroundMusicPlaying = BackgroundMusicList[i + 1];
					}

					PlayMusicStream(*BackgroundMusicPlaying);
					break;
				}

			}

		}
	}


}

void Game::Draw()
{

	//Draw Space ships
	{
		SpaceShip* currentShip = &spaceShip;
		float x = currentShip->Position.x;
		float y = currentShip->Position.y;

		DDrawTexturePro(ShipTextures, { x,y }, RadianToDegreeF(currentShip->Movement.direction), WHITE);
	}

	//Draw bullets
	for (uint32_t i = 0; i < BulletList.size(); i++) {

		Bullet* bullet = BulletList[i];

		DDrawTexturePro(bullet->drawable, { bullet->Position.x,bullet->Position.y }, RadianToDegreeF(bullet->Movement.direction), WHITE);

	}

	//Draw asteroids
	for (uint32_t i = 0; i < AsteroidList.size(); i++) {

		Asteroid* ast = AsteroidList[i];

		DDrawTexturePro(AsteroidTextures, { ast->Position.x,ast->Position.y }, RadianToDegreeF(ast->Movement.direction), WHITE);

		//DrawCircle(ast->Position.x, ast->Position.y, ast->Size.radius, RED);
	}

	//Draw Planet
	DrawTextureProCustomSize(PlanetTexture, planet.Position, planet.Size * 2, 0.0f, WHITE);

	//Draw Shader
	if (enableShaders == true) {
		if (BackgroundShader != nullptr) {

			Vector2 mapSize = {GameWidth, GameHeight};
			Vector3 pos = { activeShip->Position.x, activeShip->Position.y, activeShip->Movement.direction};

			SetShaderValue(*BackgroundShader, BgShaderLocations.UniformCoordinateLocation, &pos, SHADER_UNIFORM_VEC3);
			SetShaderValue(*BackgroundShader, BgShaderLocations.UniformMapSizeLocation, &mapSize, SHADER_UNIFORM_VEC2);

			BeginShaderMode(*BackgroundShader);

			DrawRectangle(0, 0, GameWidth, GameHeight, WHITE);

			EndShaderMode();

		}
	}

	//Debug purposes hitboxes
	if (showHitbox) {

		//Planet
		{
			Color color = PURPLE;
			color.a = 120;
			DrawCircle(static_cast<int32_t>(planet.Position.x), static_cast<int32_t>(planet.Position.y), planet.Size.radius, color);
		}

		//Asteroid
		for (size_t i = 0; i < AsteroidList.size(); i++) {
			Asteroid* ast = AsteroidList[i];

			Color color = PURPLE;
			color.a = 120;

			DrawCircle(static_cast<int32_t>(ast->Position.x), static_cast<int32_t>(ast->Position.y), ast->Size.radius, color);
		}

		//Bullet
		for (size_t i = 0; i < BulletList.size(); i++) {
			Bullet* bullet = BulletList[i];

			Color color = PURPLE;
			color.a = 120;

			for (const AABB& aabb : bullet->CollMask.rectMasks) {
				DrawRectanglePro(
					Drawable::DestWPosition({ aabb.point.x, aabb.point.y }, aabb.width, aabb.height),
					{ aabb.width / 2, aabb.height / 2 },
					RadianToDegreeF(aabb.rotation), color);
			}

		}

		//Space ship
		{
			Color color = PURPLE;
			color.a = 120;

			for (uint32_t i = 0; i < activeShip->CollMask.rectMasks.size(); i++) {
				AABB aabb = activeShip->CollMask.rectMasks[i];
				DrawRectanglePro(Drawable::DestWPosition({ aabb.point.x, aabb.point.y }, aabb.width, aabb.height),
					{ aabb.width / 2, aabb.height / 2 },
					RadianToDegreeF(aabb.rotation), color);
			}
		}

	}


	//Draw Score
	DrawText(std::string{ "Score: " + std::to_string(Score) }.c_str(), GameWidth / 2 - 90, 50, 40, RED);
	
	//Draw Hp
	DrawText(std::string{ "People on Earth: " + std::to_string(planet.HP) + "K" }.c_str(), 50, 50, 40, RED);


}

void Game::Input()
{

	if (IsKeyDown(KEY_RIGHT)) {
		activeShip->TurnRight(deltaTime);
	}

	if (IsKeyDown(KEY_LEFT)) {
		activeShip->TurnLeft(deltaTime);
	}

	if (IsKeyDown(KEY_UP)) {
		activeShip->MoveForward();
	}
	else {
		activeShip->Stop();
	}

	if (IsKeyDown(KEY_SPACE)) {

		

		if (activeShip->canShoot == true) {

			switch (activeShip->getBulletTypeCurrent())
			{
			case WeaponType::Rocket: {

				if (activeShip->canShoot == true) {
					if (playEffects) {
						PlaySound(SpaceShipShootSound);
					}
					BulletList.push_back(activeShip->Shoot());
				}
				activeShip->canShoot = false;

				break;
			}

			case WeaponType::LaserShot: {
				if (activeShip->canShoot == true) {
					if (playEffects) {
						PlaySound(SpaceShipShootSound); //same as rocket sound for now
					}
					BulletList.push_back(activeShip->Shoot());
				}
				activeShip->canShoot = false;

				break;
			}

			default:
				break;
			}

		}
	}

	if (IsKeyReleased(KEY_SPACE)) {
		activeShip->canShoot = true;
	}

	if (IsKeyReleased(KEY_Q)) {
		activeShip->BulletTypeSwap();
	}

	if (IsKeyDown(KEY_T)) {
		std::string inputCommand;
		std::cout << "Type -help to see all commands\nPlease enter your command: ";
		std::cin >> inputCommand;

		if (!inputCommand.empty()) {
			ProcessCommand(inputCommand);
		}

		std::cout << std::endl;
	}

}

void Game::Run()
{
	activeShip = &spaceShip;

	//Init Planet
	planet.Position.x = static_cast<float>(GameWidth) / 2;
	planet.Position.y = static_cast<float>(GameHeight) / 2;
	planet.Movement.velocity = 0.0f;

	planet.Size.widht = 80;
	planet.Size.height = 80;
	planet.Size.radius = 80;

	running = true;

	BackgroundMusicPlaying = BackgroundMusicList[0];
	PlayMusicStream(*BackgroundMusicPlaying);


	BgShaderLocations.UniformMapSizeLocation = GetShaderLocation(*BackgroundShader, "mapSize");
	BgShaderLocations.UniformCoordinateLocation = GetShaderLocation(*BackgroundShader, "coord");

}

bool Game::isRunning()
{
	return ((activeShip->HP > 0) && (planet.HP > 0)) && running;
}

bool Game::isInsideTheMap(const Object& obj) const
{
	if (obj.Position.x > GameWidth)
		return false;
	else if (obj.Position.x < 0)
		return false;

	if (obj.Position.y > GameHeight)
		return false;
	else if (obj.Position.y < 0)
		return false;

	return true;
}

Asteroid* Game::SpawnNewAsteroid()
{
	Asteroid* newAst = new Asteroid;

	newAst->Movement.velocity = (GetRandomValue(400, 1000) / 1000.0f) * 120.0f; // 0.4-1.0

	newAst->Size.radius = 20.0f;

	bool startPosLeft = GetRandomValue(0, 1);
	bool startPosUp = GetRandomValue(0, 1);

	if (startPosLeft == true) { newAst->Position.x = 0; }
	else { newAst->Position.x = static_cast<float>(GameWidth); }

	if (startPosUp == true) { newAst->Position.y = 0; }
	else { newAst->Position.y = static_cast<float>(GameHeight); }

	struct InBValue {
		uint16_t Beg;
		uint16_t End;
	};

	/*
	L\!U| 0	   | 1
	----|------|-------
	0	|0-90  | 270-360
	----|------|---------
	1	|90-180| 180-270
	----|------|---------
	*/

	constexpr InBValue Arr[2][2] = { {{0,90},{270,360}}, {{90,180},{180,270}} };

	newAst->Movement.direction = static_cast<float>(GetRandomValue(Arr[startPosLeft][!startPosUp].Beg, Arr[startPosLeft][!startPosUp].End));

	AsteroidCount++;

	return newAst;
}

void Game::ProcessCommand(std::string& command)
{
	if (command.empty()) {
		return;
	}

	if (command[0] != '-') {
		return;
	}

	command = command.substr(1, command.size() - 1);

	//Help
	if (strcmp(command.c_str(), "help") == 0) {
		std::cout << "-show_hitbox\tTo see all hitboxes" << std::endl; // Hitbox
		std::cout << "-log\tTo print everyting on console" << std::endl; // Hitbox
	}

	//Hitbox
	else if (strcmp(command.c_str(), "show_hitbox") == 0) {
		showHitbox = !showHitbox;

		std::cout << "Show Hitboxes : " << showHitbox << std::endl;
	}

	//Log everything
	else if (strcmp(command.c_str(), "log") == 0) {
		logEverything = !logEverything;

		std::cout << "Log everything : " << logEverything << std::endl;
	}

}

void Game::shouldPlayMusics(bool shouldPlay) {
	playMusic = shouldPlay;

	if (IsAudioStreamPlaying(BackgroundMusicPlaying->stream) && playMusic == false) {
		StopMusicStream(*BackgroundMusicPlaying);
	}

}

void Game::shouldPlayEffects(bool shouldPlay) {
	playEffects = shouldPlay;

	if (IsAudioStreamPlaying(SpaceShipShootSound.stream) && playEffects == false) {
		StopSound(SpaceShipShootSound);
	}
}
