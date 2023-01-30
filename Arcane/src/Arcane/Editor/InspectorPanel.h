#pragma once

#include <Arcane/Scene/Scene.h>
#include <Arcane/Scene/Entity.h>

namespace Arcane
{
	class InspectorPanel
	{
	public:
		InspectorPanel();

		void OnImGuiRender();

		inline void SetFocusedEntity(Entity entity) { m_FocusedEntity = entity; }
	private:
		bool DrawVec3Control(const std::string &label, glm::vec3 &values, float speed = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f);
	private:
		Entity m_FocusedEntity;
	};
}
