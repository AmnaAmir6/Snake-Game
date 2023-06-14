#include<windows.h>
#include<conio.h>
#include<iostream>
#include<fstream>
#include<iomanip>
#include<time.h>
#include<string>
#include<mmsystem.h>

using namespace std;

enum Direction {LEFT,RIGHT,UP,DOWN};

#define DBLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGREY 7
#define DARKGREY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define DWHITE 15
#define Row 86
#define Col 166

void getRowColbyLeftClick(int& rpos, int& cpos)
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD Events;
	INPUT_RECORD InputRecord;
	SetConsoleMode(hInput, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	do
	{
		ReadConsoleInput(hInput, &InputRecord, 1, &Events);
		if (InputRecord.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
		{
			cpos = InputRecord.Event.MouseEvent.dwMousePosition.X;
			rpos = InputRecord.Event.MouseEvent.dwMousePosition.Y;
			break;
		}
	} while (true);
}
void gotoRowCol(int rpos, int cpos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = cpos;
	scrn.Y = rpos;
	SetConsoleCursorPosition(hOuput, scrn);
}
void SetClr(int clr)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
}
struct Position
{
	int ri;
	int ci;
};
struct Snake
{
	Position* Ps;
	int size;
	int score;
	char sym;
	string p_Name;
	Direction dir;
	int UPKEY, DOWNKEY, LEFTKEY, RIGHTKEY;
	int color;
};
struct Food
{
	Position fp;
	bool isAlive;
	int score;
	char sym;
};
bool isLegalCoordinate(Position p, Snake* &S,int NOP,int height,int width,int Level)
{
	for (int n = 0; n < NOP; n++)
	{
		for (int i = 0; i < S[n].size; i++)
		{
			if ((S[n].Ps[i].ri == p.ri && S[n].Ps[i].ci == p.ci) || p.ri >= height - 1 || p.ci >= width - 1)
			{
				return false;
			}
		}
	}
	if (Level == 2)
	{
		for (int i = 0; i < 15; i++)
		{
			if (p.ri == height / 2 - 7 + i && p.ci == width / 3)
				return false;
			if (p.ri == height / 2 - 7 + i && p.ci == width * 0.75)
				return false;
		}
	}
	if(Level == 3)
	{
		for (int i = 0; i < 11; i++)
		{
			if (p.ri == height / 2 - 5 + i && p.ci == width / 2)
				return false;
			else if (p.ri == height / 2 && p.ci == width / 2 - 5 + i)
				return false;
			else if (p.ri == 1 + i && p.ci == width / 2)
				return false;
			else if (p.ri == height - 1 - i && p.ci == width / 2)
				return false;
			else if (p.ri == height / 2 && p.ci == 1 + i)
				return false;
			else if (p.ri == height / 2 && p.ci == width - 1 - i)
				return false;
		}
	}
	return true;
}
void generateFood(Food& f, int height, int width, Snake* &S, int NOP,int Level)
{
	do
	{
		f.fp.ri = rand() % height;
		f.fp.ci = rand() % width;
	} while (!isLegalCoordinate(f.fp, S, NOP,height,width,Level));
	f.isAlive = true;
	f.score = 1;
	f.sym = -37;
}
void displayFood(Food f)
{
	gotoRowCol(f.fp.ri, f.fp.ci);
	SetClr(4);
	cout << f.sym;
	SetClr(15);
}
void removeFood(Food f)
{
	gotoRowCol(f.fp.ri, f.fp.ci);
	cout << ' ';
	SetClr(15);
}
bool foodIsEaten(Food f, Snake* S, int NOP)
{
	for (int n = 0; n < NOP; n++)
	{
		if (S[n].Ps[0].ri == f.fp.ri && S[n].Ps[0].ci == f.fp.ci)
		{
			if (f.score == 1)
				PlaySound(TEXT("bite.wav"), NULL, SND_FILENAME | SND_ASYNC);
			else
				PlaySound(TEXT("bonusFood.wav"), NULL, SND_FILENAME | SND_ASYNC);
			return true;
		}
	}
	return false;
}
void DrawBorder(int sr,int sc,int Brows, int Bcols, int color1,int color2)
{
	char Sym = -37;
	for (int ri = 0; ri < Brows; ri++)
	{
		for (int ci = 0; ci < Bcols; ci++)
		{
			if ((ri + ci) % 2 == 0)
				SetClr(color1);
			else
				SetClr(color2);
			if (ri == 0 || ri == Brows - 1 || ci == 0 || ci == Bcols - 1)
			{
				gotoRowCol(sr+ri, sc+ci);
				cout << Sym;
			}
		}
	}
}
void DrawBox(int sr, int sc, int Brows, int Bcols, int color)

{
	char Sym = -37;
	for (int ri = 0; ri < Brows; ri++)
	{
		for (int ci = 0; ci < Bcols; ci++)
		{
			SetClr(color);
			gotoRowCol(sr + ri, sc + ci);
			cout << Sym;
		}
	}
}
void printA(int Cr, int Cc, int S, char sym)
{
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr - S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int r = 0; r < S * 2 + 1; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int r = 0; r < S * 2 + 1; r++)
	{
		gotoRowCol(Cr - S + r, Cc + (S / 2));
		cout << sym;
	}
}
void printM(int Cr, int Cc, int S, char sym)
{
	for (int r = 0, c = 0; r < S; r++, c++)
	{
		gotoRowCol(Cr - r, Cc + c);
		cout << sym;
	}
	for (int r = 0, c = 0; r < S; r++, c++)
	{
		gotoRowCol(Cr - r, Cc - c);
		cout << sym;
	}
	for (int r = 0; r < S * 2 + 1; r++)
	{
		gotoRowCol(Cr - S + r, Cc - S);
		cout << sym;
	}
	for (int r = 0; r < S * 2 + 1; r++)
	{
		gotoRowCol(Cr - S + r, Cc + S);
		cout << sym;
	}
}
void printN(int Cr, int Cc, int S, char sym)
{
	for (int r = 0; r < S * 2 + 1; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int r = 0; r < S * 2 + 1; r++)
	{
		gotoRowCol(Cr - S + r, Cc + (S / 2));
		cout << sym;
	}
	for (int r = 0, c = 0; r < S; r++, c++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2) + c);
		cout << sym;
	}
}
void printS(int Cr, int Cc, int S, char sym)
{
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr - S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr + S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int r = 0; r < S; r++)
	{
		gotoRowCol(Cr - r, (Cc - S / 2));
		cout << sym;
	}
	for (int r = 0; r < S; r++)
	{
		gotoRowCol(Cr + r, (Cc + S / 2));
		cout << sym;
	}
}
void printK(int Cr, int Cc, int S, char sym)
{
	for (int r = 0; r < S * 2 + 1; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int r = 0, c = 0; r < S + 1; r++, c++)
	{
		gotoRowCol(Cr + r, Cc - (S / 2) + c);
		cout << sym;
	}
	for (int r = 0, c = 0; r < S + 1; r++, c++)
	{
		gotoRowCol(Cr - r, Cc - (S / 2) + c);
		cout << sym;
	}
}
void printE(int Cr, int Cc, int S, char sym)
{
	for (int r = 0; r < S * 2; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr - S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr + S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr, (Cc - S / 2) + c);
		cout << sym;
	}
}
void printR(int Cr, int Cc, int S, char sym)
{
	for (int r = 0; r < S * 2 + 1; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr - S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr, (Cc - S / 2) + c);
		cout << sym;
	}
	for(int r = 0; r < S; r++)
	{
		gotoRowCol(Cr - S + r, Cc + (S / 2));
		cout << sym;
	}
	for (int r = 0, c = 0; r < S + 1; r++, c++)
	{
		gotoRowCol(Cr + r, Cc - (S/2) + c);
		cout << sym;
	}
}
void printD(int Cr, int Cc, int S, char sym)
{
	for (int r = 0; r < S * 2; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr - S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr + S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int r = 0; r < S * 2; r++)
	{
		gotoRowCol(Cr - S + r, Cc + (S / 2));
		cout << sym;
	}
}
void printO(int Cr, int Cc, int S, char sym)
{
	for (int r = 0; r < S * 2; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr - S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr + S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int r = 0; r < S * 2; r++)
	{
		gotoRowCol(Cr - S + r, Cc + (S / 2));
		cout << sym;
	}
}
void printC(int Cr, int Cc, int S, char sym)
{
	for (int r = 0; r < S * 2; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr - S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr + S, (Cc - S / 2) + c);
		cout << sym;
	}
}
void printG(int Cr, int Cc, int S, char sym)
{
	for (int r = 0; r < S * 2; r++)
	{
		gotoRowCol(Cr - S + r, Cc - (S / 2));
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr - S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int c = 0; c < S; c++)
	{
		gotoRowCol(Cr + S, (Cc - S / 2) + c);
		cout << sym;
	}
	for (int r = 0; r < S; r++)
	{
		gotoRowCol(Cr + r, Cc + (S / 2));
		cout << sym;
	}
}
void PrintMovingSineWave(int sh, int Amp, int W, int Cr, int Cols, char sym)
{
	int i = 1;
	float dtheta = (360.0 * W) / Cols;
	float theta = sh;
	for (int c = 0; c < Cols; c++)
	{
		int r = Amp * (-1) * sin(theta * 3.14 / 180) + Cr;
		gotoRowCol(r, c);
		if (c == Col - 1)
			cout << 'O';
		else
			cout << sym;
		theta += dtheta;
		SetClr(i++);
	}
}
void PrintRectangularSpiral(char sym)
{
	for (int ri = 0; ri < Row / 2; ri++)
	{
		if (ri % 2 == 0)
			SetClr(2);
		else
			SetClr(10);
		for (int c = ri; c < Col - 1 - ri; c++)
		{
			gotoRowCol(ri, c);
			cout << sym;
		}
		for (int r = ri; r < Row - 1 - ri; r++)
		{
			gotoRowCol(r, Col - 1 - ri);
			cout << sym;
		}
		for (int c = Col - 1 - ri; c > ri; c--)
		{
			gotoRowCol(Row - 1 - ri, c);
			cout << sym;
		}
		for (int r = Row - 1 - ri; r > ri; r--)
		{
			gotoRowCol(r, ri);
			cout << sym;;
		}
	}
}
void DrawGrid(int height, int width, int Brows, int Bcols)
{
	system("cls");
	for (int ri = 0; ri < height; ri++)
	{
		for (int ci = 0; ci < width; ci++)
		{
			if ((ri + ci) % 2 == 0)
			{
				DrawBox(ri * Brows, ci * Bcols, Brows, Bcols, 2);
			}
			else
			{
				DrawBox(ri * Brows, ci * Bcols, Brows, Bcols, 10);
			}
		}
	}
}
void printSnakeGame()
{
	_getch();
	PlaySound(TEXT("startingMusic.wav"), NULL, SND_FILENAME | SND_ASYNC);
	DrawGrid(Row / 4 + 1, Col / 4 + 1, 4, 4);
	SetClr(4);
	printS(Row / 2 - 10, Col / 2 - 20, 7, -37);
	printN(Row / 2 - 10, Col / 2 - 10, 7, -37);
	printA(Row / 2 - 10, Col / 2, 7, -37);
	printK(Row / 2 - 10, Col / 2 + 10, 7, -37);
	printE(Row / 2 - 10, Col / 2 + 20, 7, -37);

	printG(Row / 2 + 10, Col / 2 - 20, 7, -37);
	printA(Row / 2 + 10, Col / 2 - 9, 7, -37);
	printM(Row / 2 + 10, Col / 2 + 6, 7, -37);
	printE(Row / 2 + 10, Col / 2 + 20, 7, -37);
}
void SelectPos(Position& P)
{
	int prows, pcols;
	getRowColbyLeftClick(prows, pcols);
	P.ri = prows;
	P.ci = pcols;
}
void multipleNOP(int& NOP)
{
	SetClr(0);
	system("cls");
	DrawBox(40, 40, 10, 15, 9);
	DrawBorder(40, 40, 10, 15, 4, 7);
	SetClr(156);
	gotoRowCol(44, 41);
	cout << "  2 Player ";
	DrawBox(40, 70, 10, 15, 9);
	DrawBorder(40, 70, 10, 15, 4, 7);
	SetClr(156);
	gotoRowCol(44, 71);
	cout << "  3 Player ";
	DrawBox(40, 100, 10, 15, 9);
	DrawBorder(40, 100, 10, 15, 4, 7);
	SetClr(156);
	gotoRowCol(44, 101);
	cout << "  4 Player ";
	int c1 = 4, c2 = 7;
	for (int i = 0; i < 8; i++)
	{
		DrawBorder(40, 40, 10, 15, c1, c2);
		DrawBorder(40, 70, 10, 15, c1, c2);
		DrawBorder(40, 100, 10, 15, c1, c2);
		PrintMovingSineWave(i, 5, 3, Row / 4, Col, -37);
		PrintMovingSineWave(i, 5, 3, Row * 0.75, Col, -37);
		swap(c1, c2);
	}
	int ri, ci;
	getRowColbyLeftClick(ri, ci);
	if (ci >= 100)
		NOP = 4;
	else if (ci >= 70)
		NOP = 3;
	else
		NOP = 2;
	SetClr(15);
	system("cls");
}
void isTimeBased(bool &timeBased)
{
	system("cls");
	DrawBox(40, 50, 10, 18, 9);
	DrawBorder(40, 50, 10, 18, 4, 7);
	SetClr(156);
	gotoRowCol(44, 51);
	cout << " TIME BASED ";
	DrawBox(40, 90, 10, 18, 9);
	DrawBorder(40, 90, 10, 18, 4, 7);
	SetClr(156);
	gotoRowCol(44, 91);
	cout << "  FREE PLAY ";
	int c1 = 4, c2 = 7;
	for (int i = 0; i < 8; i++)
	{
		DrawBorder(40, 50, 10, 18, c1, c2);
		DrawBorder(40, 90, 10, 18, c1, c2);
		PrintMovingSineWave(i, 5, 3, Row / 4, Col, -37);
		PrintMovingSineWave(i, 5, 3, Row * 0.75, Col, -37);
		swap(c1, c2);
	}
	int ri, ci;
	getRowColbyLeftClick(ri, ci);
	if (ci >= 78)
		timeBased = false;
	else
		timeBased = true;
	SetClr(15);
	system("cls");
}
void getNOP(int& NOP)
{
	Sleep(1000);
	system("cls");
	DrawBox(40, 50, 10, 18, 9);
	DrawBorder(40, 50, 10, 18, 4, 7);
	SetClr(156);
	gotoRowCol(44, 51);
	cout << " Single Player ";
	DrawBox(40, 90, 10, 18, 9);
	DrawBorder(40, 90, 10, 18, 4, 7);
	SetClr(156);
	gotoRowCol(44, 91);
	cout << "  Multi Player ";
	int c1 = 4, c2 = 7;
	for (int i = 0; i < 8; i++)
	{
		DrawBorder(40, 50, 10, 18, c1, c2);
		DrawBorder(40, 90, 10, 18, c1, c2);
		PrintMovingSineWave(i, 5, 3, Row / 4, Col, -37);
		PrintMovingSineWave(i, 5, 3, Row * 0.75, Col, -37);
		swap(c1, c2);
	}
	int ri, ci;
	getRowColbyLeftClick(ri, ci);
	if (ci >= 78)
		multipleNOP(NOP);
	else
		NOP = 1;
	SetClr(15);
	system("cls");
}
void winner(Snake* S, int NOP)
{
	int win = 0;
	for (int n = 0; n < NOP - 1; n++)
	{
		if (S[n].score > S[n + 1].score)
			win = n;
	}
	DrawBox(Row / 2 - 10, Col / 2 - 20, 30, 60, 9);
	DrawBorder(Row / 2 - 10, Col / 2 - 20, 30, 60, 4, 7);
	SetClr(156);
	gotoRowCol(Row / 2, Col / 2 - 20 + 1);
	cout << setw(15) << S[win].p_Name << " wins the game";
	Sleep(2500);
}
void GameEnd()
{
	system("cls");
	PlaySound(TEXT("end1.wav"), NULL, SND_FILENAME | SND_ASYNC);
	PrintRectangularSpiral(-37);
	SetClr(4);
	printG(Row / 2 - 10, Col / 2 - 20, 7, -37);
	printA(Row / 2 - 10, Col / 2 - 9, 7, -37);
	printM(Row / 2 - 10, Col / 2 + 6, 7, -37);
	printE(Row / 2 - 10, Col / 2 + 20, 7, -37);

	printE(Row / 2 + 10, Col / 2 - 20, 7, -37);
	printN(Row / 2 + 10, Col / 2 - 10, 7, -37);
	printD(Row / 2 + 10, Col / 2 , 7, -37);
	printE(Row / 2 + 10, Col / 2 + 10, 7, -37);
	printD(Row / 2 + 10, Col / 2 + 20, 7, -37);
	_getch();
}
void getLevel(int& Level)
{
	SetClr(0);
	system("cls");
	DrawBox(40, 40, 10, 15, 9);
	DrawBorder(40, 40, 10, 15, 4, 7);
	SetClr(156);
	gotoRowCol(44, 41);
	cout << "   Easy ";
	DrawBox(40, 70, 10, 15, 9);
	DrawBorder(40, 70, 10, 15, 4, 7);
	SetClr(156);
	gotoRowCol(44, 71);
	cout << "   Medium ";
	DrawBox(40, 100, 10, 15, 9);
	DrawBorder(40, 100, 10, 15, 4, 7);
	SetClr(156);
	gotoRowCol(44, 101);
	cout << "   Hard ";
	int c1 = 4, c2 = 7;
	for (int i = 0; i < 8; i++)
	{
		DrawBorder(40, 40, 10, 15, c1, c2);
		DrawBorder(40, 70, 10, 15, c1, c2);
		DrawBorder(40, 100, 10, 15, c1, c2);
		PrintMovingSineWave(i, 5, 3, Row / 4, Col, -37);
		PrintMovingSineWave(i, 5, 3, Row * 0.75, Col, -37);
		swap(c1, c2);
	}
	int ri, ci;
	getRowColbyLeftClick(ri, ci);
	if (ci >= 100)
		Level = 3;
	else if (ci >= 70)
		Level = 2;
	else
		Level = 1;
	SetClr(15);
	system("cls");
}
void getPNames(string*& PNames,int NOP)
{
	system("cls");
	int x = 0;
	for (int i = 0; i < NOP; i++,x+=20)
	{
		SetClr(15);
		gotoRowCol(Row / 6 + x - 1, Col / 2 - 10);
		cout << "Enter Player " << i + 1 << "'s Name:   ";
		DrawBox(Row / 6 + x, Col / 2 - 10, 10, 15, 9);
		DrawBorder(Row / 6 + x, Col / 2 - 10, 10, 15, 4, 7);
		SetClr(156);
		gotoRowCol(Row / 6 + x + 4, Col / 2 - 8);
		cin >> PNames[i];
	}
	SetClr(0);
}
void init(int& NOP, string* &PNames,Snake *&S,int& Level,bool & timeBased)
{
	printSnakeGame();
	getNOP(NOP);
	getLevel(Level);
	isTimeBased(timeBased);
	PNames = new string[NOP] {};
	S = new Snake[NOP]{};
	getPNames(PNames,NOP);
	system("cls");
}
void DrawLevel3(int h, int w,int S, int c1, int c2)
{
	for (int r = 0; r < 11; r++)
	{
		if (r % 2 == 0)
			SetClr(c1);
		else
			SetClr(c2);
		gotoRowCol(h / 2 - S + r, w / 2);
		cout << char(-37);
	}
	for (int c = 0; c < 11; c++)
	{
		if (c % 2 == 0)
			SetClr(c1);
		else
			SetClr(c2);
		gotoRowCol(h / 2, w / 2 - S + c);
		cout << char(-37);
	}
	for (int i = 0; i < 11; i++)
	{
		if (i % 2 == 0)
			SetClr(c1);
		else
			SetClr(c2);
		gotoRowCol(1 + i, w / 2);
		cout << char(-37);
		gotoRowCol(h - 1 - i, w / 2);
		cout << char(-37);
		gotoRowCol(h / 2, 1 + i);
		cout << char(-37);
		gotoRowCol(h / 2, w - 1 - i);
		cout << char(-37);
	}
}
void DrawLevel2(int h, int w, int S, int c1, int c2)
{
	for (int r = 0; r < 15; r++)
	{
		if (r % 2 == 0)
			SetClr(c1);
		else
			SetClr(c2);
		gotoRowCol(h / 2 - S + r, w / 3);
		cout << char(-37);
		gotoRowCol(h / 2 - S + r, w * 0.75);
		cout << char(-37);
	}
}
void snakeInit(Snake* &S,Food &f, int height, int width,int NOP,string PNames[],int Level)
{
	DrawBorder(0,0,height, width, 2, 10);
	if (Level == 2)
		DrawLevel2(height, width, 7, 2, 10);
	if (Level == 3)
		DrawLevel3(height, width,5, 2, 10);
	for (int i = 0; i < NOP; i++)
	{
		S[i].Ps = new Position[3];
	}

	S[0].Ps[0].ri = height / 3;
	S[0].Ps[0].ci = width / 3;

	S[0].Ps[1].ri = height / 3;
	S[0].Ps[1].ci = (width / 3) - 1;

	S[0].Ps[2].ri = height / 3;
	S[0].Ps[2].ci = (width / 3) - 2;

	S[0].size = 3;
	S[0].score = 0;
	S[0].p_Name = PNames[0];
	S[0].sym = 'O';
	S[0].color = 1;
	S[0].dir = RIGHT;
	S[0].RIGHTKEY = 77;
	S[0].LEFTKEY = 75;
	S[0].UPKEY = 72;
	S[0].DOWNKEY = 80;

	if (NOP > 1)
	{
		S[1].Ps[0].ri = (2 * height) / 3;
		S[1].Ps[0].ci = (2 * width) / 3;

		S[1].Ps[1].ri = (2 * height) / 3;
		S[1].Ps[1].ci = ((2 * width) / 3) + 1;

		S[1].Ps[2].ri = (2 * height) / 3;
		S[1].Ps[2].ci = ((2 * width) / 3) + 2;

		S[1].size = 3;
		S[1].score = 0;
		S[1].p_Name = PNames[1];
		S[1].sym = 'O';
		S[1].color = 5;
		S[1].dir = LEFT;
		S[1].RIGHTKEY = 100;
		S[1].LEFTKEY = 97;
		S[1].UPKEY = 119;
		S[1].DOWNKEY = 115;
	}
	if (NOP > 2)
	{
		S[2].Ps[0].ri = (2 * height) / 3;
		S[2].Ps[0].ci = width / 3;

		S[2].Ps[2].ri = (2 * height) / 3;
		S[2].Ps[2].ci = (width / 3) + 1;

		S[2].Ps[2].ri = (2 * height) / 3;
		S[2].Ps[2].ci = (width / 3) + 2;

		S[2].size = 3;
		S[2].score = 0;
		S[2].p_Name = PNames[2];
		S[2].sym = 'O';
		S[2].color = 8;
		S[2].dir = LEFT;
		S[2].RIGHTKEY = 54;
		S[2].LEFTKEY = 52;
		S[2].UPKEY = 56;
		S[2].DOWNKEY = 50;
	}
	if (NOP > 3)
	{
		S[3].Ps[0].ri = height / 3;
		S[3].Ps[0].ci = (2 * width) / 3;

		S[3].Ps[1].ri = height / 3;
		S[3].Ps[1].ci = ((2 * width) / 3) - 1;

		S[3].Ps[2].ri = height / 3;
		S[3].Ps[2].ci = ((2 * width) / 3) - 2;

		S[3].size = 3;
		S[3].score = 0;
		S[3].p_Name = PNames[3];
		S[3].sym = 'O';
		S[3].color = 9;
		S[3].dir = RIGHT;
		S[3].RIGHTKEY = 108;
		S[3].LEFTKEY = 106;
		S[3].UPKEY = 105;
		S[3].DOWNKEY = 107;
	}
	generateFood(f, height, width, S, NOP,Level);
}
void displaySnake(Snake* S,int NOP)
{
	for (int n = 0; n < NOP; n++)
	{
		for (int i = 0; i < S[n].size; i++)
		{
			gotoRowCol(S[n].Ps[i].ri, S[n].Ps[i].ci);
			SetClr(S[n].color);
			cout << S[n].sym;
		}
	}
}
void eraseSnake(Snake* S, int NOP)
{
	SetClr(0);
	for (int n = 0; n < NOP; n++)
	{
		for (int i = 0; i < S[n].size; i++)
		{
			gotoRowCol(S[n].Ps[i].ri, S[n].Ps[i].ci);
			cout << " ";
		}
	}
}
void moveSnake(Snake*& S, int NOP,int height,int width)
{
	for (int n = 0; n < NOP; n++)
	{
		for (int i = S[n].size - 1; i - 1 >= 0; i--)
		{
			S[n].Ps[i] = S[n].Ps[i - 1];
		}
		switch (S[n].dir)
		{
		case UP:
			S[n].Ps[0].ri--;
			if (S[n].Ps[0].ri == 0)
			{
				S[n].Ps[0].ri = height - 2;
			}
			break;
		case DOWN:
			S[n].Ps[0].ri++;
			if (S[n].Ps[0].ri == height - 1)
			{
				S[n].Ps[0].ri = 1;
			}
			break;
		case LEFT:
			S[n].Ps[0].ci--;
			if (S[n].Ps[0].ci == 0)
			{
				S[n].Ps[0].ci = width - 2;
			}
			break;
		case RIGHT:
			S[n].Ps[0].ci++;
			if (S[n].Ps[0].ci == width - 1)
			{
				S[n].Ps[0].ci = 1;
			}
			break;
		}
	}
}
void changeDir(Snake*& S, char ch,int NOP)
{
	for (int n = 0; n < NOP; n++)
	{
		if (ch == S[n].UPKEY)
		{
			if (S[n].dir != DOWN)
			{
				S[n].dir = UP;
			}
			break;
		}
		if (ch == S[n].DOWNKEY)
		{
			if (S[n].dir != UP)
			{
				S[n].dir = DOWN;
			}
			break;
		}
		if (ch == S[n].LEFTKEY)
		{
			if (S[n].dir != RIGHT)
			{
				S[n].dir = LEFT;
			}
			break;
		}
		if (ch == S[n].RIGHTKEY)
		{
			if (S[n].dir != LEFT)
			{
				S[n].dir = RIGHT;
			}
			break;
		}
	}
}
void expandSnake(Snake*& S, int NOP, Food f)
{
	int x;
	for (int n = 0; n < NOP; n++)
	{
		if (S[n].Ps[0].ri == f.fp.ri && S[n].Ps[0].ci == f.fp.ci)
			x = n;
	}
	Position* NP = new Position[S[x].size + 1];
	for (int i = 0; i < S[x].size; i++)
	{
		NP[i] = S[x].Ps[i];
	}
	NP[S[x].size] = S[x].Ps[S[x].size - 1];
	S[x].size++;
	delete[]S[x].Ps;
	S[x].Ps = NP;

}
bool illegalCoordinateL3(int ri, int ci, int height, int width)
{
	for (int i = 0; i < 11; i++)
	{
		if (ri == height / 2 - 5 + i && ci == width / 2)
			return true;
		else if (ri == height / 2 && ci == width / 2 - 5 + i)
			return true;
		else if (ri == 1 + i && ci == width / 2)
			return true;
		else if (ri == height - 1 - i && ci == width / 2)
			return true;
		else if (ri == height / 2 && ci == 1 + i)
			return true;
		else if (ri == height / 2 && ci == width - 1 - i)
			return true;
	}
	return false;
}
bool illegalCoordinateL2(int ri, int ci, int height, int width)
{
	for (int i = 0; i < 15; i++)
	{
		if (ri == height / 2 - 7 + i && ci == width / 3)
			return true;
		if (ri == height / 2 - 7 + i && ci == width * 0.75)
			return true;
	}
	return false;
}
bool kill(Snake* S, int NOP,int Level,int height,int width)
{
	for (int n = 0; n < NOP; n++)
	{
		for (int y = 0; y < NOP; y++)
		{
			if (n != y)
			{
				for (int j = 0; j < S[y].size; j++)
				{
					if (S[n].Ps[0].ri == S[y].Ps[j].ri && S[n].Ps[0].ci == S[y].Ps[j].ci)
						return true;
				}
			}
		}
	}
	int x = 0;
	for (int n = 0; n < NOP; n++)
	{
		for (int i = 1; i < S[n].size; i++)
		{
			if (S[n].Ps[x].ri == S[n].Ps[i].ri && S[n].Ps[x].ci == S[n].Ps[i].ci)
				return true;
		}
	}
	int a;
	if (Level == 2 || Level == 3)
	{
		for (int n = 0; n < NOP; n++)
		{
			switch (S[n].dir)
			{
			case UP:
				a=S[n].Ps[0].ri;
				if (a == 1)
				{
					displaySnake(S, NOP);
					return true;
				}
			case DOWN:
				a=S[n].Ps[0].ri;
				if (a == height - 2)
				{
					displaySnake(S, NOP);
					return true;
				}
			case LEFT:
				a=S[n].Ps[0].ci;
				if (a == 1)
				{
					displaySnake(S, NOP);
					return true;
				}
			case RIGHT:
				a=S[n].Ps[0].ci;
				if (a == width - 2)
				{
					displaySnake(S, NOP);
					return true;
				}
			}
		}
	}
	if (Level == 2)
	{
		for (int n = 0; n < NOP; n++)
		{
			for (int i = 1; i < S[n].size; i++)
			{
				if (illegalCoordinateL2(S[n].Ps[x].ri, S[n].Ps[x].ci, height, width))
					return true;
			}
		}
	}
	if (Level == 3)
	{
		for (int n = 0; n < NOP; n++)
		{
			for (int i = 1; i < S[n].size; i++)
			{
				if (illegalCoordinateL3(S[n].Ps[x].ri,S[n].Ps[x].ci,height,width))
					return true;
			}
		}
	}
	return false;
}
void displayScore(Snake *S, int NOP)
{
	SetClr(15);
	printS(8, 120 - 20, 3, -37);
	printC(8, 120 - 10, 3, -37);
	printO(8, 120, 3, -37);
	printR(8, 120 + 10, 3, -37);
	printE(8, 120 + 20, 3, -37);

	gotoRowCol(19, 100);
	SetClr(15);
	cout << S[0].p_Name;
	DrawBox(20, 100, 10, 15, 9);
	DrawBorder(20, 100, 10, 15, 4, 7);
	SetClr(156);
	gotoRowCol(24, 101);
	cout << setw(7) << S[0].score;

	if (NOP > 1)
	{
		gotoRowCol(19, 120);
		SetClr(15);
		cout << S[1].p_Name;
		DrawBox(20, 120, 10, 15, 9);
		DrawBorder(20, 120, 10, 15, 4, 7);
		SetClr(156);
		gotoRowCol(24, 121);
		cout << setw(7) << S[1].score;
	}

	if (NOP > 2)
	{
		gotoRowCol(39, 100);
		SetClr(15);
		cout << S[2].p_Name;
		DrawBox(40, 100, 10, 15, 9);
		DrawBorder(40, 100, 10, 15, 4, 7);
		SetClr(156);
		gotoRowCol(44, 101);
		cout << setw(7) << S[2].score;
	}

	if (NOP > 3)
	{
		gotoRowCol(39, 120);
		SetClr(15);
		cout << S[3].p_Name;
		DrawBox(40, 120, 10, 15, 9);
		DrawBorder(40, 120, 10, 15, 4, 7);
		SetClr(156);
		gotoRowCol(44, 121);
		cout << setw(7) << S[3].score;
	}
}
void updateScore(Snake* S, int NOP)
{
	SetClr(156);
	gotoRowCol(24, 101);
	cout << setw(7) << S[0].score;

	if (NOP > 1)
	{
		SetClr(156);
		gotoRowCol(24, 121);
		cout << setw(7) << S[1].score;
	}

	if (NOP > 2)
	{
		SetClr(156);
		gotoRowCol(44, 101);
		cout << setw(7) << S[2].score;
	}

	if (NOP > 3)
	{
		SetClr(156);
		gotoRowCol(44, 121);
		cout << setw(7) << S[3].score;
	}
}
void changeScore(Snake*& S, int NOP, Food f)
{
	for (int n = 0; n < NOP; n++)
	{
		if (S[n].Ps[0].ri == f.fp.ri && S[n].Ps[0].ci == f.fp.ci)
			S[n].score = S[n].score + f.score;
	}
}
void generateFoodBonus(Food& f, int height, int width, Snake*& S, int NOP, int Level)
{
	do
	{
		f.fp.ri = rand() % height;
		f.fp.ci = rand() % width;
	} while (!isLegalCoordinate(f.fp, S, NOP, height, width, Level));
	f.isAlive = true;
	f.score = 5;
	f.sym = '*';
}
void Timer(bool timerBased,int time)
{
	if (timerBased)
	{
		gotoRowCol(6, 150);
		SetClr(15);
		cout << "TIMER";
		DrawBox(7, 150, 10, 15, 9);
		DrawBorder(7, 150, 10, 15, 4, 7);
		SetClr(156);
		gotoRowCol(11, 151);
		cout << setw(7) << "00:" << time;
	}
}
void displayTime(int time)
{
	SetClr(156);
	gotoRowCol(11, 151);
	cout << setw(7) << "00:" << time;
}
bool TargetReached(Snake* S, int NOP)
{
	for (int n = 0; n < NOP; n++)
	{
		if (S[n].score >= 10)
		{
			return true;
		}
	}
	return false;
}

int main()
{
	char ch;
	int height = 60, width = 85;
	int x = 0, y = 0, time = 0;
	int NOP, Level;
	bool timeBased = true;
	string* PNames;
	Snake* S;
	Food f;
	Food bonusF;
	init(NOP, PNames, S, Level,timeBased);
	snakeInit(S, f, height, width, NOP, PNames, Level);
	generateFoodBonus(bonusF, height, width, S, NOP, Level);
	displayFood(f);
	displayScore(S, NOP);
	Timer(timeBased,time);
	while (true)
	{
		if (_kbhit())
		{
			ch = _getch();
			if (ch == -32)
			{
				ch = _getch();
				changeDir(S, ch, NOP);
			}
			else
			{
				changeDir(S, ch, NOP);
			}
		}
		displaySnake(S, NOP);
		Sleep(100);
		eraseSnake(S, NOP);
		moveSnake(S, NOP, height, width);
		if (foodIsEaten(f, S, NOP))
		{
			expandSnake(S, NOP, f);
			changeScore(S, NOP, f);
			updateScore(S, NOP);
			generateFood(f, height, width, S, NOP, Level);
			displayFood(f);
		}
		if (kill(S, NOP, Level, height, width))
		{
			GameEnd();
			return 0;
		}
		if (x > 50)
		{
			displayFood(bonusF);
		}
		if (x > 100)
		{
			removeFood(bonusF);
			generateFoodBonus(bonusF, height, width, S, NOP, Level);
			x = 0;
		}
		if (foodIsEaten(bonusF, S, NOP))
		{
			expandSnake(S, NOP, bonusF);
			changeScore(S, NOP, bonusF);
			updateScore(S, NOP);
			generateFoodBonus(bonusF, height, width, S, NOP, Level);
			x = 0;
		}
		if (timeBased)
		{
			displayTime(time);
			if(time == 60)
			{
				GameEnd();
				return 0;
			}
			if (TargetReached(S, NOP))
			{
				winner(S, NOP);
				GameEnd();
				return 0;
			}
		}
		x++, y++;
		if (y % 10 == 0)
			time++;
	}
	return 0;
}