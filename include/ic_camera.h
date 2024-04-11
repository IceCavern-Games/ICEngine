#pragma once

#include <glm/glm.hpp>

namespace IC
{
    class ICCamera
    {
    private:
        glm::vec3 _lookVector;
        glm::vec3 _position;

    public:
        ICCamera();
        ~ICCamera();

        glm::vec3 GetLookVector() { return _lookVector; }
        glm::vec3 GetPosition() { return _position; }
    };
}
