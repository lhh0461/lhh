#include <lua.h>
#include <lstring.h>
#include <lualib.h>
#include <lauxlib.h>

static lua_State* L;  

void vm_init(void)
{
    //初始化环境
    L = luaL_newstate();  
    //打开库  
    luaL_openlibs(L); 
    luaS_initshr();
    printf("vm init finish!!!\n");
}

void vm_close(void)
{
    lua_close(L);
    printf("vm close finish!!!\n");
}

void do_call_script_func(char *file, const char *func, const char *fmt, va_list args)
{
    lua_pushstring(L, func);   
    int argc = 0;
    while (*fmt) {
        switch(*fmt) {
            //参数是整形
            case 'i':{
                int arg = va_arg(args, int);
                lua_pushnumber(L, arg);
                argc++;
                }
                break;
                //参数是字符串
            case 's':{
                char *arg = va_arg(args, char *);
                lua_pushstring(L, arg);
                argc++;
                }
                break;
            default: 
                printf("error args type\n");
                break;
        }
        fmt++;
    }
    int res = lua_pcall(L, argc, 0, 0);
    if (res != LUA_OK) {
        fprintf(stderr, "lua_pcall error, errno = %d\n", res);  
        lua_close(L);
        exit(1);
    }
}

// 调用lua脚本
void call_script_func(char *file, char *func, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    do_call_script_func(file, func, fmt, args);
    va_end(args);
}
