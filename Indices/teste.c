#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char* str1 = strdup("a"),*str2 = strdup("b"),*str3=strdup("a");

    if((long int)str1==(long int)str3)
    {
        printf("deu bom\n");
    }

    free(str1);
    free(str2);
    free(str3);

    return 0;
}
