#include <ic.h>

using namespace IC;

int main()
{
    Config config;
    config.name = "ICEditor";
    config.width = 1280;
    config.height = 720;

    IC_APP_INFO("Editor Booting...");

    App::run(&config);
}
