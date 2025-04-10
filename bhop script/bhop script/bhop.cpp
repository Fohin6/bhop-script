#include <windows.h>
#include <atomic>
#include <thread>
#include <iostream>

std::atomic<bool> g_running(false);
std::atomic<bool> g_shouldStop(false);

void SimulateSpacePress() {
    while (g_running.load() && !g_shouldStop.load()) {
        // Нажатие SPACE
        INPUT inputDown = { 0 };
        inputDown.type = INPUT_KEYBOARD;
        inputDown.ki.wVk = VK_SPACE;
        SendInput(1, &inputDown, sizeof(INPUT));

        // Отпускание SPACE
        INPUT inputUp = { 0 };
        inputUp.type = INPUT_KEYBOARD;
        inputUp.ki.wVk = VK_SPACE;
        inputUp.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &inputUp, sizeof(INPUT));

        Sleep(100); // Интервал между нажатиями (можно регулировать)
    }
}

void MouseButtonListener() {
    bool mouse4WasPressed = false;

    while (!g_shouldStop.load()) {
        bool mouse4IsPressed = (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0; //XBUTTON1 - mouse4, XBUTTON2 - mouse 5

        if (mouse4IsPressed && !mouse4WasPressed) {
            // Mouse4 только что нажата — запускаем цикл
            g_running.store(true);
            std::thread(SimulateSpacePress).detach();
        }
        else if (!mouse4IsPressed && mouse4WasPressed) {
            // Mouse4 отпущена — останавливаем цикл
            g_running.store(false);
        }

        mouse4WasPressed = mouse4IsPressed;
        Sleep(10); // Чтобы не грузить CPU
    }
}

int main() {
    std::cout << "thx Deepseek " << std::endl;
    std::cout << "BHOP works on mouse 5 " << std::endl;
    std::thread listener(MouseButtonListener);
    while (!g_shouldStop.load()) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            g_shouldStop.store(true);
        }
        Sleep(100);
    }

    listener.join();
    return 0;
}