#include "../common.h"

auto Helpers::GetMinecraftClass() -> jclass
{
	static jclass ret = nullptr;
	if(!ret)
	{
		jint count;
		jclass* klasses;
		jvm.GetJVMTI()->GetLoadedClasses(&count, &klasses);
		for (int i = 0; i < count; i++)
		{
			jclass klass = klasses[i];
			if(!klass)
				continue;
			char* sig;
			if(jvm.GetJVMTI()->GetClassSignature(klass, &sig, nullptr) != JVMTI_ERROR_NONE)
				continue;
			if(strlen(sig) > 5)
				continue;
			std::string name(sig);
			name = name.substr(1, name.length() - 2);
			if ("net/minecraft/client/Minecraft" == srg.GetUnobfuscatedClassName(name))
			{
				printf("Minecraft: %s\n", name.c_str());
				ret = klass;
				return klass;
			}
		}
		return nullptr;
	}
	return ret;
}

jobject Helpers::GetMinecraftObject()
{
	jclass minecraft = GetMinecraftClass();
	static jobject ret = nullptr;
	if(!ret && minecraft)
	{
		jint count;
		jfieldID* fields;
		jvm.GetJVMTI()->GetClassFields(minecraft, &count, &fields);
		for(int i = 0; i < count; i++)
		{
			jfieldID field = fields[i];
			char* sig;
			jvm.GetJVMTI()->GetFieldName(minecraft, field, nullptr, &sig, nullptr);
			std::string name(sig);
			name = name.substr(1, name.length() - 2);
			if ("net/minecraft/client/Minecraft" == srg.GetUnobfuscatedClassName(name))
			{
				ret = jvm.GetEnv()->GetStaticObjectField(minecraft, field);
				return ret;
			}
		}
		return nullptr;
	}

	return ret;
}

std::vector<jfieldID> Helpers::GetKlassFields(jclass klass)
{
	std::vector<jfieldID> ret;
	jint count;
	jfieldID* fields;
	jvm.GetJVMTI()->GetClassFields(klass, &count, &fields);
	for (int i = 0; i < count; i++)
	{
		jfieldID field = fields[i];
		ret.push_back(field);
	}
	return ret;
}
