#pragma once
#include"../Component.h"
class AnimeComp : public Component
{
public:
	AnimeComp(){}
	~AnimeComp(){}

	void SetAnime(std::string_view anime);

private:

};
