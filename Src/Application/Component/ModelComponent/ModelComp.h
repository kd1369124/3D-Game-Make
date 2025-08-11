#pragma once
#include"../Component.h"
class ModelCpmp : public Component 
{
public:
	ModelCpmp(){}
	~ModelCpmp()override{}

	void OnImGui() override;

	void SetModel(std::string modelpath);

	void SetModelWork(std::string modelpath);

	void Update(float dt) override;

	std::string GetComponentName() const override { return "ModelCpmponent"; }

private:

	std::string m_path;

};
