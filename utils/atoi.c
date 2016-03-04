#include <stdio.h>
#include <string.h>

int atoi(const char *str)
{
    if (str == NULL || strlen(str) == 0)
        return 0;
    int i = 0;
    int len = strlen(str);
    int sum = 0;
    int times = 1;
    for (; i < len; i++) {
        if (*(str+len-1-i) > '9'|| *(str+len-1-i) < '0')
            break;
        sum += times * (*(str+len-1-i) - '0');
        times *= 10;
    }
    if (i < len) return 0;
    return sum;
}


int main()
{
    int ret = atoi("01232130");
    printf("ret = %d\n", ret);
    return 0;
}
