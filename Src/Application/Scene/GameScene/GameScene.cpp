#include "GameScene.h"
#include"../SceneManager.h"
#include"../../GameObject/Character/Player/Player.h"
#include"../../Component/Transform/TransformComponent.h"
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"
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

	std::shared_ptr<TPSCamera> camera = std::make_shared<TPSCamera>();
	camera->Init();
	camera->SetTarget(player);
	m_objList.push_back(camera);

}

void GameScene::OnGui()
{

		// -------- Hierarchy --------
		if (ImGui::Begin("Hierarchy"))
		{
			if (ImGui::TreeNode("Game Object List"))
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



	//if( ImGui::Button("Add Comprnent") ) // プレイヤー追加ボタン
	//{

	//}


	static int selectedIndex = 0;
	std::vector<std::string> objectNames;
	// m_objList からオブジェクト名取得
	for (const auto& obj : m_objList)
	{
		if (obj) objectNames.push_back(obj->GetObjectName());
	}

	//int value = 5;
	////Button以外でもできるけど何でできるかは知らん
	//ImGui::Button("DragArea");
	////Dragできる場所
	//if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	//{
	//	//pathと変数を設定
	//	ImGui::SetDragDropPayload("TNTN", &value, sizeof(value));    //Drag中にText表示           
	//	ImGui::Text("nowDrag!!!");                ImGui::EndDragDropSource();
	//}
	//
	////Dropされる場所
	//ImGui::Text("DropArea");
	//if (ImGui::BeginDragDropTarget())
	//{
	//	//Dragされた情報を取得&Path確認
	//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TARGETOBJECT"))  //やりたい処理を描く payload->Dataでvoid*で格納されてる変数にアクセス可能
	//		int num = (*(int*)payload->Data);
	//	ImGui::EndDragDropTarget();
	//}


	//ImGui::Begin("GameScene Debug");
	//ImGui::Text("This is GameScene OnImGui!");
	//BaseScene::OnGui();
	//ImGui::End();
}
