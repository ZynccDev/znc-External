#include <iostream>
#include <thread>
#include "rbx.hpp"
#include "OverlayHook/OverlayHook.hpp"
#include <Windows.h>

/* 
		I added a bunch of comments so its easier to understand
		Update offsets in Update > Offsets.hpp
		You can change the overlay title in OverlayHook.cpp line 241
		You can add more stuff in rbx.hpp
		memory.hpp is for reading and writing memory so u dont need to change anything there
		instance.hpp is for getting instances like localplayer, humanoid, workspace, players etc so u dont need to change anything there if you want
*/
int main() {
	if (!memory::attach(L"RobloxPlayerBeta.exe")) { // we attach to roblox
        std::cout << "uh we didnt attach noo\n";
		system("pause"); // pause so the user can see the message
        return 1;
    }

	HWND robloxWindow = FindWindow(NULL, L"Roblox"); // find roblox window by its title
	if (!robloxWindow) { // if we didnt find the window
        std::cout << "roblox window not found so overlay might not work\n";
        system("pause");
		return 1; // exit the program
    }

	std::cout << "sick we attached\n"; // we are in the matrix

	OverlayHook->Initialize(); // start the overlay
	return 0; // exit the program and the overlay sadly closes :(
}
