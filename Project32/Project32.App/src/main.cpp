#include "../../Project32.API/src/Project32.API.h"
#include <iostream>

int main() {
    std::wcout << L"Loading engine DLL..." << std::endl;

    if (P32::Engine::LoadDLL(L"Project32.Core.dll")) {
        if (P32::Engine::Init(1024, 768, "My C++ Game")) {
            std::cout << "Engine initialized with C++ wrapper!" << std::endl;

            while (P32::Engine::IsRunning()) {
                if (P32::Engine::KeyPressed(27)) { // ESC
                    break;
                }

                float mx, my;
                P32::Engine::GetMousePos(&mx, &my);

                P32::Engine::Run();
            }

            P32::Engine::Shutdown();
        }

        P32::Engine::UnloadDLL();
    }

    return 0;
}