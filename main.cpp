#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>

#define PI 3.14159265359

typedef uint8_t byte;

// 20 is the amount of rows and 12 is the amount of columns in the game.
const int ROWS = 30;
const int COLUMNS = 20;

byte grid[ROWS][COLUMNS];
byte colliders[ROWS][COLUMNS];

// Vec2 will be used to store the positions of the tetrominos pieces.
struct Vec2
{
	byte y, x;
	Vec2() {} // default constructor function(is run when no parameter is passed).Constructor functions are run when making instances of the struct

	// A parametrized constructor. Used for initializing a struct instance.
	Vec2(byte dy, byte dx)
	{
		//"this" is used when accessing members of the struct within the struct
		this->x = dx;
		this->y = dy;
	}
};

// Combines 4 blocks(Vec2) to form a single tetromino
struct Piece
{
	// here... Vec2's default constructor is run.(Since no parameter is passed)
	Vec2 a, b, c, d; // Every piece is going to have 4 individual blocks. a,b,c,d is going to store the positions of those blocks
	Piece() {}		 // default consutructor of Piece struct

	// A parametrized constructor used to intialize the member variables of the Piece struct.
	// This is in case we give indivdual coordinates as arguments
	Piece(byte ax, byte ay, byte bx, byte by, byte cx, byte cy, byte dx, byte dy)
	{
		// initializing the member variables
		this->a = Vec2(ax, ay);
		this->b = Vec2(bx, by);
		this->c = Vec2(cx, cy);
		this->d = Vec2(dx, dy);
	}

	// Another parameterized constructor function used in case we give vectors as arguments. Essentially does the same thing as the constructor above
	Piece(Vec2 da, Vec2 db, Vec2 dc, Vec2 dd)
	{
		this->a = da;
		this->b = db;
		this->c = dc;
		this->d = dd;
	}
};

// An enum is a special type that represents a group of constants (unchangeable values).
enum PIECE_TYPE
{
	S,	 // S=0
	Z,	 // Z=1
	L,	 // L=2
	J,	 // J=3
	SQR, // SQR=4
	I,	 // I=5
	T	 // T=6
};

// functions to return a tetromino
Piece CreatePiece(PIECE_TYPE type)
{
	// hardcoded shapes of the tetrominoes
	switch (type)
	{
	case S:
		return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(0, 4), Vec2(2, 5));
		break;
	case Z:
		return Piece(Vec2(1, 4), Vec2(1, 5), Vec2(0, 5), Vec2(2, 4));
		break;
	case L:
		return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 6));
		break;
	case J:
		return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 4));
		break;
	case SQR:
		return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(0, 5), Vec2(0, 4));
		break;
	case I:
		return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(1, 7));
		break;
	case T:
		return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 5));
		break;
	default:
		break;
	}
}

// takes in a tetromino returns the it's rotated version
void rotate(Piece &piece, PIECE_TYPE type)
{
	if (type == I) // Special handling for the I piece
	{
		static bool vertical = false;

		if (vertical)
		{
			// Vertical to Horizontal (Rotated 90 degrees)
			piece.b.x = piece.a.x - 1;
			piece.b.y = piece.a.y;
			piece.c.x = piece.a.x + 1;
			piece.c.y = piece.a.y;
			piece.d.x = piece.a.x + 2;
			piece.d.y = piece.a.y;
		}
		else
		{
			// Horizontal to Vertical (Back to original position)
			piece.b.x = piece.a.x;
			piece.b.y = piece.a.y - 1;
			piece.c.x = piece.a.x;
			piece.c.y = piece.a.y + 1;
			piece.d.x = piece.a.x;
			piece.d.y = piece.a.y + 2;
		}
		vertical = !vertical;
	}
	else
	{
		float angle = 90 * (PI / 180); // converting angle to radians

		// calculating distance of all blocks of the tetromino from the center block (a is the center block)
		float b_offset_x = piece.b.x - piece.a.x;
		float c_offset_x = piece.c.x - piece.a.x;
		float d_offset_x = piece.d.x - piece.a.x;

		float b_offset_y = piece.b.y - piece.a.y;
		float c_offset_y = piece.c.y - piece.a.y;
		float d_offset_y = piece.d.y - piece.a.y;

		// Equations of rotation around a pivot point(center point a)
		// gives new coordinates of the rotated blocks.
		float pbx = piece.a.x + (b_offset_x * std::cos(angle) - b_offset_y * std::sin(angle));
		float pby = piece.a.y + (b_offset_x * std::sin(angle) + b_offset_y * std::cos(angle));

		float pcx = piece.a.x + (c_offset_x * std::cos(angle) - c_offset_y * std::sin(angle));
		float pcy = piece.a.y + (c_offset_x * std::sin(angle) + c_offset_y * std::cos(angle));

		float pdx = piece.a.x + (d_offset_x * std::cos(angle) - d_offset_y * std::sin(angle));
		float pdy = piece.a.y + (d_offset_x * std::sin(angle) + d_offset_y * std::cos(angle));

		std::cout << pbx << " " << pby << " " << pcx << " " << pcy << " " << pdx << " " << pdy << std::endl;

		// Prevents rotating into objects and walls
		if (colliders[(byte)pby][(byte)pbx] != 2 &&
			colliders[(byte)pcy][(byte)pcx] != 2 &&
			colliders[(byte)pdy][(byte)pdx] != 2 &&

			(pbx >= 0 && pbx <= COLUMNS && pby >= 0 && pby <= ROWS-1) &&
			(pcx >= 0 && pcx <= COLUMNS && pcy >= 0 && pcy <= ROWS-1) &&
			(pdx >= 0 && pdx <= COLUMNS && pdy >= 0 && pdy <= ROWS-1) &&

			piece.a.x != 1 && piece.a.y != 1)
		{
			// assign the x and y coordinates of the tetrominoes to the rotated x and y coordinates
			piece.b.x = pbx;
			piece.b.y = pby;

			piece.c.x = pcx;
			piece.c.y = pcy;

			piece.d.x = pdx;
			piece.d.y = pdy;
		}
	}
}

//Functions to setup and display text
void setupInstructionText(sf::Text &score, sf::Text &pause_text, sf::Font &font, sf::Text &game_over_text ,int width, int height)
{
	font.loadFromFile("./fonts/VeniteAdoremus-rgRBA.ttf");

	score.setFont(font);
	pause_text.setFont(font);
	game_over_text.setFont(font);

	score.setCharacterSize(15);
	pause_text.setCharacterSize(30);
	game_over_text.setCharacterSize(30);

	
	score.setFillColor(sf::Color::White);
	pause_text.setFillColor(sf::Color::Blue);
	game_over_text.setFillColor(sf::Color::Blue);
	
	score.setPosition(10, 25);
	pause_text.setPosition(width/2  -200, height/2 - 150);
	game_over_text.setPosition(width/2 - 110, height/2 - 150);

	sf::Vector2<float> small_scale(2.5f, 2.5f);
	sf::Vector2<float> large_scale(3.0f, 3.0f);
	score.setScale(small_scale);
	pause_text.setScale(large_scale);
	game_over_text.setScale(large_scale);

	score.setString("Line: 0");
	game_over_text.setString("Game Over!");
	pause_text.setString("Game Paused!");
}

void displayText(sf::RenderWindow &window, sf::Text score, sf::Text pause_text, sf::Text game_over_text,int game_over, int pause)
{
	window.draw(score);
	if(game_over == 1)
	{
		window.draw(game_over_text);
	}
	else if(pause == 1)
	{
		window.draw(pause_text);
	}
}

//Drawing current falling piece and the next piece that will come
void drawCurrentAndNext(sf::RenderWindow &window, Piece &piece, Piece &next_piece, sf::Sprite tile, float tile_width, float tile_height, int next_piece_section_position_x, int next_piece_section_position_y)
{
	sf::Sprite piece_tile = tile;

	piece_tile.setPosition(tile_width * piece.a.x, tile_height * piece.a.y);
	window.draw(piece_tile);

	piece_tile.setPosition(tile_width * piece.b.x, tile_height * piece.b.y);
	window.draw(piece_tile);

	piece_tile.setPosition(tile_width * piece.c.x, tile_height * piece.c.y);
	window.draw(piece_tile);

	piece_tile.setPosition(tile_width * piece.d.x, tile_height * piece.d.y);
	window.draw(piece_tile);

	// Draw next piece
	sf::Sprite next_piece_tile = tile;

	next_piece_tile.setPosition((next_piece_section_position_x + next_piece.a.x * tile_width)-2*(tile_width)/1.5, next_piece_section_position_y + next_piece.a.y * tile_height + 3*tile_height);
	window.draw(next_piece_tile);

	next_piece_tile.setPosition(next_piece_section_position_x + next_piece.b.x * tile_width -2*(tile_width)/1.5, next_piece_section_position_y + next_piece.b.y * tile_height + 3*tile_height);
	window.draw(next_piece_tile);

	next_piece_tile.setPosition(next_piece_section_position_x + next_piece.c.x * tile_width-2*(tile_width)/1.5, next_piece_section_position_y + next_piece.c.y * tile_height + 3*tile_height);
	window.draw(next_piece_tile);

	next_piece_tile.setPosition(next_piece_section_position_x + next_piece.d.x * tile_width-2*(tile_width)/1.5, next_piece_section_position_y + next_piece.d.y * tile_height + 3*tile_height);
	window.draw(next_piece_tile);
}

//Drawing tiles which are not moving
void drawStaticTiles(sf::RenderWindow &window, sf::Sprite tile, float tile_width, float tile_height)
{
	for (size_t i = 0; i < ROWS; i++)
		{
			for (size_t j = 0; j < COLUMNS; j++)
			{
				if (colliders[i][j] == 2)
				{
					sf::Sprite t = tile;
					t.setPosition(tile_width * j, tile_height * i);
					window.draw(t);
				}
			}
		}
}

//check game over
void checkGameOver(int &game_over)
{
	for (size_t i = 0; i < COLUMNS; i++)
		{
			if (colliders[0][i] == 2)
			{
				game_over = 1;
			}
		}
}

int main()
{
	srand(time(NULL)); // setting a seed

	sf::Texture tile_tex;
	tile_tex.loadFromFile("images/tetris_tile.png");

	int width = 600, height = 900; //width and height of play area

	sf::Sprite tile(tile_tex);
	float x_scale = ((width/(float)COLUMNS)/tile_tex.getSize().x);
	float y_scale= ((height/(float)ROWS)/tile_tex.getSize().y);
	tile.setScale(x_scale,  y_scale);

	float tile_width = width/COLUMNS;
	float tile_height = height/ROWS;



	// Declaring all the important textfor the game
	sf::Text score;
	sf::Text game_over_text;
	sf::Text pause_text;
	sf::Font font;

	// Setting score, game_over,pause text properties properties
	setupInstructionText(score,pause_text, font, game_over_text, width, height);


	int right_section_width = width/1.5;
	int right_section_height = height;
	sf::RectangleShape right_section(sf::Vector2f(right_section_width, right_section_height));
	right_section.setFillColor(sf::Color::Green);
	right_section.setPosition(width, 0);

	// Setting up the sections which will show the next piece that will come
	int next_piece_section_width = right_section_width - right_section_width*0.1;
	int next_piece_section_height = right_section_height/3;
	int next_piece_section_position_x = width + right_section_width*0.1/2.0;
	int next_piece_section_position_y = 50;


	sf::RectangleShape next_piece_section(sf::Vector2f(next_piece_section_width, next_piece_section_height));
	next_piece_section.setFillColor(sf::Color::Black);
	next_piece_section.setPosition(next_piece_section_position_x, next_piece_section_position_y);

	// Setup window & create first piece
	sf::RenderWindow window(sf::VideoMode(width + right_section_width, height), "Tetris");
	window.setKeyRepeatEnabled(true);
	window.setFramerateLimit(60);


	PIECE_TYPE piece_type = static_cast<PIECE_TYPE>((rand() % 7));
	Piece piece = CreatePiece(piece_type);
	PIECE_TYPE next_piece_type = static_cast<PIECE_TYPE>((rand() % 7));
	Piece next_piece = CreatePiece(next_piece_type);

	//important variables for the game logic
	unsigned int timer = 0, scoreCounter = 0;
	float gamespeed = 20;
	float gamespeed_changing = 20;
	int game_over = 0, pause = 0;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close the game if cross button is clicked or ESCAPE key is pressed
			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				window.close();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				// Movement of the tetromino
				// Rotate the tetromino if UP arrow key is pressed
				if (event.key.code == sf::Keyboard::Up && game_over == 0 && pause == 0)
				{
					rotate(piece, piece_type);
				}
				else if ( // Move the tetromino left IF
						  // When the left arrow key is pressed
					event.key.code == sf::Keyboard::Left &&
					// no part of the tetromino is at left boundary of game
					piece.a.x != 0 && piece.b.x != 0 && piece.c.x != 0 && piece.d.x != 0 &&
					// There are no static tetromino(tetromino from previous rounds) on the left of any part of the tetromino
					(colliders[piece.a.y][piece.a.x - 1]) != 2 && (colliders[piece.b.y][piece.b.x - 1]) != 2 &&
					(colliders[piece.c.y][piece.c.x - 1]) != 2 && (colliders[piece.d.y][piece.d.x - 1]) != 2 && game_over == 0 && pause == 0 )
					
				{

					piece.a.x--;
					piece.b.x--;
					piece.c.x--;
					piece.d.x--;
				}
				else if ( // Move the piece right IF
						  // the right arrow key is pressed
					event.key.code == sf::Keyboard::Right &&
					// no part of the tetromino is at the right boundary of the game
					piece.a.x != COLUMNS-1 && piece.b.x != COLUMNS-1 && piece.c.x != COLUMNS-1 && piece.d.x != COLUMNS-1 &&
					// There are no static tetromino(tetromino from previous rounds) on the right of any part of the tetromino
					(colliders[piece.a.y][piece.a.x + 1]) != 2 && (colliders[piece.b.y][piece.b.x + 1]) != 2 &&
					(colliders[piece.c.y][piece.c.x + 1]) != 2 && (colliders[piece.d.y][piece.d.x + 1]) != 2 && game_over == 0 && pause == 0)
				{
					piece.a.x++;
					piece.b.x++;
					piece.c.x++;
					piece.d.x++;
				}
				else if(event.key.code == sf::Keyboard::P)
				{
					if(pause == 0)
					{
						pause = 1;
					}
					else
					{
						pause = 0;
					}
				}
			}
			// Increasing the magnitude of the gamespeed variables decreases the speed of the game
			if (event.type == sf::Event::KeyReleased)
			{
				gamespeed = gamespeed_changing;
			}
		}

		// Increase the speed of the game if DOWN arrow key is pressed
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		{
			gamespeed = 1;
		}

		window.clear(sf::Color(0, 0, 0, 0));

		// Refresh the grid-array
		for (size_t i = 0; i < ROWS; i++)
		{
			for (size_t j = 0; j < COLUMNS; j++)
			{
				if (colliders[i][j] == 2)
				{
					grid[i][j] = 2;
				}
				else
				{
					grid[i][j] = 0;
				}
			}
		}

		

		// Clock
		if (timer > gamespeed && game_over == 0 && pause == 0)
		{


			// Collision checks
			if ( // IF there are static tetrominoes below the falling piece
				grid[piece.a.y + 1][piece.a.x] == 2 ||
				grid[piece.b.y + 1][piece.b.x] == 2 ||
				grid[piece.c.y + 1][piece.c.x] == 2 ||
				grid[piece.d.y + 1][piece.d.x] == 2 ||
				// OR there is the bottom of the game below the falling piece
				piece.a.y == ROWS-1 || piece.b.y == ROWS-1 || piece.c.y == ROWS-1 || piece.d.y == ROWS-1)
			{
				grid[piece.a.y][piece.a.x] = 2;
				grid[piece.b.y][piece.b.x] = 2;
				grid[piece.c.y][piece.c.x] = 2;
				grid[piece.d.y][piece.d.x] = 2;

				colliders[piece.a.y][piece.a.x] = 2;
				colliders[piece.b.y][piece.b.x] = 2;
				colliders[piece.c.y][piece.c.x] = 2;
				colliders[piece.d.y][piece.d.x] = 2;

				piece_type = next_piece_type;
				piece = CreatePiece(piece_type);
				next_piece_type = static_cast<PIECE_TYPE>((rand() % 7));
				next_piece = CreatePiece(next_piece_type);
			}
			else
			{
				// Implementing gravity
				grid[piece.a.y + 1][piece.a.x] = 1;
				grid[piece.b.y + 1][piece.b.x] = 1;
				grid[piece.c.y + 1][piece.c.x] = 1;
				grid[piece.d.y + 1][piece.d.x] = 1;

				piece.a.y++;
				piece.b.y++;
				piece.c.y++;
				piece.d.y++;
			}



			// Check if the player has a line or 'tetris'
			byte tetris_row = 0;
			for (size_t i = 0; i < ROWS; i++)
			{
				byte blocks_in_a_row = 0;
				for (size_t j = 0; j < COLUMNS; j++)
				{
					if (colliders[i][j] == 2)
					{
						blocks_in_a_row++;
					}
				}
				if (blocks_in_a_row == 12)
				{
					for (size_t k = i; k > 0; k--)
					{
						for (size_t l = 0; l < COLUMNS; l++)
						{
							colliders[k][l] = colliders[k - 1][l];
						}
						
					}
					if(gamespeed_changing > 5)
					{
						gamespeed_changing -= 2;
					}
					scoreCounter++;
					score.setString("Line: " + std::to_string(scoreCounter));
				}
			}

			
			// check game over
			checkGameOver(game_over);
			
			//reset timer
			timer = 0;
		}
		else
		{
			timer++;
		}

		window.draw(right_section);
		window.draw(next_piece_section);

		// Draw the current falling piece
		drawCurrentAndNext(window, piece,next_piece,tile, tile_width, tile_height, next_piece_section_position_x, next_piece_section_position_y);

		// Draws the static tiles
		drawStaticTiles(window, tile, tile_width, tile_height);

		// Draw the text and finally update the window
		displayText(window, score, pause_text, game_over_text,game_over, pause);
		window.display();
	}

	return 0;
}
