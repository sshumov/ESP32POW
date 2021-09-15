#include "main.h"
#include "lua.h"
#include <FS.h>

Task LUA_task(1000, TASK_FOREVER, &lua_task_callback);

// load wasm code from spiffs
unsigned char *lua_code;
size_t  lua_code_len;

void lua_task_callback() {
}

bool load_lua_code(const char *file_r) {
File f;
f = SPIFFS.open(file_r, "r");
if(!f) {
    print_DEBUG("load_lua_code unable to open file: /code.lua");
    return false;
}
lua_code_len = f.size();
Serial.println(file_r);
Serial.println(lua_code_len);
lua_code = (unsigned char*) malloc(lua_code_len + 1);
if (lua_code == NULL) {
    print_DEBUG("load_lua_code: Unable allocate memory");
    return false;
    }
size_t readb = f.readBytes((char *)lua_code,lua_code_len);
if(readb != lua_code_len) {
    print_DEBUG("load_lua_code: Read error");
    return false;
    }
f.close();
return true;
}

void init_lua(Scheduler *sc) {
 lua_code_len = 0;
  print_DEBUG("Start LUA ...");

if(load_lua_code("/start.lua") == true) {
 sc->addTask(LUA_task);
 LUA_task.enable();

} else {

}

}

