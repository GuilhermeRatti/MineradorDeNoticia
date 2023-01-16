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
    int tam_vet;
    char *nome_doc;
    char *classe;
    IndiceDocumentos *vet;
};

p_Documentos documentos_cria(char*classe,char*diretorio)
{
    p_Documentos doc = (p_Documentos)calloc(1,sizeof(struct Documentos));
    doc->vet = (IndiceDocumentos*)calloc(0,sizeof(IndiceDocumentos));

    doc->classe = (char*)calloc(strlen(classe),sizeof(char));
    strcpy(doc->classe,classe);

    // Pequeno while para extrair somente o nome do arquivo do diretorio do arquivo
    int i=0;
    while (1)
    {
        if(diretorio[strlen(diretorio)-i]=='/')
            break;
        
        i++;
    }

    doc->nome_doc = (char*)calloc(strlen(&diretorio[strlen(diretorio)-i+1]),sizeof(char));
    strcpy(doc->nome_doc,&diretorio[strlen(diretorio)-i+1]);

    return doc;
}