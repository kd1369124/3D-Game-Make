#include "TitleScene.h"
#include "../SceneManager.h"
#include"../../Component/Transform/TransformComponent.h"
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

	// -------- Hierarchy --------
	if (ImGui::Begin("Hierarchy"))
	{
		if (ImGui::TreeNode("Title Object List"))
		{
			for (auto& obj : m_objList)
			{
				if (!obj) continue;
				ImGui::PushID(obj.get());

				ImGuiTreeNodeFlags objFlags =
					ImGuiTreeNodeFlags_OpenOnArrow |
					ImGuiTreeNodeFlags_SpanAvailWidth;
				// 選択中はハイライト
				if (!m_selected.expired() && m_selected.lock().get() == obj.get())
					objFlags |= ImGuiTreeNodeFlags_Selected;

				bool open = ImGui::TreeNodeEx(obj->GetObjectName().c_str(), objFlags);

				// 行クリックで選択
				if (ImGui::IsItemClicked())
					m_selected = obj;

				if (open)
				{
					// ---- コンポーネント一覧（中身はInspectorで編集するので見出しだけでもOK）----
					for (auto& comp : obj->GetComponents())
					{
						if (!comp) continue;
						ImGui::PushID(comp.get());

						std::string label = comp->GetComponentName() + "##node";
						ImGuiTreeNodeFlags compFlags =
							ImGuiTreeNodeFlags_OpenOnArrow |
							ImGuiTreeNodeFlags_SpanAvailWidth |
							ImGuiTreeNodeFlags_Leaf;

						ImGui::TreeNodeEx(label.c_str(), compFlags);
						ImGui::TreePop();

						ImGui::PopID();
					}
					ImGui::TreePop();
				}

				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		// 右上に “Prefab” タブを並べたいなら、ここで BeginTabBar を併設してもOK
	}
	ImGui::End();

	// -------- Inspector --------
	if (ImGui::Begin("Inspector"))
	{
		if (auto sp = m_selected.lock())
		{
			// Transformなど共通ヘッダ
			if (auto trans = sp->GetComponent<TransformComponent>())
			{
				ImGui::PushID(trans.get());
				if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
				{
					trans->OnImGui();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}

			// その他コンポーネント
			for (auto& comp : sp->GetComponents())
			{
				if (!comp) continue;
				if (std::dynamic_pointer_cast<TransformComponent>(comp)) continue; // さっき描いたのでスキップ

				ImGui::PushID(comp.get());
				std::string header = comp->GetComponentName() + "##ins";
				if (ImGui::TreeNodeEx(header.c_str(), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
				{
					comp->OnImGui();
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
		}
		else
		{
			ImGui::TextUnformatted("NOT SELECT");
		}
	}
	ImGui::End();
}
