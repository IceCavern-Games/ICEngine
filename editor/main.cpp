#include <ic.h>

using namespace IC;

int main() {
    Config config;
    config.name = "ICEditor";
    config.width = 1600;
    config.height = 900;

    App::Run(&config);
}
