#pragma once

typedef struct Palavras *p_Palavras;

p_Palavras palavras_cria(p_Palavras*,int,char*);

void palavras_organiza_ordem(p_Palavras*,int);

int palavras_verifica_existencia(p_Palavras*,int,p_Palavras);