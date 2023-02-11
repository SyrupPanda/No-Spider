#include "Player.h"

#include <sstream>

Player::Player(int argc, char* argv[]) : Game(argc, argv), _cBulletVelocity(1.0f), _cReloadTime(120), _cFirerate(0.5), _cMaxMagazine(6), _cHealthPickUpAmmount(5), _cAmmoPickUpAmmount(4), _cMaxAmmo(24), _cMoveSpeed(0.1f), _cPlayerFrameTime(250), _cMunchieFrameTime(500), _cRotationSpeed(0.004), angle(0), _cPlayerSprintSpeed(1.6f), _cPlayerSprintBar(100), _cPlayerSprintConsumption(2), _cSpiderSpeed(0.004f), _cPlayerSprintRegain(1)
{
	_player = new protagonist();
	_menu = new menu();

	int i;
	for (i = 0; i < AMMOCOUNT; i++)
	{
		_ammo[i] = new ammo();
		_ammo[i]->CurrentFrameTime = 0;
		_ammo[i]->FrameTime = rand() % 500 + 50;
		_ammo[i]->Frame = rand() % 1;
	}

	int x;
	for (x = 0; x < HEALTHCOUNT; x++)
	{
		_health[x] = new health();
		_health[x]->CurrentFrameTime = 0;
		_health[x]->FrameTime = rand() % 500 + 50;
		_health[x]->Frame = rand() % 1;
	}

	int n;
	for (n = 0; n < SPIDERCOUNT; n++)
	{
		_spider[n] = new spider();
		_spider[n]->Alive = true;
		
	}

	int b;
	for (b = 0; b < BULLETCOUNT; b++)
	{
		_bullet[b] = new bullet();
	}

	_menu->Paused = false;
	_menu->PKeyDown = false;
	_player->CurrentFrameTime = 0;
	_player->Frame = 0;
	_player->CurrentSprintBar = _cPlayerSprintBar;
	_player->Speed = _cMoveSpeed;
	_menu->Start = true;
	_menu->GameLoop = false;
	_player->Alive = true;
	_currentAction = false;
	_bulletNumber = 0;
	_magazineAmount = 7;
	_lives = 3;
	_ammoCollected = 0;

	_in = false;

	_bite = new SoundEffect();
	_healthPickup = new SoundEffect();
	_ammoPickup = new SoundEffect();

	Audio::Initialise();

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

	int x;
	for (x = 0; x < HEALTHCOUNT; x++)
	{
		delete _health[x]->Position;
		delete _health[x]->Texture;
		delete _health[x]->Rect;
		delete _health[x];
	}
	delete _health;

	int n;
	for (n = 0; n < SPIDERCOUNT; n++)
	{
		delete _spider[i]->Position;
		delete _spider[i]->Texture;
		delete _spider[i]->SourceRect;
		delete _spider[i];
	}
	delete _spider;

	int b;
	for (b = 0; b < BULLETCOUNT; b++)
	{
		delete _bullet[b]->Position;
		delete _bullet[b]->Texture;
		delete _bullet[b]->Rect;
		delete _bullet[b];
	}
	delete _bullet;

	delete _bite;
	delete _healthPickup;
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
	Texture2D* ammoTexture = new Texture2D();
	ammoTexture->Load("Textures/Ammo.png", true);
	for (i = 0; i < AMMOCOUNT; i++)
	{
		_ammo[i]->Rect = new Rect(0.0f, 0.0f, 16, 16);
		_ammo[i]->Texture = ammoTexture;
		_ammo[i]->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ammo[i]->Alive = true;
	}
	_ammoPickup->Load("SoundEffects/gunRack.wav");

	int x;
	Texture2D* healthTexture = new Texture2D();
	healthTexture->Load("Textures/Herbz.png", true);
	for (x = 0; x < HEALTHCOUNT; x++)
	{
		_health[x] ->Rect = new Rect(0.0f, 0.0f, 16, 16);
		_health[x]->Texture = healthTexture;
		_health[x]->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_health[x]->Alive = true;
	}
	_healthPickup->Load("SoundEffects/healthPickup.wav");

	//load spider
	int n;
	Texture2D* spiderTexture = new Texture2D();
	spiderTexture->Load("Textures/Spider.png", true);
	for (n = 0; n < SPIDERCOUNT; n++)
	{
		_spider[n]->SourceRect = new Rect(0.0f,0.0f,64,64);
		_spider[n]->Texture = spiderTexture;
		_spider[n]->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		if ((_spider[n]->Position->Y > 200 && _spider[n]->Position->Y < 500) && (_spider[n]->Position->X > 200 && _spider[n]->Position->X < 500))
		{
			_spider[n]->Position->Y += 400;
			_spider[n]->Position->X += 400;
		}
	}
	//Sound effect for spider
	_bite->Load("SoundEffects/spiderBite.wav");

	//load bullet
	int b;
	Texture2D* bulletTexture = new Texture2D();
	bulletTexture->Load("Textures/Bullet.png", true);
	for(b = 0; b < BULLETCOUNT; b++)
	{
		_bullet[b]->Texture = new Texture2D();
		_bullet[b]->Texture->Load("textures/Bullet.png", true);
		_bullet[b]->Rect = new Rect(0.0f, 0.0f, 5, 30);
		_bullet[b]->Angle = 0;
	}

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	//Menu
	_menu->Background = new Texture2D();
	_menu->Background->Load("Textures/Transparency.png", false);
	_menu->Rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth()/2.0f - 10, Graphics::GetViewportHeight() / 2.0f + 10);
	_menu->StringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - 10, Graphics::GetViewportHeight() / 2.0f + 10);
	_menu->ScorePosition = new Vector2(10.0f, 30.0f);
}

void Player::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();

	if (!_menu->Start) 
	{
		if (!_menu->Paused)
		{
			Input(elapsedTime, keyboardState);
			UpdatePlayer(elapsedTime);
			CheckViewportCollision();
			Action(elapsedTime, mouseState, keyboardState);
			CheckSpiderCollisions();
			CheckAmmoCollisions();
			CheckAmmoScore();
			CheckHealthCollisions();
			CheckHealthScore();
			BulletMovement(elapsedTime);
			int n;
			for (n = 0; n < SPIDERCOUNT; n++)
			{
				if (_spider[n]->Alive)
					SpiderMovement(_spider[n], elapsedTime);
			}
		}
		CheckPause(keyboardState, Input::Keys::P);
	}
	CheckStart(keyboardState, Input::Keys::SPACE);
	CheckRespawn(keyboardState, Input::Keys::F);
	
}

void Player::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;

	SpriteBatch::BeginDraw(); // Starts Drawing

	stream << "Player X: " << _player->Position->X << " Y: " << _player->Position->Y << endl << "Sprint Bar: " << _player->CurrentSprintBar << endl << "Ammo collected: " << _ammoCollected << endl << "Lives Remaining: " << _lives + _livesGained;
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _menu->ScorePosition, Color::Green);
	
	if (_player->Alive)
	{
		SpriteBatch::Draw(_player->Texture, _player->Position, _player->SourceRect);// Draws Pacman
	}

	int i;
	for (i = 0; i < AMMOCOUNT; i++)
	{
		if (_ammo[i]->Alive)
		{
			SpriteBatch::Draw(_ammo[i]->Texture, _ammo[i]->Position, _ammo[i]->Rect);
		}
	}
	
	int x;
	for (x = 0; x < HEALTHCOUNT; x++)
	{
		if (_health[x]->Alive)
		{
			SpriteBatch::Draw(_health[x]->Texture, _health[x]->Position, _health[x]->Rect);
		}
	}

	int n;
	for (n = 0; n < SPIDERCOUNT; n++)
	{
		if (_spider[n]->Alive)
		{
			SpriteBatch::Draw(_spider[n]->Texture, _spider[n]->Position, _spider[n]->SourceRect);
		}
	}

	int b;
	for (b = 0; b < BULLETCOUNT; b++)
	{
		if (_bullet[b]->Alive)
		{
			SpriteBatch::Draw(_bullet[b]->Texture, _bullet[b]->Position, _bullet[b]->Rect);
		}
	}

	//Menu
	if (_menu->Paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menu->Background, _menu->Rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->StringPosition, Color::Red);
	}

	if (_menu->Start && !_menu->GameLoop)
	{
		std::stringstream menuStream;
		menuStream << "To START press 'SPACE'" << endl << "To PAUSE press 'P'";

		SpriteBatch::Draw(_menu->Background, _menu->Rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->StringPosition, Color::Red);
	}

	if (!_player->Alive)
	{
		std::stringstream menuStream;
		menuStream << "You are 'DEAD'" << endl << "To RESPAWN press 'F'";

		SpriteBatch::Draw(_menu->Background, _menu->Rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menu->StringPosition, Color::Red);
	}

	SpriteBatch::EndDraw(); // Ends Drawing
	
}