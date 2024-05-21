#pragma once

#include "ic_components.h"

#include <memory>
#include <vector>

namespace IC {
    class GameObject {
    public:
        GameObject(std::string name);
        ~GameObject();

        std::vector<std::shared_ptr<GameObjectComponent>> &Components() { return _components; }
        std::shared_ptr<Transform> &GetTransform() { return _transform; }
        template <typename T> void AddComponent(const T &component) {
            _components.push_back(std::make_shared<T>(component));
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
        std::shared_ptr<Transform> _transform;
        std::vector<std::shared_ptr<GameObjectComponent>> _components;
    };
} // namespace IC