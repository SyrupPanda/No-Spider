#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#define AMMOCOUNT 7
#define HEALTHCOUNT 3
#define SPIDERCOUNT 12
#define BULLETCOUNT 20
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"
#include <iostream>

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

//Structure for player
struct protagonist
{
	// Data to represent Player
	Vector2* Position;
	Rect* SourceRect;
	Texture2D* Texture;
	int Frame;
	int CurrentFrameTime;
	bool Alive;
	float CurrentSprintBar;
	float Speed;
};

//Structure for the ammo
struct ammo
{
	// Data to represent Pick-ups
	Rect* Rect;
	Texture2D* Texture;
	Vector2* Position;
	int CurrentFrameTime;
	int Frame;
	int FrameTime;
	bool Alive;
};

struct health
{
	// Data to represent Pick-ups
	Rect* Rect;
	Texture2D* Texture;
	Vector2* Position;
	int CurrentFrameTime;
	int Frame;
	int FrameTime;
	bool Alive;
};

//structure for the spider
struct spider
{
	//Data to represent the enemy
	Vector2* Position;
	Rect* SourceRect;
	Texture2D* Texture;
	int Frame;
	int CurrentFrameTime;
	bool Alive;
	float Speed;
	Vector2* Direction;
};

struct bullet
{
	Vector2* Position;
	Texture2D* Texture;
	Rect* Rect;
	bool Alive;
	float Angle;
};

struct menu
{
	Texture2D* Background;
	Rect* Rectangle;
	Vector2* StringPosition;
	Vector2* ScorePosition;
	bool Paused;
	bool PKeyDown;
	bool Start;
	bool GameLoop;
};

// Declares the Player class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Player : public Game
{
private:	
	//initalise struct
	protagonist* _player;
	ammo* _ammo[AMMOCOUNT];
	health* _health[HEALTHCOUNT];
	spider* _spider[SPIDERCOUNT];
	bullet* _bullet[BULLETCOUNT];
	menu* _menu;

	//sound effects
	SoundEffect* _bite;
	SoundEffect* _healthPickup;
	SoundEffect* _ammoPickup;

	//movement and rotation
	float angle;
	Vector2 point;

	//gunplay
	int _ammoAmount;
	int _magazineAmount;
	bool _currentAction;
	int _bulletNumber;

	// Position for String
	Vector2* _stringPosition;

	//Data to hold
	int _ammoCollected;
	int _lives;
	int _livesGained;

	//conditions
	bool _in;

	//constant
	const float _cMoveSpeed;
	const int _cPlayerFrameTime;
	const int _cMunchieFrameTime;
	const float _cRotationSpeed;
	const int _cMaxAmmo;
	const int _cMaxMagazine;
	const int _cAmmoPickUpAmmount;
	const float _cBulletVelocity;
	const float _cFirerate;
	const float _cReloadTime;
	const int _cHealthPickUpAmmount;
	const float _cPlayerSprintSpeed;
	const float _cPlayerSprintBar;
	const float _cPlayerSprintConsumption;
	const float _cPlayerSprintRegain;
	const float _cSpiderSpeed;

	//input methods
	void Input(int elapsedTime, Input::KeyboardState* state) 
	{
		if (state->IsKeyDown(Input::Keys::LEFTSHIFT) && _player->CurrentSprintBar > 0)
		{
			_player->Speed = _cMoveSpeed * _cPlayerSprintSpeed;
			_player->CurrentSprintBar -= _cPlayerSprintConsumption;
		}
		if (!(state->IsKeyDown(Input::Keys::LEFTSHIFT)) || _player->CurrentSprintBar <= 0)
		{
			_player->Speed = _cMoveSpeed;
			if (_player->CurrentSprintBar < 300)
			{
				_player->CurrentSprintBar += _cPlayerSprintRegain;
			}
		}

		if (state->IsKeyDown(Input::Keys::W))
		{
			_player->Position->X += sin(angle) * _player->Speed * elapsedTime;
			_player->Position->Y += cos(angle) * _player->Speed * elapsedTime;
		}

		if (state->IsKeyDown(Input::Keys::S))
		{
			_player->Position->X -= sin(angle) * _player->Speed * elapsedTime;
			_player->Position->Y -= cos(angle) * _player->Speed * elapsedTime;
		}

		if (state->IsKeyDown(Input::Keys::D))
			angle -= (_cRotationSpeed * elapsedTime);

		if (state->IsKeyDown(Input::Keys::A))
			angle += (_cRotationSpeed * elapsedTime);

	}

	void Action(int elapsedTime, Input::MouseState*mouseState, Input::KeyboardState*state)
	{
		if (_magazineAmount > 0 && mouseState->LeftButton == Input::ButtonState::PRESSED && !_currentAction)
		{
			_currentAction = true;
			_bullet[_bulletNumber]->Alive = true;
			_bullet[_bulletNumber]->Position = _player->Position;
			_bullet[_bulletNumber]->Angle = angle;
			_bulletNumber += 1;
		}
		if (state->IsKeyDown(Input::Keys::R) && !_currentAction)
		{
			_currentAction = true;
			_magazineAmount = 0;
			if (_ammoAmount < _cMaxMagazine)
			{
				_magazineAmount = _ammoAmount;
				_ammoAmount = 0;
			}
			else
			{
				_magazineAmount = _cMaxMagazine;
				_ammoAmount -= _cMaxMagazine;
			}
		}
	}

	void BulletMovement(int elapsedTime)
	{
		int b;
		for (b = 0; b < BULLETCOUNT; b++)
		{
			if (_bullet[b]->Alive)
			{
				_bullet[b]->Position->X += sin(_bullet[b]->Angle) * _cBulletVelocity;
				_bullet[b]->Position->Y += cos(_bullet[b]->Angle) * _cBulletVelocity;
			}
		}
	}

	void CheckSpiderCollisions()
	{
		int i = 0;
		int bottom1 = _player->Position->Y + _player->SourceRect->Height;
		int bottom2 = 0;
		int left1 = _player->Position->X;
		int left2 = 0;
		int right1 = _player->Position->X + _player->SourceRect->Width;
		int right2 = 0;
		int top1 = _player->Position->Y;
		int top2 = 0;

		for (i = 0; i < SPIDERCOUNT; i++)
		{
			bottom2 = _spider[i]->Position->Y + _spider[i]->SourceRect->Height;
			left2 = _spider[i]->Position->X;
			right2 = _spider[i]->Position->X + _spider[i]->SourceRect->Width;
			top2 = _spider[i]->Position->Y;

			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
			{
				_player->Alive = false;
				Audio::Play(_bite);
				_menu->Start = true;
				i = SPIDERCOUNT;
			}
		}
	}

	void CheckAmmoCollisions()
	{
		int i = 0;
		int bottom1 = _player->Position->Y + _player->SourceRect->Height;
		int bottom2 = 0;
		int left1 = _player->Position->X;
		int left2 = 0;
		int right1 = _player->Position->X + _player->SourceRect->Width;
		int right2 = 0;
		int top1 = _player->Position->Y;
		int top2 = 0;

		for (i = 0; i < AMMOCOUNT; i++)
		{
			bottom2 = _ammo[i]->Position->Y + _ammo[i]->Rect->Height;
			left2 = _ammo[i]->Position->X;
			right2 = _ammo[i]->Position->X + _ammo[i]->Rect->Width;
			top2 = _ammo[i]->Position->Y;

			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
			{
				_ammo[i]->Alive = false;
				Audio::Play(_ammoPickup);
				i = AMMOCOUNT;
			}

		}
	}

	void CheckAmmoScore()
	{
		int i;
		int s = 0;
		for (i = 0; i < AMMOCOUNT; i++)
		{
			if (!_ammo[i]->Alive)
				s += 1;
			_ammoCollected = s;
		}
	}

	void CheckHealthCollisions()
	{
		int i = 0;
		int bottom1 = _player->Position->Y + _player->SourceRect->Height;
		int bottom2 = 0;
		int left1 = _player->Position->X;
		int left2 = 0;
		int right1 = _player->Position->X + _player->SourceRect->Width;
		int right2 = 0;
		int top1 = _player->Position->Y;
		int top2 = 0;

		for (i = 0; i < HEALTHCOUNT; i++)
		{
			bottom2 = _health[i]->Position->Y + _health[i]->Rect->Height;
			left2 = _health[i]->Position->X;
			right2 = _health[i]->Position->X + _health[i]->Rect->Width;
			top2 = _health[i]->Position->Y;

			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
			{
				_health[i]->Alive = false;
				Audio::Play(_healthPickup);
				i = HEALTHCOUNT;
			}
		}
	}

	void CheckHealthScore()
	{
		int i;
		int s = 0;
		for (i = 0; i < HEALTHCOUNT; i++)
		{
			if (!_health[i]->Alive)
				s += 1;
			_livesGained = s;
		}
	}

	void SpiderMovement(spider* spider,int elapsedTime)
	{
		int targetX = _player->Position->X - spider->Position->X;
		int targetY = _player->Position->Y - spider->Position->Y;

		spider->Position->X += targetX * _cSpiderSpeed;
		spider->Position->Y += targetY * _cSpiderSpeed;
	}

	//check methods
	void CheckStart(Input::KeyboardState* state, Input::Keys key) 
	{
		if (state->IsKeyDown(key) && !_menu->GameLoop)
		{
			_menu->GameLoop = true;
			_menu->Start = !_menu->Start;
		}

	}
	void CheckPause(Input::KeyboardState* state, Input::Keys key) 
	{
		//Pause switch
		if (state->IsKeyDown(key) && !_menu->PKeyDown)
		{
			_menu->PKeyDown = true;
			_menu->Paused = !_menu->Paused;
		}

		if (state->IsKeyUp(key))
			_menu->PKeyDown = false;
	}
	void CheckRespawn(Input::KeyboardState* state, Input::Keys key)
	{
		if (state->IsKeyDown(key) && !_player)
		{
			_player->Alive = true;
			_player->Position = new Vector2(400.0f, 400.0f);
		}
	}
	void CheckViewportCollision() 
	{
		if (_player->Position->X + _player->SourceRect->Width > Graphics::GetViewportWidth())
		{
			_player->Position->X = Graphics::GetViewportWidth() - _player->SourceRect->Width;
		}

		if (_player->Position->X - _player->SourceRect->Width < -(64))
		{
			_player->Position->X = -(64) + _player->SourceRect->Width;
		}

		if (_player->Position->Y + _player->SourceRect->Height > Graphics::GetViewportHeight())
		{
			_player->Position->Y = Graphics::GetViewportHeight() - _player->SourceRect->Height;
		}

		if (_player->Position->Y - _player->SourceRect->Height < -(64))
		{
			_player->Position->Y = -(64) + _player->SourceRect->Height;
		}
	}

	//update methods
	void UpdatePlayer(int elapsedTime) 
	{
		_player->CurrentFrameTime += elapsedTime;
		if (_player->CurrentFrameTime > _cPlayerFrameTime)
		{
			_player->Frame++;

			if (_player->Frame >= 2)
				_player->Frame = 0;

			_player->CurrentFrameTime = 0;
		}
		_player->SourceRect->X = _player->SourceRect->Width * _player->Frame;
	}
	void UpdateMunchie(int elapsedTime);
	void UpdateSpider();

public:
	/// <summary> Constructs the Player class. </summary>
	Player(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Player class. </summary>
	virtual ~Player();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};