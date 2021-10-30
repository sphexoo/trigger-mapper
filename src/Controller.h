#include <xinput.h>
#include <string>

struct TriggerState
{
    BYTE value = 0;         // current analoge trigger value
    BOOL isPressed = false; // current digital trigger value
    BYTE threshold;         // threshold when trigger (de-)activates
    WORD key;               // key mapped to the trigger

    void setThreshold(BYTE value) { threshold = value; }
    void setKey(WORD value) { key = value; }
};

class Controller
{
public:
    void update(XINPUT_GAMEPAD state);
    static void handleTriggerEvent(BYTE& newValue, TriggerState& state);
    void loadConfig(std::string& path);

private:
    TriggerState leftTrigger;
    TriggerState rightTrigger;
};