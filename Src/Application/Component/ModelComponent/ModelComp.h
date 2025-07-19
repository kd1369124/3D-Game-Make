#pragma once
#include"../Component.h"
class ModelCpmp : public Component 
{
public:
	ModelCpmp();
	~ModelCpmp()override{}

	void OnImGui() override;

	void SetModel(std::string modelpath)
	{

	}

private:

};
