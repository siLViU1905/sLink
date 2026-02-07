#ifndef SLINK_UI_LAYER_H
#define SLINK_UI_LAYER_H

#include <vector>
#include <memory>
#include <components/UIComponent.h>

namespace sLink::ui::layer
{
    class UILayer
    {
    public:
        void render() const;

        void addComponent(std::unique_ptr<component::UIComponent> component);
    private:
        std::vector<std::unique_ptr<component::UIComponent>> m_Components;
    };
}
#endif
