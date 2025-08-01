﻿#include "SceneManager.h"

#include "BaseScene/BaseScene.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"

void SceneManager::PreUpdate()
{
	// シーン切替
	if (m_currentSceneType != m_nextSceneType)
	{
		ChangeScene(m_nextSceneType);
	}

	m_currentScene->PreUpdate();
}

void SceneManager::Update()
{

		m_currentScene->Update();
	
	
}

void SceneManager::PostUpdate()
{

		m_currentScene->PostUpdate();
	

}

void SceneManager::PreDraw()
{
	m_currentScene->PreDraw();
}

void SceneManager::Draw()
{
	m_currentScene->Draw();
}

void SceneManager::DrawSprite()
{
	m_currentScene->DrawSprite();
}

void SceneManager::DrawDebug()
{
	m_currentScene->DrawDebug();
}
std::shared_ptr<BaseScene> SceneManager::CreateScene(SceneType type)
{
	switch (type)
	{
	case SceneType::GameScene:
		return std::make_shared<GameScene>();
	case SceneType::TitleScene:
		return std::make_shared<TitleScene>();
		// 必要に応じて追加
	default:
		return nullptr;
	}
}
const std::list<std::shared_ptr<KdGameObject>>& SceneManager::GetObjList()
{
	return m_currentScene->GetObjList();
}

void SceneManager::AddObject(const std::shared_ptr<KdGameObject>& _obj)
{
	m_currentScene->AddObject(_obj);
}


void SceneManager::ChangeScene(SceneType _sceneType)
{

	// 次のシーンを作成し、現在のシーンにする
	switch (_sceneType)
	{
	case SceneType::TitleScene:
		m_currentScene = std::make_shared<TitleScene>();
		break;
	case SceneType::GameScene:
		m_currentScene = std::make_shared<GameScene>();
		break;
	}

	// 現在のシーン情報を更新
	m_currentSceneType = _sceneType;


}
