#include "Palavras.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int IdxDocumento;
    int Frequencia;
    double TFIDF;
} IndicePalavras;

struct Palavras
{
    int idx;
    int tam_vet;
    int tam_allcd;
    char *palavra;
    IndicePalavras *vet;
};

int compara_palavras(const void *a, const void *b)
{
    p_Palavras p1 = *((p_Palavras*)a), p2 = *((p_Palavras*)b);
    
    return strcmp(p1->palavra,p2->palavra);
}

int compara_indices_pal(const void *a, const void *b)
{
    IndicePalavras p1 = *((IndicePalavras*)a), p2 = *((IndicePalavras*)b);
    
    return p1.IdxDocumento - p2.IdxDocumento;
}

p_Palavras palavras_cria(p_Palavras*vet,int qtd,char* palavra)
{
    p_Palavras p = (p_Palavras)calloc(1,sizeof(struct Palavras));
    p->tam_allcd=1;
    p->vet = (IndicePalavras*)calloc(p->tam_allcd,sizeof(IndicePalavras));
    p->palavra = (char*)calloc(strlen(palavra)+1,sizeof(char));
    strcpy(p->palavra,palavra);

    int existe = palavras_verifica_existencia(vet,qtd,p);
    if(existe)
    {
        free(p->palavra);
        free(p->vet);
        free(p);
        return NULL;
    }

    return p;
}

int palavras_verifica_existencia(p_Palavras* vet,int qtd, p_Palavras p)
{
    p_Palavras* item = (p_Palavras*)bsearch(&p,vet,qtd,sizeof(p_Palavras),compara_palavras);

    if(item==NULL)
        return 0;
    else
        return 1;
}

void palavras_organiza_ordem(p_Palavras* vet,int qtd)
{
    qsort(vet,qtd,sizeof(p_Palavras),compara_palavras);
}

int palavras_get_indice(p_Palavras* vet, char* palavra, int qtd)
{
    p_Palavras p = (p_Palavras)calloc(1, sizeof(struct Palavras));
    p->palavra = (char*)realloc(p->palavra,(strlen(palavra)+1)*sizeof(char));
    strcpy(p->palavra, palavra);

    p_Palavras* item = (p_Palavras*)bsearch(&p,vet,qtd,sizeof(p_Palavras),compara_palavras);
    
    if(item==NULL)
        exit(printf("ALGO DE MUITO ERRADO ACONTECEU. NAO ACHEI O INDICE DE UMA PALAVRA Q JA ERA PRA ESTAR REGISTRADA ( ???????? )\n"));
    
    free(p->palavra);
    free(p);
    return (*item)->idx;
}

void palavras_registra_frequencia(p_Palavras *vet_Pal,int idx_doc, int idx_pal, int ja_registrado)
{
    if(ja_registrado)
    {
        IndicePalavras id_p;
        id_p.IdxDocumento = idx_doc;

        IndicePalavras * item = (IndicePalavras*)bsearch(&id_p,vet_Pal[idx_pal]->vet,vet_Pal[idx_pal]->tam_vet,sizeof(IndicePalavras),compara_indices_pal);

        if(item==NULL)
        {
            printf("DOC: %d PALAVRA: %s\n", idx_doc, vet_Pal[idx_pal]->palavra);
            exit(printf("ALGO DE MUITO ERRADO ACONTECEU. NAO ACHEI UM DOCUMENTO QUE JA ERA PRA ESTAR REGISTRADO NA PALAVRA ( ???? )\n"));
        }

        item->Frequencia++;
        return;
    }

    if(vet_Pal[idx_pal]->tam_vet == vet_Pal[idx_pal]->tam_allcd)
    {
        vet_Pal[idx_pal]->tam_allcd*=2;
        vet_Pal[idx_pal]->vet = (IndicePalavras*)realloc(vet_Pal[idx_pal]->vet,vet_Pal[idx_pal]->tam_allcd*sizeof(IndicePalavras));
    }

    vet_Pal[idx_pal]->vet[vet_Pal[idx_pal]->tam_vet].IdxDocumento = idx_doc;
    vet_Pal[idx_pal]->vet[vet_Pal[idx_pal]->tam_vet].Frequencia = 1;
    vet_Pal[idx_pal]->tam_vet++;
}

void palavras_registra_indice(p_Palavras pal, int idx)
{
    pal->idx = idx;
}

void palavras_imprime_informacoes(p_Palavras *vet_pal, int qtd, char* palavra)
{
    p_Palavras p = (p_Palavras)calloc(1,sizeof(struct Palavras));
    p->palavra = (char*)calloc(strlen(palavra)+1,sizeof(char));
    strcpy(p->palavra,palavra);

    p_Palavras *res = (p_Palavras*)bsearch(&p,vet_pal,qtd,sizeof(p_Palavras),compara_palavras);

    printf("\n\nPALAVRA: %s\n",(*res)->palavra);
    printf("TAMANHO: %d\nINDEX: %d\n",(*res)->tam_vet,(*res)->idx);
    printf("\nIndices:\n");
    
    int i;
    for(i=0;i<(*res)->tam_vet;i++)
    {
        printf("DOC: %d FREQ: %d\n",(*res)->vet[i].IdxDocumento,(*res)->vet[i].Frequencia);
    }

    free(p->palavra);
    free(p);
}

void palavras_free(p_Palavras p)
{
    free(p->vet);
    free(p->palavra);
    free(p);
}