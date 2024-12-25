#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdio>
#include <iostream>

/*
#define is a macro. A macro is a preprocessor directive, meaning that it is dealt with before the compilation.
During preprocessing, macros are replaced with their corresponding values.
Wherever there is the word PI in the code, it gets replaced with 3.14159265359.
*/
#define PI 3.14159265359

/*
Typedef:
typedef keyword in C++ is used for aliasing existing data types.
typedef is limited to giving symbolic names to types only, whereas #define can be used to define an alias for values as well.
*/
/*
uint8_t:
1. unsigned 8 bit integer (u --> unsigned, int--> integer, 8--> 8bits, t--> type)
why use these?
Because they are not platform dependent. The sizes of int, long etc ate different on different platforms.
uintN_t gives a integer of size N bits no matter what the platform is.
*/
typedef uint8_t byte; // now by writing byte, you are basically writing uint8_t.

/*
Difference between typedef and #define:
1. #define is a instruction for the preprocessor. it gets executed by the preprocessor. While typedef is handled by the compiler.
2. #define can alias values as well while typedef is limited to data types
*/

// 20 is the amount of rows and 12 is the amount of columns in the game.
byte grid[20][12];
byte colliders[20][12]; // two arrays?

/*
Structures (also called structs) are a way to group several related variables into one place. Each variable in the structure is known as a member of the structure.
Unlike Arrays, they can store multiple data types.
you can access members of the struct using the dot notation. (myStruct.member1)
To declare a variable that is a struct:  myStructName var;
*/

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

// takes in a tetromino returns the rotated it's rotated version
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

			(pbx >= 0 && pbx <= 11 && pby >= 0 && pby <= 19) &&
			(pcx >= 0 && pcx <= 11 && pcy >= 0 && pcy <= 19) &&
			(pdx >= 0 && pdx <= 11 && pdy >= 0 && pdy <= 19) &&

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

int main()
{

	srand(time(NULL)); // setting a seed
	// 16x16 tiles
	// Loading texture from a file
	// What is a texture?
	sf::Texture tile_tex;
	tile_tex.loadFromFile("images/tetris_tile.png");

	// Creating a Sprite of texture "tile_tex"
	// what is a sprite?
	sf::Sprite tile(tile_tex);
	tile.setScale(2.83, 2.83);

	// what on EARTH is happening here?!!
	float tile_size = tile_tex.getSize().x * tile.getScale().x;
	size_t width = tile_size * 12, height = tile_size * 20;

	// Setup score counter
	sf::Text score;
	sf::Font font;

	// Setting score properties
	font.loadFromFile("./fonts/Roboto-Black.ttf");
	score.setFont(font);
	score.setCharacterSize(15);
	score.setFillColor(sf::Color::Blue);
	score.setPosition(10, 25);
	sf::Vector2<float> score_scale(2.0f, 2.0f);
	score.setScale(score_scale);
	score.setString("Lines: 0");

	// Setup window & create first piece
	sf::RenderWindow window(sf::VideoMode(width + 400, height), "Tetris", sf::Style::Titlebar | sf::Style::Close);
	window.setKeyRepeatEnabled(true);
	window.setFramerateLimit(60);

	// Setting up the sections which will show the next piece that will come
	int next_piece_section_width = 350;
	int next_piece_section_height = 200;
	int next_piece_section_position_x = width + 20;
	int next_piece_section_position_y = 50;

	sf::RectangleShape right_section(sf::Vector2f(400, height));
	right_section.setFillColor(sf::Color::Green);
	right_section.setPosition(width, 0);

	sf::RectangleShape next_piece_section(sf::Vector2f(next_piece_section_width, next_piece_section_width));
	next_piece_section.setFillColor(sf::Color::Black);
	next_piece_section.setPosition(next_piece_section_position_x, next_piece_section_position_y);

	PIECE_TYPE piece_type = static_cast<PIECE_TYPE>((rand() % 7));
	Piece piece = CreatePiece(piece_type);
	PIECE_TYPE next_piece_type = static_cast<PIECE_TYPE>((rand() % 7));
	Piece next_piece = CreatePiece(next_piece_type);

	unsigned int timer = 0, gamespeed = 10, scoreCounter = 0;

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
				if (event.key.code == sf::Keyboard::Up)
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
					(colliders[piece.c.y][piece.c.x - 1]) != 2 && (colliders[piece.d.y][piece.d.x - 1]) != 2)
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
					piece.a.x != 11 && piece.b.x != 11 && piece.c.x != 11 && piece.d.x != 11 &&
					// There are no static tetromino(tetromino from previous rounds) on the right of any part of the tetromino
					(colliders[piece.a.y][piece.a.x + 1]) != 2 && (colliders[piece.b.y][piece.b.x + 1]) != 2 &&
					(colliders[piece.c.y][piece.c.x + 1]) != 2 && (colliders[piece.d.y][piece.d.x + 1]) != 2)
				{
					piece.a.x++;
					piece.b.x++;
					piece.c.x++;
					piece.d.x++;
				}
			}
			// Increasing the magnitude of the gamespeed variables decreases the speed of the game
			if (event.type == sf::Event::KeyReleased)
			{
				gamespeed = 20;
			}
		}

		// Increase the speed of the game if DOWN arrow key is pressed
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		{
			gamespeed = 1;
		}

		window.clear(sf::Color(0, 0, 0, 1));

		// Refresh the grid-array
		for (size_t i = 0; i < 20; i++)
		{
			for (size_t j = 0; j < 12; j++)
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

		window.draw(right_section);
		window.draw(next_piece_section);

		
		// Clock
		if (timer > gamespeed)
		{
			// Collision checks
			if ( // IF there are static tetrominoes below the falling piece
				grid[piece.a.y + 1][piece.a.x] == 2 ||
				grid[piece.b.y + 1][piece.b.x] == 2 ||
				grid[piece.c.y + 1][piece.c.x] == 2 ||
				grid[piece.d.y + 1][piece.d.x] == 2 ||
				// OR there is the bottom of the game below the falling piece
				piece.a.y == 19 || piece.b.y == 19 || piece.c.y == 19 || piece.d.y == 19)
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
			for (size_t i = 0; i < 20; i++)
			{
				byte blocks_in_a_row = 0;
				for (size_t j = 0; j < 12; j++)
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
						for (size_t l = 0; l < 12; l++)
						{
							colliders[k][l] = colliders[k - 1][l];
						}
					}
					scoreCounter++;
					score.setString("Line: " + std::to_string(scoreCounter));
				}
			}

			// If game over, then close application
			for (size_t i = 0; i < 12; i++)
			{
				if (colliders[0][i] == 2)
				{
					window.close();
				}
			}

			timer = 0;

			// Prints out the grid in the console (if enabled) for testing purposes
			for (size_t i = 0; i < 20; i++)
			{
				for (size_t j = 0; j < 12; j++)
				{
					printf("%i", grid[i][j]);
					std::cout << grid[i][j] << " ";
				}
				std::cout << "\n";
			}
		}
		else
		{
			timer++;
		}

		// Draw next piece
		sf::Sprite next_piece_tile = tile;

		next_piece_tile.setPosition(next_piece_section_position_x + next_piece.a.x * tile_size - 60, next_piece_section_position_y + next_piece.a.y * tile_size + 100);
		window.draw(next_piece_tile);

		next_piece_tile.setPosition(next_piece_section_position_x + next_piece.b.x * tile_size - 60, next_piece_section_position_y + next_piece.b.y * tile_size + 100);
		window.draw(next_piece_tile);

		next_piece_tile.setPosition(next_piece_section_position_x + next_piece.c.x * tile_size - 60, next_piece_section_position_y + next_piece.c.y * tile_size + 100);
		window.draw(next_piece_tile);

		next_piece_tile.setPosition(next_piece_section_position_x + next_piece.d.x * tile_size - 60, next_piece_section_position_y + next_piece.d.y * tile_size + 100);
		window.draw(next_piece_tile);


		// Draw the current falling piece
		sf::Sprite piece_tile = tile;

		piece_tile.setPosition(tile_size * piece.a.x, tile_size * piece.a.y);
		window.draw(piece_tile);

		piece_tile.setPosition(tile_size * piece.b.x, tile_size * piece.b.y);
		window.draw(piece_tile);

		piece_tile.setPosition(tile_size * piece.c.x, tile_size * piece.c.y);
		window.draw(piece_tile);

		piece_tile.setPosition(tile_size * piece.d.x, tile_size * piece.d.y);
		window.draw(piece_tile);

		// Draws the static tiles
		for (size_t i = 0; i < 20; i++)
		{
			for (size_t j = 0; j < 12; j++)
			{
				if (colliders[i][j] == 2)
				{
					sf::Sprite t = tile;
					t.setPosition(tile_size * j, tile_size * i);
					window.draw(t);
				}
			}
		}

		// Draw the score and finally update the window
		window.draw(score);
		window.display();
	}

	return 0;
}
