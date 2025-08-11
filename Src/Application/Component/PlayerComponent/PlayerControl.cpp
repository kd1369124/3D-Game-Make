#include "PlayerControl.h"
#include "../../../Framework/GameObject/KdGameObject.h"
#include "../Transform/TransformComponent.h"
#include"../Component.h"
void PlayerControl::Update(float dt)
{
	if (auto owner = m_owner.lock()) {
		auto transform = owner->GetComponent<TransformComponent>();
		if (transform) {
			if (GetAsyncKeyState('W') & 0x8000) 
			{
				transform->m_pos.z += 5.0f * dt;
			}
			if (GetAsyncKeyState('S') & 0x8000) 
			{
				transform->m_pos.z -= 5.0f * dt;
			}
			if (GetAsyncKeyState('A') & 0x8000) 
			{
				transform->m_pos.x -= 5.0f * dt;
			}
			if (GetAsyncKeyState('D') & 0x8000) 
			{
				transform->m_pos.x += 5.0f * dt;
			}
			if (GetAsyncKeyState(VK_SPACE) & 0x8000)
			{

			}
		}
	}
}

void PlayerControl::OnImGui()
{
	//if (ImGui::CollapsingHeader("PlayerControl Component"))
	{
		ImGui::Text("WASD move enabled");
	}
	
}
