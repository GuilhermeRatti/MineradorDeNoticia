#include "Palavras.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int IdxDocumento;
    int Frequencia;
    double TFIDF;
} IndicePalavras;

struct Palavras
{
    int tam_vet;
    char *palavra;
    IndicePalavras *vet;
};
