#include "Documentos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <math.h>

typedef struct
{
    char *palavra;
    int Frequencia;
    double TFIDF;
} IndiceDocumentos;

struct Documentos
{
    int idx;
    int tam_vet;
    int tam_allcd;
    char *nome_doc;
    char *classe;
    IndiceDocumentos *vet;
};

int compara_indices_doc(const void *a, const void *b)
{
    IndiceDocumentos idx1 = *((IndiceDocumentos*)a), idx2 = *((IndiceDocumentos*)b);
    return strcmp(idx1.palavra,idx2.palavra);
}

int compara_classe_documentos(const void *a, const void *b)
{
    p_Documentos doc1 = *((p_Documentos*)a), doc2 = *((p_Documentos*)b);
    return strcmp(doc1->classe,doc2->classe);
}

void documentos_organiza_ordem(p_Documentos *vet, int qtd)
{
    qsort(vet,qtd,sizeof(p_Documentos),compara_classe_documentos);
}

int documentos_verifica_registrado(char* palavra, double tfidf, int qtd, IndiceDocumentos* vet)
{
    IndiceDocumentos i_doc;
    i_doc.palavra = (char*)calloc(strlen(palavra)+1,sizeof(char));
    strcpy(i_doc.palavra,palavra);

    IndiceDocumentos* item = (IndiceDocumentos*)bsearch(&i_doc,vet,qtd,sizeof(IndiceDocumentos),compara_indices_doc);

    free(i_doc.palavra);
    if(item==NULL)
        return 0;
    
    item->Frequencia++;
    item->TFIDF += tfidf; // Isso eh pra poder reutilizar essa funcao na construcao de centroides... como no preenchimento de frequencia de documentos normais
                          // os tfidf dos documentos SEMPRE serao 0, somar 0 com 0 nao afetara em nada, somente ira afetar casos de centroides, que eh o objetivo
    return 1;
}

void documentos_organiza_indices(int qtd, IndiceDocumentos* vet)
{
    qsort(vet,qtd,sizeof(IndiceDocumentos),compara_indices_doc);
}

p_Documentos documentos_cria(char*classe,char*diretorio, int idx)
{
    p_Documentos doc = (p_Documentos)calloc(1,sizeof(struct Documentos));
    doc->idx = idx;
    doc->tam_allcd=1;
    doc->vet = (IndiceDocumentos*)calloc(doc->tam_allcd,sizeof(IndiceDocumentos));

    doc->classe = strdup(classe);

    // Pequeno while para extrair somente o nome do arquivo do diretorio do arquivo
    int i=0;
    while (1)
    {
        if(i==strlen(diretorio))
        {
            i++;
            break;
        }

        if(diretorio[strlen(diretorio)-i]=='/')
            break;
        
        i++;
    }
    doc->nome_doc = strdup(&diretorio[strlen(diretorio)-i+1]);

    return doc;
}

char * documentos_retorna_classe(p_Documentos doc)
{
    return doc->classe;
}

int documentos_retorna_id(p_Documentos doc)
{
    return doc->idx;
}

int documentos_retorna_quantidade_palavras(p_Documentos doc)
{
    return doc->tam_vet;
}

void documentos_imprime(p_Documentos doc)
{
    printf("\nNOME: %s\nCLASSE: %s\nN_DOCS: %d\n",doc->nome_doc,doc->classe,doc->tam_vet);
    int i;

    for(i=0;i<doc->tam_vet;i++)
    {
        printf("Palavra: %s, Freq: %d, TFIDF: %.2f\n",doc->vet[i].palavra,doc->vet[i].Frequencia,doc->vet[i].TFIDF);
    }
}

void documentos_registra_frequencia(p_Documentos doc, char *palavra)
{
    int ja_registrada = documentos_verifica_registrado(palavra,0,doc->tam_vet,doc->vet);
    if(ja_registrada)
        return;

    if(doc->tam_vet==doc->tam_allcd)
    {
        doc->tam_allcd*=2;
        doc->vet = (IndiceDocumentos*)realloc(doc->vet,doc->tam_allcd*sizeof(IndiceDocumentos));
    }

    
    doc->vet[doc->tam_vet].palavra = (char*)calloc(strlen(palavra)+1,sizeof(char));
    strcpy(doc->vet[doc->tam_vet].palavra,palavra);
    doc->vet[doc->tam_vet].Frequencia = 1;
    doc->tam_vet++;
    documentos_organiza_indices(doc->tam_vet,doc->vet);
}

int documentos_requisita_TFIDF(p_Documentos doc,char***palavras_out){

    int i;
    (*palavras_out) = (char**)calloc(doc->tam_vet,sizeof(char*));

    for(i=0;i<doc->tam_vet;i++)
    {
        (*palavras_out)[i] = doc->vet[i].palavra;
    }

    return doc->tam_vet;
}

void documentos_preenche_TFIDF(p_Documentos doc, double* tfidfs)
{
    int i;
    for(i=0;i<doc->tam_vet;i++)
    {
        doc->vet[i].TFIDF = tfidfs[i];
    }
}

void documentos_preenche_centroide(p_Documentos centroide,char**palavras,double*tfidfs,int qtd)
{
    int i,ja_registrado;
    for(i=0;i<qtd;i++)
    {
        ja_registrado = documentos_verifica_registrado(palavras[i], tfidfs[i], centroide->tam_vet, centroide->vet);
        if(ja_registrado)
            continue;

        if(centroide->tam_vet == centroide->tam_allcd)
        {
            centroide->tam_allcd*=2;
            centroide->vet = (IndiceDocumentos*)realloc(centroide->vet, centroide->tam_allcd*sizeof(IndiceDocumentos));
        }

        centroide->vet[centroide->tam_vet].Frequencia = 1;
        centroide->vet[centroide->tam_vet].palavra = strdup(palavras[i]);
        centroide->vet[centroide->tam_vet].TFIDF = tfidfs[i];
        centroide->tam_vet++;
        documentos_organiza_indices(centroide->tam_vet,centroide->vet);
    }
}

double documentos_calcula_cosseno(p_Documentos doc1, p_Documentos doc2)
{
    double upper_part=0,lower_part_doc1=0,lower_part_doc2=0, holder_tfidf=0;
    int i;
    IndiceDocumentos *item;

    for(i=0;i<doc1->tam_vet;i++)
    {
        item = (IndiceDocumentos*)bsearch(&(doc1->vet[i]),doc2->vet,doc2->tam_vet,sizeof(IndiceDocumentos),compara_indices_doc);
        if(doc1->vet[i].TFIDF!=0)
        {
            if(item==NULL)
                holder_tfidf = 0;
            else
                holder_tfidf = (*item).TFIDF;
        
            upper_part += holder_tfidf * doc1->vet[i].TFIDF;
            lower_part_doc1 += pow(doc1->vet[i].TFIDF,2);
        }
    }

    for(i=0;i<doc2->tam_vet;i++)
    {
        if(doc2->vet[i].TFIDF!=0)
            lower_part_doc2 += pow(doc2->vet[i].TFIDF,2);
    }

    if(lower_part_doc2==0)
    {
        return -1;
    }
    else if(lower_part_doc1==0)
    {
        return 0;
    }
    

    return upper_part/(sqrt(lower_part_doc1)* sqrt(lower_part_doc2));
}

void documentos_calcula_media_centroide(p_Documentos centroide)
{
    int i;
    for(i=0;i<centroide->tam_vet;i++)
    {
        centroide->vet[i].TFIDF/=centroide->vet[i].Frequencia;
    }
}

/*Ordem de escritura:
    idx de doc                      - int
    tam_vet                         - int
    tam_nome_doc                    - int
    nome doc                        - char (* tam_nome_doc)
    tam_nome_classe                 - int
    classe                          - char (* tam_nome_classe)
    vet[tam_vet]:                   - IndiceDocumentos
        tam_palavra                 - int     
        palavra                     - char *
        freq                        - int
        TFIDF                       - double
*/
void documentos_escrever_arquivo_bin(FILE *arq, p_Documentos *vet_doc, int qtdDoc)
{
    int i=0, j=0, tam_documento = 0, tam_nome_classe = 0, tam_palavra;

    for ( i = 0; i < qtdDoc; i++)
    {

        fwrite(&(vet_doc[i]->idx), 1, sizeof(int), arq);//idx de documentos int

        fwrite(&(vet_doc[i]->tam_vet), 1, sizeof(int), arq);//tam_vet int

        tam_documento = strlen(vet_doc[i]->nome_doc)+1;
        fwrite(&(tam_documento), 1, sizeof(int), arq);//tam_nome int
        
        fwrite(vet_doc[i]->nome_doc, tam_documento, sizeof(char), arq);//nome char (* tam_nome)

        tam_nome_classe = strlen(vet_doc[i]->classe)+1;
        fwrite(&(tam_nome_classe), 1, sizeof(int), arq);//tam_classe int

        fwrite(vet_doc[i]->classe, tam_nome_classe, sizeof(char), arq);//clase char (* tam_clasee)
        
        for (j = 0; j < vet_doc[i]->tam_vet; j++)
        {
            tam_palavra = strlen(vet_doc[i]->vet[j].palavra)+1;
            fwrite(&(tam_palavra), 1, sizeof(int), arq);//tam_palavra int
            
            fwrite(vet_doc[i]->vet[j].palavra, tam_palavra, sizeof(char), arq);//palavra char (* tam_palavra)

            fwrite(&(vet_doc[i]->vet[j].Frequencia), 1, sizeof(int), arq);//freq int

            fwrite(&(vet_doc[i]->vet[j].TFIDF), 1, sizeof(double), arq);//TFIDF double
        }
    }
}


/*Ordem de leitura:
    idx de doc                      - int
    tam_vet                         - int
    tam_nome_doc                    - int
    nome doc                        - char (* tam_nome_doc)
    tam_nome_classe                 - int
    classe                          - char (* tam_nome_classe)
    vet[tam_vet]:                   - IndiceDocumentos
        tam_palavra                 - int     
        palavra                     - char *
        freq                        - int
        TFIDF                       - double
*/
void documentos_le_arquivo_bin(FILE *arq, p_Documentos *vet_doc, int qtdDoc)
{
    int i=0, j=0, tam_documento = 0, tam_nome_classe = 0, tam_palavra;

    for ( i = 0; i < qtdDoc; i++)
    {
        vet_doc[i] = (p_Documentos)calloc(1,sizeof(struct Documentos));//alocacao

        fread(&(vet_doc[i]->idx), 1, sizeof(int), arq);//idx de documentos int

        fread(&(vet_doc[i]->tam_vet), 1, sizeof(int), arq);//tam_vet int

        fread(&(tam_documento), 1, sizeof(int), arq);//tam_nome int
        
        vet_doc[i]->nome_doc = (char *)calloc(tam_documento, sizeof(char));//alocacao
        fread(vet_doc[i]->nome_doc, tam_documento, sizeof(char), arq);//nome char (* tam_nome)

        fread(&(tam_nome_classe), 1, sizeof(int), arq);//tam_classe int

        vet_doc[i]->classe = (char *)calloc(tam_nome_classe, sizeof(char));//alocacao
        fread(vet_doc[i]->classe, tam_nome_classe, sizeof(char), arq);//clase char (* tam_clasee)
        
        vet_doc[i]->vet = (IndiceDocumentos *)calloc(vet_doc[i]->tam_vet, sizeof(IndiceDocumentos));//alocacao
        vet_doc[i]->tam_allcd = vet_doc[i]->tam_vet;
        for (j = 0; j < vet_doc[i]->tam_vet; j++)
        {
            fread(&(tam_palavra), 1, sizeof(int), arq);//tam_palavra int
            
            vet_doc[i]->vet[j].palavra = (char *)calloc(tam_palavra, sizeof(char));//alocacao
            fread(vet_doc[i]->vet[j].palavra, tam_palavra, sizeof(char), arq);//palavra char (* tam_palavra)

            fread(&(vet_doc[i]->vet[j].Frequencia), 1, sizeof(int), arq);//freq int

            fread(&(vet_doc[i]->vet[j].TFIDF), 1, sizeof(double), arq);//TFIDF double
        }
    }
}


void documentos_free(p_Documentos doc)
{
    int i;

    free(doc->classe);
    free(doc->nome_doc);
    
    for(i=0;i<doc->tam_vet;i++)
    {
        free(doc->vet[i].palavra);
    }
    
    free(doc->vet);
    free(doc);
}