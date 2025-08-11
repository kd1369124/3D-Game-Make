#include "AnimeComponent.h"

void AnimeComp::SetAnime(std::string_view anime)
{
	if (auto owner = m_owner.lock())
	{
		if (owner->m_animator == nullptr)
		{
			owner->m_animator = std::make_shared<KdAnimator>();
			owner->m_animator->AdvanceTime(owner->m_modelWork->WorkNodes());
			owner->m_modelWork->CalcNodeMatrices();
			owner->m_animator->SetAnimation(owner->m_modelWork->GetData()->GetAnimation(anime));
		}
		else
		{
			owner->m_animator->SetAnimation(owner->m_modelWork->GetData()->GetAnimation(anime));
		}
	}
}
