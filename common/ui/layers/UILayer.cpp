#include "UILayer.h"

namespace sLink::ui::layer
{
	void UILayer::render() const
	{
		for (const auto& component : m_Components)
			component->render();
	}

	void UILayer::addComponent(std::unique_ptr<component::UIComponent>&& component)
	{
		m_Components.push_back(std::move(component));
	}
}