#include "main.h"
#include "lua.h"
#include <FS.h>

Task LUA_task(1000, TASK_FOREVER, &lua_task_callback);

String lua_code;
size_t  lua_code_len;

lua_State *LUA_state;       // VM lua
bool lua_st;                // статус ВМ

static int lua_wrapper_print(lua_State *L) {
    int n = lua_gettop(L);  /* number of arguments */
    int i;
    String msg = "";
    lua_getglobal(L, "tostring");
    for (i=1; i<=n; i++) {
        const char *s;
        size_t l;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        s = lua_tolstring(L, -1, &l);  /* get result */
        if (s == NULL) return luaL_error(L, "'tostring' must return a string to 'print'");
        msg += String(s);
        lua_pop(L, 1);  /* pop result */
    }
    print_DEBUG(msg);
    return 0;
}

static int lua_wrapper_putnvr(lua_State *lua_state) {
    const char *key;
    const char *value;
    key   = lua_tostring(lua_state,1);
    value = lua_tostring(lua_state,2);
    spurt(key,value);
    return 0;
}
static int lua_wrapper_readnvr(lua_State *lua_state) {
    const char *key;
    String ret;
    key   = lua_tostring(lua_state,1);
    ret = slurp(key);
    //print_DEBUG(ret);
    lua_pushstring(lua_state,ret.c_str());
    return 1;
}

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
u_int ret;
lua_code_len = 0;
LUA_state = luaL_newstate();
luaopen_base(LUA_state);
luaopen_table(LUA_state);
luaopen_string(LUA_state);
luaopen_math(LUA_state);
lua_register(LUA_state, "print",     (const lua_CFunction)  &lua_wrapper_print);
lua_register(LUA_state, "save_nvram",(const lua_CFunction)  &lua_wrapper_putnvr);
lua_register(LUA_state, "read_nvram",(const lua_CFunction)  &lua_wrapper_readnvr);
ret = 1000;

if(load_lua_code("/start.lua") == true) {
    lua_st = luaL_loadbuffer(LUA_state,lua_code.c_str(),lua_code.length(),"ESP32-POW");
    lua_put_global_param(LUA_state);
    if(lua_pcall(LUA_state, 0, 1 , 0) != LUA_OK ) {
        print_DEBUG("Init lua failed");
        const char* err = lua_tostring(LUA_state, -1);
        print_DEBUG(err);
    } else {
        ret = lua_tonumber(LUA_state, -1);
        lua_pop(LUA_state,1);
        lua_pop(LUA_state, lua_gettop(LUA_state));
        lua_code="";
        sc->addTask(LUA_task);
        LUA_task.setInterval(ret);
        LUA_task.enable();
    }

} else {
    print_DEBUG("Unable load lua script");
}

}

