#include <lua.h> 
#include <lauxlib.h>  
#include <assert.h>
#include <stdlib.h>

#include "config.h"

static lua_State* L;  

static const char *config_file = "./config.ini";
static const char *load_file = "./load_config.lua";

static void __init_config(void) 
{
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) {
        int keyt = lua_type(L, -2);
        if (keyt != LUA_TSTRING) {
            fprintf(stderr, "Invalid config table\n");
            exit(1);
        }
        const char * key = lua_tostring(L, -2);
        if (lua_type(L, -1) == LUA_TBOOLEAN) {
            int b = lua_toboolean(L, -1);
            config_set_int(key, b);
            //skynet_setenv(key,b ? "true" : "false" );
        } 
        else if (lua_type(L, -1) == LUA_TNUMBER) {
            int value = lua_tointeger(L, -1);
            config_set_int(key, value);
        } else {
            const char * value = lua_tostring(L, -1);
            if (value == NULL) {
                fprintf(stderr, "Invalid config table key = %s\n", key);
                exit(1);
            }
            config_set_string(key, value);
        }
        lua_pop(L,1);
    }
    lua_pop(L,1);
}

void config_init()
{
    //初始化环境
    L = luaL_newstate();  
    //打开库  
    luaL_openlibs(L);  
    luaS_initshr();
    if (luaL_loadfile(L, load_file))
    {  
        printf("loadstring error\n");  
        lua_close(L);
        exit(1);
    }  
    //push进lua函数  
    lua_pushstring(L, config_file);   
    int res = lua_pcall(L, 1, 1, 0); 
    if (res != LUA_OK) {
        fprintf(stderr, "lua_pcall error, errno = %d\n", res);  
        lua_close(L);
        exit(1);
    }
    __init_config();
}

const char *config_get_string(const char *key)
{
    const char *result;
    lua_getglobal(L, key);
    assert(!lua_isnil(L, -1));
    result = lua_tostring(L, -1);
    lua_pop(L,1);
    return result;
}

int config_get_int(const char *key)
{
    int result;
    lua_getglobal(L, key);
    //assert(!lua_isnil(L, -1));
    result = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return result;
}

void config_set_int(const char *key, int value)
{
    lua_getglobal(L, key);
    assert(lua_isnil(L, -1));
    lua_pop(L,1);
    lua_pushinteger(L,value);
    lua_setglobal(L,key);  
}

void config_set_string(const char *key, const char *value)
{
    lua_getglobal(L, key);
    assert(lua_isnil(L, -1));
    lua_pop(L, 1);
    lua_pushstring(L, value);
    lua_setglobal(L, key);  
}


int main()
{
    config_init();
    int a = config_get_int("MAX_ONLINE");
    const char* b = config_get_string("IP");
    int c = config_get_int("PORT");
    int d = config_get_int("HARBORID");
    printf("get a = %d\n", a);
    printf("get b = %s\n", b);
    printf("get c = %d\n", c);
    printf("get d = %d\n", d);
    return 0;
}
