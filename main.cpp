#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>

#define SCREEN_X 120
#define SCREEN_Y 60

std::string gSl; // Global screen line

#ifdef _WIN32
#include <windows.h>
inline static void ClearTerminal()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };  // Home for the cursor
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
        return;
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}
#else
void clear_console() {
    std::cout << "\033c";  // Unix-like OS: send escape sequence to reset terminal
}
#endif

struct Vec2
{
    double x, y;

    Vec2(const double _x, const double _y) : x(_x), y(_y) {}

    Vec2& operator=(const Vec2& other)
    {
        if (this != &other)
        {  // Self-assignment guard
            this->x = other.x;
            this->y = other.y;
        }
        return *this;
    }

    Vec2& operator+=(const Vec2& other)
    {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    Vec2& operator*=(const Vec2& other)
    {
        this->x *= other.x;
        this->y *= other.y;
        return *this;
    }

    Vec2& operator*=(const double sc)
    {
        this->x *= sc;
        this->y *= sc;
        return *this;
    }

    Vec2 operator*(const double scale)
    {
        return Vec2(this->x * scale, this->y * scale);
    }


    inline void Normalize()
    {
        double sz = std::sqrt(x * x + y * y);
        if (sz > 0)
        {
            x /= sz;
            y /= sz;
        }
    }
};

inline static double GetDist(const Vec2& v, const Vec2& v2)
{
    return std::sqrt((v.x - v2.x) * (v.x - v2.x) +
        (v.y - v2.y) * (v.y - v2.y));
}

struct Ball
{
    Vec2 vel;
    Vec2 pos;
    char mat;

    Ball(const Vec2& p, const char _mat)
        :
        vel(0, 0), pos(p), mat(_mat)
    {}
};

struct CircleCol
{
    Vec2 pos;
    double r;
    char mat;
    CircleCol(const Vec2& p, const int _r, const char _mat)
        :
        pos(p),r(_r),mat(_mat)
    {}
};

static void DrawScene(const Ball& b, const CircleCol& col)
{
    bool dirtyLine = true;
    for (int y = 0; y < SCREEN_Y; y++)
    {
        if (dirtyLine) // If dirty reset
        {
            gSl.clear();
            gSl.insert(gSl.begin(), SCREEN_X, '.');
            dirtyLine = false;
        }

        if (y == (int)b.pos.y)
        {
            if (b.pos.x >= 0 || b.pos.x < SCREEN_X)
            {
                gSl[(int)(b.pos.x)] = b.mat;
                dirtyLine = true;
            }
        }
        Vec2 screenCoord(0, y); // Avoid new allocation in 2nd loop
        for(int x = 0; x < SCREEN_X; x++)
        {
            screenCoord.x = x;
            if (GetDist(screenCoord, col.pos) <= col.r)
            {
                gSl[x] = col.mat;
                dirtyLine = true;
            }
        }

        std::cout << gSl << std::endl;
    }
}

template <typename T>
inline static bool IsInRange(const T t, const T incluMin, const T incluMax)
{
    return (t >= incluMin && t <= incluMax);
}

static void TickPhysics(Ball& b, const CircleCol& col)
{
    // Move ball according to x velocity
    b.pos.x += b.vel.x;
    if (!IsInRange<double>(b.pos.x, 0, (double)(SCREEN_X - 1))) // Movement will result in screen collision
    {
        b.pos.x -= b.vel.x * 2.0;
        b.vel.x *= -1.0; // Invert velocity
    }
    
    // Move ball according to y velocity
    b.pos.y += b.vel.y;
    if (!IsInRange<double>(b.pos.y, 0, (double)(SCREEN_Y - 1))) // Movement will result in screen collision
    {
        b.pos.y -= b.vel.y * 2.0;
        b.vel.y *= -1.0; // Invert velocity
    }

    if(GetDist(b.pos, col.pos) <= col.r)
    {
        Vec2 n = Vec2(b.pos.x - col.pos.x, b.pos.y - col.pos.y);
        n.Normalize();
        b.pos += n * col.r;
        b.vel *= n;
    }
}

int main()
{
    bool exit = false;
    double t = 0.0;
    Ball ball(Vec2(5, 5),'#');
    ball.vel = Vec2(2, 0);
    CircleCol col(Vec2(SCREEN_X/2, SCREEN_Y), 10, 'x');
    do
    {
        t += 0.01;

        // Crappy gravity
        ball.vel.y += 0.4;
        // Move circle collider around
        col.pos.y = abs(sin(t)) * SCREEN_Y;
        ClearTerminal();
        TickPhysics(ball,col);
        DrawScene(ball, col);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (!exit);
    return 0;
}
