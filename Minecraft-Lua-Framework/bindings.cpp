#include "common.h"

sol::state lua = sol::state();

auto Bindings::GetLuaFolder() -> std::string
{
	static std::string roaming = getenv("APPDATA"); // C:\users\user\appdata\roaming
	printf("Roaming: %s\n", roaming.c_str());
	static std::string final = roaming + R"(\.minecraft\lua\)";
	return final;
}

int exception_handler(lua_State* L, sol::optional<const std::exception&> exception, sol::string_view desc)
{
	std::cout << "Exception Encountered!\n";
	if (exception)
	{
		std::cout << "Error: ";
		const std::exception& ex = *exception;
		std::cout << ex.what() << std::endl;
	}
	else
	{
		std::cout << "Description: ";
		std::cout.write(desc.data(), desc.size());
		std::cout << std::endl;
	}

	return sol::stack::push(L, desc);
}



auto Bindings::Init() -> bool
{
	lua = {};
	lua.open_libraries(sol::lib::base,
		sol::lib::string,
		sol::lib::coroutine,
		sol::lib::os,
		sol::lib::math,
		sol::lib::table,
		sol::lib::bit32,
		sol::lib::io,
		sol::lib::package,
		sol::lib::jit); // required libs

	lua.set_exception_handler(&exception_handler);
	
	// Init user types
	Globals();
	//JavaBindings();
	MinecraftClass();
	
	lua.set_function("include", [](std::string path) {lua.script_file(path); });

	if (!LoadFile("libs\\hook.lua"))
		return false;

	std::ifstream file(this->GetLuaFolder() + "loadorder.txt");
	std::string line;
	while(std::getline(file, line))
	{
		this->LoadFile(line);
	}

	HookCall("PostExecute");

	return true;
}

bool Bindings::LoadFile(std::string dir)
{
	dir = this->GetLuaFolder() + dir;
	sol::protected_function_result res = lua.script_file(dir, sol::script_pass_on_error);
	if(!res.valid())
	{
		sol::error err = res;
		std::cout << "\n" << err.what() << "\n" << std::endl;
		return false;
	}
	return true;
}


auto Bindings::Globals() -> void
{
	lua["MinecraftObject"] = (uintptr_t)Helpers::GetMinecraftObject();
	lua["Java"] = jvm.GetEnv();
	lua["JVM"] = lua.create_table();
	lua["JVM"]["SetInt"] = [](uintptr_t obj, DWORD field, int val) -> void
	{
		jvm.GetEnv()->SetIntField((jobject)obj, (jfieldID)field, val);
	};
	lua["JVM"]["GetInt"] = [](uintptr_t obj, DWORD field) -> int
	{
		return jvm.GetEnv()->GetIntField((jobject)obj, (jfieldID)field);
	};
}

void Bindings::MinecraftClass()
{

	lua.new_usertype<std::any>("Minecraft");

	auto klass = Helpers::GetMinecraftClass();
	auto fields = Helpers::GetKlassFields(Helpers::GetMinecraftClass());
	char* sig;
	jvm.GetJVMTI()->GetClassSignature(klass, &sig, NULL);
	std::string signature(sig);
	std::string class_name = signature.substr(1, signature.size() - 2);
	for (auto field : fields)
	{
		char* name; char* field_signature;char* generic_ptr;
		jvm.GetJVMTI()->GetFieldName(klass, field, &name, &field_signature, &generic_ptr);
		std::string field_name = name;
		field_name = srg.GetMCPFieldName(srg.GetSRGFieldName(srg.GetUnobfuscatedClassName(class_name), field_name));
		lua["Minecraft"][field_name] = (DWORD)field; // bind to field id
	}
}


void Bindings::JavaBindings()
{
	lua.new_usertype<JNIEnv>("Env",
		"SetInt", [](JNIEnv& env, jobject obj, jfieldID field, int val)
		{
			env.SetIntField(obj, field, val);
		});
}

Bindings bindings;
