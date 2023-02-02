#include <stdio.h>
#include <stdlib.h>
#include "FileManager.h"
#include "Classificadores.h"
#include "HashTable.h"
#include <string.h>
#include <ctype.h>

#define MULTIPLICADOR_TEXTO 2


void executa_menu(p_HashTable table);
void executa_buscar_noticias(p_HashTable table);
void executa_classificar_noticias(p_HashTable table, int*qtd_novos_textos);
void executa_relatorio_noticias(p_HashTable table);
void executa_relatorio_documento(p_HashTable table);

int main(int argc, char const *argv[])
{
    if(argc < 2)
    {
        exit(printf("QUANTIDADE INSUFICIENTE DE INFORMACOES DISPONIBILIZADAS!\nINFORME O ARQUIVO BIN E O NUMERO K DE VIZINHOS!!\n"));
    }

    if( argv[1][strlen(argv[1])-4]!='.' ||
        tolower(argv[1][strlen(argv[1])-3])!='b' || 
        tolower(argv[1][strlen(argv[1])-2])!='i' || 
        tolower(argv[1][strlen(argv[1])-1])!='n')
    {
        exit(printf("NOME DO ARQUIVO DE ENTRADA ESTA FORMATADO INCORRETAMENTE!\nO FORMATO DEVE TERMINAR EM '.bin'!!\n"));
    }

    //**Parte da leitura do arquivo.bin para preenchimento da tabela HASH:

    p_HashTable table = hash_initialize_table();
    
    hash_le_arquivo_bin(table, argv[1]);

    //**Parte do menu:

    executa_menu(table);

    //**
    //hash_imprime_palavra(table,"de");
    //hash_imprime_documento(table,1);

    //hash_print_amount_of_items(table);

    hash_free(table);
}


void executa_menu(p_HashTable table)
{
    int qtd_novos_docs_registrados=0;
    int opcao = -1;

    while (opcao)
    {
        printf("\n=> Programa Principal:\n1 - Buscar noticias\n2 - Classificar noticias\n3 - Relatorio de palavras\n4 - Relatorio de documentos\n0 - Sair\n\n\tOpcao: ");
        scanf("%d", &opcao);
        
        if (opcao == 1)
        {
            executa_buscar_noticias(table);
        }
        else if (opcao == 2)
        {
            executa_classificar_noticias(table, &qtd_novos_docs_registrados);
        }
        else if (opcao == 3)
        {
            executa_relatorio_noticias(table);
        }
        else if (opcao == 4)
        {
            executa_relatorio_documento(table);
        }
        else if (opcao != 0)  
        {
            printf("\nOpcao invalida!\n");
        }
    }
}

void executa_buscar_noticias(p_HashTable table)
{
    int texto_alc = 125, texto_tam = 0;
    char char_atual = ' ';
    char *texto = calloc(texto_alc+1, sizeof(char));

    printf("\n=> Buscar noticias:\n\n\tDigite o texto para busca: ");
    
    scanf("\n%c", &char_atual);
    while (char_atual != '\n')
    {
        if (texto_tam >= texto_alc)
        {
            texto_alc *= MULTIPLICADOR_TEXTO;
            texto = realloc(texto, texto_alc * sizeof(char));
        }
        
        texto[texto_tam] = char_atual;
        texto_tam++;
        
        scanf("%c", &char_atual);
    }

    texto[texto_tam] = '\0';
    
    hash_buscar_noticias(table, texto);

    free(texto);
}

void executa_classificar_noticias(p_HashTable table, int *qtd_novos_docs)
{
    int texto_alc = 125, texto_tam = 0;
    char char_atual = ' ';
    char *texto = calloc(texto_alc+1, sizeof(char));

    int qtd_docs_treino = hash_retorna_qtd_doc(table);

    printf("\n=> Classificar noticias:\n\n\tDigite o texto para classificar: ");
    
    scanf("\n%c", &char_atual);
    while (char_atual != '\n')
    {
        if (texto_tam >= texto_alc)
        {
            texto_alc *= MULTIPLICADOR_TEXTO;
            texto = realloc(texto, texto_alc * sizeof(char));
        }
        
        texto[texto_tam] = char_atual;
        texto_tam++;
        
        scanf("%c", &char_atual);
    }

    texto[texto_tam] = '\0';

    int opt,k_vizinhos=0;
    printf("\n\n\tDigite 1 ou 2 para escolher entre um classificador KNN ou Centroide mais proximo, respectivamente: ");
    scanf("%d",&opt);

    char nome[50],classe[4];

    sprintf(nome,"Teste%d.txt",(*qtd_novos_docs+1));
    sprintf(classe,"tbd");
    p_Documentos doc = documentos_cria(classe,nome,qtd_docs_treino+(*qtd_novos_docs));
    hash_registra_noticia_do_terminal(table,doc,texto);
    hash_calcula_TFIDF_do_doc(table,doc,qtd_docs_treino+(*qtd_novos_docs),TESTE);

    if(opt==K_NEAREST_NEIGHBOURS)
    {
        printf("\tDigite a quantidade de K vizinhos a ser analisada: ");
        scanf("%d",&k_vizinhos);
    }

    char* classe_predita = hash_classifica_nova_noticia(table,hash_retorna_qtd_doc(table)-1,*qtd_novos_docs,opt,k_vizinhos);

    printf("\n\t####################################\n");
    printf("\t####### CLASSE PREDITA: %s ########\n",classe_predita);
    printf("\t####################################\n");

    (*qtd_novos_docs)++;

    free(texto);
}

void executa_relatorio_noticias(p_HashTable table)
{
    char palavra_relatorio[50], lixo;
 
    printf("\n=> Relatorio de noticias:\n\n\tDigite a palavra para busca: ");
    
    scanf("\n%s", palavra_relatorio);
    
    //elimina o espaço
    do{
        lixo = fgetc(stdin);
    } while (lixo == ' ');

    //verifica se foi digitado mais que uma palavra
    if (lixo != EOF && lixo != '\n')
    {
        do{
            lixo = fgetc(stdin);
        } while (lixo != EOF && lixo != '\n');

        printf("\nDigite apenas uma palavra!\n");
    }
    else
    {
        hash_relatorio_noticias(table, palavra_relatorio);
    }
}

void executa_relatorio_documento(p_HashTable table)
{
    hash_relatorio_documentos(table);
}