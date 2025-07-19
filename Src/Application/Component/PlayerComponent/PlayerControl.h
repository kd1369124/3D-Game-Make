#pragma once
#include "../Component.h"

class PlayerControl : public Component {
public:
	void Update(float dt) override;

	void OnImGui() override;
	std::string GetComponentName() const override { return "PlayerControl"; }
};
