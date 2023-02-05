#include "../headers/Classificadores.h"
#include "../headers/Documentos.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct 
{
    char* classe;
    int posicao_no_vet;
    double cosseno;
}Resultados;

char* k_nearest_neighbours(p_Documentos *vet_doc, int qtd, p_Documentos doc_analisado, int k);

char* centroide_mais_proxima(p_Documentos *vet_doc, int qtd, p_Documentos doc_analisado, int lixo);

void calcula_resultados(p_Documentos *dataset, Resultados *res, int qtd, p_Documentos doc_analisado);

int compara_resultados(const void *a, const void *b)
{
    Resultados r1 = (*(Resultados*)a), r2 = (*(Resultados*)b);

    if(r1.cosseno==r2.cosseno)
        return 0;
    else if(r1.cosseno>r2.cosseno)
        return -1;
    else
        return 1;
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

void calcula_resultados(p_Documentos *dataset, Resultados *res, int qtd, p_Documentos doc_analisado)
{
    int i;
    for(i=0;i<qtd;i++)
    {
        res[i].classe = documentos_retorna_classe(dataset[i]);
        res[i].cosseno = documentos_calcula_cosseno(dataset[i],doc_analisado);
        res[i].posicao_no_vet = i;
    }

    qsort(res,qtd,sizeof(Resultados),compara_resultados);
}

char* k_nearest_neighbours(p_Documentos *dataset, int qtd, p_Documentos doc_analisado, int k_vizinhos)
{
    Resultados res[qtd];
    Resultados resultado_knn[k_vizinhos];
    int qtd_classes_contabilizadas=0,ja_foi=0;
    char *classes_ja_contabilizadas[k_vizinhos];
    calcula_resultados(dataset,res,qtd,doc_analisado);
    
    int i,j;
    for(i=0;i<k_vizinhos;i++)
    {
        if(i==qtd)
            break;
        
        for(j=0;j<qtd_classes_contabilizadas;j++)
        {
            if(!strcmp(classes_ja_contabilizadas[j],res[i].classe))
            {
                resultado_knn[j].cosseno++; 
                ja_foi=1;
                break;
            }
        }
        if(!ja_foi)
        {
            classes_ja_contabilizadas[j]=res[i].classe;
            resultado_knn[j].classe=res[i].classe;
            resultado_knn[j].posicao_no_vet=res[i].posicao_no_vet;
            resultado_knn[j].cosseno=1;
            qtd_classes_contabilizadas++;
        }
    }

    qsort(resultado_knn,qtd_classes_contabilizadas,sizeof(Resultados),compara_resultados);
    
    return documentos_retorna_classe(dataset[resultado_knn[0].posicao_no_vet]);
}

char* centroide_mais_proxima(p_Documentos *dataset, int qtd, p_Documentos doc_analisado, int lixo)
{
    Resultados res[qtd];
    calcula_resultados(dataset,res,qtd,doc_analisado);

    return documentos_retorna_classe(dataset[res[0].posicao_no_vet]);
}