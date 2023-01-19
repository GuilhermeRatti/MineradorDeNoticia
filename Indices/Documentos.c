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

p_Documentos documentos_preenche_frequencia(p_Documentos doc, p_Palavras* vet_pal, char* caminho, int qtd_pal)
{
    FILE *arqDoc = fopen(caminho,"r");

    char palavra[50];
    int idx, ja_registrada;
    while (fscanf(arqDoc,"%s",palavra)!=EOF)
    {
        idx = palavras_get_indice(vet_pal,palavra,qtd_pal);
        ja_registrada = documentos_verifica_registrado(idx,doc->tam_vet,doc->vet);

        if(ja_registrada)
        {
            palavras_registra_frequencia(vet_pal,doc->idx,idx,ja_registrada);
            continue;
        }

        if(doc->tam_vet==doc->tam_allcd)
        {
            doc->tam_allcd*=2;
            doc->vet = (IndiceDocumentos*)realloc(doc->vet,doc->tam_allcd*sizeof(IndiceDocumentos));
        }

        doc->vet[doc->tam_vet].IdxPalavra = idx;
        doc->vet[doc->tam_vet].Frequencia = 1;
        doc->tam_vet++;
        palavras_registra_frequencia(vet_pal,doc->idx,idx,ja_registrada);

        documentos_organiza_indices(doc->tam_vet,doc->vet);
    }
    
    fclose(arqDoc);

    return doc;
}

void documentos_free(p_Documentos doc)
{
    free(doc->classe);
    free(doc->nome_doc);
    free(doc->vet);
    free(doc);
}