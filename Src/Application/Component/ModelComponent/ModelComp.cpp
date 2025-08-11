#include "ModelComp.h"

void ModelCpmp::OnImGui()
{
	//if (ImGui::CollapsingHeader("Model Component"))
	{
		// パス用の一時バッファ
		static char pathBuffer[256];

		// 初期化（ModelCompのm_pathを反映）
		static bool initialized = false;
		if (!initialized)
		{
			strncpy(pathBuffer, m_path.c_str(), sizeof(pathBuffer));
			initialized = true;
		}

		ImGui::Text("Model Path:");

		// 入力ボックス（編集可）
		if (ImGui::InputText("##ModelPath", pathBuffer, IM_ARRAYSIZE(pathBuffer)))
		{
			// 編集されたら反映
			m_path = pathBuffer;
		}
	}

}

void ModelCpmp::SetModel(std::string modelpath)
{

	if (auto owner = m_owner.lock())
	{
		if (owner->m_modelData == nullptr)
		{
			owner->m_modelData = std::make_shared<KdModelData>();
			owner->m_modelData->Load(modelpath);
			m_path = modelpath;
		}
	}

}

void ModelCpmp::SetModelWork(std::string modelpath)
{

	if (auto owner = m_owner.lock())
	{
		if (owner->m_modelWork == nullptr)
		{
			owner->m_modelWork = std::make_shared<KdModelWork>();
			owner->m_modelWork->SetModelData(modelpath);
			m_path = modelpath;
		}
	}
}

void ModelCpmp::Update(float dt)
{

}
