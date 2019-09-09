#pragma once
extern sol::state lua;

class Bindings
{
private:
	std::string GetLuaFolder();
	void Globals();
	void MinecraftClass();
	void JavaBindings();
public:

	bool Init();
	bool LoadFile(std::string dir);

	// Call Hooks
	template<typename... Args>
	static void HookCall(std::string name, Args&... args)
	{
		try{lua["hook"]["Call"](name, args...);}
		catch (std::exception& e) { std::cout << e.what() << std::endl; }
	}
};

extern Bindings bindings;