#ifndef _LUA_
#define _LUA_
#define LUA_USE_C89
#include "lua/lua.hpp"
#include <TaskSchedulerDeclarations.h>
void lua_task_callback(void);
void init_lua(Scheduler *sc);
extern lua_State *LUA_state;       // VM lua

void luaopen_libesp32(lua_State* L);
#endif
