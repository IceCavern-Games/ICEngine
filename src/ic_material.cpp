#include <ic_material.h>

namespace IC {
    void MaterialTemplate::AddBinding(int bindingIndex, std::string name, BindingType bindingType,
                                      ShaderDataType dataType) {
        ShaderBinding binding{};
        binding.index = bindingIndex;
        binding.name = name;
        binding.bindingType = bindingType;
        binding.dataType = dataType;

        _bindings[bindingIndex] = binding;
    }

    MaterialInstance::MaterialInstance(MaterialTemplate &materialTemplate) : _template{materialTemplate} {
        // initialize shader binding values
        for (auto &[index, binding] : _template.Bindings()) {
            ShaderBindingValue value{.binding = binding};
            _bindingValues[index] = std::make_shared<ShaderBindingValue>(value);
        }
    }

    void MaterialInstance::SetBindingValue(int index, void *value, size_t size) {
        _bindingValues[index]->value = value;
        _bindingValues[index]->size = size;
    }
} // namespace IC