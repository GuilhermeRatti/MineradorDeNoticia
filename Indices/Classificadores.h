#pragma once
#include <stdio.h>
#include "Documentos.h"

typedef void(*Classificador)(p_Documentos *ListaDeDocumentos, int qtd, p_Documentos Documento_a_classificar);

typedef enum
{
    K_NEAREST_NEIGHBOURS = 1,
    CENTROIDE_MAIS_PROXIMA = 2,
}TIPOS_DISPONIVEIS;

Classificador classificadores_retorna_tipo(TIPOS_DISPONIVEIS);