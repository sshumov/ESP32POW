#include "main.h"
#include "ds3231.hpp"
#include "lua.h"


DS3231 RTCClock;
RTClib RTC;

static int ds3231_get(lua_State *L) {
    DateTime now = RTC.now();
    lua_pushinteger(L,now.second());
    lua_pushinteger(L,now.minute());
    lua_pushinteger(L,now.hour());
    lua_pushinteger(L,now.day());
    lua_pushinteger(L,now.month());
    lua_pushinteger(L,now.year());
    return 6;
}
static int ds3231_set_year(lua_State *L) {
    RTCClock.setYear(luaL_checkinteger(L,1));
}

static int ds3231_set_month(lua_State *L) {
    RTCClock.setMonth(luaL_checkinteger(L,1));
}

static int ds3231_set_day(lua_State *L) {
    RTCClock.setDate(luaL_checkinteger(L,1));
}

static int ds3231_set_hour(lua_State *L) {
    RTCClock.setHour(luaL_checkinteger(L,1));
}

static int ds3231_set_minute(lua_State *L) {
    RTCClock.setMinute(luaL_checkinteger(L,1));
}

static int ds3231_set_second(lua_State *L) {
    RTCClock.setSecond(luaL_checkinteger(L,1));
}

static int ds3231_set_dow(lua_State *L) {
    RTCClock.setDoW(luaL_checkinteger(L,1));
}

static int ds3231_set_clock_mode(lua_State *L) {
    RTCClock.setClockMode(luaL_checkinteger(L,1));
}

static int ds3231_unixtime(lua_State *L) {
    DateTime now = RTC.now();
    lua_pushinteger(L,now.unixtime());
    return 1;
}

static const luaL_Reg ds3231lib[] = {
    {"get", ds3231_get},
    {"utime", ds3231_unixtime},
    {"setSec", ds3231_set_second},
    {"setMin", ds3231_set_minute},
    {"setHour", ds3231_set_hour},
    {"setDay", ds3231_set_day},
    {"setMonth", ds3231_set_month},
    {"setYear", ds3231_set_year},
    {"setDayofWeek", ds3231_set_dow},
    {"setClockMode", ds3231_set_clock_mode}, // 0 - 12H 
    {NULL, NULL}
};


LUAMOD_API int luaopen_ds3231 (lua_State *L) {
    luaL_newlib(L, ds3231lib);
    return 1;
}

void init_ds3231(lua_State *L) {
    luaL_requiref(L, "rtc", luaopen_ds3231, 1);
    lua_pop(L, 1);
}

