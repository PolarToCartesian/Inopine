#include <Inopine/Inopine.hpp>
#include <numbers>

int main(int argc, char** argv) {
    //std::cout << IE::Matf32::MakeRotationX(std::numbers::pi) << '\n';
    
    ::IE::Window window(500, 500, "yo le rap");

    while (window.IsRunning()) {
        window.Update();
    }

    return 0;
}