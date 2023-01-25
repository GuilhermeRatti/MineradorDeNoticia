#include "Documentos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int IdxPalavra;
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
    return idx1.IdxPalavra - idx2.IdxPalavra;
}

int documentos_verifica_registrado(int idx, int qtd, IndiceDocumentos* vet)
{
    IndiceDocumentos i_doc;
    i_doc.IdxPalavra=idx;

    IndiceDocumentos* item = (IndiceDocumentos*)bsearch(&i_doc,vet,qtd,sizeof(IndiceDocumentos),compara_indices_doc);

    if(item==NULL)
        return 0;
    
    item->Frequencia++;
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

    doc->classe = (char*)calloc(strlen(classe)+1,sizeof(char));
    strcpy(doc->classe,classe);

    // Pequeno while para extrair somente o nome do arquivo do diretorio do arquivo
    int i=0;
    while (1)
    {
        if(diretorio[strlen(diretorio)-i]=='/')
            break;
        
        i++;
    }

    doc->nome_doc = (char*)calloc(strlen(&diretorio[strlen(diretorio)-i+1])+1,sizeof(char));
    strcpy(doc->nome_doc,&diretorio[strlen(diretorio)-i+1]);

    return doc;
}

void documentos_imprime(p_Documentos doc)
{
    printf("\nNOME: %s\nCLASSE: %s\n",doc->nome_doc,doc->classe);
    int i;

    for(i=0;i<doc->tam_vet;i++)
    {
        printf("ID: %d, Freq: %d\n",doc->vet[i].IdxPalavra,doc->vet[i].Frequencia);
    }
}

p_Documentos documentos_registra_frequencia(p_Documentos doc, int idx_pal)
{
    
    int ja_registrada = documentos_verifica_registrado(idx_pal,doc->tam_vet,doc->vet);
    if(ja_registrada)
        return doc;

    if(doc->tam_vet==doc->tam_allcd)
    {
        doc->tam_allcd*=2;
        doc->vet = (IndiceDocumentos*)realloc(doc->vet,doc->tam_allcd*sizeof(IndiceDocumentos));
    }

    doc->vet[doc->tam_vet].IdxPalavra = idx_pal;
    doc->vet[doc->tam_vet].Frequencia = 1;
    doc->tam_vet++;
    documentos_organiza_indices(doc->tam_vet,doc->vet);

    return doc;
    // FILE *arqDoc = fopen(caminho,"r");

    // char palavra[50];
    // int idx, ja_registrada;
    // while (fscanf(arqDoc,"%s",palavra)!=EOF)
    // {
    //     idx = palavras_get_indice(vet_pal,palavra,qtd_pal);
    //     ja_registrada = documentos_verifica_registrado(idx,doc->tam_vet,doc->vet);

    //     if(ja_registrada)
    //     {
    //         palavras_registra_frequencia(vet_pal,doc->idx,idx,ja_registrada);
    //         continue;
    //     }

    //     if(doc->tam_vet==doc->tam_allcd)
    //     {
    //         doc->tam_allcd*=2;
    //         doc->vet = (IndiceDocumentos*)realloc(doc->vet,doc->tam_allcd*sizeof(IndiceDocumentos));
    //     }

    //     doc->vet[doc->tam_vet].IdxPalavra = idx;
    //     doc->vet[doc->tam_vet].Frequencia = 1;
    //     doc->tam_vet++;
    //     palavras_registra_frequencia(vet_pal,doc->idx,idx,ja_registrada);

    //     documentos_organiza_indices(doc->tam_vet,doc->vet);
    // }
    
    // fclose(arqDoc);
}

p_Documentos documentos_preenche_tfidf(p_Documentos doc, int hash_palavra, double tfidf){

    IndiceDocumentos id_doc;
    id_doc.IdxPalavra = hash_palavra;

    IndiceDocumentos * item = (IndiceDocumentos*)bsearch(&id_doc, doc->vet,doc->tam_vet,sizeof(IndiceDocumentos),compara_indices_doc);

    if(item==NULL)
        exit(printf("DEU RUIM NA GAMBIARRA DO TFIDF N SEI PQ\n"));

    (*item).TFIDF = tfidf;

    return doc;
}

/*
// Ordem de escritura:
//qtd de documetos      int
//  idx de doc              int
//	tam_vet                 int
//	tam_nome_doc            int
//	nome doc                char (* tam_nome_doc)
//  tam_nome_classe         int
//  classe                  char (* tam_nome_classe)
//  vet[tam_vet]:     
//		idx pal                 int
//		freq                    int
//		TFIDF                   double
*/
void documentos_escrever_arquivo_bin(FILE *arq, p_Documentos *vet_doc, int qtdDoc)
{
    int i=0, j=0, tam_documento = 0, tam_nome_classe = 0;

    fwrite(&qtdDoc, 1, sizeof(int), arq);//qtd de palavras int
    
    for ( i = 0; i < qtdDoc; i++)
    {
        fwrite(&(vet_doc[i]->idx), 1, sizeof(int), arq);//idx de palavra int

        fwrite(&(vet_doc[i]->tam_vet), 1, sizeof(int), arq);//tam_palavra int

        tam_documento = strlen(vet_doc[i]->nome_doc)+1;
        fwrite(&(tam_documento), 1, sizeof(int), arq);//tam_palavra int
        
        fwrite(vet_doc[i]->nome_doc, tam_documento, sizeof(char), arq);//palavra char (* tam_palavra)

        tam_nome_classe = strlen(vet_doc[i]->classe)+1;
        fwrite(&(tam_nome_classe), 1, sizeof(int), arq);//tam_palavra int

        fwrite(vet_doc[i]->classe, tam_nome_classe, sizeof(char), arq);//palavra char (* tam_palavra)
        
        for (j = 0; j < vet_doc[i]->tam_vet; j++)
        {
            fwrite(&(vet_doc[i]->vet->Frequencia), 1, sizeof(int), arq);//idx doc int

            fwrite(&(vet_doc[i]->vet->IdxPalavra), 1, sizeof(int), arq);//freq int
            
            fwrite(&(vet_doc[i]->vet->TFIDF), 1, sizeof(double), arq);//TFIDF double
        }
    }
}

void documentos_free(p_Documentos doc)
{
    free(doc->classe);
    free(doc->nome_doc);
    free(doc->vet);
    free(doc);
}