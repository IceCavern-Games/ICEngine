#pragma once

#include "events/ic_event.h"

#include <string>
#include <vector>

namespace IC {
    class Layer {
    public:
        Layer(const std::string &name = "Layer");
        virtual ~Layer();

        virtual void OnAttach() {}
        virtual void OnDetach() {}
        virtual void OnUpdate() {}
        virtual void OnEvent(Event &event) {}

        inline const std::string &GetName() const { return _debugName; }

    protected:
        std::string _debugName;
    };

    class LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        void PushLayer(Layer *layer);
        void PushOverlay(Layer *overlay);
        void PopLayer(Layer *layer);
        void PopOverlay(Layer *layer);

        std::vector<Layer *>::iterator begin() { return _layers.begin(); };
        std::vector<Layer *>::iterator end() { return _layers.end(); };

    private:
        std::vector<Layer *> _layers;
        std::vector<Layer *>::iterator _layerInsertIndex;
    };
} // namespace IC
