#include <ic.h>

using namespace IC;

int main() {
    Config config;
    config.Name = "ICEditor";
    config.Width = 1280;
    config.Height = 720;

    App::Run(&config);
}
