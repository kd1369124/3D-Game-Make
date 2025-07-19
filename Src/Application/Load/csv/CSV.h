#pragma once
class C_CSV
{
public:
	C_CSV(){}
	~C_CSV();

	void LoadCSV(char *filename);

	void Write( char* filename,std::vector<std::vector<std::string>> savedate);

	std::vector<std::vector<std::string>> GetSavedate() { return savedate; }



private:

	std::vector<std::vector<std::string>> savedate;


};
