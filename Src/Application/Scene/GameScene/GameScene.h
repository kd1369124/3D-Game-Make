#pragma once

#include"../BaseScene/BaseScene.h"

class GameScene : public BaseScene
{
public :

	GameScene()  { Init(); }
	~GameScene() {}

	std::string GetSceneName()	override { return "GameScene"; }

private:

	void Event() override;
	void Init()  override;
	void OnGui() override;

};
