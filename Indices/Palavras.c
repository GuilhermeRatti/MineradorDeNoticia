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
    double idf;
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

p_Palavras palavras_cria(char* palavra, int pos)
{
    p_Palavras p = (p_Palavras)calloc(1,sizeof(struct Palavras));
    p->tam_allcd=1;
    p->idx = pos;
    p->vet = (IndicePalavras*)calloc(p->tam_allcd,sizeof(IndicePalavras));
    p->palavra = (char*)calloc(strlen(palavra)+1,sizeof(char));
    strcpy(p->palavra,palavra);

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

p_Palavras palavras_registra_frequencia(p_Palavras p, int doc)
{
    IndicePalavras holder;
    holder.IdxDocumento = doc;

    IndicePalavras * item = (IndicePalavras*)bsearch(&holder,p->vet,p->tam_vet,sizeof(IndicePalavras),compara_indices_pal);

    if(item!=NULL)
    {
        item->Frequencia++;

        return p;
    }

    if(p->tam_vet==p->tam_allcd)
    {
        p->tam_allcd*=2;
        p->vet =  (IndicePalavras*)realloc(p->vet,p->tam_allcd*sizeof(IndicePalavras));
    }

    p->vet[p->tam_vet].IdxDocumento = doc;
    p->vet[p->tam_vet].Frequencia = 1;
    p->tam_vet++;

    return p;
}

void palavras_registra_indice(p_Palavras pal, int idx)
{
    pal->idx = idx;
}

void palavras_imprime_informacoes(p_Palavras p)
{
    printf("\n\nPALAVRA: %s\n",p->palavra);
    printf("TAMANHO: %d\nINDEX: %d\n",p->tam_vet,p->idx);
    printf("\nIndices:\n");
    
    int i;
    for(i=0;i<p->tam_vet;i++)
    {
        printf("DOC: %d FREQ: %d TFIDF: %.2f\n",p->vet[i].IdxDocumento,p->vet[i].Frequencia,p->vet[i].TFIDF);
    }
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

p_Palavras palavras_preenche_IDF(p_Palavras p, int qtdDoc)
{   
    p->idf = calcula_idf(qtdDoc,p->tam_vet);

    return p;
}

double palavras_preenche_TFIDF(p_Palavras *p, int doc)
{
    IndicePalavras holder;
    holder.IdxDocumento = doc;

    IndicePalavras* item = (IndicePalavras*)bsearch(&holder,(*p)->vet,(*p)->tam_vet,sizeof(IndicePalavras),compara_indices_pal);
    
    (*item).TFIDF = (*p)->idf*(*item).Frequencia;

    return (*item).TFIDF;
}

double palavras_preenche_e_retorna_TFIDF(p_Palavras *p, int doc)
{
    IndicePalavras holder;
    holder.IdxDocumento = doc;

    IndicePalavras* item = (IndicePalavras*)bsearch(&holder,(*p)->vet,(*p)->tam_vet,sizeof(IndicePalavras),compara_indices_pal);
    
    (*item).TFIDF = (*p)->idf*(*item).Frequencia;

    return (*item).TFIDF;
}

/* Ordem de escritura:
    p_Palavras{                         - p_Palavras
        idx de palavra                  - int
        tam_vet                         - int
        tam_palavra                     - int
        palavra                         - char (* tam_palavra)
        vet[tam_vet]:                   - IndicePalavras
            idx doc                     - int
            freq                        - int
            TFIDF                       - double
    }
*/
void palavras_escrever_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal)
{
    int i=0, j=0, tam_palavra=0;

    for ( i = 0; i < qtdPal; i++)
    {
        fwrite(&(vet_pal[i]->idx), 1, sizeof(int), arq);//idx de palavra int

        fwrite(&(vet_pal[i]->idf),1,sizeof(double),arq); //idf da palavra double

        fwrite(&(vet_pal[i]->tam_vet), 1, sizeof(int), arq);//tam_palavra int

        tam_palavra = strlen(vet_pal[i]->palavra)+1;
        fwrite(&(tam_palavra), 1, sizeof(int), arq);//tam_palavra int
        
        fwrite(vet_pal[i]->palavra, tam_palavra, sizeof(char), arq);//palavra char (* tam_palavra)


        for (j = 0; j < vet_pal[i]->tam_vet; j++)
        {
            fwrite(&(vet_pal[i]->vet[j].IdxDocumento), 1, sizeof(int), arq);//idx doc int

            fwrite(&(vet_pal[i]->vet[j].Frequencia), 1, sizeof(int), arq);//freq int
            
            fwrite(&(vet_pal[i]->vet[j].TFIDF), 1, sizeof(double), arq);//TFIDF double
        }
    }
}

/* Ordem de leitura:
    p_Palavras{                         - p_Palavras
        idx de palavra                  - int
        tam_vet                         - int
        tam_palavra                     - int
        palavra                         - char (* tam_palavra)
        vet[tam_vet]:                   - IndicePalavras
            idx doc                     - int
            freq                        - int
            TFIDF                       - double
    }
*/
void palavras_le_arquivo_bin(FILE *arq, p_Palavras *vet_pal, int qtdPal)
{
    int i=0, j=0, tam_palavra=0;

    for ( i = 0; i < qtdPal; i++)
    {
        vet_pal[i] = (p_Palavras)calloc(1,sizeof(struct Palavras));//alocacao

        fread(&(vet_pal[i]->idx), 1, sizeof(int), arq);//idx de palavra int

        fread(&(vet_pal[i]->idf),1,sizeof(double),arq); //idf da palavra double

        fread(&(vet_pal[i]->tam_vet), 1, sizeof(int), arq);//tam_palavra int

        fread(&(tam_palavra), 1, sizeof(int), arq);//tam_palavra int
        
        vet_pal[i]->palavra = (char *)calloc(tam_palavra, sizeof(char));//alocacao
        fread(vet_pal[i]->palavra, tam_palavra, sizeof(char), arq);//palavra char (* tam_palavra)


        vet_pal[i]->vet = (IndicePalavras *)calloc(vet_pal[i]->tam_vet, sizeof(IndicePalavras));//alocacao
        vet_pal[i]->tam_allcd = vet_pal[i]->tam_vet;

        for (j = 0; j < vet_pal[i]->tam_vet; j++)
        {
            fread(&(vet_pal[i]->vet[j].IdxDocumento), 1, sizeof(int), arq);//idx doc int

            fread(&(vet_pal[i]->vet[j].Frequencia), 1, sizeof(int), arq);//freq int
            
            fread(&(vet_pal[i]->vet[j].TFIDF), 1, sizeof(double), arq);//TFIDF double
        }
    }
}

void palavras_free(p_Palavras p)
{
    free(p->vet);
    free(p->palavra);
    free(p);
}