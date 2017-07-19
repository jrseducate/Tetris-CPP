#include <iostream>
#include <thread>
#include <Windows.h>
#include <conio.h>
#include <string.h>
#include "TypeDefs.h"
#include "Utilities.h"
using namespace std;

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 30

struct color
{
};

static color NextColor = {};

struct block
{
	b32 Filled;
	color Color;
};

struct piece_block
{
	s32 X;
	s32 Y;
};

struct piece
{
	color Color;
	u32 X;
	u32 Y;

	piece_block Blocks[4];
};

enum piece_type
{
	piece_none,
	piece_l,
	piece_rev_l,
	piece_z,
	piece_rev_z,
	piece_line,
	piece_square,
};

piece_type NextPiece = piece_l;

void CreatePiece(piece* Piece, piece_type PieceType)
{
	switch (PieceType)
	{
		case piece_l:
		{
			Piece->Blocks[0].X = -1;
			Piece->Blocks[0].Y = 0;
			Piece->Blocks[1].X = 0;
			Piece->Blocks[1].Y = 0;
			Piece->Blocks[2].X = 0;
			Piece->Blocks[2].Y = 1;
			Piece->Blocks[3].X = 0;
			Piece->Blocks[3].Y = 2;
			Piece->X = 5;
			Piece->Y = 3;
			Piece->Color = NextColor;
		} break;
		default:
		{
			Assert(!"No Piece Type given!");
		} break;
	}
}

struct gameboard
{
	u32 Width;
	u32 Height;
	u32 Pitch;

	block Blocks[BOARD_WIDTH * BOARD_HEIGHT];
	piece ControlPiece;
};

gameboard CreateGameboard()
{
	gameboard Result = {};

	Result.Pitch = BOARD_WIDTH;

	CreatePiece(&Result.ControlPiece, piece_l);

	Result.ControlPiece.Color = NextColor;

	return (Result);
}

enum input_type
{
	input_none,
	input_moveleft,
	input_moveright,
	input_rotateleft,
	input_rotateright,
	input_fallinstantly,
};

struct input
{
	input_type Action;
};

char KeyPressed = 0;
void GetKeyPressed()
{
	while (true)
	{
		KeyPressed = _getch();
	}
}

void GetInput(input* Input)
{
	Input->Action = input_none;

	if (KeyPressed != 0)
	{
		int STOP = 1;
	}

	switch (KeyPressed)
	{
		case 'a':
		{
			Input->Action = input_moveleft;
		} break;
		case 'd':
		{
			Input->Action = input_moveright;
		} break;
		case 'q':
		{
			Input->Action = input_rotateleft;
		} break;
		case 'e':
		{
			Input->Action = input_rotateright;
		} break;
		case 's':
		{
			Input->Action = input_fallinstantly;
		} break;
	}

	KeyPressed = 0;
}

void UpdateGame(input* Input, gameboard* Gameboard)
{
	static u8 UpdateCount = 0;
	piece* ControlPiece = &Gameboard->ControlPiece;

	b32 MoveIsValid = true;

	for (u32 i = 0;
		i < ArrayCount(ControlPiece->Blocks);
		i++)
	{
		piece_block* PieceBlock = ControlPiece->Blocks + i;

		s32 PieceBlockX = ControlPiece->X + PieceBlock->X;

		if (PieceBlockX <= 0)
		{
			MoveIsValid = false;
		}

		if (PieceBlockX >= BOARD_WIDTH - 1)
		{
			MoveIsValid = false;
		}
	}

	switch (Input->Action)
	{
		case input_moveleft:
		{
			if (MoveIsValid)
			{
				ControlPiece->X -= 1;
			}
		} break;
		case input_moveright:
		{
			if (MoveIsValid)
			{
				ControlPiece->X += 1;
			}
		} break;
		case input_rotateleft:
		{
			for (u32 i = 0;
				i < ArrayCount(ControlPiece->Blocks);
				i++)
			{
				piece_block* PieceBlock = ControlPiece->Blocks + i;

				PieceBlock->X *= -1;
				Swap(PieceBlock->X, PieceBlock->Y, s32);

				s32 PieceBlockX = ControlPiece->X + PieceBlock->X;
				if (PieceBlockX < 0)
				{
					ControlPiece->X++;
				}
				else if (PieceBlockX >= BOARD_WIDTH)
				{
					ControlPiece->X--;
				}

			}
		} break;
		case input_rotateright:
		{
			for (u32 i = 0;
				i < ArrayCount(ControlPiece->Blocks);
				i++)
			{
				piece_block* PieceBlock = ControlPiece->Blocks + i;

				PieceBlock->Y *= -1;
				Swap(PieceBlock->X, PieceBlock->Y, s32);

				s32 PieceBlockX = ControlPiece->X + PieceBlock->X;
				s32 PieceBlockY = ControlPiece->Y + PieceBlock->Y;

				if (PieceBlockX < 0)
				{
					ControlPiece->X++;
				}
				else if (PieceBlockX >= BOARD_WIDTH)
				{
					ControlPiece->X--;
				}
			}
		} break;
		case input_fallinstantly:
		{
			UpdateCount = 10;
		} break;

	}
	//default:
	{
		if (UpdateCount == 10)
		{
			UpdateCount = 0;
			ControlPiece->Y += 1;

			for (u32 i = 0;
				i < ArrayCount(ControlPiece->Blocks);
				i++)
			{
				piece_block* PieceBlock = ControlPiece->Blocks + i;

				u32 PieceBlockX = ControlPiece->X + PieceBlock->X;
				u32 PieceBlockY = ControlPiece->Y + PieceBlock->Y;

				block* GameboardBlock = Gameboard->Blocks + (((PieceBlockY + 1) * Gameboard->Pitch) + PieceBlockX);

				if (PieceBlockY >= BOARD_HEIGHT || GameboardBlock->Filled)
				{
					for (u32 i = 0;
						i < ArrayCount(ControlPiece->Blocks);
						i++)
					{
						piece_block* PieceBlock = ControlPiece->Blocks + i;

						u32 PieceBlockX = ControlPiece->X + PieceBlock->X;
						u32 PieceBlockY = ControlPiece->Y + PieceBlock->Y;

						block* GameboardBlock = Gameboard->Blocks + (((PieceBlockY) * Gameboard->Pitch) + PieceBlockX);
						GameboardBlock->Filled = true;
						GameboardBlock->Color = ControlPiece->Color;
					}
					
					CreatePiece(ControlPiece, piece_l);

					break;
				}
			}
		}
		else
		{
			UpdateCount++;
		}
	} //break;

	for (u32 y = 0;
		y < BOARD_HEIGHT;
		y++)
	{
		b32 IsLine = true;

		for (u32 x = 0;
			x < BOARD_WIDTH;
			x++)
		{
			block* GameboardBlock = Gameboard->Blocks + ((y * Gameboard->Pitch) + x);

			if (!GameboardBlock->Filled)
			{
				IsLine = false;
				break;
			}
		}

		if (IsLine)
		{
			for (u32 x = 0;
				x < BOARD_WIDTH;
				x++)
			{
				block* GameboardBlock = Gameboard->Blocks + ((y * Gameboard->Pitch) + x);
				GameboardBlock->Filled = false;
			}

			for (u32 lineY = y;
				lineY > 0;
				lineY--)
			{
				for (u32 x = 0;
					x < BOARD_WIDTH;
					x++)
				{
					block* GameboardBlock = Gameboard->Blocks + ((lineY * Gameboard->Pitch) + x);
					block* GameboardBlockAbove = Gameboard->Blocks + (((lineY - 1) * Gameboard->Pitch) + x);
					*GameboardBlock = *GameboardBlockAbove;
					*GameboardBlockAbove = {};
				}
			}
		}
	}
}

void DrawGameboard(gameboard* Gameboard)
{
	piece* ControlPiece = &Gameboard->ControlPiece;
	for (u32 i = 0;
		i < ArrayCount(ControlPiece->Blocks);
		i++)
	{
		piece_block* PieceBlock = ControlPiece->Blocks + i;

		u32 PieceBlockX = ControlPiece->X + PieceBlock->X;
		u32 PieceBlockY = ControlPiece->Y + PieceBlock->Y;

		block* GameboardBlock = Gameboard->Blocks + ((PieceBlockY * Gameboard->Pitch) + PieceBlockX);

		GameboardBlock->Color = ControlPiece->Color;
		GameboardBlock->Filled = true;
	}

	u32 Pitch = BOARD_WIDTH;

	for (u32 y = 0;
		y < BOARD_HEIGHT;
		y++)
	{
		for (u32 x = 0;
			x < BOARD_WIDTH;
			x++)
		{
			COORD CursorP = {};
			CursorP.X = x;
			CursorP.Y = y;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorP);

			block* Block = Gameboard->Blocks + ((y * Gameboard->Pitch) + x);
			if (Block->Filled)
			{
				cout << '*';
			}
			else
			{
				cout << ' ';
			}
		}
	}

	for (u32 i = 0;
		i < ArrayCount(ControlPiece->Blocks);
		i++)
	{
		piece_block* PieceBlock = ControlPiece->Blocks + i;

		u32 PieceBlockX = ControlPiece->X + PieceBlock->X;
		u32 PieceBlockY = ControlPiece->Y + PieceBlock->Y;

		block* GameboardBlock = Gameboard->Blocks + ((PieceBlockY * Gameboard->Pitch) + PieceBlockX);

		GameboardBlock->Filled = false;
	}
}

int main()
{
	b32 Running = true;

	input Input = {};
	gameboard Gameboard = CreateGameboard();

	thread GetKeyPressedThread = thread(GetKeyPressed);
	CONSOLE_CURSOR_INFO CursorInfo;

	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CursorInfo);
	CursorInfo.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CursorInfo);

	while (Running)
	{
		GetInput(&Input);
		UpdateGame(&Input, &Gameboard);
		DrawGameboard(&Gameboard);

		Sleep(50);
	}

	return(0);
}