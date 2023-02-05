#pragma once
#include <stdio.h>
#include "Documentos.h"

typedef char*(*Classificador)(p_Documentos *, int , p_Documentos, int);

typedef enum
{
    K_NEAREST_NEIGHBOURS = 1,
    CENTROIDE_MAIS_PROXIMA = 2,
}TIPOS_DISPONIVEIS;

Classificador classificadores_retorna_tipo(TIPOS_DISPONIVEIS);