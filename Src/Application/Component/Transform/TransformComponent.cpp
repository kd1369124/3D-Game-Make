#include "TransformComponent.h"
#include "../../../Framework/GameObject/KdGameObject.h"
#include"../../../../../Library/imgui/imgui.h"
#include"../../../../../Library/imgui/ImGuizmo.h"

void TransformComponent::Update(float dt)
{
	if (auto owner = m_owner.lock()) {
		owner->m_mWorld =
			Math::Matrix::CreateScale(m_scale) *
			Math::Matrix::CreateFromYawPitchRoll(m_rot.y, m_rot.x, m_rot.z) *
			Math::Matrix::CreateTranslation(m_pos);
	}
}

void TransformComponent::OnImGui()
{

	//if (ImGui::CollapsingHeader("Transform Component"))
	{

		float pos[3] = { m_pos.x, m_pos.y, m_pos.z };
		if (ImGui::DragFloat3("Position", pos, 0.1f)) {
			m_pos = { pos[0], pos[1], pos[2] };
		}

		float rot[3] = { m_rot.x, m_rot.y, m_rot.z };
		if (ImGui::DragFloat3("Rotation", rot, 0.1f)) {
			m_rot = { rot[0], rot[1], rot[2] };
		}

		float scale[3] = { m_scale.x, m_scale.y, m_scale.z };
		if (ImGui::DragFloat3("Scale", scale, 0.1f)) {
			m_scale = { scale[0], scale[1], scale[2] };
		}

		ImGui::Checkbox("Use Gizmo", &m_useGizmo);

		if (m_useGizmo) {
			const char* ops[] = { "Translate", "Rotate", "Scale" };
			ImGui::Combo("Operation", &m_operation, ops, IM_ARRAYSIZE(ops));
			ImGui::Checkbox("Local Space", &m_local);
		}
	}

}

void TransformComponent::DrawGizmo(const float* view, const float* proj)
{
	if (!m_useGizmo) return;

	if (auto owner = m_owner.lock()) {
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		Math::Matrix objMat =
			Math::Matrix::CreateScale(m_scale) *
			Math::Matrix::CreateFromYawPitchRoll(m_rot.y, m_rot.x, m_rot.z) *
			Math::Matrix::CreateTranslation(m_pos);

		float obj[16];
		memcpy(obj, &objMat, sizeof(float) * 16);

		ImGuizmo::Manipulate(
			view, proj,
			(ImGuizmo::OPERATION)m_operation,
			m_local ? ImGuizmo::LOCAL : ImGuizmo::WORLD,
			obj
		);

		if (ImGuizmo::IsUsing()) {
			Math::Vector3 trans, scale;
			Math::Quaternion rot;
			Math::Matrix newMat;
			memcpy(&newMat, obj, sizeof(float) * 16);
			newMat.Decompose(scale, rot, trans);

			m_pos = trans;
			m_scale = scale;

			Math::Vector3 rotEuler;
			rotEuler.x = asinf(-2 * (rot.y * rot.z - rot.w * rot.x));
			rotEuler.y = atan2f(2 * (rot.x * rot.z + rot.w * rot.y),
				rot.w * rot.w - rot.x * rot.x - rot.y * rot.y + rot.z * rot.z);
			rotEuler.z = atan2f(2 * (rot.x * rot.y + rot.w * rot.z),
				rot.w * rot.w - rot.x * rot.x + rot.y * rot.y - rot.z * rot.z);

			m_rot = rotEuler;
		}
	}
}
