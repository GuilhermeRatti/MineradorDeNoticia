#include "Classificadores.h"
#include "Documentos.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct 
{
    int posicao_no_vet;
    double cosseno;
}Resultados;

void k_nearest_neighbours(p_Documentos *vet_doc, int qtd, p_Documentos doc_analisado);

void centroide_mais_proxima(p_Documentos *vet_doc, int qtd, p_Documentos doc_analisado);

int compara_resultados(const void *a, const void *b)
{
    Resultados r1 = (*(Resultados*)a), r2 = (*(Resultados*)b);

    if(r1.cosseno==r2.cosseno)
        return 0;
    else if(r1.cosseno>r2.cosseno)
        return 1;
    else
        return -1;
}


Classificador classificadores_retorna_tipo(TIPOS_DISPONIVEIS tipo_desejado)
{
    if(tipo_desejado == K_NEAREST_NEIGHBOURS)
        return k_nearest_neighbours;

    if(tipo_desejado == CENTROIDE_MAIS_PROXIMA)
        return centroide_mais_proxima;
    else
        return NULL;
}

void k_nearest_neighbours(p_Documentos *vet_doc, int qtd, p_Documentos doc_analisado)
{
    Resultados res[qtd];
    
    int i;
    for(i=0;i<qtd;i++)
    {
        res[i].posicao_no_vet = i;
        res[i].cosseno = documentos_calcula_cosseno(vet_doc[i],doc_analisado);
    }

    qsort(res,qtd,sizeof(Resultados),compara_resultados);
}

void centroide_mais_proxima(p_Documentos *vet_doc, int qtd, p_Documentos doc_analisado)
{
    printf("Este eh Centroide\n");
}