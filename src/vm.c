#include <lua.h>
#include <lauxlib.h>

static lua_State* L;  

void vm_init()
{
    //初始化环境
    L = luaL_newstate();  
    //打开库  
    luaL_openlibs(L);  
    printf("vm init finish!!!\n");
}

void vm_close()
{
    lua_close(L);
    printf("vm close finish!!!\n");
}

void call_script_func()
{
    if (luaL_loadfile(L, load_config))
    int res = lua_pcall(L, 1, 1, 0); 

}
