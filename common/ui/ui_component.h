#ifndef SLINK_UI_COMPONENT_H
#define SLINK_UI_COMPONENT_H

namespace sLink::ui
{
    class UIComponent
    {
    public:
        virtual void render() const = 0;

        virtual ~UIComponent() = default;
    };
}
#endif
