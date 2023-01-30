#pragma once
#include "HashTable.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum
{
    TRAIN = 1,
    TESTE = 2,
}TIPO_LEITURA;

p_HashTable manager_read_txt(FILE*,char*,p_HashTable,TIPO_LEITURA);

p_HashTable manager_read_from_terminal(p_HashTable,int*);