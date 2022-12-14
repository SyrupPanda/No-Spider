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

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Player : public Game
{
private:
	// Data to represent Player
	Vector2* _playerPosition;
	Rect* _playerSourceRect;
	Texture2D* _playerTexture;
	int _playerFrame;
	int _playerCurrentFrameTime;
	bool _playerAlive;

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

	// Data to represent Pick-ups
	Rect* _ammoRect;
	Texture2D* _ammoTexture;
	Vector2* _ammoPosition;

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
			angle += (_cRotationSpeed * elapsedTime);

		if (state->IsKeyDown(Input::Keys::A))
			angle -= (_cRotationSpeed * elapsedTime);

		if (state->IsKeyDown(Input::Keys::W))
		{
			_playerPosition->X += sin(angle) * _cMoveSpeed * elapsedTime;
			_playerPosition->Y += cos(angle) * _cMoveSpeed * elapsedTime;
		}

		if (state->IsKeyDown(Input::Keys::S))
		{
			_playerPosition->X -= sin(angle) * _cMoveSpeed * elapsedTime;
			_playerPosition->Y -= cos(angle) * _cMoveSpeed * elapsedTime;
		}
	}
	void Action(int elapsedTime, Input::KeyboardState* state)
	{
		if (_magazineAmount > 0 && state->IsKeyDown(Input::Keys::J) && !_currentAction)
		{
			_currentAction = true;
			SpriteBatch::Draw(_bulletTexture, _playerPosition, _bulletRect);
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

		if (!_playerAlive)
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
		if (_playerPosition->X + _playerSourceRect->Width > Graphics::GetViewportWidth())
		{
			_playerPosition->X = Graphics::GetViewportWidth() - _playerSourceRect->Width;
		}

		if (_playerPosition->X - _playerSourceRect->Width < -64)
		{
			_playerPosition->X = -64 + _playerSourceRect->Width;
		}

		if (_playerPosition->Y + _playerSourceRect->Height > Graphics::GetViewportHeight())
		{
			_playerPosition->Y = Graphics::GetViewportHeight() - _playerSourceRect->Width;
		}

		if (_playerPosition->Y - _playerSourceRect->Width < -64)
		{
			_playerPosition->Y = -64 + _playerSourceRect->Height;
		}
	}

	//update methods
	void UpdatePlayer(int elapsedTime) 
	{
		_playerCurrentFrameTime += elapsedTime;
		if (_playerCurrentFrameTime > _cPlayerFrameTime)
		{
			_playerFrame++;

			if (_playerFrame >= 2)
				_playerFrame = 0;

			_playerCurrentFrameTime = 0;
		}
		_playerSourceRect->X = _playerSourceRect->Width * _playerFrame;
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