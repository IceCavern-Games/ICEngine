#include <ic.h>

class Editor : public IC::App {
public:
    Editor(const IC::Config &config) : IC::App(config) {}
    ~Editor() {}
};

int main() {
    IC::Config config;
    config.name = "ICEditor";

    auto app = new Editor(config);
    app->Run();
    delete app;
}
