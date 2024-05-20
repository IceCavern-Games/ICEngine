#pragma once

#include "ic_components.h"

#include <memory>
#include <vector>

namespace IC {
    class GameObject {
    public:
        GameObject(std::string name);
        ~GameObject();

        std::vector<std::unique_ptr<GameObjectComponent>> &Components() { return _components; }
        std::unique_ptr<Transform> &GetTransform() { return _transform; }
        template <typename T> void AddComponent(const T &component) {
            _components.push_back(std::make_unique<T>(component));
        }
        template <typename T> GameObjectComponent *GetComponent() {
            for (auto component : _components) {
                if (typeid(*component) == typeid(T)) {
                    return component.get();
                }
            }
        }

        void Gui();

    private:
        std::string _name;
        std::unique_ptr<Transform> _transform;
        std::vector<std::unique_ptr<GameObjectComponent>> _components;
    };
} // namespace IC