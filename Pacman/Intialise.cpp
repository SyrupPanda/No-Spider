#include "Player.h"

#include <sstream>

Player::Player(int argc, char* argv[]) : Game(argc, argv), _cBulletVelocity(2.0f), _cReloadTime(120), _cFirerate(0.5), _cMaxMagazine(6), _cHealthPickUpAmmount(5), _cAmmoPickUpAmmount(4), _cMaxAmmo(24), _cMoveSpeed(0.1f), _cPlayerFrameTime(250), _cMunchieFrameTime(500), _cRotationSpeed(0.004), angle(0), _cPlayerSprintSpeed(1.6f), _cPlayerSprintBar(100), _cPlayerSprintConsumption(2)
{
	_player = new protagonist();

	int i;
	for (i = 0; i < AMMOCOUNT; i++)
	{
		_ammo[i] = new ammo();
		_ammo[i]->CurrentFrameTime = 0;
		_ammo[i]->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ammo[i]->FrameTime = rand() % 500 + 50;
		_ammo[i]->Frame = rand() % 1;
	}

	int n;
	for (n = 0; n < SPIDERCOUNT; n++)
	{
		_spider[n] = new spider();
		_spider[n]->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	}

	_paused = false;
	_pKeyDown = false;
	_player->CurrentFrameTime = 0;
	_player->Frame = 0;
	_player->CurrentSprintBar = _cPlayerSprintBar;
	_player->Speed = _cMoveSpeed;
	_start = true;
	_player->Alive = true;
	_currentAction = false;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 800, 800, false, 25, 25, "No spider", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Player::~Player()
{
	delete _player->Texture;
	delete _player->SourceRect;

	int i;
	for (i = 0; i < AMMOCOUNT; i++)
	{
		delete _ammo[i]->Position;
		delete _ammo[i]->Texture;
		delete _ammo[i]->Rect;
		delete _ammo[i];
	}
	delete _ammo;

	int n;
	for (n = 0; n < SPIDERCOUNT; n++)
	{
		delete _spider[i]->Position;
		delete _spider[i]->Texture;
		delete _spider[i]->SourceRect;
		delete _spider[i];
	}
}

void Player::LoadContent()
{
	// Load Pacman
	_player->Texture = new Texture2D();
	_player->Texture->Load("Textures/Player.png", false);
	_player->Position = new Vector2(400.0f, 400.0f);
	_player->SourceRect = new Rect(0.0f, 0.0f, 64, 64);

	// Load Ammo packs
	int i;
	Texture2D* ammotexture = new Texture2D();
	ammotexture->Load("Textures/Ammo.png", true);
	for (i = 0; i < AMMOCOUNT; i++)
	{
		_ammo[i]->Rect = new Rect(0.0f, 0.0f, 16, 16);
		_ammo[i]->Texture = ammotexture;
	}

	//load spider
	int n;
	Texture2D* spiderTexture = new Texture2D();
	spiderTexture->Load("Textures/Spider.png", true);
	for (n = 0; n < SPIDERCOUNT; n++)
	{
		_spider[n]->SourceRect = new Rect(0.0f,0.0f,64,64);
		_spider[n]->Texture = spiderTexture;
	}

	//load bullet
	_bulletTexture = new Texture2D();
	_bulletTexture->Load("textures/Bullet.png", true);
	_bulletRect = new Rect(0.0f, 0.0f, 5, 30);
	_bulletPosition = _player->Position;

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	//Menu
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth()/2.0f - 10, Graphics::GetViewportHeight() / 2.0f + 10);
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - 10, Graphics::GetViewportHeight() / 2.0f + 10);
}

void Player::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	if (!_start) 
	{
		if (!_paused)
		{
			Input(elapsedTime, keyboardState);
			UpdatePlayer(elapsedTime);
			CheckViewportCollision();
			Action(elapsedTime, mouseState, keyboardState);
		}
		CheckPause(keyboardState, Input::Keys::P);
	}
	CheckStart(keyboardState, Input::Keys::SPACE);
}

void Player::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Player X: " << _player->Position->X << " Y: " << _player->Position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_player->Texture, _player->Position, _player->SourceRect);// Draws Pacman

	int i;
	for (i = 0; i < AMMOCOUNT; i++)
	{
		SpriteBatch::Draw(_ammo[i]->Texture, _ammo[i]->Position, _ammo[i]->Rect);
	}
	
	int n;
	for (n = 0; n < SPIDERCOUNT; n++)
	{
		SpriteBatch::Draw(_spider[n]->Texture, _spider[n]->Position, _spider[n]->SourceRect);
	}

	//Menu
	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	if (_start)
	{
		std::stringstream menuStream;
		menuStream << "To START press 'SPACE'" << endl << "To PAUSE press 'P'";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing
	
}