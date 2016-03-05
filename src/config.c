#include <lua.h> 
#include <lauxlib.h>  
#include <assert.h>
#include <stdlib.h>

static lua_State* L;  

char *config_file;


void config_set_int(const char *key, int value);
void config_set_string(const char *key, const char *value);

static const char * load_config = "\
    local config_name = ...\
    local f = assert(io.open(config_name))\
    local code = assert(f:read \'*a\')\
    local function getenv(name) return assert(os.getenv(name), \'os.getenv() failed: \' .. name) end\
    code = string.gsub(code, \'%$([%w_%d]+)\', getenv)\
    f:close()\
    local result = {}\
    assert(load(code,\'=(load)\',\'t\',result))()\
    return result\
    ";



static void __init_config(void) 
{
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -1) != 0) {
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
    //if (luaL_loadfile(L,"./load_config.lua"))
    if (luaL_loadstring(L,load_config))
    {  
        printf("error\n");  
    }  
    //push进lua函数  
    lua_pushstring(L, "./config.ini");   
    int res = lua_pcall(L,1,1,0); 
    if (res != LUA_OK) {
        printf("lua_pcall error\n");  
        lua_close(L);
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
    printf("get a = %d\n", a);
    return 0;
}
