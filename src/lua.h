#ifndef _LUA_
#define _LUA_
#define LUA_USE_C89
#include "lua.hpp"
#include <TaskSchedulerDeclarations.h>
#define LUA_OK  0
void lua_task_callback(void);
void init_lua(Scheduler *sc);
extern lua_State *LUA_state;       // VM lua


void luaopen_libesp32(lua_State* L);
#endif
