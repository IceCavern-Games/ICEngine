#pragma once

#include <glm/glm.hpp>

namespace IC
{
    class ICCamera
    {
    public:
        ICCamera();
        ~ICCamera();

        glm::vec3 get_position() { return position; }
        glm::vec3 get_look_vector() { return lookVector; }

    private:
        glm::vec3 position;
        glm::vec3 lookVector;
    };
}