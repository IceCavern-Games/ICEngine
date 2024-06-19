#include <ic_layer.h>

namespace IC {
    Layer::Layer(const std::string &debugName) : _debugName(debugName) {}

    Layer::~Layer() {}

    LayerStack::LayerStack() {
        _layerInsertIndex = 0;
    }

    LayerStack::~LayerStack() {
        for (Layer *layer : _layers)
            delete layer;
    }

    void LayerStack::PushLayer(Layer *layer) {
        _layers.emplace(_layers.begin() + _layerInsertIndex, layer);
        _layerInsertIndex++;
    }

    void LayerStack::PushOverlay(Layer *overlay) {
        _layers.emplace_back(overlay);
    }

    void LayerStack::PopLayer(Layer *layer) {
        auto it = std::find(_layers.begin(), _layers.end(), layer);

        if (it != _layers.end()) {
            _layers.erase(it);
            _layerInsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Layer *overlay) {
        auto it = std::find(_layers.begin(), _layers.end(), overlay);

        if (it != _layers.end()) {
            _layers.erase(it);
        }
    }
} // namespace IC
