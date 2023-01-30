#include "Classificadores.h"
#include "Documentos.h"
#include <stdio.h>
#include <stdlib.h>

void k_nearest_neighbours(p_Documentos *vet_doc, p_Documentos doc_analisado);

void centroide_mais_proxima(p_Documentos *vet_doc, p_Documentos doc_analisado);


Classificador classificadores_retorna_tipo(TIPOS_DISPONIVEIS tipo_desejado)
{
    if(tipo_desejado == K_NEAREST_NEIGHBOURS)
        return k_nearest_neighbours;

    if(tipo_desejado == CENTROIDE_MAIS_PROXIMA)
        return centroide_mais_proxima;
    else
        return NULL;
}

void k_nearest_neighbours(p_Documentos *vet_doc, p_Documentos doc_analisado)
{
    printf("Este eh o KNN\n");
}

void centroide_mais_proxima(p_Documentos *vet_doc, p_Documentos doc_analisado)
{
    printf("Este eh Centroide\n");
}