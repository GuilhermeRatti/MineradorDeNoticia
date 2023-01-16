#include "Documentos.h"
#include <stdio.h>
#include <stdlib.h>

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
    char classe[5];
    IndiceDocumentos *vet;
};
