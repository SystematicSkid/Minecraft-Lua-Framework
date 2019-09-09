#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define CURL_STATICLIB
#include <Windows.h>
#include <winternl.h>
#include <Xinput.h>
#include <comdef.h>
#include <minwindef.h>
#include <WinBase.h>
#include <intrin.h>
#include <Psapi.h>
#include <Shlobj.h>
#include <cstdint>


#include <iostream>
#include <vector>
#include <ctime>
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <cctype>
#include <clocale>
#include <algorithm>
#include <sstream>
#include <vector>
#include <functional>
#include <iterator> 
#include <regex>
#include <map>
#include <set>
#include <any>


// JVM
#include <jvmti.h>
#include <jni.h>
#include "jvm.h"

// curl
#include <curl/curl.h>
#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Iphlpapi.lib")

// Core
#include "srg.h"
#include "Minecraft/helpers.h"
#include "Minecraft/definitions.h"
#include "lua.h"
#include "bindings.h"
#include "hooks.h"