#include <ic.h>

using namespace IC;

int main()
{
    Config config;
    config.name = "ICEngine Test App";
    config.width = 1280;
    config.height = 720;

    App::run(&config);
}