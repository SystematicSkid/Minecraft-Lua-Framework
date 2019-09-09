#pragma once

class SRG
{
private:
	std::string url;
	std::string file;
	std::string fields;
	std::map <std::string, std::string> mcp_field_mappings;
	std::string methods;
	std::map <std::string, std::string> mcp_method_mappings;
	std::string previous_class;
	std::map<std::string, std::string> class_mappings;
	std::map<std::string, std::map<std::string, std::string>> field_mappings;
private:
	auto GetFilePath(std::string obfuscated_name)->std::string;
	auto GetClasses() -> void;
public:
	void LoadFile(std::string version);
	std::string GetUnobfuscatedClassName(std::string obfuscated_name);
	std::string GetSRGFieldName(std::string obfuscated_class, std::string obfuscated_name);
	std::string GetMCPFieldName(std::string srg_name) const;
};

extern SRG srg;