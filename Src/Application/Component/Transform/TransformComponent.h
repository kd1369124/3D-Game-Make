#pragma once
#include "../Component.h"

class TransformComponent : public Component {
public:
	Math::Vector3 m_pos = { 0, 0, 0 };
	Math::Vector3 m_rot = { 0, 0, 0 };
	Math::Vector3 m_scale = { 1, 1, 1 };

	bool m_useGizmo = true;

	int m_operation = 0; // 0: Translate, 1: Rotate, 2: Scale
	bool m_local = true;

	void Update(float dt) override;

	void OnImGui() override;

	std::string GetComponentName() const override { return "TransformComponent"; }

	void DrawGizmo(const float* view, const float* proj);
};
