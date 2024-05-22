#pragma once

#include "ic_components.h"

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <vector>

namespace IC {
    class GameObject {
    public:
        GameObject(std::string name);
        ~GameObject();

        std::shared_ptr<Transform> &GetTransform() { return _transform; }
        template <typename T> bool HasComponent() { return _componentMap.contains(typeid(T)); }

        template <typename T> std::shared_ptr<T> AddComponent() {
            // return component if game object already has one attached
            if (HasComponent<T>()) {
                return static_pointer_cast<T>(_componentMap.at(typeid(T)));
            }

            // otherwise create new component of type T
            _componentMap[typeid(T)] = std::make_shared<T>();
            return static_pointer_cast<T>(_componentMap[typeid(T)]);
        }

        template <typename T> std::shared_ptr<T> GetComponent() {
            if (HasComponent<T>()) {
                return static_pointer_cast<T>(_componentMap.at(typeid(T)));
            } else {
                return nullptr;
            }
        }

        void Gui();

    private:
        std::string _name;
        std::shared_ptr<Transform> _transform;
        std::map<std::type_index, std::shared_ptr<Component>> _componentMap;
    };
} // namespace IC