#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <xinput.h>
#include <bitset>
#include <winuser.h>
#include <chrono>
#include <thread>




void sendInput(char key)
{
    std::cout << "Sending input " << key << "\n";
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE;
    inputs[0].ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(key)), 0);

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.dwFlags = KEYEVENTF_SCANCODE;
    inputs[1].ki.wScan = MapVirtualKey(LOBYTE(VkKeyScan(key)), 0);
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        std::cout << (L"SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }
}


struct TriggerState
{
    BYTE value = 0;     // current analoge trigger value
    BOOL isPressed = false; // current digital trigger value
    BYTE threshold; // threshold when trigger (de-)activates
    WORD key;       // key mapped to the trigger

    void setThreshold(BYTE value) { threshold = value; }
    void setKey(WORD value) { key = value; }
};


class TriggerHandler
{
public:
    void update(XINPUT_GAMEPAD state)
    {
        handleTriggerEvent(state.bLeftTrigger, leftTrigger);
        handleTriggerEvent(state.bRightTrigger, rightTrigger);
    }

    static void handleTriggerEvent(BYTE& newValue, TriggerState& state)
    {
        if (newValue != state.value)
        {
            state.value = newValue;

            if (state.isPressed)
            {
                if (newValue < state.threshold)
                {
                    state.isPressed = false;
                }
            }
            else
            {
                if (newValue >= state.threshold)
                {
                    state.isPressed = true;
                    sendInput(state.key);
                }
            }
        }
    }

    void loadConfig(std::string& path)
    {
        std::ifstream file(path); 
        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string key, value;
            if ( !(iss >> key >> value) ) { break; } // EOF

            if (key == "LEFT_TRIGGER_KEY")
            {
                char c = (value.c_str())[0];
                unsigned char tmp = static_cast<unsigned char>(c);
                leftTrigger.setKey(tmp);
            }
            else if (key == "LEFT_TRIGGER_THRESHOLD")
            {
                int i = std::stoi(value);
                unsigned char c = static_cast<unsigned char>(i);
                leftTrigger.setThreshold(c);
            }
            else if (key == "RIGHT_TRIGGER_KEY")
            {
                char c = (value.c_str())[0];
                unsigned char tmp = static_cast<unsigned char>(c);
                rightTrigger.setKey(tmp);
            }
            else if (key == "RIGHT_TRIGGER_THRESHOLD")
            {
                int i = std::stoi(value);
                unsigned char c = static_cast<unsigned char>(i);
                rightTrigger.setThreshold(c);
            }
        }
    }
    
private:
    TriggerState leftTrigger;
    TriggerState rightTrigger;
};




int main()
{
    TriggerHandler triggerHandler;
    std::string path = "config.txt";
    triggerHandler.loadConfig(path);

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

    DWORD lastPacketNumber = 0;
    while (true)
    {
        dwResult = XInputGetState(controllerId, &state);

        if (state.dwPacketNumber != lastPacketNumber)
        {
            lastPacketNumber = state.dwPacketNumber;
            triggerHandler.update(state.Gamepad);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    
    return 0;
}