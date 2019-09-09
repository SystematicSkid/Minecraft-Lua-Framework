#pragma once
#pragma once
#include "lua.hpp"

extern "C" {
# include "lua.h"
# include "luajit.h"
# include "lauxlib.h"
# include "lualib.h"
}

#pragma comment(lib, "lua51.lib")
#pragma comment(lib, "luajit.lib")
#define SOL_LUAJIT
#define SOL_LUAJIT_VERSION 99999
#include <sol.hpp>