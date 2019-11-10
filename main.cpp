#include "pch.h"

#pragma region generalDirectives
// SDL libs
#pragma comment(lib, "sdl2.lib")
#pragma comment(lib, "SDL2main.lib")

// OpenGL libs
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"Glu32.lib")

// SDL extension libs 
#pragma comment(lib, "SDL2_image.lib") // Library to load image files
#pragma comment(lib, "SDL2_ttf.lib") // Library to use fonts

// SDL and OpenGL Includes
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL\GLU.h>

#include <SDL_image.h> // Image loading
#include <SDL_ttf.h> // Font
#pragma endregion generalDirectives

#include <iostream>
#include <string>
#include <ctime>
#include <chrono>
#include <array>

#include "structs.h"
#include "Block.h"
#include "Color.h"
#include "Paddle.h"
#include "Ball.h"


#pragma region windowInformation
const float g_WindowWidth{ 1400.0f };
const float g_WindowHeight{ 700.0f };
const std::string g_WindowTitle{ "Breakout - Group - 1DAE08" };
bool g_IsVSyncOn{ true };
#pragma endregion windowInformation

#pragma region textureDeclarations
struct Texture
{
	GLuint id;
	float width;
	float height;
};

bool TextureFromFile(const std::string& path, Texture& texture);
bool TextureFromString(const std::string& text, TTF_Font* pFont, const Color4f& textColor, Texture& texture);
bool TextureFromString(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor, Texture& texture);
void TextureFromSurface(const SDL_Surface* pSurface, Texture& textureData);
void DrawTexture(const Texture& texture, const Point2f& dstBottomLeft, const Rectf& srcRect = {});
void DrawTexture(const Texture& texture, const Rectf& dstRect, const Rectf& srcRect = {});
void DeleteTexture(Texture& texture);

#pragma endregion textureDeclarations

#pragma region coreDeclarations
// Functions
void Initialize( );
void Run( );
void Cleanup( );
void QuitOnSDLError( );
void QuitOnOpenGlError( );
void QuitOnSDLError();
void QuitOnOpenGlError();
void QuitOnImageError();
void QuitOnTtfError();

// Variables
SDL_Window* g_pWindow{ nullptr }; // The window we'll be rendering to
SDL_GLContext g_pContext; // OpenGL context
Uint32 g_MilliSeconds{};
const Uint32 g_MaxElapsedTime{ 100 };
#pragma endregion coreDeclarations

#pragma region gameDeclarations

// Update
void Update(float elapsedSec);

// Movement functions
void MoveGameBall(float elapsedTime);

// Create objects
void CreateGameBlockObjects();

// Draw functions
void Draw( );
void ClearBackground( );
void DrawBlockObjects();
void DrawPaddle();
void DrawBall();
void DrawHelperLine();	// will become splitscreen/field coop.
void DrawControleScheme();
void DrawPauzeTexture();

// Game logic functions 
void InitGameResources();
void FreeGameResources();
void CheckBlockDestruction();
void ProcessKeyDownEvent(const SDL_KeyboardEvent& e);
void ProcessKeyUpEvent(const SDL_KeyboardEvent& e);
void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e);
void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e);
void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e);

// Variables
const int g_Border{ 20 };
const int g_BlockSize{ 50 };	// Move to class
const int g_Depth{ 35 };		// Move to class
const int g_BlockRows{ 3 };

float g_ElapsedSeconds{};
int g_Score{};
int g_BlockAmount{};
bool g_Debugging{ true };
bool g_Pause{ false };
bool g_ControleSchemeToggle{ false };
bool g_Quit{ false };

Block* g_GameBlocksDynamic;
Paddle g_GamePaddle{ 120.f, 30.f, 800.f, Point2f{ (g_WindowWidth / 2) - 60, g_Border} };
Ball g_GameBall{ Point2f{(g_WindowWidth / 2), (g_WindowHeight / 2)}, 20.f, 120.f };

Texture g_GameInformation{};
Texture g_PauzedTexture{};

std::chrono::steady_clock::time_point g_StartTime{};

#pragma endregion gameDeclarations

#pragma region TODO

	/*
		0a) Check arrays (Pointers, References, Coppies, memory usage.)
		0b) Change color array to Color4f and pass by reference. 
		1) Block spacing.
		2) Block attributes.
		3) Multi destruction testing and optimization. 
	->	4) Pauze button.
		5) Better hit detection system
		6a) Start screen with player options.
		6b) Split screen 2 player mode.
		7) Powerups logic.
		8) Powerups spawning.
		9) Multi level creating, level switch on completion.
		10) DYNAMIC LEVELS! 
			- Computer generated levels.
			- Random alltering blocks & atributes.
			- Random placing of blocks within set boundries.
		11) Score on screen.
		12) Playername on screen.
		13) FPS counter on screen.
		14) Info button with info about used keys.
		15) Slicker animation & movement.
			- Smoother ball & paddle movement.
			- Bounce particles when bal hits paddle, boundries or blocks
			- Block cracks when hit.
			- Block disolve particles when destroyed. 
			a) Find a way to make particles, if no good way. Using forloop to generate randomly placed black squares within boundries as part of the 'block explosion'.
		16) READY FOR Q-WEEK!!! One can only hope. 
	*/

#pragma endregion gameDeclarations


int main( int argc, char* args[] )
{
	// seed the pseudo random number generator
	srand(unsigned int(time(nullptr)));	

	// Initialize SDL and OpenGL
	Initialize( );

	// Create objects
	CreateGameBlockObjects();

	// Event loop
	Run( );

	// Clean up SDL and OpenGL
	Cleanup( );

	return 0;
}

#pragma region gameImplementations

void InitGameResources()
{
	TextureFromFile("Resources/BreakoutRules.png", g_GameInformation);
	TextureFromFile("Resources/Pauzed.png", g_PauzedTexture);
}

void FreeGameResources()
{
	DeleteTexture(g_GameInformation);
	DeleteTexture(g_PauzedTexture);
}

void Update(float elapsedSec)
{
	MoveGameBall(elapsedSec);
	CheckBlockDestruction();
}


#pragma region objectCreation

void CreateGameBlockObjects()
{
	Block gameBlocks[250];
	Color colors[5]{ Color::red, Color::green, Color::blue, Color::yellow, Color::white };
	float colorValues[5][4]{ {1.f, 0.f, 0.f, 1.f}, {0.f, 1.f, 0.f, 1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 1.f, 0.f, 1.f}, {1.f, 1.f, 1.f, 1.f} };
	int counter{ 0 };
	const int blockAmount{ int((g_WindowWidth - (2 * g_Border)) / g_BlockSize) };
	for (int i = 0; i < blockAmount; i++)
	{
		for (int y = 1; y <= g_BlockRows; y++)
		{
			std::array<float, 4> ColorValues{ colorValues[counter % 5][0] , colorValues[counter % 5][1], colorValues[counter % 5][2], colorValues[counter % 5][3] };
			gameBlocks[counter] = Block((g_WindowHeight - g_Border) - (y * g_Depth), float(g_Border + (i * g_BlockSize)), colors[counter % 5], ColorValues);
			counter++;
		}
	}

	int locationCounter{};
	for (Block block : gameBlocks)
	{
		if (block.GetLowerLimit() == 0 && block.GetStartingHorizontalValue() == 0)
		{
			break;
		}
		locationCounter++;
	}

	g_GameBlocksDynamic = new Block[locationCounter];

	g_BlockAmount = locationCounter;
	locationCounter = 0;
	for (Block block : gameBlocks)
	{
		if (block.GetLowerLimit() != 0 && block.GetStartingHorizontalValue() != 0)
		{
			g_GameBlocksDynamic[locationCounter] = block;
			locationCounter++;
		}
	}
	std::cout << g_GameBlocksDynamic[5].GetStartingHorizontalValue() << "\n";
	std::cout << g_GameBlocksDynamic[28].GetStartingHorizontalValue() << "\n";
}

#pragma endregion objectCreation

#pragma region drawFunctions

void Draw()
{
	ClearBackground();
	if (!g_ControleSchemeToggle)
	{
		DrawBlockObjects();
		DrawPaddle();
		DrawBall();
		if(g_Pause)
		{
			DrawPauzeTexture();
		}
	}
	else if (g_ControleSchemeToggle)
	{
		DrawControleScheme();
	}
	
	// Will become coop.
	/*if (g_Debugging)
	{
		DrawHelperLine();
	}*/
}

void ClearBackground()
{
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void DrawBlockObjects()
{
	for (int i = 0; i < g_BlockAmount; i++)
	{
		Block block = g_GameBlocksDynamic[i];
		if (!block.IsDestroyed())
		{
			glColor4f(block.GetColorValue()[0], block.GetColorValue()[1], block.GetColorValue()[2], block.GetColorValue()[3]);
			glBegin(GL_POLYGON);
			glVertex2f(block.GetStartingHorizontalValue(), block.GetLowerLimit());
			glVertex2f(block.GetStartingHorizontalValue(), block.GetLowerLimit() + g_Depth);
			glVertex2f(block.GetStartingHorizontalValue() + g_BlockSize, block.GetLowerLimit() + g_Depth);
			glVertex2f(block.GetStartingHorizontalValue() + g_BlockSize, block.GetLowerLimit());
			glEnd();
		}
	}
	//g_GameBlocksDynamic[rand() % 100].SetDestroyed(); // simulate block destruction.
}

void DrawPaddle()
{
	glColor4f(1.f, 1.f, 1.f, 1.f);
	glBegin(GL_POLYGON);
	glVertex2f(g_GamePaddle.GetPaddlePosition().x, g_GamePaddle.GetPaddlePosition().y);
	glVertex2f(g_GamePaddle.GetPaddlePosition().x, g_GamePaddle.GetPaddlePosition().y + g_GamePaddle.GetPaddleHeight());
	glVertex2f(g_GamePaddle.GetPaddlePosition().x + g_GamePaddle.GetPaddleWidth(), g_GamePaddle.GetPaddlePosition().y + g_GamePaddle.GetPaddleHeight());
	glVertex2f(g_GamePaddle.GetPaddlePosition().x + g_GamePaddle.GetPaddleWidth(), g_GamePaddle.GetPaddlePosition().y);
	glEnd();
}

void DrawBall()
{
	float angle{};
	glColor4f(.4f, .9f, .5f, 1.f);
	glBegin(GL_LINE_LOOP);
	for (angle; angle < (M_PI * 2); angle += float((M_PI * 2)) / 360.f)
	{
		glVertex2f(g_GameBall.GetPosition().x + (g_GameBall.GetRadius() * cos((angle))), g_GameBall.GetPosition().y + (g_GameBall.GetRadius() * sin((angle))));
	}
	glEnd();
}

void DrawHelperLine()
{		
	glBegin(GL_LINES);
	glVertex2f(g_WindowWidth / 2, 0);
	glVertex2f(g_WindowWidth / 2, g_WindowHeight);
	glEnd();
}

void DrawControleScheme()
{
	DrawTexture(g_GameInformation, Point2f{ ((g_WindowWidth - g_GameInformation.width) / 2), ((g_WindowHeight - g_GameInformation.height) / 2)});
}

void DrawPauzeTexture()
{
	DrawTexture(g_PauzedTexture, Point2f{ ((g_WindowWidth - g_PauzedTexture.width) / 2), ((g_WindowHeight - g_PauzedTexture.height) / 2) });
}

#pragma endregion drawFunctions

#pragma region movementFunction

void MoveGameBall(float elapsedTime) {
	const int scoreForLosingBall{ 250 };

	if (g_GameBall.GetPosition().y - g_GameBall.GetRadius() <= 0)
	{
		g_GameBall.SetLocation(Point2f{ (g_WindowWidth / 2), (g_WindowHeight / 2) });
		g_Score -= scoreForLosingBall;
		std::cout << "Ball has been lost, respawing at the middle of the screen! You lost " << scoreForLosingBall << " Score points, and are now at: " << g_Score << " total score. \n";
	}
	else if (g_GameBall.GetPosition().y + g_GameBall.GetRadius() >= g_WindowHeight)
	{
		g_GameBall.SetYSpeed(-g_GameBall.GetYSpeed());
	}
	else if (g_GameBall.GetPosition().x - g_GameBall.GetRadius() <= 0 || g_GameBall.GetPosition().x + g_GameBall.GetRadius() >= g_WindowWidth)
	{
		g_GameBall.SetXSpeed(-g_GameBall.GetXSpeed());
	}

	if (g_GameBall.GetPosition().y  - g_GameBall.GetRadius() <= (g_GamePaddle.GetPaddlePosition().y + g_GamePaddle.GetPaddleHeight()) && g_GameBall.GetPosition().x >= g_GamePaddle.GetPaddlePosition().x && g_GameBall.GetPosition().x <= g_GamePaddle.GetPaddlePosition().x + g_GamePaddle.GetPaddleWidth() && g_GameBall.GetYSpeed() < 0)
	{
		g_GameBall.SetYSpeed(-g_GameBall.GetYSpeed());
	}

	g_GameBall.ChangeLocation(elapsedTime);
}

#pragma endregion movementFunctions

#pragma region gameLogicFunctions

void CheckBlockDestruction() {
	const int scoreForDestroyingBlock{ 100 };
	for (int i = g_BlockAmount; i > 0; i--)
	{
		Block block = g_GameBlocksDynamic[i];
		if (!block.IsDestroyed())
		{
			if (g_GameBall.GetPosition().y + g_GameBall.GetRadius() >= block.GetLowerLimit() && g_GameBall.GetPosition().x >= block.GetStartingHorizontalValue() && g_GameBall.GetPosition().x <= (block.GetStartingHorizontalValue() + g_BlockSize))
			{
				g_GameBlocksDynamic[i].SetDestroyed();
				g_Score += scoreForDestroyingBlock;
				std::cout << "Block destroyed! You gained " << scoreForDestroyingBlock << " Score points, and are now at: " << g_Score << " total score. \n";
				if (g_GameBall.GetYSpeed() > 0)
				{
					g_GameBall.SetYSpeed(-g_GameBall.GetYSpeed());
				}
				break;
			}
		}
	}
}

void ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
		case SDLK_LEFT:
			if (g_GamePaddle.GetPaddlePosition().x >= 0)
			{
				g_GamePaddle.ChangePaddlePosition(-1, g_ElapsedSeconds);
			}
			break;
		case SDLK_RIGHT:
			if ((g_GamePaddle.GetPaddlePosition().x + g_GamePaddle.GetPaddleWidth()) <= g_WindowWidth)
			{
				g_GamePaddle.ChangePaddlePosition(0, g_ElapsedSeconds);
			}
			break;
		default:
			break;
	}
}

void ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
		case SDLK_p:
			g_Pause = !g_Pause;
			if (!g_Pause)
			{
				g_StartTime = std::chrono::steady_clock::now();
			}
			std::cout << "Game is paused. \n";
			break;
		case SDLK_i:
			g_ControleSchemeToggle = !g_ControleSchemeToggle;
			if (!g_ControleSchemeToggle)
			{
				g_StartTime = std::chrono::steady_clock::now();
			}
			if (g_ControleSchemeToggle) std::cout << "Showing Controll scheme. \n";
			break;
		case SDLK_UP:
			g_GameBall.IncreseaSpeedDifficulty();
			std::cout << "Speed difficulty has been increased! Speed is now at: " << abs(g_GameBall.GetXSpeed()) << ". You can increase and decrease it further. \n";
			break;
		case SDLK_DOWN:
			g_GameBall.DecreaseSpeedDifficulty();
			std::cout << "Speed difficulty has been decreased! Speed is now at: " << abs(g_GameBall.GetXSpeed()) << ". You can increase and decrease it further. \n";
			break;
		default:
			break;
	}
	if (e.type == SDL_QUIT)
	{
		g_Quit = true;
	}
}

void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
	//Point2f mousePos{ float( e.x ), float( g_WindowHeight - e.y ) };
}

void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{

}

void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
	//std::cout << "  [" << e.x << ", " << e.y << "]\n";
	switch (e.button)
	{
		case SDL_BUTTON_LEFT:
		{
			//std::cout << "Left mouse button released\n";
			//Point2f mousePos{ float( e.x ), float( g_WindowHeight - e.y ) };
			break;
		}
		case SDL_BUTTON_RIGHT:
			//std::cout << "Right mouse button released\n";
			break;
		case SDL_BUTTON_MIDDLE:
			//std::cout << "Middle mouse button released\n";
			break;
	}
}


#pragma endregion gameLogicFunctions

#pragma endregion gameImplementations

#pragma region coreImplementations
void Initialize( )
{
	//Initialize SDL
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		QuitOnSDLError( );
	}

	//Use OpenGL 2.1
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

	//Create window
	g_pWindow = SDL_CreateWindow(
		g_WindowTitle.c_str( ),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		int( g_WindowWidth ),
		int( g_WindowHeight ),
		SDL_WINDOW_OPENGL );

	if ( g_pWindow == nullptr )
	{
		QuitOnSDLError( );
	}

	// Create an opengl context and attach it to the window 
	g_pContext = SDL_GL_CreateContext( g_pWindow );
	if ( g_pContext == nullptr )
	{
		QuitOnSDLError( );
	}

	if ( g_IsVSyncOn )
	{
		// Synchronize buffer swap with the monitor's vertical refresh
		if ( SDL_GL_SetSwapInterval( 1 ) < 0 )
		{
			QuitOnSDLError( );
		}
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}


	// Initialize Projection matrix
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	// Set the clipping (viewing) area's left, right, bottom and top
	gluOrtho2D( 0, g_WindowWidth, 0, g_WindowHeight );

	// The viewport is the rectangular region of the window where the image is drawn.
	// Set it to the entire client area of the created window
	glViewport(0, 0, int(g_WindowWidth), int(g_WindowHeight));

	//Initialize Modelview matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// Enable color blending and use alpha blending
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		QuitOnImageError();
	}

	//Initialize SDL_ttf
	if (TTF_Init() == -1)
	{
		QuitOnTtfError();
	}
}

void Run( )
{
	g_StartTime = std::chrono::steady_clock::now();

	InitGameResources();

	SDL_Event e{};
	while (!g_Quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (!g_Pause)
			{
				switch (e.type)
				{
				case SDL_QUIT:
					ProcessKeyUpEvent(e.key);
				case SDL_KEYDOWN:
					ProcessKeyDownEvent(e.key);
					break;
				case SDL_KEYUP:
					ProcessKeyUpEvent(e.key);
					break;
				case SDL_MOUSEMOTION:
					break;
				case SDL_MOUSEBUTTONDOWN:
					break;
				case SDL_MOUSEBUTTONUP:
					break;
				default:
					break;
				}
			}
			else
			{
				switch (e.type)
				{
					case SDL_QUIT:
						ProcessKeyUpEvent(e.key);
					case SDL_KEYUP:
						ProcessKeyUpEvent(e.key);
					default:
						break;
				}
			}
		}

		if (!g_Quit)
		{
			if (!g_Pause && !g_ControleSchemeToggle)
			{
				std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
				g_ElapsedSeconds = std::chrono::duration<float>(endTime - g_StartTime).count();
				g_StartTime = endTime;

				if (g_ElapsedSeconds > g_MaxElapsedTime)
				{
					g_ElapsedSeconds = g_MaxElapsedTime;
				}
				Update(g_ElapsedSeconds);
			}
			Draw();
			SDL_GL_SwapWindow(g_pWindow);
		}
	}
	FreeGameResources();
}

void Cleanup( )
{
	SDL_GL_DeleteContext( g_pContext );

	SDL_DestroyWindow( g_pWindow );
	g_pWindow = nullptr;

	SDL_Quit( );
}


void QuitOnSDLError( )
{
	std::cout << "Problem during SDL initialization: ";
	std::cout << SDL_GetError( );
	std::cout << std::endl;
	Cleanup( );
	exit( -1 );
}
void QuitOnOpenGlError( )
{
	std::cout << "Problem during OpenGL initialization: ";
	std::cout << SDL_GetError( );
	std::cout << std::endl;
	Cleanup( );
	exit( -1 );
}

void QuitOnImageError()
{
	std::cout << "Problem during SDL_image initialization: ";
	std::cout << IMG_GetError();
	std::cout << std::endl;
	Cleanup();
	exit(-1);
}

void QuitOnTtfError()
{
	std::cout << "Problem during SDL_ttf initialization: ";
	std::cout << TTF_GetError();
	std::cout << std::endl;
	Cleanup();
	exit(-1);
}

#pragma endregion coreImplementations

#pragma region textureImplementations

bool TextureFromFile(const std::string& path, Texture& texture)
{
	//Load file for use as an image in a new surface.
	SDL_Surface* pLoadedSurface = IMG_Load(path.c_str());
	if (pLoadedSurface == nullptr)
	{
		std::cerr << "TextureFromFile: SDL Error when calling IMG_Load: " << SDL_GetError() << std::endl;
		return false;
	}

	TextureFromSurface(pLoadedSurface, texture);

	//Free loaded surface
	SDL_FreeSurface(pLoadedSurface);

	return true;
}

bool TextureFromString(const std::string& text, const std::string& fontPath, int ptSize, const Color4f& textColor, Texture& texture)
{
	// Create font
	TTF_Font* pFont{};
	pFont = TTF_OpenFont(fontPath.c_str(), ptSize);
	if (pFont == nullptr)
	{
		std::cout << "TextureFromString: Failed to load font! SDL_ttf Error: " << TTF_GetError();
		std::cin.get();
		return false;
	}

	// Create texture using this fontand close font afterwards
	bool textureOk = TextureFromString(text, pFont, textColor, texture);
	TTF_CloseFont(pFont);

	return textureOk;
}

bool TextureFromString(const std::string& text, TTF_Font* pFont, const Color4f& color, Texture& texture)
{
	//Render text surface
	SDL_Color textColor{};
	textColor.r = Uint8(color.r * 255);
	textColor.g = Uint8(color.g * 255);
	textColor.b = Uint8(color.b * 255);
	textColor.a = Uint8(color.a * 255);

	SDL_Surface* pLoadedSurface = TTF_RenderText_Blended(pFont, text.c_str(), textColor);
	//SDL_Surface* pLoadedSurface = TTF_RenderText_Solid(pFont, textureText.c_str(), textColor);
	if (pLoadedSurface == nullptr)
	{
		std::cerr << "TextureFromString: Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << '\n';
		return false;
	}

	// copy to video memory
	TextureFromSurface(pLoadedSurface, texture);

	//Free loaded surface
	SDL_FreeSurface(pLoadedSurface);

	return true;
}

void TextureFromSurface(const SDL_Surface* pSurface, Texture& texture)
{
	//Get image dimensions
	texture.width = float(pSurface->w);
	texture.height = float(pSurface->h);

	// Get pixel format information and translate to OpenGl format
	GLenum pixelFormat{ GL_RGB };
	switch (pSurface->format->BytesPerPixel)
	{
	case 3:
		if (pSurface->format->Rmask == 0x000000ff)
		{
			pixelFormat = GL_RGB;
		}
		else
		{
			pixelFormat = GL_BGR;
		}
		break;
	case 4:
		if (pSurface->format->Rmask == 0x000000ff)
		{
			pixelFormat = GL_RGBA;
		}
		else
		{
			pixelFormat = GL_BGRA;
		}
		break;
	default:
		std::cerr << "TextureFromSurface error: Unknow pixel format, BytesPerPixel: " << pSurface->format->BytesPerPixel << "\nUse 32 bit or 24 bit images.\n";;
		texture.width = 0;
		texture.height = 0;
		return;
	}

	//Generate an array of textures.  We only want one texture (one element array), so trick
	//it by treating "texture" as array of length one.
	glGenTextures(1, &texture.id);

	//Select (bind) the texture we just generated as the current 2D texture OpenGL is using/modifying.
	//All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
	glBindTexture(GL_TEXTURE_2D, texture.id);

	// check for errors.
	GLenum e = glGetError();
	if (e != GL_NO_ERROR)
	{
		std::cerr << "TextureFromSurface, error binding textures, Error id = " << e << '\n';
		texture.width = 0;
		texture.height = 0;
		return;
	}

	//Specify the texture's data.  This function is a bit tricky, and it's hard to find helpful documentation.  A summary:
	//   GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
	//               0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
	//                         not cached smaller copies)
	//         GL_RGBA:    Specifies the number of color components in the texture.
	//                     This is how OpenGL will store the texture internally (kinda)--
	//                     It's essentially the texture's type.
	//      surface->w:    The width of the texture
	//      surface->h:    The height of the texture
	//               0:    The border.  Don't worry about this if you're just starting.
	//     pixelFormat:    The format that the *data* is in--NOT the texture! 
	//GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
	//                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
	//                         any value that fits in one byte (so 0 through 255).  These values are to be interpreted as
	//                         *unsigned* values (since 0x00 should be dark and 0xFF should be bright).
	// surface->pixels:    The actual data.  As above, SDL's array of bytes.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pSurface->w, pSurface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, pSurface->pixels);

	//Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
	//*smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
	//each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
	//further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
	//them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void DeleteTexture(Texture& texture)
{
	glDeleteTextures(1, &texture.id);
}

void DrawTexture(const Texture& texture, const Point2f& dstBottomLeft, const Rectf& srcRect)
{
	Rectf dstRect{ dstBottomLeft.x, dstBottomLeft.y, srcRect.width, srcRect.height };
	DrawTexture(texture, dstRect, srcRect);
}


void DrawTexture(const Texture& texture, const Rectf& dstRect, const Rectf& srcRect)
{
	// Determine texture coordinates using srcRect and default destination width and height
	float textLeft{};
	float textRight{};
	float textTop{};
	float textBottom{};

	float defaultDstWidth{};
	float defaultDstHeight{};
	if (!(srcRect.width > 0.0f && srcRect.height > 0.0f)) // No srcRect specified
	{
		// Use complete texture
		textLeft = 0.0f;
		textRight = 1.0f;
		textTop = 0.0f;
		textBottom = 1.0f;

		defaultDstHeight = texture.height;
		defaultDstWidth = texture.width;
	}
	else // srcRect specified
	{
		// Convert to the range [0.0, 1.0]
		textLeft = srcRect.left / texture.width;
		textRight = (srcRect.left + srcRect.width) / texture.width;
		textTop = (srcRect.bottom - srcRect.height) / texture.height;
		textBottom = srcRect.bottom / texture.height;

		defaultDstHeight = srcRect.height;
		defaultDstWidth = srcRect.width;
	}

	// Determine vertex coordinates
	float vertexLeft{ dstRect.left };
	float vertexBottom{ dstRect.bottom };
	float vertexRight{};
	float vertexTop{};
	if (!(dstRect.width > 0.0f && dstRect.height > 0.0f)) // If no size specified use default size
	{
		vertexRight = vertexLeft + defaultDstWidth;
		vertexTop = vertexBottom + defaultDstHeight;
	}
	else
	{
		vertexRight = vertexLeft + dstRect.width;
		vertexTop = vertexBottom + dstRect.height;
	}

	// Tell opengl which texture we will use
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Draw
	glEnable(GL_TEXTURE_2D);
	{
		glBegin(GL_QUADS);
		{
			glTexCoord2f(textLeft, textBottom);
			glVertex2f(vertexLeft, vertexBottom);

			glTexCoord2f(textLeft, textTop);
			glVertex2f(vertexLeft, vertexTop);

			glTexCoord2f(textRight, textTop);
			glVertex2f(vertexRight, vertexTop);

			glTexCoord2f(textRight, textBottom);
			glVertex2f(vertexRight, vertexBottom);
		}
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

}
#pragma endregion textureImplementations