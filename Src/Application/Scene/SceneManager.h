#pragma once

class BaseScene;

class SceneManager
{
public :

	// シーン情報
	enum class SceneType
	{
		TitleScene,
		GameScene,
	};

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreDraw();
	void Draw();
	void DrawSprite();
	void DrawDebug();

	// 次のシーンをセット (次のフレームから切り替わる)
	//void SetNextScene(SceneType _nextScene)
	//{
	//	// すでに同じシーンなら何もしない
	//	if (_nextScene == m_currentSceneType) return;

	//	m_nextSceneType = _nextScene;
	//	m_isSceneChanging = true;

	//	if (m_jobSystem)
	//	{
	//		// スレッドセーフに job を投げる
	//		m_jobSystem->PushJob([this, nextScene = _nextScene]()
	//			{
	//				// 新しいシーンを生成
	//				auto newScene = CreateScene(nextScene);

	//				// シーン切替バッファに格納（スレッドセーフに）
	//				{
	//					std::lock_guard<std::mutex> lock(m_sceneMutex);
	//					m_sceneSwapBuffer = newScene;
	//					m_currentSceneType = nextScene;
	//					m_sceneChangeRequested = true;
	//				}
	//			});
	//	}
	//	else
	//	{
	//		// 同期処理（JobSystemなし時の保険）
	//		auto newScene = CreateScene(_nextScene);
	//		{
	//			std::lock_guard<std::mutex> lock(m_sceneMutex);
	//			m_sceneSwapBuffer = newScene;
	//			m_currentSceneType = _nextScene;
	//			m_sceneChangeRequested = true;
	//		}
	//	}
	//}

	void SetNextScene(SceneType _nextScene)
	{
		m_nextSceneType = _nextScene;
	}

	// 現在のシーンのオブジェクトリストを取得
	const std::list<std::shared_ptr<KdGameObject>>& GetObjList();

	// 現在のシーンにオブジェクトを追加
	void AddObject(const std::shared_ptr<KdGameObject>& _obj);

	std::shared_ptr<BaseScene> GetNowScene() const { return m_currentScene; }

	//std::mutex m_sceneMutex;

	//std::unique_ptr<JobSystem> m_jobSystem;


private :

	// マネージャーの初期化
	// インスタンス生成(アプリ起動)時にコンストラクタで自動実行
	void Init()
	{
		// 開始シーンに切り替え

		//m_currentScene = CreateScene(m_currentSceneType);

		ChangeScene(m_currentSceneType);
	}

	std::shared_ptr<BaseScene>  CreateScene(SceneType type);

	// シーン切り替え関数
	void ChangeScene(SceneType _sceneType);
	
	// 現在のシーンのインスタンスを保持しているポインタ
	std::shared_ptr<BaseScene> m_currentScene = nullptr;

	// 現在のシーンの種類を保持している変数
	SceneType m_currentSceneType = SceneType::TitleScene;
	
	// 次のシーンの種類を保持している変数
	SceneType m_nextSceneType = m_currentSceneType;

	//bool m_sceneChangeRequested = false;
	//bool m_isSceneChanging = false;
	//bool m_jobSceneCreated = false;
	//std::shared_ptr<BaseScene> m_sceneSwapBuffer = nullptr;

	//// 非同期ジョブ
	//std::future<std::shared_ptr<BaseScene>> m_futureScene;

private:

	SceneManager() { Init(); }
	~SceneManager() {}

public:

	// シングルトンパターン
	// 常に存在する && 必ず1つしか存在しない(1つしか存在出来ない)
	// どこからでもアクセスが可能で便利だが
	// 何でもかんでもシングルトンという思考はNG
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}
};
