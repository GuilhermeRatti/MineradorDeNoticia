#pragma once
#include "HashTable.h"
#include <stdio.h>
#include <stdlib.h>

p_HashTable manager_read_txt(FILE*,char*,p_HashTable);

p_HashTable manager_read_from_terminal(p_HashTable,int*);