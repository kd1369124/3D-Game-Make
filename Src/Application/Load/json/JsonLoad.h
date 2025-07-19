#pragma once
#include <string>
#include <vector>
#include "json.hpp"
struct NodeData {
	std::string name;
	std::string type;
	Math::Vector3 pos;
	Math::Vector3 rotation;
	Math::Vector3 scale;
	nlohmann::json params; 
};
class JsonNodeLoader {
public:
	//static std::vector<NodeData> LoadNodes(const std::string& filepath);
	static std::vector<NodeData> LoadNodeS(const std::string& filepath);
};