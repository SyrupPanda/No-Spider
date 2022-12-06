#include "Pacman.h"

#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(250), _cMunchieFrameTime(500)
{
	_frameCount = 0;
	_paused = false;
	_pKeyDown = false;
	_pacmanCurrentFrameTime = 0;
	_pacmanFrame = 0;
	_munchieCurrentFrameTime = 0;

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacmanTexture;
	delete _pacmanSourceRect;
	delete _munchieBlueTexture;
	delete _munchieInvertedTexture;
	delete _munchieRect;
}

void Pacman::LoadContent()
{
	// Load Pacman
	_pacmanTexture = new Texture2D();
	_pacmanTexture->Load("Textures/Pacman.tga", false);
	_pacmanPosition = new Vector2(350.0f, 350.0f);
	_pacmanSourceRect = new Rect(0.0f, 0.0f, 32, 32);
	_pacmanDirection = 0;

	// Load Munchie
	_munchieBlueTexture = new Texture2D();
	_munchieBlueTexture->Load("Textures/Munchie.tga", true);
	_munchieInvertedTexture = new Texture2D();
	_munchieInvertedTexture->Load("Textures/MunchieInverted.tga", true);
	_munchieRect = new Rect(100.0f, 450.0f, 12, 12);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	//Menu
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth()/2.0f, Graphics::GetViewportHeight() / 2.0f);
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	if (!_paused)
	{
		// Checks if D key is pressed
		if (keyboardState->IsKeyDown(Input::Keys::D))
		{
			_pacmanPosition->X += _cPacmanSpeed * elapsedTime; //Moves Pacman across X axis
			_pacmanDirection = 0;
		}
		if (keyboardState->IsKeyDown(Input::Keys::A))
		{
			_pacmanPosition->X -= _cPacmanSpeed * elapsedTime;
			_pacmanDirection = 2;
		}
		if (keyboardState->IsKeyDown(Input::Keys::W))
		{
			_pacmanPosition->Y -= _cPacmanSpeed * elapsedTime;
			_pacmanDirection = 3;
		}
		if (keyboardState->IsKeyDown(Input::Keys::S))
		{
			_pacmanPosition->Y += _cPacmanSpeed * elapsedTime;
			_pacmanDirection = 1;
		}
		//End of movemnet

		//Changes sprite of player towards direction
		_pacmanSourceRect->Y = _pacmanSourceRect->Height * _pacmanDirection;

		//Pacman animation
		_pacmanCurrentFrameTime += elapsedTime;
		if (_pacmanCurrentFrameTime > _cPacmanFrameTime)
		{
			_pacmanFrame++;

			if (_pacmanFrame >= 2)
				_pacmanFrame = 0;

			_pacmanCurrentFrameTime = 0;
		}

		_pacmanSourceRect->X = _pacmanSourceRect->Width * _pacmanFrame;

		//Start of off screen
		if (_pacmanPosition->X + _pacmanSourceRect->Width > Graphics::GetViewportWidth())
		{
			_pacmanPosition->X = -32 + _pacmanSourceRect->Width;
		}

		if (_pacmanPosition->X - _pacmanSourceRect->Width < -32)
		{
			_pacmanPosition->X = Graphics::GetViewportWidth() - _pacmanSourceRect->Width;
		}

		if (_pacmanPosition->Y + _pacmanSourceRect->Height > Graphics::GetViewportHeight())
		{
			_pacmanPosition->Y = -32 + _pacmanSourceRect->Height;
		}

		if (_pacmanPosition->Y - _pacmanSourceRect->Width < -32)
		{
			_pacmanPosition->Y = Graphics::GetViewportHeight() - _pacmanSourceRect->Width;
		}

		_frameCount++;
	}

	//Pause switch
	if (keyboardState->IsKeyDown(Input::Keys::P) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
		
	}

	if (keyboardState->IsKeyUp(Input::Keys::P))
	{
		_pKeyDown = false;
	}

}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacmanPosition->X << " Y: " << _pacmanPosition->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing
	SpriteBatch::Draw(_pacmanTexture, _pacmanPosition, _pacmanSourceRect); // Draws Pacman

	if (_munchieCurrentFrameTime > _cMunchieFrameTime)
	{
		_frameCount++;
		if (_frameCount >= 2)
			_frameCount = 0;

		_munchieCurrentFrameTime = 0;
	}
	
	//Menu
	if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";

		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing
	
}