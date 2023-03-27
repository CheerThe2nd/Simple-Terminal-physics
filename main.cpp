#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
using namespace std;

#define WIDTH 200
#define HEIGHT 50

inline void DrawFrame(vector<string> &Screen)
{
	system("CLS");
	for (int y = 0; y < HEIGHT; y++)
	{
		cout << Screen[y] << endl;
	}
}


void UpdateScreen(vector<string> &Screen,int Ballx, int Bally)
{
	
	Screen[Bally][Ballx] = '#';
}

int main() 
{
	//screen init
	vector<string>Screen;
	string Line = "";
	for (int x = 0; x < WIDTH; x++)
	{
		Line += ".";
	}

	for(int y = 0; y < HEIGHT; y++)
	{
		Screen.push_back(Line);
	}

	float vX = 0, vY = 0;
	float oldvX = 0, oldvY = 0;
	float Ballx = 20, Bally = 0;
	float Drag = 0;
	long FrameLenght = 0;
	float Gravity = 0;
	vX = 1; //start speed


	cout << "Horizontales Starttempo: ";
	cin >> vX;
	cout << "Luftwiderstand: ";
	cin >> Drag;
	cout << "Framedauer in ms: ";
	cin >> FrameLenght;
	cout << "Start X: ";
	cin >> Ballx;
	if(Ballx == 0 || Ballx == WIDTH-1)
	{
		cout << "Invalid Position" << endl;
		cin >> Ballx;
	}

	cout << "Start Y: ";
	cin >> Bally;
	if (Bally == 0 || Bally == HEIGHT - 1)
	{
		cout << "Invalid Position" << endl;
		cin >> Bally;
	}

	cout << "Gravitation: ";
	cin >> Gravity;



	while(true)
	{

		if(Ballx >= WIDTH-1)
		{
			Ballx = WIDTH - 2;
			oldvX = vX;
			vX = -oldvX/Drag;
		}
		else if(Ballx <= 0)
		{
			Ballx = 0;
			oldvX = vX;
			vX = -oldvX/Drag;
		}

		
		vY += Gravity;
		Bally += vY;
		Ballx += vX;
		


		if(Bally >= HEIGHT)
		{
			Bally = HEIGHT - 1;
			oldvY = vY;
			vY = 0;
			vY = -oldvY / Drag;
		}
		else if(Bally <= 0)
		{
			Bally = 0;
			oldvY = vY;
			vY = 0;
			vY = -oldvY/Drag;
		}
		UpdateScreen(Screen, floor(Ballx), floor(Bally));
		DrawFrame(Screen);
		std::this_thread::sleep_for(std::chrono::milliseconds(FrameLenght));
	}


}
