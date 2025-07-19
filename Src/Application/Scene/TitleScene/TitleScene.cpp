#include "TitleScene.h"
#include "../SceneManager.h"

void TitleScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::GameScene
		);
	}
}

void TitleScene::Init()
{
}

void TitleScene::OnGui()
{
	ImGui::Text("=== TitleScene Debug ===");

	if (ImGui::TreeNode("Title Object List"))
	{
		for (auto& obj : m_objList)
		{
			if (!obj) continue;

			if (ImGui::TreeNode(obj->GetObjectName().c_str()))
			{
				for (auto& comp : obj->GetComponents())
				{
					if (!comp) continue;

					ImGui::BulletText("%s", comp->GetComponentName().c_str());
					//comp->OnImGui();
				}
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
}
