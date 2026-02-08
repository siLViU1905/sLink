#ifndef SLINK_UI_LAYER_H
#define SLINK_UI_LAYER_H

namespace sLink::ui::layer
{
    class UILayer
    {
    public:
        virtual void render() = 0;
    
        virtual ~UILayer() = default;
    };
}
#endif
