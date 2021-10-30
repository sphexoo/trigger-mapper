#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <xinput.h>
#include <bitset>
#include <winuser.h>
#include <chrono>
#include <thread>

#include "Controller.h"

int main()
{
    Controller controller;
    std::string path = "config.txt";
    controller.loadConfig(path);

    DWORD dwResult;    
    DWORD controllerId;
    BOOL isConnected = false;

    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

    for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
    {
        dwResult = XInputGetState( i, &state );

        if( dwResult == ERROR_SUCCESS )
        {
            isConnected = true;
            controllerId = i;
            std::cout << "Controller " << controllerId << " connected." << std::endl;
            break;
        }
    }

    if (!isConnected)
    {
        std::cout << "Controller not found. Application is terminating." << std::endl;
        std::cin.get();
        return -1;
    }

    //std::cout << "Press ENTER to start listening for inputs and hide console window." << std::endl;
    //std::cin.get();

    //HWND hWin = GetForegroundWindow();
    //ShowWindow(hWin, SW_HIDE);

    DWORD lastPacketNumber = 0;
    while (true)
    {
        dwResult = XInputGetState(controllerId, &state);

        if (state.dwPacketNumber != lastPacketNumber)
        {
            lastPacketNumber = state.dwPacketNumber;
            controller.update(state.Gamepad);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    
    return 0;
}   