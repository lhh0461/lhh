#include "uthash.h"  
#include <stdlib.h>   /* malloc */  
#include <stdio.h>    /* printf */  
#include <time.h>  
#include <string.h>  

typedef struct example_user_t {  
    char id[10];  
    int cookie;  
    UT_hash_handle hh;  
} example_user_t;  

char* itoa(int num,char*str,int radix)
{/*索引表*/
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned unum;/*中间变量*/
    int i=0,j,k;
    /*确定unum的值*/
    if(radix==10&&num<0)/*十进制负数*/
    {
        unum=(unsigned)-num;
        str[i++]='-';
    }
    else unum=(unsigned)num;/*其他情况*/
    /*转换*/
    do{
        str[i++]=index[unum%(unsigned)radix];
        unum/=radix;
    }while(unum);
    str[i]='\0';
    /*逆序*/
    if(str[0]=='-')k=1;/*十进制负数*/
    else k=0;
    char temp;
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}

int main(int argc,char *argv[]) {  
    char *str = "abd1";
    int i;  
    example_user_t *user, *users=NULL;  

    srand((unsigned int)time(NULL));  
    /* create elements */  
    for(i=0;i<10;i++) {  
        if ( (user = (example_user_t*)malloc(sizeof(example_user_t))) == NULL) exit(-1);  
        memset(user, '\0', sizeof(*user));
        strncpy(user->id,  str + "1", strlen(str));
        user->cookie = i*i;  
        HASH_ADD_INT(users,id,user);  
    }
    char *j = "dwadw";
    HASH_FIND_STR(users, &j, user);
    if (user != NULL)
    {
        printf ("The key(%s) exists in hash. \n", user->id);
    }
    

    for(user=users; user != NULL; user=(example_user_t*)(user->hh.next)) {  
        printf("user %s, cookie %d\n", user->id, user->cookie);  
    }  
    return 0;  
}  
