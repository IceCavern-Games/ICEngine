#include <ic_material.h>

namespace IC {
    LitMaterial::LitMaterial() {
        flags = MaterialFlags::Lit;
        vertShaderData = DEFAULT_VERT_SHADER_PATH;
        fragShaderData = DEFAULT_FRAG_SHADER_PATH;

        constants.color = glm::vec4(1.0f);
    }

    UnlitMaterial::UnlitMaterial() {
        flags = MaterialFlags::None;
        vertShaderData = DEFAULT_VERT_SHADER_PATH;
        fragShaderData = DEFAULT_FRAG_SHADER_PATH;

        constants.color = glm::vec4(1.0f);
    }
} // namespace IC