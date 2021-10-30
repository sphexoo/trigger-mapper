#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <winuser.h>
#include <chrono>
#include <thread>

#include "Controller.h"

static void sendInput(char key)
{
#ifdef _DEBUG
    std::cout << "Sending input " << key << "\n";
#endif
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
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
#ifdef _DEBUG
    if (uSent != ARRAYSIZE(inputs))
    {
        std::cout << (L"SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }
#endif
}

void Controller::update(XINPUT_GAMEPAD state)
{
    handleTriggerEvent(state.bLeftTrigger, leftTrigger);
    handleTriggerEvent(state.bRightTrigger, rightTrigger);
}

void Controller::handleTriggerEvent(BYTE &newValue, TriggerState &state)
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

void Controller::loadConfig(std::string &path)
{
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (!(iss >> key >> value))
        {
            break;
        } // EOF

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
