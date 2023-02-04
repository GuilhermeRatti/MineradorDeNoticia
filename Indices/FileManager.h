#pragma once
#include "HashTable.h"
#include "Documentos.h"
#include <stdio.h>
#include <stdlib.h>

void manager_read_txt_train(FILE*,char*,p_HashTable,TIPO_LEITURA);
int manager_read_txt_test(FILE*,char*,p_Documentos**,p_HashTable);
