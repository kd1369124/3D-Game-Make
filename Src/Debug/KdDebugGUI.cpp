#include "../../../Application/main.h"
#include "KdDebugGUI.h"
#include"../../../Application/Scene/SceneManager.h"
#include"../../../Application/Scene/BaseScene/BaseScene.h"
#include"../../../Application/GameObject/Character/CharacterBase.h"
#include"../../../Application/GameObject/Character/Player/Player.h"
KdDebugGUI::KdDebugGUI()
{}
KdDebugGUI::~KdDebugGUI()
{ 
	GuiRelease(); 
}

void KdDebugGUI::GuiInit()
{
	// 初期化済みなら動作させない
	if (m_uqLog) return;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	// Setup Dear ImGui style
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();
	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(Application::Instance().GetWindowHandle());
	ImGui_ImplDX11_Init(
		KdDirect3D::Instance().WorkDev(), KdDirect3D::Instance().WorkDevContext());

#include "imgui/ja_glyph_ranges.h"
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	// 日本語対応
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);

	m_uqLog = std::make_unique<ImGuiAppLog>();
}

void KdDebugGUI::GuiProcess()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	//===========================================================
	// ImGui開始
	//===========================================================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//===========================================================
	// 以下にImGui描画処理を記述
	//===========================================================

	// デバッグウィンドウ(日本語を表示したい場合はこう書く)
//	if (ImGui::Begin(U8("えふぴぃえす")))
//	{
		// FPS
//		ImGui::Text("FPS : %d", Application::Instance().GetNowFPS());
//	}
//	ImGui::End();

	// ログウィンドウ
	//m_uqLog->Draw("Log Window");

	if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_FittingPolicyScroll))
	{
		// -------- Debug タブ --------
		if (ImGui::BeginTabItem("Debug"))
		{
			ImGui::Text("MainFPS: %d", Application::Instance().GetNowFPS());
			ImGui::Text("SubFPS : %d", Application::Instance().GetSubThreadFPS());
			ImGui::Text("DeltaTime: %.4f", Application::Instance().GetDeltaTime());

			static int currentScene = 0;
			const char* scenes[] = { "TitleScene", "GameScene", "ResultScene" };
			if (ImGui::Combo("Scene", &currentScene, scenes, IM_ARRAYSIZE(scenes)))
			{
				// 切り替えを実行
				SceneManager::SceneType nextScene = static_cast<SceneManager::SceneType>(currentScene);
				SceneManager::Instance().SetNextScene(nextScene);
				auto a = SceneManager::Instance().GetNowScene();

			}
			if( ImGui::Button("Entity Create") )
			{
				if (ImGui::Begin(U8("せいせいめにゅー")))
				{
					m_debugType = DebugType::CreateEntity;
				}
				ImGui::End();
			}
			

			ImGui::EndTabItem();
		}
		
		// -------- 各シーンタブ --------
		if (SceneManager::Instance().GetNowScene())
		{
			if (ImGui::BeginTabItem(SceneManager::Instance().GetNowScene()->GetSceneName().c_str()))
			{
				std::weak_ptr<BaseScene> sceneWp = SceneManager::Instance().GetNowScene();
				if (auto sceneSp = sceneWp.lock())
				{
					sceneSp->OnGui();
				}
				ImGui::EndTabItem();
			}
		}
		// -------- AnotherScene タブ --------
		if (ImGui::BeginTabItem("AnotherScene"))
		{
			ImGui::Text(U8("座標系タブ"));

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	if(m_debugType == DebugType::CreateEntity)
	{
		if (ImGui::Begin(U8("せいせいめにゅー")))
		{
			ImGui::Text("Create Entity Menu");
			ImGui::Text("Entity Name: %s");
			//ImGui::InputText("Entity Name",);
			static int Entity = 0;
			const char* Object[] = { "Player", "Enemy", "Enemy01" };
			if (ImGui::Combo(U8("生成するオブジェクトを選択") ,&Entity, Object, IM_ARRAYSIZE(Object)))
			{
				
			}
			if (ImGui::Button("Create"))
			{
				switch (Entity)
				{
				case 0: // Player
				{
					std::shared_ptr<Player>	obj = std::make_shared<Player>();
					SceneManager::Instance().GetNowScene()->AddObject(obj);
				}
				break;
				case 1: // Enemy
				{}
				break;
				case 2: // Enemy01
				{}
				break;
				// 他のオブジェクトを追加する場合はここにcaseを追加
				default:
					break;
				}
			}


			if( ImGui::Button("Close"))
			{
				m_debugType = DebugType::None;
			}

			ImGui::End();
		}
		else
		{
			m_debugType = DebugType::None;
		}
	}

	ImGui::Spacing();

	//=====================================================
	// ログ出力 ・・・ AddLog("～") で追加
	//=====================================================

//	m_uqLog->AddLog("hello world\n");

	//=====================================================
	// 別ソースファイルからログを出力する場合
	//=====================================================

//	KdDebugGUI::Instance().AddLog("TestLog\n");

	//===========================================================
	// ここより上にImGuiの描画はする事
	//===========================================================
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void KdDebugGUI::AddLog(const char* fmt,...)
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	va_list args;
	va_start(args, fmt);
	m_uqLog->AddLog(fmt);
	va_end(args);
}

void KdDebugGUI::GuiRelease()
{
	// 初期化されてないなら動作させない
	if (!m_uqLog) return;

	m_uqLog = nullptr;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
