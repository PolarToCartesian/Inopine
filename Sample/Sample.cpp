#include <Inopine/Inopine.hpp>
#include <numbers>
#include <chrono>
#include <thread>

int main(int argc, char** argv) {
    //std::cout << IE::Matf32::MakeRotationX(std::numbers::pi) << '\n';
    
    ::IE::Window window(500, 500, "Inopine Test Window");

    auto s = window.GetClientDimensions();
    bool prev = false;
    while (window.IsRunning()) {
        window.Update();

        if (s != window.GetClientDimensions())
        {
            s = window.GetClientDimensions();
            std::cout << s << '\n';
        }
    
        if (!prev && window.IsKeyDown('A'))
        {
            prev = true;
            std::cout << "A IS DOWN\n";
        }

        if (prev && window.IsKeyUp('A')) {
            prev = false;
            std::cout << "A IS UP\n";
        }

        if (window.IsLeftButtonDown()) {
            std::cout << "LEFT DOWN\n";
        }

        if (window.IsRightButtonDown()) {
            std::cout << "RIGHT DOWN\n";
        }

        std::cout << window.GetRelativeCursorPosition() << '\n';

        std::this_thread::sleep_for(std::chrono::microseconds(16));
    }

    return 0;
}