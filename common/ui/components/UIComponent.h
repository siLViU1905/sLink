#ifndef SLINK_UI_COMPONENT_H
#define SLINK_UI_COMPONENT_H

namespace sLink::ui::component
{
    class UIComponent
    {
    public:
        virtual void render() = 0;

        virtual ~UIComponent() = default;
    };
}
#endif