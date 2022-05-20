#include "raylib.h"
#include "Game.h"
#include <iostream>

#define GLSL_VERSION 330

enum class MenuDesicion {
	NewGame,
	Options,
	Exit,
	NoDesicion
};


MenuDesicion MainMenu();
void DrawMainMenu();

MenuDesicion OptionsMenu();
void DrawOptionsMenu();

const int screenWidth = 1280;
const int screenHeight = 720;

bool playSound = true;
bool playEffects = true;

static ResourceManager ResManager;

void InitResources();
void UnloadResources();

int main(void)
{

	// Initialization
	//--------------------------------------------------------------------------------------


	InitWindow(screenWidth, screenHeight, "Protect The Planet");
	InitAudioDevice();

	SetTargetFPS(std::numeric_limits<int>::max());               // Set our game to run at 120 frames-per-second
	//--------------------------------------------------------------------------------------

	InitResources();

	float dt = 0;
	bool exit = false;

	while (!exit) {

		switch (MainMenu())
		{
		case MenuDesicion::NewGame: {
			Game game;

			for (size_t i = 0; i < ResManager.CountOfType(ResourceManager::SourceType::Music); i++) {
				game.BackgroundMusicList.push_back(ResManager.getSource<Music>({ "Music" + std::to_string(i) }));
			}

			game.SpaceShipShootSound = *ResManager.getSource<Sound>("SpaceShipShootSound");

			SpaceShip spaceShip;

			spaceShip.Position.x = static_cast<float>(game.GameWidth) / 4;
			spaceShip.Position.y = static_cast<float>(game.GameHeight) / 4;
			spaceShip.Size = SizeInfo{ SpaceShip::DefWidth, SpaceShip::DefHeight };
			spaceShip.CollMask.origin = spaceShip.Position;

			spaceShip.CollMask.AddNewMask(AABB({ spaceShip.Position.x - spaceShip.Size.widht / 4, spaceShip.Position.y },
				{ spaceShip.Size.widht / 2, spaceShip.Size.height }));

			spaceShip.CollMask.AddNewMask(AABB({ spaceShip.Position.x + spaceShip.Size.widht / 4, spaceShip.Position.y },
				{ spaceShip.Size.widht / 2, spaceShip.Size.height / 3 }));


			spaceShip.setBulletTypeFirst(WeaponType::Rocket);
			spaceShip.setBulletTypeSecond(WeaponType::LaserShot);
			spaceShip.BulletTextureFirst = ResManager.getSource<Drawable>("BulletDrawable");
			spaceShip.BulletTextureSecond = ResManager.getSource<Drawable>("LaserDrawable");

			game.spaceShip = spaceShip;

			game.ShipTextures = ResManager.getSource<Drawable>("ShipDrawable");
			game.AsteroidTextures = ResManager.getSource<Drawable>("AsteroidDrawable");
			game.PlanetTexture = ResManager.getSource<Drawable>("PlanetDrawable");

			game.BackgroundShader = ResManager.getSource<Shader>("BackgroundShader");

			game.Run();

			game.shouldPlayMusics(playSound);
			game.shouldPlayEffects(playEffects);


			while (!WindowShouldClose() && game.isRunning())    // Detect window close button or ESC key
			{
				float beg = static_cast<float>(GetTime());

				//Tick
				game.NextTick(dt);

				//Begining of Draw
				BeginDrawing();

				ClearBackground(BLACK);

				DDrawTexturePro(ResManager.getSource<Drawable>("BackgroundDrawable"), { screenWidth / 2, screenHeight / 2 }, 0.0f, WHITE);

				game.Draw();

				DrawFPS(10, 10);

				//End of draw
				EndDrawing();

				dt = static_cast<float>(GetTime()) - beg;
			}

			break;
		}

		case MenuDesicion::Options: {
			bool closeOptions = false;
			PollInputEvents();
			while (!closeOptions) {

				if (OptionsMenu() == MenuDesicion::Exit) {
					closeOptions = !closeOptions;
				}

				BeginDrawing();

				ClearBackground(BLACK);

				DrawOptionsMenu();

				EndDrawing();
			}

			break;
		}

		case MenuDesicion::Exit: {
			exit = !exit;
			break;
		}

		case MenuDesicion::NoDesicion: {

			break;
		}

		default: {

			break;
		}

		}

		BeginDrawing();

		ClearBackground(BLACK);

		DrawMainMenu();

		EndDrawing();

	}

	CloseWindow();        // Close window and OpenGL context
	CloseAudioDevice();     // Close audio device (music streaming is automatically stopped)
	UnloadResources();

	return 0;
}

void InitResources() {

	//Load Textures
	std::string path = "source/texture/";
	Texture2D shipTexture = LoadTexture(std::string(path + "SpaceShip1.png").c_str());
	Texture2D bulletTexture = LoadTexture(std::string(path + "bullet.png").c_str());
	Texture2D asteroidTexture = LoadTexture(std::string(path + "asteroid.png").c_str());
	Texture2D planetTexture = LoadTexture(std::string(path + "planet.png").c_str());
	Texture2D backGround = LoadTexture(std::string(path + "background.png").c_str());
	Texture2D laserTexture = LoadTexture(std::string(path + "laser.png").c_str());
	Texture2D plasmaTexture = LoadTexture(std::string(path + "plasma.png").c_str());
	Texture2D explosionTexture = LoadTexture(std::string(path + "explosion.png").c_str());

	Drawable* ShipDrawable = new Drawable(shipTexture);
	ShipDrawable->NewTextureWidth(70);
	ShipDrawable->NewTextureHeight(46);

	Drawable* AsteroidDrawable = new Drawable(asteroidTexture);
	AsteroidDrawable->NewTextureWidth(50);
	AsteroidDrawable->NewTextureHeight(60);

	Drawable* PlanetDrawable = new Drawable(planetTexture);

	Drawable* BackgroundDrawable = new Drawable(backGround);
	BackgroundDrawable->NewTextureWidth(screenWidth);
	BackgroundDrawable->NewTextureHeight(screenHeight);

	Drawable* BulletDrawable = new Drawable(bulletTexture);
	BulletDrawable->NewTextureWidth(70);
	BulletDrawable->NewTextureHeight(23);

	Drawable* LaserDrawable = new Drawable(laserTexture);
	LaserDrawable->NewTextureWidth(70);
	LaserDrawable->NewTextureHeight(16);

	Drawable* PlasmaDrawable = new Drawable(plasmaTexture);
	PlasmaDrawable->NewTextureWidth(100);
	PlasmaDrawable->NewTextureHeight(40);

	


	//Load Sounds
	Sound* SpaceShipShootSound = new Sound(LoadSound("source/sounds/fx/SpaceShipShoot.wav"));

	const uint16_t musicCount = 8;

	for (uint16_t i = 0; i < musicCount; i++) {
		std::string filePath = "source/sounds/background/bg" + std::to_string(i) + ".mp3";

		Music* newMusic = new Music(LoadMusicStream(filePath.c_str()));
		newMusic->looping = false;

		ResManager.AddSource(std::string{ "Music" + std::to_string(i) }, newMusic);
	}

	//Load Shaders
	Shader* BackgroundShader = new Shader(LoadShader(0, TextFormat("shaders/backgroundShader.fs", GLSL_VERSION)));

	ResManager.AddSource("ShipDrawable", ShipDrawable);
	ResManager.AddSource("AsteroidDrawable", AsteroidDrawable);
	ResManager.AddSource("PlanetDrawable", PlanetDrawable);
	ResManager.AddSource("BackgroundDrawable", BackgroundDrawable);
	ResManager.AddSource("BulletDrawable", BulletDrawable);
	ResManager.AddSource("LaserDrawable", LaserDrawable);
	ResManager.AddSource("PlasmaDrawable", PlasmaDrawable);

	ResManager.AddSource("SpaceShipShootSound", SpaceShipShootSound);

	ResManager.AddSource("BackgroundShader", BackgroundShader);

}

void UnloadResources() {

	//Unload Music
	{
		std::vector<ResourceManager::Source<Music>> list = ResManager.getSourceList<Music>(ResourceManager::SourceType::Music);

		for (uint16_t i = 0; i < list.size(); i++) {
			UnloadMusicStream(*list[i].data);
			delete list[i].data;
		}
	}

	//Unload Sound
	{
		std::vector<ResourceManager::Source<Sound>> list = ResManager.getSourceList<Sound>(ResourceManager::SourceType::Sound);

		for (size_t i = 0; i < list.size(); i++) {
			UnloadSound(*list[i].data);
			delete list[i].data;
		}
	}

	//Unload Textures
	{
		std::vector<ResourceManager::Source<Drawable>> list = ResManager.getSourceList<Drawable>(ResourceManager::SourceType::Drawable);

		for (size_t i = 0; i < list.size(); i++) {
			UnloadTexture(list[i].data->texture);
			delete list[i].data;
			//NOT: ExplosionAnimation yüzünden hata alınabilir 
			//https://wiki.sei.cmu.edu/confluence/display/cplusplus/OOP52-CPP.+Do+not+delete+a+polymorphic+object+without+a+virtual+destructor
		}

	}

	//Unload Shader
	{
		std::vector<ResourceManager::Source<Shader>> list = ResManager.getSourceList<Shader>(ResourceManager::SourceType::Shader);

		for (size_t i = 0; i < list.size(); i++) {
			UnloadShader(*list[i].data);
			delete list[i].data;
		}

	}
}

MenuDesicion MainMenu() {

	Vector2 mousePos = GetMousePosition();

	//New game button
	if (CheckCollisionPointRec(mousePos, Rectangle{ screenWidth / 2 - 200, 200, 400, 100 })) {
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			return MenuDesicion::NewGame;
		}
	}

	//Options button
	if (CheckCollisionPointRec(mousePos, Rectangle{ screenWidth / 2 - 200, 350, 400, 100 })) {
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			return MenuDesicion::Options;
		}
	}

	//Exit button
	if (CheckCollisionPointRec(mousePos, Rectangle{ screenWidth / 2 - 200, 500, 400, 100 })) {
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			return MenuDesicion::Exit;
		}
	}

	return MenuDesicion::NoDesicion;
}

void DrawMainMenu() {

	DrawText("Protect The Planet", screenWidth / 2 - 290, 70, 60, RED);

	DrawRectangle(screenWidth / 2 - 200, 200, 400, 100, DARKBLUE);
	DrawText("Play", screenWidth / 2 - 75, 210, 75, RED);

	DrawRectangle(screenWidth / 2 - 200, 350, 400, 100, DARKPURPLE);
	DrawText("Options", screenWidth / 2 - 130, 365, 75, RED);

	DrawRectangle(screenWidth / 2 - 200, 500, 400, 100, DARKGRAY);
	DrawText("Exit", screenWidth / 2 - 60, 515, 75, RED);

}


MenuDesicion OptionsMenu() {
	Vector2 mousePos = GetMousePosition();

	//Play sound button
	if (CheckCollisionPointRec(mousePos, Rectangle{ screenWidth / 2 - 200, 200, 400, 100 })) {
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			playSound = !playSound;
			return MenuDesicion::NoDesicion;
		}
	}

	//Play effects button
	if (CheckCollisionPointRec(mousePos, Rectangle{ screenWidth / 2 - 200, 350, 400, 100 })) {
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			playEffects = !playEffects;
			return MenuDesicion::NoDesicion;
		}
	}

	//Exit button
	if (CheckCollisionPointRec(mousePos, Rectangle{ screenWidth / 2 - 200, 500, 400, 100 })) {
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			return MenuDesicion::Exit;
		}
	}

	return MenuDesicion::NoDesicion;

}


void DrawOptionsMenu() {
	DrawText("Options", screenWidth / 2 - 140, 70, 80, RED);

	DrawRectangle(screenWidth / 2 - 200, 200, 400, 100, DARKBLUE);

	Color color;

	if (playSound) {
		color = GREEN;
	}
	else {
		color = RED;
	}

	DrawText("Play Sound", screenWidth / 2 - 165, 215, 60, color);


	DrawRectangle(screenWidth / 2 - 200, 350, 400, 100, DARKPURPLE);

	if (playEffects) {
		color = GREEN;
	}
	else {
		color = RED;
	}

	DrawText("Effects", screenWidth / 2 - 130, 365, 75, color);

	DrawRectangle(screenWidth / 2 - 200, 500, 400, 100, DARKGRAY);
	DrawText("Exit", screenWidth / 2 - 60, 515, 75, RED);

}

