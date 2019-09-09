#include "common.h"

// Should only be called once
void SRG::GetClasses()
{
	std::map<std::string, std::string> ret;
	std::istringstream lines(this->file);
	std::string current_line;
	std::map<std::string, std::string> fields;

	while (getline(lines, current_line))
	{
		if (current_line.c_str()[0] == '\t') // This is a field or function
		{
			if (!previous_class.empty())
			{
				if (strstr(current_line.c_str(), "(")) // method
				{

				}
				else
				{
					std::string field_obfuscated_name = current_line.substr(1, current_line.find(' ') - 1); // skip the \t
					const std::string field_name = current_line.substr(current_line.find(' ') + 1);
					fields[field_obfuscated_name] = field_name;
				}
			}
			continue; // go to next while iter
		}
		// Found class

		std::string obfuscated_name = current_line.substr(0, current_line.find(' '));
		if (!fields.empty())
			this->field_mappings[previous_class] = fields;

		const std::string clean_name = current_line.substr(current_line.find(' ') + 1);
		ret[obfuscated_name] = clean_name;
		previous_class = clean_name;
		fields.clear();
	}
	this->class_mappings = ret;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void SRG::LoadFile(std::string version)
{
	CURL* curl;
	CURLcode res;
	std::string readBuffer;
	std::string fieldBuffer;
	std::string methodBuffer;
	std::string url = R"(https://raw.githubusercontent.com/MinecraftForge/MCPConfig/master/versions/)" + version + R"(/joined.tsrg)";
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		this->file = readBuffer;




	}

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://export.mcpbot.bspk.rs/fields.csv");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fieldBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		this->fields = fieldBuffer;
	}

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://export.mcpbot.bspk.rs/methods.csv");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &methodBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		this->methods = methodBuffer;
	}


	GetClasses(); // init our classes
}

std::string SRG::GetUnobfuscatedClassName(std::string obfuscated_name)
{
	return this->class_mappings[obfuscated_name];
}

auto SRG::GetSRGFieldName(const std::string obfuscated_class, std::string obfuscated_name) -> std::string
{
	auto fields = this->field_mappings[obfuscated_class];
	return fields[obfuscated_name];
}

std::string SRG::GetMCPFieldName(std::string srg_name) const
{
	std::istringstream lines(this->fields);
	std::string current_line;
	while (getline(lines, current_line))
	{
		if (strstr(current_line.c_str(), srg_name.c_str()))
		{
			std::string&& rem_field = current_line.substr(current_line.find_first_of(',') + 1);
			std::string&& real_name = rem_field.substr(0, rem_field.find(','));
			return std::move(real_name);
		}

	}
	return "NotFound";
}

SRG srg;
