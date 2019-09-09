#include "common.h"

auto JVM::HookJVM() -> bool
{
	jsize count;
	if(JNI_GetCreatedJavaVMs(&this->vm, 1, &count) != JNI_OK)
	{
		printf("JNI_GetCreatedJavaVMs: Failed!\n");
		return false;
	}

	jint result = this->vm->GetEnv(reinterpret_cast<void**>(&this->env), JNI_VERSION_1_6);
	if(result == JNI_EDETACHED)
		result = this->vm->AttachCurrentThread(reinterpret_cast<void**>(&this->env), nullptr);
	if(result != JNI_OK)
	{
		printf("AttachCurrentThread: Failed!\n");
		return false;
	}
	result = this->vm->GetEnv(reinterpret_cast<void**>(&this->jvmti), JVMTI_VERSION_1_0);
	if (result != JNI_OK)
	{
		printf("GetEnv: Failed!\n");
		return false;
	}
	return true;
}

auto JVM::GetVM() const -> JavaVM*
{
	return this->vm;
}
auto JVM::GetEnv() const -> JNIEnv*
{
	return this->env;
}
auto JVM::GetJVMTI() const -> jvmtiEnv*
{
	return this->jvmti;
}

auto JVM::Init() -> bool
{
	return HookJVM();
}

JVM jvm;