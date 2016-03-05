#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{

    void (*module_init)(void);
    void (*startup)(void);
    
    int ch;
    printf("start server\n");
    //1.单个字符，表示选项
    //2.单个字符后接一个冒号：表示该选项后必须跟一个参数。参数紧跟在选项后或者以空格隔开。
    //3.单个字符后跟两个冒号，表示该选项后可以跟一个参数，也可以不跟。如果跟一个参数，参数必须紧跟在选项后不能以空格隔开。
    while ((ch = getopt(argc, argv, "nlf:")) !=  - 1) {
        printf("optind: %d\n", optind);
        switch(ch) {
            case 'n':
                module_init = netd_init;
                startup = netd_start;
                break; 
            case 'l':
                module_init = gamed_init;
                startup = gamed_start;
                break; 
            case 'f':
                config_file = optarg;
                break; 
        }
    }

    module_init();
    startup();
    return 0;
}
