#pragma once

class Helpers
{
public:
	static jclass GetMinecraftClass();
	static jobject GetMinecraftObject();
	static std::vector<jfieldID> GetKlassFields(jclass klass);
};