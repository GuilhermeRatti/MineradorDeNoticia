#include "Palavras.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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


//n == total docs // df == qtd de docs em que apareceu
double calcula_idf(int n, int df){
    double res=0;
    
    res = log((1+n)/(1+df)) + 1;
    
    //res = (1+n)/(1+df);
    //res = log(res);
    //res += 1;

    return res;
}

void palavras_preenche_tfidf(p_Palavras *vet_pal, int qtdPal, int qtdDoc)
{
    int i=0,j=0;
    double idf=0;

    for (i = 0; i < qtdPal; i++)
    {
        idf = calcula_idf(qtdDoc, vet_pal[i]->tam_vet);

        for (j = 0; j < vet_pal[i]->tam_vet; j++)
        {
            vet_pal[i]->vet[j].TFIDF = (vet_pal[i]->vet[j].Frequencia * idf);
        }
    }
    
}

double palavras_busca_TFIDF(p_Palavras *vet_pal, int qtdPal, int idxDocPesq, int idxPalAlvo)
{
    IndicePalavras dummie;
    dummie.IdxDocumento = idxDocPesq;

    return ((IndicePalavras *)bsearch(&dummie, vet_pal[idxPalAlvo]->vet, vet_pal[idxPalAlvo]->tam_vet, sizeof(IndicePalavras), compara_indices_pal))->TFIDF;

}

/* Ordem de escritura:
//qtd de palavras         int
//	idx de palavra          int
//	tam_vet                 int
//	tam_palavra             int
//	palavra                 char (* tam_palavra)
	//vet[tam_vet]:     
//		idx doc                 int
//		freq                    int
//		TFIDF                   double
*/
void palavras_escrever_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal)
{
    int i=0, j=0, tam_palavra=0;

    fwrite(&qtdPal, 1, sizeof(int), arq);//qtd de palavras int
    
    for ( i = 0; i < qtdPal; i++)
    {
        fwrite(&(vet_pal[i]->idx), 1, sizeof(int), arq);//idx de palavra int

        fwrite(&(vet_pal[i]->tam_vet), 1, sizeof(int), arq);//tam_palavra int

        tam_palavra = strlen(vet_pal[i]->palavra)+1;
        fwrite(&(tam_palavra), 1, sizeof(int), arq);//tam_palavra int
        
        fwrite(vet_pal[i]->palavra, tam_palavra, sizeof(char), arq);//palavra char (* tam_palavra)

        for (j = 0; j < vet_pal[i]->tam_vet; j++)
        {
            fwrite(&(vet_pal[i]->vet->Frequencia), 1, sizeof(int), arq);//idx doc int

            fwrite(&(vet_pal[i]->vet->IdxDocumento), 1, sizeof(int), arq);//freq int
            
            fwrite(&(vet_pal[i]->vet->TFIDF), 1, sizeof(double), arq);//TFIDF double
        }
    }
    

}

void palavras_free(p_Palavras p)
{
    free(p->vet);
    free(p->palavra);
    free(p);
}