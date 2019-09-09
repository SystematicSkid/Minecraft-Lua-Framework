#pragma once

class JVM
{
private:
	JavaVM* vm = nullptr;
	JNIEnv* env = nullptr;
	jvmtiEnv* jvmti = nullptr;
private:
	bool HookJVM();
public:
	JavaVM* GetVM() const;
	JNIEnv* GetEnv() const;
	jvmtiEnv* GetJVMTI() const;

	bool Init();
};

extern JVM jvm;