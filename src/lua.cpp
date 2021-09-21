#include "main.h"
#include "lua.h"
#include <FS.h>

#ifdef USE_DS3231
#include "ds3231.hpp"
#endif

#ifdef USE_TOP323
#include "top323.h"
#endif




Task LUA_task(1000, TASK_FOREVER, &lua_task_callback);
String lua_func;
String lua_code;
size_t  lua_code_len;

lua_State *LUA_state;       // VM lua
bool lua_st;                // статус ВМ

void lua_put_global_param(lua_State *lua_state) {
    lua_pushstring(LUA_state,WiFiSettings.hostname.c_str());lua_setglobal(LUA_state, "wifi_hostname"); // set hostname
    lua_pushboolean(LUA_state,wifi_status);lua_setglobal(LUA_state, "wifi_status"); // set wifi status
}
void lua_gcollector(void) {
    uint used_lua_memory;
    used_lua_memory = lua_gc(LUA_state, LUA_GCCOUNT, 0);

    if(used_lua_memory > 64 )  {
        #ifdef DEBUG
            print_DEBUG("LUA: START gcollector");
            mem_stat();
        #endif
        lua_gc (LUA_state, LUA_GCCOLLECT, 0);
        #ifdef DEBUG
            print_DEBUG("LUA: END gcollector");
            mem_stat();
        #endif
    }
}

void lua_task_callback() {
    String func = "tick";
    lua_put_global_param(LUA_state);
    lua_getglobal(LUA_state, func.c_str());
        if (lua_isfunction(LUA_state, -1)) {
            if (lua_pcall(LUA_state, 0, 0, 0) == LUA_OK) {
                lua_pop(LUA_state, lua_gettop(LUA_state));
            } else {
                lua_st = 1;
                #ifdef DEBUG
                   print_DEBUG("LUA: Script exec return error: " + String(lua_tostring(LUA_state, -1) ));
                #endif
                lua_pop(LUA_state, 1);
            }
        } else {
            #ifdef DEBUG
                print_DEBUG("LUA: function " + func + " not define in lua script");
            #endif
        }
        lua_gcollector();
}

bool load_lua_code(const char *file_r) {
File f;
f = SPIFFS.open(file_r, "r");

if(!f) {
    print_DEBUG("load_lua_code unable to open file: /start.lua");
    return false;
}
lua_code = f.readString();
#ifdef DEBUG
 //print_DEBUG("LUA script: ----------------------------------------------------------------- ");
 //print_DEBUG(lua_code);
 //print_DEBUG("----------------------------------------------------------------------------- ");
#endif
f.close();
return true;
}

void init_lua(Scheduler *sc) {
print_DEBUG("Init LUA ...");
u_int lua_run_time; // Переодичность выполнения lua скрипта
lua_code_len = 0;
LUA_state = luaL_newstate();

luaL_openlibs(LUA_state);
luaopen_libesp32(LUA_state);

#ifdef USE_TOP323
init_top323(LUA_state);
#endif

#ifdef USE_DS3231
init_ds3231(LUA_state);
#endif





lua_run_time = 1000;

if(load_lua_code("/start.lua") == true) {
    lua_st = luaL_loadbuffer(LUA_state,lua_code.c_str(),lua_code.length(),"ESP32-POW");
    lua_put_global_param(LUA_state);
    if(lua_pcall(LUA_state, 0, 1 , 0) != LUA_OK ) {
        print_DEBUG("Init lua failed");
        const char* err = lua_tostring(LUA_state, -1);
        print_DEBUG(err);
    } else {
        lua_run_time = lua_tonumber(LUA_state,-1);
        lua_pop(LUA_state,-1);
        Serial.print("Time interval: ");
        Serial.println(lua_run_time);
        lua_code="";
        sc->addTask(LUA_task);
        LUA_task.setInterval(lua_run_time);
        LUA_task.enable();
    }
    lua_gcollector();
} else {
    print_DEBUG("Unable load lua script");
}

}

