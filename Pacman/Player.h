#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"

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
};

//Structure for the ammo
struct ammo
{
	// Data to represent Pick-ups
	Rect* Rect;
	Texture2D* Texture;
	Vector2* Position;
};

// Declares the Player class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Player : public Game
{
private:	
	//initalise struct
	protagonist* _player;
	ammo* _ammo;

	//movement and rotation
	float angle;
	Vector2 point;

	//gunplay
	Vector2* _bulletPosition;
	int _ammoAmount;
	int _magazineAmount;
	bool _currentAction;
	Texture2D* _bulletTexture;
	Rect* _bulletRect;

	// Position for String
	Vector2* _stringPosition;

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

	//Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _pKeyDown;
	bool _start;
	bool _gameLoop;

	//input methods
	void Input(int elapsedTime, Input::KeyboardState* state) 
	{
		if (state->IsKeyDown(Input::Keys::D))
			angle -= (_cRotationSpeed * elapsedTime);

		if (state->IsKeyDown(Input::Keys::A))
			angle += (_cRotationSpeed * elapsedTime);

		if (state->IsKeyDown(Input::Keys::W))
		{
			_player->Position->X += sin(angle) * _cMoveSpeed * elapsedTime;
			_player->Position->Y += cos(angle) * _cMoveSpeed * elapsedTime;
		}

		if (state->IsKeyDown(Input::Keys::S))
		{
			_player->Position->X -= sin(angle) * _cMoveSpeed * elapsedTime;
			_player->Position->Y -= cos(angle) * _cMoveSpeed * elapsedTime;
		}
	}
	void Action(int elapsedTime, Input::KeyboardState* state)
	{
		if (_magazineAmount > 0 && state->IsKeyDown(Input::Keys::J) && !_currentAction)
		{
			_currentAction = true;
			SpriteBatch::Draw(_bulletTexture, _player->Position, _bulletRect);
			_bulletPosition->X += sin(angle) * _cBulletVelocity * elapsedTime;
			_bulletPosition->Y += cos(angle) * _cBulletVelocity * elapsedTime;
		}
		if (state->IsKeyDown(Input::Keys::L) && !_currentAction)
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
		if (state->IsKeyDown(Input::Keys::K) && !_currentAction) 
		{
			_currentAction = true;
		}
	}

	//check methods
	void CheckStart(Input::KeyboardState* state, Input::Keys key) 
	{
		if (state->IsKeyDown(key) && !_gameLoop)
		{
			_gameLoop = true;
			_start = !_start;
		}

		if (!_player->Alive)
			_gameLoop = false;
	}
	void CheckPause(Input::KeyboardState* state, Input::Keys key) 
	{
		//Pause switch
		if (state->IsKeyDown(key) && !_pKeyDown)
		{
			_pKeyDown = true;
			_paused = !_paused;
		}

		if (state->IsKeyUp(key))
			_pKeyDown = false;
	}
	void CheckViewportCollision() 
	{
		if (_player->Position->X + _player->SourceRect->Width > Graphics::GetViewportWidth())
		{
			_player->Position->X = Graphics::GetViewportWidth() - _player->SourceRect->Width;
		}

		if (_player->Position->X - _player->SourceRect->Width < -64)
		{
			_player->Position->X = -64 + _player->SourceRect->Width;
		}

		if (_player->Position->Y + _player->SourceRect->Height > Graphics::GetViewportHeight())
		{
			_player->Position->Y = Graphics::GetViewportHeight() - _player->SourceRect->Width;
		}

		if (_player->Position->Y - _player->SourceRect->Width < -64)
		{
			_player->Position->Y = -64 + _player->SourceRect->Height;
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

public:
	/// <summary> Constructs the Pacman class. </summary>
	Player(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Player();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};