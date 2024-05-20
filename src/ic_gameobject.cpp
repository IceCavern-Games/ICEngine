#include <ic_gameobject.h>

namespace IC {
    GameObject::GameObject(std::string name) : _name{name} {
        _transform = std::make_unique<Transform>();
    }
    GameObject::~GameObject() {}

    void GameObject::Gui() {
        ImGui::Begin(_name.c_str());
        for (auto &component : _components) {
            component->Gui();
        }
        ImGui::End();
    }

} // namespace IC