#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Character/Player/Player.h"
#include"../../Component/Transform/TransformComponent.h"
void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::TitleScene
		);
	}
}

void GameScene::Init()
{
	std::shared_ptr<Player> player = std::make_shared<Player>();
	player->Init();
	m_objList.push_back(player);
}

void GameScene::OnGui()
{
	// ✅ 絶対に BeginTabBar は呼ばない
	// ✅ 必要なら BeginChild はOK

	// 例: シーン固有のデバッグ情報
	ImGui::Text("=== GameScene Debug ===");

	if (ImGui::TreeNode("Game Object List"))
	{
		for (auto& obj : m_objList)
		{
			if (!obj) continue;

			if (ImGui::TreeNode(obj->GetObjectName().c_str()))
			{
				for (auto& comp : obj->GetComponents())
				{
					if (!comp) continue;

					if (ImGui::TreeNode("%s", comp->GetComponentName().c_str()))
					{

						comp->OnImGui(); // コンポーネントの OnImGui を呼ぶ
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
	if( ImGui::Button("Add Comprnent") ) // プレイヤー追加ボタン
	{

	}
	
	// 例: 選択オブジェクトの詳細
	//if (!m_selectedObj.expired())
	//{
	//	if (auto selected = m_selectedObj.lock())
	//	{
	//		ImGui::Separator();
	//		ImGui::Text("Selected Object: %s", selected->GetObjectName().c_str());
	//		selected->OnImGui(); // コンポーネント側の OnImGui を呼ぶ
	//	}
	//}

	//ImGui::Begin("GameScene Debug");
	//ImGui::Text("This is GameScene OnImGui!");
	//BaseScene::OnGui();
	//ImGui::End();
}
