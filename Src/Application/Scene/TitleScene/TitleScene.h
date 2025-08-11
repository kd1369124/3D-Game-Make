#pragma once

#include"../BaseScene/BaseScene.h"

class TitleScene : public BaseScene
{
public :

	TitleScene()  { Init(); }
	~TitleScene() {}

	std::string GetSceneName()	override { return "TitleScene"; }

private :

	void Event() override;
	void Init()  override;
	void OnGui() override;
	std::weak_ptr<KdGameObject> m_selected;   // 現在選択中
};
