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

bool playSound   = true;
bool playEffects = true;

int main(void)
{

	// Initialization
	//--------------------------------------------------------------------------------------


	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Protect The Planet");
	InitAudioDevice();

	SetTargetFPS(120);               // Set our game to run at 120 frames-per-second
	//--------------------------------------------------------------------------------------


	//Load Textures
	Texture2D shipTexture = LoadTexture("source/texture/SpaceShip1.png");
	Texture2D bulletTexture = LoadTexture("source/texture/bullet.png");
	Texture2D asteroidTexture = LoadTexture("source/texture/asteroid.png");
	Texture2D planetTexture = LoadTexture("source/texture/planet.png");
	Texture2D backGround = LoadTexture("source/texture/background.png");

	Drawable* ShipDrawable = new Drawable(shipTexture);
	ShipDrawable->NewTextureWidth(70);
	ShipDrawable->NewTextureHeight(46);

	Drawable* BulletDrawable = new Drawable(bulletTexture);
	BulletDrawable->NewTextureWidth(70);
	BulletDrawable->NewTextureHeight(23);

	Drawable* AsteroidDrawable = new Drawable(asteroidTexture);
	AsteroidDrawable->NewTextureWidth(50);
	AsteroidDrawable->NewTextureHeight(60);

	Drawable* PlanetDrawable = new Drawable(planetTexture);

	Drawable* BackgroundDrawable = new Drawable(backGround);
	BackgroundDrawable->NewTextureWidth(screenWidth);
	BackgroundDrawable->NewTextureHeight(screenHeight);

	//Load Sounds
	Sound spaceShipShootSound = LoadSound("source/sounds/fx/SpaceShipShoot.wav");

	const uint16_t musicCount = 8;

	std::vector<Music> musicList;

	for (uint16_t i = 0; i < musicCount; i++) {
		std::string filePath = "source/sounds/background/bg" + std::to_string(i) + ".mp3";
		Music newMusic = LoadMusicStream(filePath.c_str());
		newMusic.looping = false;
		musicList.push_back(newMusic);
	}

	//Load Shaders

	Shader backgroundShader = LoadShader(0, TextFormat("shaders/backgroundShader.fs", GLSL_VERSION));


	float dt = 0;
	bool exit = false;

	while (!exit) {

		switch (MainMenu())
		{
		case MenuDesicion::NewGame: {
			Game game;

			game.AsteroidTextures.push_back(AsteroidDrawable);
			game.ShipTextures.push_back(ShipDrawable);
			game.BulletTextures.push_back(BulletDrawable);
			game.PlanetTexture = PlanetDrawable;
			

			for (size_t i = 0; i < musicList.size(); i++) {
				game.BackgroundMusicList.push_back(&musicList[i]);
			}

			game.SpaceShipShootSound = spaceShipShootSound;


			SpaceShipOptions ShipOptions;
			ShipOptions.objectVariables.Movement.direction = 0;
			ShipOptions.objectVariables.Movement.velocity = 0;
			ShipOptions.objectVariables.Position.x = static_cast<float>(game.GameWidth) / 4;
			ShipOptions.objectVariables.Position.y = static_cast<float>(game.GameHeight) / 4;
			ShipOptions.objectVariables.CollMask.origin = ShipOptions.objectVariables.Position;

			SpaceShip spaceShip(ShipOptions);

			game.spaceShip = spaceShip;

			game.BackgroundShader = &backgroundShader;

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

				DDrawTexturePro(BackgroundDrawable, { screenWidth / 2, screenHeight / 2 }, 0.0f, WHITE);

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

	for (uint16_t i = 0; i < musicList.size(); i++) {
		UnloadMusicStream(musicList[i]);
	}

	UnloadSound(spaceShipShootSound);

	UnloadTexture(shipTexture);
	UnloadTexture(bulletTexture);
	UnloadTexture(asteroidTexture);
	UnloadTexture(planetTexture);
	UnloadTexture(backGround);
	
	UnloadShader(backgroundShader);

	delete ShipDrawable;
	delete BulletDrawable;
	delete AsteroidDrawable;
	delete PlanetDrawable;
	delete BackgroundDrawable;


	return 0;
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


