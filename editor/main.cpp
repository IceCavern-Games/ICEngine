#include <ic.h>

class DummyLayer : public IC::Layer {
public:
    DummyLayer() : IC::Layer("Dummy") {}

    void OnAttach() { IC_APP_INFO("Dummy Layer attached"); }

    void OnDetach() { IC_APP_INFO("Dummy Layer detached"); }

    void OnEvent(IC::Event &e) {
        IC::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<IC::KeyPressedEvent>([this](IC::KeyPressedEvent &e) { return OnKeyPressed(e); });
    }

    bool OnKeyPressed(IC::KeyPressedEvent &e) {
        IC_APP_INFO("Keycode {0} was pressed!", (int)e.GetKeyCode());

        return false;
    }
};

class Editor : public IC::App {
public:
    Editor(const IC::Config &config) : IC::App(config) { PushLayer(new DummyLayer()); }
    ~Editor() {}
};

int main() {
    IC::Config config;
    config.name = "ICEditor";

    auto app = new Editor(config);
    app->Run();
    delete app;
}
