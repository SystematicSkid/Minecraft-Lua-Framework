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
	Usertypes::Define();
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

	lua["JVM"] = lua.create_table();
	lua["JVM"]["SetInt"] = [](uintptr_t obj, DWORD field, int val) -> void
	{
		jvm.GetEnv()->SetIntField((jobject)obj, (jfieldID)field, val);
	};
	lua["JVM"]["GetInt"] = [](uintptr_t obj, DWORD field) -> int
	{
		return static_cast<int>(jvm.GetEnv()->GetIntField((jobject)obj, (jfieldID)field));
	};
	lua["JVM"]["GetDouble"] = [](uintptr_t obj, DWORD field) -> double
	{
		return static_cast<double>(jvm.GetEnv()->GetDoubleField((jobject)obj, (jfieldID)field));
	};
	lua["JVM"]["SetBool"] = [](uintptr_t obj, DWORD field, bool val) -> void
	{
		jvm.GetEnv()->SetBooleanField((jobject)obj, (jfieldID)field, val);
	};
	lua["JVM"]["SetFloat"] = [](uintptr_t obj, DWORD field, double val) -> void
	{
		jvm.GetEnv()->SetFloatField((jobject)obj, (jfieldID)field, (float)val);
	};
	lua["JVM"]["GetObject"] = [](uintptr_t obj, DWORD field) -> uintptr_t
	{
		return reinterpret_cast<uintptr_t>(jvm.GetEnv()->GetObjectField((jobject)obj, (jfieldID)field));
	};
}

void Bindings::MinecraftClass()
{

	jint count;
	jclass* klasses;
	jvm.GetJVMTI()->GetLoadedClasses(&count, &klasses);

	std::ofstream out;
	out.open("C:\\Minecraft\\usertypes.h");
	for(int i = 0; i < count; i++)
	{
		jclass klass = klasses[i];
		char* sig;
		if(jvm.GetJVMTI()->GetClassSignature(klass, &sig, NULL) != JVMTI_ERROR_NONE)
			continue;

		if (strstr(sig, "/") || strstr(sig, "$")) // not obf class
			continue;
		
		std::string signature(sig);
		

		std::string class_name = signature.substr(1, signature.size() - 2);
		std::string clean_name = srg.GetUnobfuscatedClassName(class_name);
		clean_name = clean_name.substr(clean_name.find_last_of('/') + 1);
		if(clean_name.empty())
			continue;
		if(lua[clean_name] == nullptr)
			continue;

		auto fields = Helpers::GetKlassFields(klass);

		for (auto field : fields)
		{
			char* name; char* field_signature; char* generic_ptr;
			jvm.GetJVMTI()->GetFieldName(klass, field, &name, &field_signature, &generic_ptr);
			std::string field_name = name;
			field_name = srg.GetMCPFieldName(srg.GetSRGFieldName(srg.GetUnobfuscatedClassName(class_name), field_name));
			lua[clean_name][field_name] = (DWORD)field; // bind to field id
		}
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
