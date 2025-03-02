// Project Specific Includes (custom)
#include "Game.h"
//#include <Windows.h>

//int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
int main()
{
    // SRand seed initialization
    std::srand(static_cast<unsigned>(time(NULL)));

    // Init game engine
    Game game;

    // Debug
    game.setDisplayTitleFps(false);

    while (game.getIsRunning())
    {
        // Update
        game.update();
        
        // Render
        game.render();
    }

    return 0;
}