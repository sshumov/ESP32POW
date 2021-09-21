#include "main.h"
#include "lua.h"

static char const VERSION[] = "0.1.0";

static int lua_print(lua_State *L) {
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
static int lua_putnvr(lua_State *lua_state) {
    const char *key;
    const char *value;
    key   = lua_tostring(lua_state,1);
    value = lua_tostring(lua_state,2);
    Serial.print("Store to nvram: ");Serial.print(key);Serial.print(" -> ");Serial.println(value);
    spurt(key,value);
    return 0;
}
static int lua_readnvr(lua_State *lua_state) {
    const char *key;
    String ret;
    key   = lua_tostring(lua_state,1);
    ret = slurp(key);
    //print_DEBUG(ret);
    lua_pushstring(lua_state,ret.c_str());
    return 1;
}

void luaopen_libesp32(lua_State* L) {
lua_register(LUA_state, "print",     (const lua_CFunction)  &lua_print);
lua_register(LUA_state, "save_nvram",(const lua_CFunction)  &lua_putnvr);
lua_register(LUA_state, "read_nvram",(const lua_CFunction)  &lua_readnvr);
}
