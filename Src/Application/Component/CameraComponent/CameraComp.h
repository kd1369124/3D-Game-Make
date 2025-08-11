#pragma once
class CameraCompo : public Component
{
public:

	~CameraCompo(){}


	void Update(float dt) override;

	void OnImGui() override;


	std::string GetComponentName() const override { return "CameraCpmponent"; }

private:
	CameraCompo()
	{
		if (auto owner = m_owner.lock())
		{



		}
	}
};
