#include<iostream>
#include<vector>
#include<Windows.h>
#include<conio.h>
#include<stdio.h>
#include<random>
#include<ctime>
#include<chrono>
#include<thread>
#define GAMELIMBOT 50
#define GAMELIMRIGHT 100




using namespace std;


void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

void setCursorPosition(int x, int y)
{
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	//std::cout.flush();
	COORD coord = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hOut, coord);
}


struct my_coord {
	int x;
	int y;
	my_coord(int xx,int yy)
		:x(xx),y(yy){}
	bool operator ==(my_coord const& coord) {
		return coord.x == x && coord.y == y;
	}
};
CHAR_INFO buffer[GAMELIMBOT][GAMELIMRIGHT];
COORD buffer_size = { GAMELIMRIGHT,GAMELIMBOT };
COORD buffer_coord = { 0, 0 };
SMALL_RECT rc_region = { 0, 0, GAMELIMRIGHT - 1, GAMELIMBOT - 1 };
bool speed_update = false;
my_coord gen_food() {
	srand(chrono::system_clock::now().time_since_epoch().count());
	std::default_random_engine eng(chrono::system_clock::now().time_since_epoch().count());
	uniform_int_distribution<int>uni_disty(0, GAMELIMBOT-2);
	uniform_int_distribution<int>uni_distx(0, GAMELIMRIGHT-2);
	return my_coord(int(uni_distx(eng)), int(uni_disty(eng)));
}

class Snake {

public:
	int score;
	bool alive = true;
	vector<my_coord> cells;
	char dir;
	Snake(char dr, my_coord start_pos) {
		dir = dr;
		score = 0;
		cells.push_back(start_pos);
		switch (dr) {
		case 'w': {
			for (int i = 1; i < 4; ++i) {
				cells.push_back(my_coord(start_pos.x,start_pos.y+i));
			}
			break;
		}
		case 's': {
			for (int i = 1; i < 4; ++i) {
				cells.push_back(my_coord(start_pos.x, start_pos.y-i));
			}
			break;
		}
		case 'a': {
			for (int i = 1; i < 4; ++i) {
				cells.push_back(my_coord(start_pos.x+i, start_pos.y));
			}
			break;
		}
		case 'd': {
			for (int i = 1; i < 4; ++i) {
				cells.push_back(my_coord(start_pos.x-i, start_pos.y));
			}
			break;
		}
		}

	}
	
	void move(my_coord& food,unsigned long long speed,unsigned long long& timer) {
		static bool moved = false;
		if (moved) {
			moved = false;
			if (GetAsyncKeyState('W')) {
				if (dir != 's')
					dir = 'w';
			}
			else if (GetAsyncKeyState('S')) {
				if (dir != 'w')
					dir = 's';
			}
			else if (GetAsyncKeyState('D')) {
				if (dir != 'a')
					dir = 'd';
			}
			else if (GetAsyncKeyState('A')) {
				if (dir != 'd')
					dir = 'a';
			}
		}
		using namespace std::chrono_literals;
		unsigned long long start = std::chrono::high_resolution_clock::now().time_since_epoch().count();

		if (start - timer > speed) {
			moved = true;
			timer = start;
			my_coord temp = cells[0];
			switch (dir) {
			case'w': { //w for north wasd keyboard
				cells[0].y--;
				break;
			}
			case's': {
				cells[0].y++;
				break;
			}
			case'd': {
				cells[0].x++;
				break;
			}
			case'a': {
				cells[0].x--;
				break;
			}
			}

			for (int i = 1; i < cells.size(); ++i) {
				my_coord temp2 = cells[i];
				cells[i] = temp;
				temp = temp2;
			}
			//eating
			if (cells[0] == food) {
				speed_update = true;
				score++;
				my_coord last(cells[cells.size() - 1].x, cells[cells.size() - 1].y);
				my_coord before_last(cells[cells.size() - 2].x, cells[cells.size() - 2].y);
				if (last.x == before_last.x) {
					if (before_last.y > last.y) {
						cells.push_back(my_coord(last.x, last.y - 1));
					}
					else {
						cells.push_back(my_coord(last.x, last.y + 1));
					}
				}
				else if (last.y == before_last.y) {
					if (last.x < before_last.x) {
						cells.push_back(my_coord(last.x - 1, last.y));
					}
					else {
						cells.push_back(my_coord(last.x - 1, last.y));
					}
				}
			}
		}
			

		
	}
	
	void draw_game(my_coord& food,HANDLE h_output,int speed) {
		/*
		using namespace std::chrono_literals;
		auto start = std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(100ms- std::chrono::milliseconds(speed));
		auto end = std::chrono::high_resolution_clock::now();
		*/
		ReadConsoleOutput(h_output, (CHAR_INFO*)buffer, buffer_size, buffer_coord, &rc_region);
		if (cells[0].x >= GAMELIMRIGHT -1|| cells[0].y >= GAMELIMBOT-1 || cells[0].x < 0 || cells[0].y < 0)alive = false;
		//setCursorPosition(0, 0);
		for (int i = 0; i < GAMELIMBOT; ++i) {
			
			for (int j = 0; j < GAMELIMRIGHT; ++j) {
				bool onsnake = false;
				//setCursorPosition(j, i);
				if (i == cells[0].y && j == cells[0].x) {
					//cout << '8';
					buffer[i][j].Char.AsciiChar = '8';
					onsnake = true;
				}
					
				else 
				for (int t = 1; t < cells.size(); ++t) {
					if (cells[0] == cells[t])alive = false;
					if (i == cells[t].y && j == cells[t].x) {
						//cout << '#';
						buffer[i][j].Char.AsciiChar = '#';
						onsnake = true;
						break;
					}
				}
				
				if (!onsnake) {
					if (i == food.y && j == food.x) {
						//cout << 'F';
						buffer[i][j].Char.AsciiChar = 'F';
					}
					else {
						//cout << ' ';
						buffer[i][j].Char.AsciiChar = ' ';
					}
				}
				else if (i == food.y && j == food.x) {
					food = gen_food();
				}
				
				if (i == GAMELIMBOT - 1 && j != GAMELIMRIGHT - 1) {
					//cout << 'I';
					buffer[i][j].Char.AsciiChar = 'I';
				}
			}//end of j loop
			//cout << "|";
			buffer[i][GAMELIMRIGHT-1].Char.AsciiChar = '|';
			//cout << endl;
			buffer[i][GAMELIMRIGHT - 1].Char.AsciiChar = '\n';
		}//end of i loop
		
		setCursorPosition(GAMELIMRIGHT+4, 5);
		cout << "SCORE IS:::" << score<<endl;
		//cout << "coord is:: x::" << cells[0].x<<" y::"<<cells[0].y << endl;
		
			
		WriteConsoleOutput(h_output, (CHAR_INFO*)buffer, buffer_size, buffer_coord, &rc_region);
			
	}
	

};//end of class

my_coord food = gen_food();




int main() {
	HANDLE h_out = (HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleDisplayMode(h_out, CONSOLE_FULLSCREEN_MODE, 0);
	char inp='d';
	Snake pip(inp, my_coord(20, 5));
	pip.dir = inp;
	bool escaped = false;
	ShowConsoleCursor(false);
	unsigned long long slower = 90000000;
	unsigned long long timer= std::chrono::high_resolution_clock::now().time_since_epoch().count();

	while(pip.alive) {
		if (speed_update) {
			slower -= 50 ;
			speed_update = false;
		}
		
		if (GetAsyncKeyState(VK_ESCAPE)) {
			escaped = true;
			break;
		}
		pip.move(food,slower,timer);
		pip.draw_game(food, h_out,slower);
		
	}
	system("cls");
	if(escaped){
		setCursorPosition(GAMELIMRIGHT / 2, GAMELIMBOT / 2);
		cout << "GAME OVER ,press ENTER to close" << endl;
		setCursorPosition(GAMELIMRIGHT / 2, (GAMELIMBOT / 2) + 1);
		cout << "SCORE is::" << pip.score << endl;
	}
	else {
		setCursorPosition(GAMELIMRIGHT / 2, GAMELIMBOT / 2);
		cout << "YEEERRRRR DEED! ,press ENTER to close" << endl;
		setCursorPosition(GAMELIMRIGHT / 2, (GAMELIMBOT / 2) + 1);
		cout << "SCORE is::" << pip.score << endl;
	}
	system("pause");

}