#include <stdio.h>  
#include <lua.h> 
#include <lualib.h>  
#include <lauxlib.h>  
lua_State* L;  
int add(lua_State* L);  

int add(lua_State* L)  
{  
    //从L栈中取出索引为1的数值，并检查  
    int x = luaL_checkinteger(L,1);  
    //从L栈中取出索引为2的数值，并检查  
    int y = luaL_checkinteger(L,2);  
    printf("result:%d\n",x+y);  
    return 1;  
}  

int main(int argc, char* argv[])  
{  
    //初始化全局L  
    L = luaL_newstate();  
    //打开库  
    luaL_openlibs(L);  
    //把函数压入栈中  
    lua_pushcfunction(L, add);  
    //设置全局ADD  
    lua_setglobal(L, "ADD");   
    //加载我们的lua脚本文件  
    if (luaL_loadfile(L,"./1.lua"))
    {  
        printf("error\n");  
    }  
    //安全检查  
    lua_pcall(L,0,0,0);  
    //push进lua函数  
    lua_getglobal(L, "mylua");   
    lua_pcall(L,0,0,0);  

    printf("hello my lua\n");  
    return 0;  
}  
