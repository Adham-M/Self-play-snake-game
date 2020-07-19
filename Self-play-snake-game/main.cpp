#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using namespace std;
#include "..\CMUgraphics\CMUgraphics.h"
#include "Queue.h"
#include "string.h"
#include <time.h>
#include <set>
#include <stack>

#define KEY_UP 'w'
#define KEY_DOWN 's'
#define KEY_LEFT 'a'
#define KEY_RIGHT 'd'
#define KEY_QUIT 'q'

#define KEY_UP_c 'W'
#define KEY_DOWN_c 'S'
#define KEY_LEFT_c 'A'
#define KEY_RIGHT_c 'D'
#define KEY_QUIT_c 'Q'

#define ARROW_UP '\b'
#define ARROW_DOWN '\x2'
#define ARROW_LEFT '\x4'
#define ARROW_RIGHT '\x6'

#define ROW 30 
#define COL 40 

bool quit = false;

//You can customize these parameters and the parameters in class Output
int Speed = 50; //The speed of the game, better be multiple of 5
bool bourderKills = false;


enum Directions
{
	UP, RIGHT, DOWN, LEFT
};

struct Point
{
	int x;
	int y;
};

struct UI_Info	//User Interface Info.
{
	int	dw, dh,				//Maximum snake width and height
		wx, wy,				//Window starting coordinates
		width, height,		//Window width and height
		StatusBarHeight,	//Status Bar Height
		SnakeSize,			//Snack body parts size
		TargetRadius;		//Target size

	color SnakeColor;		//Snake color
	color MsgColor;			//Messages color
	color BkGrndColor;		//Background color
	color StatusBarColor;	//Status bar color
	int PenWidth;			//width of the pen that draws shapes
	int StatusBarLineWidth;	//width of the above under the status bar


}UI;	//create a global object UI


class Output   // forward declaration
{
private:
	window* pWind;	//Pointer to the Graphics Window
public:
	Output()
	{
		UI.dw = 40; //Maximum snake parts fitting in the x-axis
		UI.dh = 30; //Maximum snake parts fitting in the y-axis 

		UI.SnakeSize = 18;
		UI.TargetRadius = UI.SnakeSize / 2;

		UI.StatusBarHeight = 50;
		UI.StatusBarLineWidth = 2;

		UI.wx = 350;
		UI.wy = 125;

		UI.width = (UI.dw /*+ UI.wx*/) * UI.SnakeSize + 15;
		UI.height = (UI.dh /*+ UI.wy*/) * UI.SnakeSize + UI.StatusBarHeight + UI.StatusBarLineWidth;


		UI.SnakeColor = WHITE;
		UI.MsgColor = WHITE;		//Messages color
		UI.BkGrndColor = BLACK;		//Background color
		UI.StatusBarColor = BLACK;
		UI.PenWidth = 3;			//width of the figures frames
		UI.StatusBarLineWidth = 2;	//width of the line under the tool bar

		//Create the output window
		pWind = CreateWind(UI.width, UI.height, UI.wx, UI.wy);
		//Change the title
		pWind->ChangeTitle("Snake game");

		CreateStatusBar();
	}


	window* CreateWind(int w, int h, int x, int y) const	//creates the application window
	{
		window* pW = new window(w, h, x, y);
		pW->SetBrush(UI.BkGrndColor);
		pW->SetPen(UI.BkGrndColor, 1);
		pW->DrawRectangle(0, 0, w, h);
		return pW;
	}

	window* GetWindow()const
	{
		return pWind;
	}

	void CreateStatusBar() const	//create the status bar
	{
		pWind->SetPen(WHITE, UI.StatusBarLineWidth);
		pWind->DrawLine(0, UI.height - UI.StatusBarHeight, UI.width, UI.height - UI.StatusBarHeight);
	}


	void ClearArea() const
	{
		pWind->SetBrush(UI.BkGrndColor);
		pWind->SetPen(UI.BkGrndColor, 1);
		pWind->DrawRectangle(0, 0, UI.width, UI.height - UI.StatusBarHeight + UI.StatusBarLineWidth);
	}

	void ClearStatusBar() const
	{
		//Clear Status bar by drawing a filled white rectangle
		pWind->SetPen(WHITE, UI.StatusBarLineWidth);
		pWind->DrawLine(0, UI.height - UI.StatusBarHeight, UI.width, UI.height - UI.StatusBarHeight);

		pWind->SetPen(UI.StatusBarColor, 1);
		pWind->SetBrush(UI.StatusBarColor);
		pWind->DrawRectangle(0, UI.height - UI.StatusBarHeight, UI.width, UI.height);
	}

	void PrintMessage(string msg) const	//Prints a message on status bar
	{
		ClearStatusBar();	//First clear the status bar

		pWind->SetPen(UI.MsgColor, 50);
		pWind->SetFont(20, BOLD, BY_NAME, "Arial");
		pWind->DrawString(10, UI.height - (int)(UI.StatusBarHeight / 1.2), msg);
	}

	void PrintMessage(string msg, int s) const	//Prints a message on status bar
	{
		pWind->SetPen(UI.MsgColor, 50);
		pWind->SetFont(20, BOLD, BY_NAME, "Arial");
		pWind->DrawString(s, UI.height - (int)(UI.StatusBarHeight / 1.2), msg);
	}

};


class BodyPart
{
	Point cord;
public:
	void SetCord(int ix, int iy)
	{
		cord.x = ix;
		cord.y = iy;
	}

	Point GetCord() const
	{
		return cord;
	}

	void DrawPart(Output* pout)
	{
		window* w = pout->GetWindow();
		w->SetPen(BLACK, 1);
		w->SetBrush(UI.SnakeColor);
		w->DrawRectangle(cord.x * UI.SnakeSize, cord.y * UI.SnakeSize, cord.x * UI.SnakeSize + UI.SnakeSize, cord.y * UI.SnakeSize + UI.SnakeSize, FILLED);
	}

	void ClearPart(Output* pout)
	{
		window* w = pout->GetWindow();
		w->SetPen(UI.BkGrndColor, 1);
		w->SetBrush(UI.BkGrndColor);
		w->DrawRectangle(cord.x * UI.SnakeSize, cord.y * UI.SnakeSize, cord.x * UI.SnakeSize + UI.SnakeSize, cord.y * UI.SnakeSize + UI.SnakeSize, FILLED);
	}
};

class Target
{
	Point position;
	int score;
public:

	Target()
	{
		srand(time(0));
		position.x = ((rand() % (UI.dw - 3)) + 1);
		position.y = ((rand() % (UI.dh - 3)) + 1);
		score = 0;
	}

	bool IsEaten(Point p)
	{
		if (p.x == position.x && p.y == position.y)
			return true;
		return false;
	}

	Point ChangeTarPos()
	{
		srand(time(0));
		position.x = ((rand() % (UI.dw - 3)) + 1);
		position.y = ((rand() % (UI.dh - 3)) + 1);

		return position;
	}

	Point ChangeTarPos(int x, int y)
	{
		position.x = position.x + x < UI.dw ? position.x + x : position.x + x - UI.dw;
		position.y = position.y + y < UI.dh ? position.y + y : position.y + y - UI.dh;
		return position;
	}

	int GetScore() const
	{
		return score;
	}

	void IncScore()
	{
		score++;
	}

	void DrawTarget(Output* pout) const
	{
		window* w = pout->GetWindow();
		w->SetPen(BLACK, 1);
		w->SetBrush(UI.SnakeColor);
		//w->DrawRectangle(position.x * UI.SnakeSize, position.y * UI.SnakeSize, position.x * UI.SnakeSize + UI.SnakeSize, position.y * UI.SnakeSize + UI.SnakeSize, FILLED);	//This function draws the target in a square form
		w->DrawCircle(position.x * UI.SnakeSize + UI.TargetRadius, position.y * UI.SnakeSize + UI.TargetRadius, UI.TargetRadius, FILLED);	//This function draws the target in a circle form
	}

	Point getPosition() const
	{
		return position;
	}
};

class Snake
{
	BodyPart* head;
	BodyPart* tail;
	int CurrentDirection;
	BodyPart* body[500];
	Queue<BodyPart*> snake;
	int count = 4;
	bool alive;
	Output* pOut;

public:
	int grid[ROW][COL];

	Snake(Output* pout)
	{
		for (int ii = 0; ii < ROW; ii++)
			for (int jj = 0; jj < COL; jj++)
				grid[ii][jj] = 1;

		alive = true;
		CurrentDirection = RIGHT;
		pOut = pout;
		head = new BodyPart;
		BodyPart * p1 = new BodyPart;
		BodyPart * p2 = new BodyPart;
		tail = new BodyPart;
		body[0] = head;
		body[1] = p1;
		body[2] = p2;
		body[3] = tail;
		int k = UI.dw / 3;
		head->SetCord(--k, UI.dh / 2); 
		p1->SetCord(--k, UI.dh / 2); grid[UI.dh / 2][k] = 0;
		p2->SetCord(--k, UI.dh / 2); grid[UI.dh / 2][k] = 0;
		tail->SetCord(--k, UI.dh / 2); grid[UI.dh / 2][k] = 0;


		int i = 3;
		while (i > -1)
		{
			body[i]->DrawPart(pOut);
			snake.enqueue(body[i]);
			i--;
		}
	}

	Point getHeadPosition() const
	{
		return head->GetCord();
	}

	bool move(Output* pout, Target tar)
	{
		window* w = pout->GetWindow();
		Point h = head->GetCord();
		int org = CurrentDirection;


		//If the user gave an action it is processed
		char current;

		if (w->GetKeyPress(current))
		{
			if (current == KEY_UP || current == KEY_UP_c || current == ARROW_UP)
				CurrentDirection = UP;
			else if (current == KEY_RIGHT || current == KEY_RIGHT_c || current == ARROW_RIGHT)
				CurrentDirection = RIGHT;
			else if (current == KEY_DOWN || current == KEY_DOWN_c || current == ARROW_DOWN)
				CurrentDirection = DOWN;
			else if (current == KEY_LEFT || current == KEY_LEFT_c || current == ARROW_LEFT)
				CurrentDirection = LEFT;
			else if (current == KEY_QUIT || current == KEY_QUIT_c)
				quit = true;
		}
		else
			return false;


		//If the user entered a reverse direction the snake will keep moving forward
		if (CurrentDirection == UP && org == DOWN)	CurrentDirection = DOWN;
		else if (CurrentDirection == RIGHT && org == LEFT)	CurrentDirection = LEFT;
		else if (CurrentDirection == DOWN && org == UP)	CurrentDirection = UP;
		else if (CurrentDirection == LEFT && org == RIGHT)	CurrentDirection = RIGHT;

		//Moving the snake
		if (CurrentDirection == UP)	h.y -= 1;
		else if (CurrentDirection == RIGHT)	h.x += 1;
		else if (CurrentDirection == DOWN)	h.y += 1;
		else if (CurrentDirection == LEFT)	h.x -= 1;

		//If the snake passed the wall it appears from the second one
		if (h.y < 0)
		{
			h.y = UI.dh - 1;
			alive = false;
		}
		else if (h.y > UI.dh - 1)
		{
			h.y = 0;
			alive = false;
		}
		else if (h.x < 0)
		{
			h.x = UI.dw - 1;
			alive = false;
		}
		else if (h.x > UI.dw - 1)
		{
			h.x = 0;
			alive = false;
		}

		if (!alive)
			if (!bourderKills)
				alive = true;



		//Cheaking if the snake killed itself
		snake.peekFront(tail);
		Point t = tail->GetCord();
		if (h.x != t.x || h.y != t.y)
			if(grid[h.y][h.x] == 0)
				alive = false;


		if (!tar.IsEaten(h)) //If the snake didnt eat
		{
			BodyPart* par;
			Point templ = head->GetCord();
			grid[templ.y][templ.x] = 0;
			snake.dequeue(par);
			templ = par->GetCord();
			grid[templ.y][templ.x] = 1;
			par->ClearPart(pOut);	//Clear tail
			par->SetCord(h.x, h.y);	//Set its new coords
			par->DrawPart(pOut);	//Draw it in the head position
			snake.enqueue(par);
			head = par;
			return false;
		}
		else  //If the snake ate
		{
			Point templ = head->GetCord();
			grid[templ.y][templ.x] = 0;
			BodyPart* np = new BodyPart;	//Adding new part to the snake
			np->SetCord(h.x, h.y);			//Its coords is the same as the target
			np->DrawPart(pOut);
			snake.enqueue(np);
			head = np;
			body[count++] = np;

			return true;
		}
	}

	void moveSnake(int dir, Target tar)
	{
		Point h = head->GetCord();
		if (dir == UP) {h.y -= 1;}
		else if (dir == RIGHT) {h.x += 1;}
		else if (dir == DOWN) {h.y += 1;}
		else if (dir == LEFT) {h.x -= 1;}

		if (!tar.IsEaten(h)) //If the snake didnt eat
		{
			BodyPart* par;
			Point templ = head->GetCord();
			grid[templ.y][templ.x] = 0;
			snake.dequeue(par);
			templ = par->GetCord();
			grid[templ.y][templ.x] = 1;
			par->ClearPart(pOut);	//Clear tail
			par->SetCord(h.x, h.y);	//Set its new coords
			par->DrawPart(pOut);	//Draw it in the head position
			snake.enqueue(par);
			head = par;
		}
		else  //If the snake ate
		{
			Point templ = head->GetCord();
			grid[templ.y][templ.x] = 0;
			BodyPart* np = new BodyPart;	//Adding new part to the snake
			np->SetCord(h.x, h.y);			//Its coords is the same as the target
			np->DrawPart(pOut);
			snake.enqueue(np);
			head = np;
			body[count++] = np;
		}
	}

	bool stillalive() const
	{
		return alive;
	}

	void DrawSnake(Output* pOut) const	//Draws the whole snake
	{
		window* w = pOut->GetWindow();

		for (int i = 0; i < count; i++)
		{
			Point pos = body[i]->GetCord();
			w->SetPen(BLACK, 1);
			w->SetBrush(UI.SnakeColor);
			w->DrawRectangle(pos.x * UI.SnakeSize, pos.y * UI.SnakeSize, pos.x * UI.SnakeSize + UI.SnakeSize, pos.y * UI.SnakeSize + UI.SnakeSize, FILLED);
		}
	}

	bool PosAvailable(Point t)
	{
		bool ok = true;
		for (int i = 0; i < count; i++)
		{
			Point p = body[i]->GetCord();
			if (p.x == t.x && p.y == t.y)
				ok = false;
		}
		return ok;
	}
};

//-------------------------------------------------------------------------------------------------------------

// Creating a shortcut for int, int pair type 
typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type 
typedef pair<double, pair<int, int>> pPair;

// A structure to hold the neccesary parameters 
struct cell
{
	// Row and Column index of its parent 
	// Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1 
	int parent_i, parent_j;
	// f = g + h 
	double f, g, h;
};

// A Utility Function to check whether given cell (row, col) 
// is a valid cell or not. 
bool isValid(int row, int col)
{
	// Returns true if row number and column number 
	// is in range 
	return (row >= 0) && (row < ROW) &&
		(col >= 0) && (col < COL);
}

// A Utility Function to check whether the given cell is 
// blocked or not 
bool isUnBlocked(int grid[][COL], int row, int col)
{
	// Returns true if the cell is not blocked else false 
	if (grid[row][col] == 1)
		return (true);
	else
		return (false);
}

// A Utility Function to check whether destination cell has 
// been reached or not 
bool isDestination(int row, int col, Pair dest)
{
	return (row == dest.first && col == dest.second);
}

// A Utility Function to calculate the 'h' heuristics. 
double calculateHValue(int row, int col, Pair dest)
{
	// Return using the distance formula 
	return abs(row - dest.first) + abs(col - dest.second);
}

// A Utility Function to trace the path from the source 
// to destination 
void tracePath(cell cellDetails[][COL], Pair dest, Snake* snk, Target targ)
{
	printf("\nThe Path is ");
	int row = dest.first;
	int col = dest.second;

	stack<Pair> Path;

	while (!(cellDetails[row][col].parent_i == row
		&& cellDetails[row][col].parent_j == col))
	{
		Path.push(make_pair(row, col));
		int temp_row = cellDetails[row][col].parent_i;
		int temp_col = cellDetails[row][col].parent_j;
		row = temp_row;
		col = temp_col;
	}

	//Path.push(make_pair(row, col));
	int h_pos_row = row;
	int h_pos_col = col;
	while (!Path.empty())
	{
		pair<int, int> p = Path.top();
		Path.pop();
		if (h_pos_row > p.first) { snk->moveSnake(UP, targ); h_pos_row = p.first; }
		else if (h_pos_row < p.first) { snk->moveSnake(DOWN, targ); h_pos_row = p.first; }
		else if (h_pos_col > p.second) { snk->moveSnake(LEFT, targ); h_pos_col = p.second; }
		else if (h_pos_col < p.second) { snk->moveSnake(RIGHT, targ); h_pos_col = p.second; }
		


		//Speed of the game is customizable
		/*int cs = targ.GetScore();
		if (Speed*1.3 - (cs / 10) > Speed)
			Sleep(Speed*1.3 - (cs / 10));
		else
			Sleep(Speed - (cs - Speed * 3) / Speed * 0.3);*/

		Sleep(30);
	}

	return;
}

// A Function to find the shortest path between 
// a given source cell to a destination cell according 
// to A* Search Algorithm 
void aStarSearch(int grid[][COL], Pair src, Pair dest, Snake* snk, Target targ)
{
	// If the source is out of range 
	if (isValid(src.first, src.second) == false)
	{
		printf("Source is invalid\n");
		while (1) {}
		return;
	}

	// If the destination is out of range 
	if (isValid(dest.first, dest.second) == false)
	{
		printf("Destination is invalid\n");
		while (1) {}
		return;
	}

	// Either the source or the destination is blocked 
	if (isUnBlocked(grid, src.first, src.second) == false ||
		isUnBlocked(grid, dest.first, dest.second) == false)
	{
		printf("Source or the destination is blocked\n");
		while (1) {}
		return;
	}

	// If the destination cell is the same as source cell 
	if (isDestination(src.first, src.second, dest) == true)
	{
		printf("We are already at the destination\n");
		return;
	}

	// Create a closed list and initialise it to false which means 
	// that no cell has been included yet 
	// This closed list is implemented as a boolean 2D array 
	bool closedList[ROW][COL];
	memset(closedList, false, sizeof(closedList));

	// Declare a 2D array of structure to hold the details 
	//of that cell 
	cell cellDetails[ROW][COL];

	int i, j;

	for (i = 0; i < ROW; i++)
	{
		for (j = 0; j < COL; j++)
		{
			cellDetails[i][j].f = FLT_MAX;
			cellDetails[i][j].g = FLT_MAX;
			cellDetails[i][j].h = FLT_MAX;
			cellDetails[i][j].parent_i = -1;
			cellDetails[i][j].parent_j = -1;
		}
	}

	// Initialising the parameters of the starting node 
	i = src.first, j = src.second;
	cellDetails[i][j].f = 0.0;
	cellDetails[i][j].g = 0.0;
	cellDetails[i][j].h = 0.0;
	cellDetails[i][j].parent_i = i;
	cellDetails[i][j].parent_j = j;

	/*
	 Create an open list having information as-
	 <f, <i, j>>
	 where f = g + h,
	 and i, j are the row and column index of that cell
	 Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
	 This open list is implenented as a set of pair of pair.*/
	set<pPair> openList;

	// Put the starting cell on the open list and set its 
	// 'f' as 0 
	openList.insert(make_pair(0.0, make_pair(i, j)));

	// We set this boolean value as false as initially 
	// the destination is not reached. 
	bool foundDest = false;

	while (!openList.empty())
	{
		pPair p = *openList.begin();

		// Remove this vertex from the open list 
		openList.erase(openList.begin());

		// Add this vertex to the closed list 
		i = p.second.first;
		j = p.second.second;
		closedList[i][j] = true;

		/*
		 Generating all the 4 successor of this cell

			       N
			       |
				   |
		     W----Cell----E
				   |
				   |
			       S

		 Cell-->Popped Cell (i, j)
		 N -->  North       (i-1, j)
		 S -->  South       (i+1, j)
		 E -->  East        (i, j+1)
		 W -->  West        (i, j-1)*/

		 // To store the 'g', 'h' and 'f' of the 4 successors 
		double gNew, hNew, fNew;

		//----------- 1st Successor (North) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i - 1, j) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i - 1, j, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i - 1][j].parent_i = i;
				cellDetails[i - 1][j].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest, snk, targ);
				foundDest = true;
				return;
			}
			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i - 1][j] == false &&
				isUnBlocked(grid, i - 1, j) == true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i - 1, j, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i - 1][j].f == FLT_MAX ||
					cellDetails[i - 1][j].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i - 1, j)));

					// Update the details of this cell 
					cellDetails[i - 1][j].f = fNew;
					cellDetails[i - 1][j].g = gNew;
					cellDetails[i - 1][j].h = hNew;
					cellDetails[i - 1][j].parent_i = i;
					cellDetails[i - 1][j].parent_j = j;
				}
			}
		}

		//----------- 2nd Successor (South) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i + 1, j) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i + 1, j, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i + 1][j].parent_i = i;
				cellDetails[i + 1][j].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest, snk, targ);
				foundDest = true;
				return;
			}
			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i + 1][j] == false &&
				isUnBlocked(grid, i + 1, j) == true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i + 1, j, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i + 1][j].f == FLT_MAX ||
					cellDetails[i + 1][j].f > fNew)
				{
					openList.insert(make_pair(fNew, make_pair(i + 1, j)));
					// Update the details of this cell 
					cellDetails[i + 1][j].f = fNew;
					cellDetails[i + 1][j].g = gNew;
					cellDetails[i + 1][j].h = hNew;
					cellDetails[i + 1][j].parent_i = i;
					cellDetails[i + 1][j].parent_j = j;
				}
			}
		}

		//----------- 3rd Successor (East) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i, j + 1) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i, j + 1, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i][j + 1].parent_i = i;
				cellDetails[i][j + 1].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest, snk, targ);
				foundDest = true;
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i][j + 1] == false &&
				isUnBlocked(grid, i, j + 1) == true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i, j + 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i][j + 1].f == FLT_MAX ||
					cellDetails[i][j + 1].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i, j + 1)));

					// Update the details of this cell 
					cellDetails[i][j + 1].f = fNew;
					cellDetails[i][j + 1].g = gNew;
					cellDetails[i][j + 1].h = hNew;
					cellDetails[i][j + 1].parent_i = i;
					cellDetails[i][j + 1].parent_j = j;
				}
			}
		}

		//----------- 4th Successor (West) ------------ 

		// Only process this cell if this is a valid one 
		if (isValid(i, j - 1) == true)
		{
			// If the destination cell is the same as the 
			// current successor 
			if (isDestination(i, j - 1, dest) == true)
			{
				// Set the Parent of the destination cell 
				cellDetails[i][j - 1].parent_i = i;
				cellDetails[i][j - 1].parent_j = j;
				printf("The destination cell is found\n");
				tracePath(cellDetails, dest, snk, targ);
				foundDest = true;
				return;
			}

			// If the successor is already on the closed 
			// list or if it is blocked, then ignore it. 
			// Else do the following 
			else if (closedList[i][j - 1] == false &&
				isUnBlocked(grid, i, j - 1) == true)
			{
				gNew = cellDetails[i][j].g + 1.0;
				hNew = calculateHValue(i, j - 1, dest);
				fNew = gNew + hNew;

				// If it isn’t on the open list, add it to 
				// the open list. Make the current square 
				// the parent of this square. Record the 
				// f, g, and h costs of the square cell 
				//                OR 
				// If it is on the open list already, check 
				// to see if this path to that square is better, 
				// using 'f' cost as the measure. 
				if (cellDetails[i][j - 1].f == FLT_MAX ||
					cellDetails[i][j - 1].f > fNew)
				{
					openList.insert(make_pair(fNew,
						make_pair(i, j - 1)));

					// Update the details of this cell 
					cellDetails[i][j - 1].f = fNew;
					cellDetails[i][j - 1].g = gNew;
					cellDetails[i][j - 1].h = hNew;
					cellDetails[i][j - 1].parent_i = i;
					cellDetails[i][j - 1].parent_j = j;
				}
			}
		}

	}

	// When the destination cell is not found and the open 
	// list is empty, then we conclude that we failed to 
	// reach the destiantion cell. This may happen when the 
	// there is no way to destination cell (due to blockages) 
	if (foundDest == false) {
		printf("Failed to find the Destination Cell\n");
		while (1) {}
	}

	return;
}
//-------------------------------------------------------------------------------------------------------------


void PrintScores(Output* pout, int cs, int hs)
{
	pout->PrintMessage("Score = " + to_string(cs));
	string s_hs = to_string(hs);
	pout->PrintMessage("High Score = " + s_hs, UI.width - 160 - (s_hs.length() * 6));
}

int main()
{
	Output* pOut = new Output;

	Target* tar = new Target;
	Snake* snak = new Snake(pOut);

	int hs = 0;
	int cs = 0;


	tar->DrawTarget(pOut);

	PrintScores(pOut, cs, hs);

	Point temppt = tar->getPosition();
	Point tempph = snak->getHeadPosition();
	aStarSearch(snak->grid, make_pair(tempph.y, tempph.x), make_pair(temppt.y, temppt.x), snak, *tar);

	while (!quit)
	{
		/*if (snak->move(pOut, *tar))	//If the snake ate the target the "move" function returns true
		{
			*/


			tar->IncScore();		//Incrementing the score
			Point p = tar->ChangeTarPos();		//Changing target position
			while (!snak->PosAvailable(p))		//Cheak if it is a valid position
				p = tar->ChangeTarPos(rand() % 5, rand() % 5);
			tar->DrawTarget(pOut);		//Drawing the target
			cs = tar->GetScore();
			hs = cs > hs ? cs : hs;		//Cheak if it is a high score or not
			PrintScores(pOut, cs, hs);		//printing scores

			temppt = tar->getPosition();
			tempph = snak->getHeadPosition();
			aStarSearch(snak->grid, make_pair(tempph.y, tempph.x), make_pair(temppt.y, temppt.x), snak, *tar);
		/*}

		if (!snak->stillalive())	//If the snake ate itself it return true 
		{
			//Deleting last game objects and creating new
			delete snak;
			delete tar;
			tar = new Target;
			snak = new Snake(pOut);
			pOut->ClearArea();
			tar->DrawTarget(pOut);
			cs = tar->GetScore();
			PrintScores(pOut, cs, hs);
		}
		*/


	}
}
