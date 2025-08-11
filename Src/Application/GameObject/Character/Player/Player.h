#pragma once
#include"../CharacterBase.h"
class Player : public CharacterBase
{
public:
	Player(){}
	~Player() override {}

	void Init() override;

	void Update() override;

	void DrawLit() override;

	std::string GetObjectName() const override
	{
		return "Player";
	}

private:

};
