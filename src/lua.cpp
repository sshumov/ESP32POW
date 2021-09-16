#include "main.h"
#include "lua.h"
#include <FS.h>

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

void lua_task_callback() {
    String func = "tick";
    uint used_lua_memory;
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
    used_lua_memory = lua_gc(LUA_state, LUA_GCCOUNT, 0);
    if(used_lua_memory > 64 )  {
        lua_gc (LUA_state, LUA_GCCOLLECT, 0);
        #ifdef DEBUG
                print_DEBUG("LUA: run gcollector");
        #endif
    }
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
luaopen_base(LUA_state);
luaopen_table(LUA_state);
luaopen_string(LUA_state);
luaopen_math(LUA_state);
luaopen_libesp32(LUA_state);


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

} else {
    print_DEBUG("Unable load lua script");
}

}

