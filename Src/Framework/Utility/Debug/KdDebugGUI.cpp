#include "../../../Application/main.h"
#include "KdDebugGUI.h"
#include"../../../Application/Scene/SceneManager.h"
#include"../../../Application/Scene/BaseScene/BaseScene.h"
#include"../../../Application/GameObject/Character/CharacterBase.h"
#include"../../../Application/GameObject/Character/Player/Player.h"
void KdDebugGUI::DrawEditorPanels()
{
	static bool showEntityWindow = false;

	if (ImGui::Begin("Debug Info"))
	{
		ImGui::Text("MainFPS: %d", Application::Instance().GetNowFPS());
		ImGui::Text("SubFPS : %d", Application::Instance().GetSubThreadFPS());
		ImGui::Text("DeltaTime: %.5f", Application::Instance().GetDeltaTime());

		static int currentScene = 0;
		const char* scenes[] = { "TitleScene", "GameScene", "ResultScene" };
		if (ImGui::Combo("Scene", &currentScene, scenes, IM_ARRAYSIZE(scenes)))
		{
			auto next = static_cast<SceneManager::SceneType>(currentScene);
			SceneManager::Instance().SetNextScene(next);
		}

		if (ImGui::Button("Open: Entity Create"))
		{
			m_debugType = DebugType::CreateEntity;
			showEntityWindow = true;
		}
	}
	ImGui::End();

	// 現在シーン用パネル
	if (auto nowScene = SceneManager::Instance().GetNowScene())
	{
		// 例: "GameScene Debug" など現在のシーン名＋" Debug" をタイトルにする
		//std::string sceneWinTitle = nowScene->GetSceneName() + " Debug";
		//if (ImGui::Begin(sceneWinTitle.c_str()))
		//{
			nowScene->OnGui();
		/*}
		ImGui::End();*/
	}



	// エンティティ生成（独立ウィンドウ）
	if (m_debugType == DebugType::CreateEntity && showEntityWindow)
	{
		if (ImGui::Begin("Entity Create Menu", &showEntityWindow))
		{
			ImGui::Text("Create Entity Menu");

			static int entityType = 0;
			const char* objectList[] = { "Player", "Enemy", "Enemy01" };
			ImGui::Combo("生成するオブジェクトを選択", &entityType, objectList, IM_ARRAYSIZE(objectList));

			if (ImGui::Button("Create"))
			{
				switch (entityType)
				{
				case 0: // Player
				{
					auto obj = std::make_shared<Player>();
					SceneManager::Instance().GetNowScene()->AddObject(obj);
				} break;
				// TODO: 他タイプを追加
				}
			}

			if (ImGui::Button("Close"))
			{
				m_debugType = DebugType::None;
				showEntityWindow = false;
			}
		}
		ImGui::End();
	}

	if (ImGui::Begin("GameScreen"))
	{
		if (m_gameCopySRV)
		{
			// 利用可能な領域にフィットさせる（アスペクト維持＆レターボックス）
			ImVec2 avail = ImGui::GetContentRegionAvail();
			float availW = ImMax(1.0f, avail.x);
			float availH = ImMax(1.0f, avail.y);
			float texW = (float)m_gameCopyW, texH = (float)m_gameCopyH;
			float scale = ImMin(availW / texW, availH / texH);
			ImVec2 size(texW * scale, texH * scale);

			// 中央寄せ
			ImVec2 cursor = ImGui::GetCursorPos();
			ImGui::SetCursorPos(ImVec2(cursor.x + (availW - size.x) * 0.5f,
				cursor.y + (availH - size.y) * 0.5f));

			ImGui::Image((ImTextureID)m_gameCopySRV.Get(), size);
		}
		else
		{
			ImGui::TextUnformatted("No Frame Yet");
		}
	}
	ImGui::End();


}
void KdDebugGUI::ShowMainDockspace()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

	ImGuiWindowFlags hostFlags =
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | 
		ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;            

	ImGui::Begin("###HostDockspace", nullptr, hostFlags);

	ImGuiID dockspace_id = ImGui::GetID("MainDockspace");
	ImGuiDockNodeFlags dockFlags =
		ImGuiDockNodeFlags_PassthruCentralNode |  // ← 中央をクリック透過
		ImGuiDockNodeFlags_AutoHideTabBar;

	ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockFlags);

	static bool built = false;
	if (!built) { built = true; BuildDefaultLayout(dockspace_id); }

	ImGui::End();
}
void KdDebugGUI::BuildDefaultLayout(ImGuiID id)
{
	ImGui::DockBuilderRemoveNode(id);
	ImGui::DockBuilderAddNode(id, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(id, ImGui::GetMainViewport()->Size);

	// 大枠：中央(main) / 右(22%) / 下(22%)
	ImGuiID dock_main = id;
	ImGuiID dock_right = 0;
	ImGuiID dock_down = 0;
	ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.22f, &dock_right, &dock_main);
	ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Down,  0.22f, &dock_down,  &dock_main);

	// 右を上下分割：上=ヒエラルキー/プレハブ(タブ)、下=インスペクタ
	ImGuiID dock_right_top = 0, dock_right_bottom = 0;
	ImGui::DockBuilderSplitNode(dock_right, ImGuiDir_Down, 0.55f, &dock_right_bottom, &dock_right_top);

	// 下段を4分割：Scene / ObjectManager / Info / CameraControl 風
	ImGuiID d1 = 0, d2 = 0, d3 = 0, d4 = 0;
	ImGui::DockBuilderSplitNode(dock_down, ImGuiDir_Right, 0.25f, &d1, &dock_down);
	ImGui::DockBuilderSplitNode(dock_down, ImGuiDir_Right, 0.33f, &d2, &dock_down);
	ImGui::DockBuilderSplitNode(dock_down, ImGuiDir_Right, 0.50f, &d3, &d4);

	// ここに割り当てるウィンドウ名は Begin() のタイトルと一致させること
	ImGui::DockBuilderDockWindow("GameScreen",			dock_main);
	ImGui::DockBuilderDockWindow("Hierarchy",			dock_right_top);
	ImGui::DockBuilderDockWindow("Prefab",				dock_right_top);   // 同ノード=タブ化
	ImGui::DockBuilderDockWindow("Inspector",			dock_right_bottom);
	ImGui::DockBuilderDockWindow("GameScene Debug",		dock_right_top);
	ImGui::DockBuilderDockWindow("TitleScene Debug",	dock_right_top);
	ImGui::DockBuilderDockWindow("Add Comprnent",	    dock_right_top);

	ImGui::DockBuilderDockWindow("Scene",					d1);
	ImGui::DockBuilderDockWindow("ObjectManager",			d2);
	ImGui::DockBuilderDockWindow("Info",					d3);
	ImGui::DockBuilderDockWindow("CameraControl",			d4);
	ImGui::DockBuilderDockWindow("Debug Info",				d1);
	ImGui::DockBuilderDockWindow("AnotherScene",            d2);
	ImGui::DockBuilderDockWindow("Entity Create Menu",      d3);

	// 既存のウィンドウも入れる（ログ、デバッグなど）
	//ImGui::DockBuilderDockWindow("Log Window", d3);
	//ImGui::DockBuilderDockWindow("Debug", d1);

	ImGui::DockBuilderFinish(id);
}
KdDebugGUI::KdDebugGUI()
{}
KdDebugGUI::~KdDebugGUI()
{ 
	GuiRelease(); 
}

void KdDebugGUI::ToggleEditor()
{
	m_showEditor = !m_showEditor;
}

void KdDebugGUI::SetEditorVisible(bool v)
{
	m_showEditor = v;
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

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    

	
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
 

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
	static bool showEntityWindow = false;


if (GetAsyncKeyState(VK_F1) & 1) ToggleEditor();

if (m_showEditor)
{
	ShowMainDockspace();
	DrawEditorPanels();
}

	

	//ImGui::Spacing();

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

	// Viewport 有効時は必ずこれを呼ぶ（別ウィンドウの更新＆描画）
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		KdDirect3D::Instance().WorkDevContext()                                                
			->OMSetRenderTargets(1,
				KdDirect3D::Instance().GetBackBuffer()->WorkRTViewAddress(),
				KdDirect3D::Instance().GetZBuffer()->WorkDSView()
			);
	}

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

void KdDebugGUI::CaptureGameViewFromBackbuffer()
{
	auto* dev = KdDirect3D::Instance().WorkDev();
	auto* ctx = KdDirect3D::Instance().WorkDevContext();
	auto* swap = KdDirect3D::Instance().WorkSwapChain();

	// バックバッファ取得
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuf;
	if (FAILED(swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuf.GetAddressOf()))) return;

	// サイズ・フォーマット
	D3D11_TEXTURE2D_DESC bbDesc{};
	backBuf->GetDesc(&bbDesc);

	// コピー先を用意
	bool needRecreate = (!m_gameCopyTex) || (m_gameCopyW != (int)bbDesc.Width) || (m_gameCopyH != (int)bbDesc.Height);
	if (needRecreate) {
		m_gameCopyTex.Reset();
		m_gameCopySRV.Reset();

		D3D11_TEXTURE2D_DESC td{};
		td.Width = bbDesc.Width;
		td.Height = bbDesc.Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = bbDesc.Format; 
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		if (FAILED(dev->CreateTexture2D(&td, nullptr, m_gameCopyTex.GetAddressOf()))) return;
		if (FAILED(dev->CreateShaderResourceView(m_gameCopyTex.Get(), nullptr, m_gameCopySRV.GetAddressOf()))) return;

		m_gameCopyW = (int)td.Width;
		m_gameCopyH = (int)td.Height;
	}

	// バックバッファ→コピー先へコピー
	ctx->CopyResource(m_gameCopyTex.Get(), backBuf.Get());
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
