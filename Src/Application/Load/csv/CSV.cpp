#include "CSV.h"

C_CSV::~C_CSV()
{

	savedate.clear();

}

void C_CSV::LoadCSV(char* filename)
{
	std::ifstream file(filename);
	std::string line;

	std::vector<std::string> same;
		

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			std::stringstream  ss(line);
			std::string value;
			while (std::getline(ss, value, ','))
			{
				same.push_back(value);

				
				
			}
			savedate.push_back(same);
			same.clear();
		}
		file.close();
	}
	

}

void C_CSV::Write( char* filename,std::vector<std::vector<std::string>> savedate)
{

	std::ofstream file(filename);
	std::string line;

	if (file.is_open())
	{
		for (int i = 0; i < savedate.size();i++)
		{
			for (int a = 0; a < savedate.operator[](i).size(); a++)
			{


				file << savedate.operator[](i).operator[](a);
				if (savedate.operator[](i).size() -1 != a )
				{
					file << ',';
				}


			}

			if (savedate.size() - 1 != i)
			{
				file << std::endl;
			}

		}
	}


}

