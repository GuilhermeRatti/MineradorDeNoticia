#include "../headers/HashTable.h"
#include "../headers/Palavras.h"
#include "../headers/Documentos.h"
#include "../headers/Classificadores.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int SIZE_OF_TABLE = 23131;

typedef struct
{
    p_Palavras *vet_indice;
    int qtd_palavras_no_indice;
} HashIndex;

/*
    Apesar de haver uma lista de palavras e de documentos/classes no hash,
    o hash so será calculado para as palavras; nao ha necessidade
    de calcular o hash dos documentos, visto a linearidade da leitura
    dos documentos e a impossibilidade de haver documentos duplicados
*/
struct HashTable
{
    HashIndex *pal_table;
    p_Documentos *doc_table;
    p_Documentos *class_table;

    int doc_allcd,
        pal_allcd,
        class_allcd;

    int qtd_doc,
        qtd_pal,
        qtd_class;
};

/*
Estrutura local para organização de identificadores com relação a algum fator numerico
*/
typedef struct
{
    long int identificador;
    double fator;
    char *rotulo;
} Organizador;

// Verifica se um identificador ja esta presente em um vetor de organizadores, retornando seu id ou -1 se não econtrado
int organizador_verifica_existencia(Organizador *vet_org, int tam_vet, long int identificador);

// Cria  um novo organizador e o adiciona a um vetor de organizadores
Organizador *organizador_adiciona(Organizador *vet_org, int *tam_vet, long int identificador, double fator, char *rotulo);

/*Recebe:
 *- Um vetor de Organizador organizado
 *- Um inteiro do tamanho do vetor
 *- Um inteiro para limita a quantidade de impressoes (ignorado caso passe do tamnho do vetor)
 *- Um char que pode assumir possibilita 4 metodos de impressao:
 *    - 'n' -> Numerico:          imprime identificador e o fator         | trata o identificador como um numero
 *    - 's' -> String:            imprime identificador e o fator         | trata o identificador como uma string
 *    - 'N' -> Numerico Triplo:   imprime identificador ,fator e o rotulo | trata o identificador como um numero
 *    - 'S' -> String Triplo:     imprime identificador ,fator e o rotulo | trata o identificador como uma string
 */
void organizador_imprime(Organizador *vet_org, int tam_vet, int limitador, char modo);

// Compara os fatores dos organizadores: F1 > F2 retorna 1; F1 < F2 retorna -1; F1 == F2 retorna 0;
int organizador_sorteia_crescente(const void *elemento_1, const void *elemento_2);

// Compara os fatores dos organizadores: F1 > F2 retorna -1; F1 < F2 retorna 1; F1 == F2 retorna 0;
int organizador_sorteia_decrescente(const void *elemento_1, const void *elemento_2);

// Libera um vetor de organizadores
void organizador_free(Organizador *vet_org, int tam_vet);

/*
- Usa os centroides para conseguir um identificador unico comparando a classe do documento com a clase do centroide.
- O vetor de centroides tem apenas strings de classes unicas. Essas strings sao ponteiros que sao salvas como um numero,
assim é possivel fazer um identificadr unico que tambem referencia a uma string.
*/
char *classe_retorna_string_unica(p_HashTable table, p_Documentos doc);

p_HashTable hash_initialize_table()
{
    p_HashTable hash_table = (p_HashTable)calloc(1, sizeof(struct HashTable));
    hash_table->doc_allcd = 10;
    hash_table->pal_allcd = SIZE_OF_TABLE; // Um numero primo maior que 20000
    hash_table->class_allcd = 1;

    hash_table->doc_table = (p_Documentos *)calloc(hash_table->doc_allcd, sizeof(p_Documentos));
    hash_table->pal_table = (HashIndex *)calloc(hash_table->pal_allcd, sizeof(HashIndex));
    hash_table->class_table = (p_Documentos *)calloc(hash_table->class_allcd, sizeof(p_Documentos));

    return hash_table;
}

int hash_get_index(char *palavra)
{
    int i, hash = 0, tam = strlen(palavra);

    for (i = 0; i < tam; i++)
    {
        hash += palavra[i] * (i + 1);
    }

    return hash % SIZE_OF_TABLE;
}

int hash_retorna_qtd_doc(p_HashTable table)
{
    return table->qtd_doc;
}

void hash_register_new_doc(p_HashTable table, p_Documentos doc)
{

    if (table->qtd_doc == table->doc_allcd)
    {
        table->doc_allcd *= 2;
        table->doc_table = (p_Documentos *)realloc(table->doc_table, table->doc_allcd * sizeof(p_Documentos));
    }

    table->doc_table[table->qtd_doc] = doc;

    p_Documentos *item = (p_Documentos *)bsearch(&(doc), table->class_table, table->qtd_class, sizeof(p_Documentos), compara_classe_documentos);

    if (item == NULL)
    {
        if (table->qtd_class == table->class_allcd)
        {
            table->class_allcd *= 2;
            table->class_table = (p_Documentos *)realloc(table->class_table, table->class_allcd * sizeof(p_Documentos));
        }

        char *class_name = documentos_retorna_classe(doc);
        table->class_table[table->qtd_class] = documentos_cria(class_name, class_name, table->qtd_class);
        table->qtd_class++;
        // printf("QTD CLASSE: %d\n",table->qtd_class);
        documentos_organiza_ordem(table->class_table, table->qtd_class);
    }

    table->qtd_doc++;
}

void hash_register_new_item(p_HashTable table, p_Documentos doc, char *palavra, int p_index)
{

    p_Palavras p = NULL;
    p = palavras_cria(palavra, p_index);
    int pos_no_indice = table->pal_table[p_index].qtd_palavras_no_indice;
    documentos_registra_frequencia(doc, palavra);

    p_Palavras *pal_item = (p_Palavras *)bsearch(&p,
                                                 table->pal_table[p_index].vet_indice,
                                                 table->pal_table[p_index].qtd_palavras_no_indice,
                                                 sizeof(p_Palavras),
                                                 compara_palavras);

    if (pal_item != NULL)
    {

        palavras_registra_frequencia((*pal_item), documentos_retorna_id(doc));
        palavras_free(p);
        return;
    }

    table->qtd_pal++;
    table->pal_table[p_index].qtd_palavras_no_indice++;
    if (table->pal_table[p_index].qtd_palavras_no_indice <= 1)
        table->pal_table[p_index].vet_indice = (p_Palavras *)calloc(1, sizeof(p_Palavras));

    else
        table->pal_table[p_index].vet_indice = (p_Palavras *)realloc(table->pal_table[p_index].vet_indice,
                                                                     table->pal_table[p_index].qtd_palavras_no_indice * sizeof(p_Palavras));

    palavras_registra_frequencia(p, documentos_retorna_id(doc));
    table->pal_table[p_index].vet_indice[pos_no_indice] = p;
    palavras_organiza_ordem(table->pal_table[p_index].vet_indice, table->pal_table[p_index].qtd_palavras_no_indice);
}

void hash_print_amount_of_items(p_HashTable table)
{
    printf("Quantidade de documentos: %d\n", table->qtd_doc);
    printf("Quantidade total de palavras diferentes: %d\n", table->qtd_pal);
}

void hash_imprime_palavra(p_HashTable table, char *palavra)
{
    int hash = hash_get_index(palavra), i, match = 0;
    HashIndex desired_index = table->pal_table[hash];

    p_Palavras p = palavras_cria(palavra, 0); // Encapsulando a palavra pra fazer comparacao

    for (i = 0; i < desired_index.qtd_palavras_no_indice; i++)
    {
        match = compara_palavras(&(desired_index.vet_indice[i]), &p);

        if (!match)
        {
            palavras_imprime_informacoes(desired_index.vet_indice[i]);
            break;
        }
    }

    palavras_free(p);

    if (match != 0)
    {
        printf("PALAVRA '%s' NAO ENCONTRADA!!\nERA PRA ELA ESTAR REGISTRADA!?!?\n\n", palavra);
    }
}

void hash_imprime_documento(p_HashTable table, int posicao)
{
    documentos_imprime(table->doc_table[posicao]);
    documentos_imprime(table->class_table[posicao]);
}

void hash_calcula_IDF(p_HashTable table)
{
    int i, j;
    for (i = 0; i < SIZE_OF_TABLE; i++)
    {
        for (j = 0; j < table->pal_table[i].qtd_palavras_no_indice; j++)
        {
            palavras_preenche_IDF(table->pal_table[i].vet_indice[j], table->qtd_doc);
        }
    }
}

void hash_calcula_TFIDF_em_massa(p_HashTable table, int beginning)
{
    int i;
    for (i = beginning; i < table->qtd_doc; i++)
    {
        hash_calcula_TFIDF_do_doc(table, table->doc_table[i], i, TRAIN);
    }
}

void hash_calcula_TFIDF_do_doc(p_HashTable table, p_Documentos doc, int posicao, TIPO_LEITURA opt)
{
    int j, hash;
    char **palavras;
    int qtd;
    qtd = documentos_requisita_TFIDF(doc, &palavras);
    double tfidf[qtd];

    for (j = 0; j < qtd; j++)
    {
        hash = hash_get_index(palavras[j]);
        p_Palavras p = palavras_cria(palavras[j], hash);

        p_Palavras *item = (p_Palavras *)bsearch(&p,
                                                 table->pal_table[hash].vet_indice,
                                                 table->pal_table[hash].qtd_palavras_no_indice,
                                                 sizeof(p_Palavras),
                                                 compara_palavras);
        if (item == NULL)
        {
            hash_free(table);
            exit(printf("ALGO DE MUITO ERRADO ACONTECEU, N ACHEI UMA PALAVRA Q ERA PRA JA ESTAR REGISTRADA!!\n"));
        }

        palavras_free(p);

        tfidf[j] = palavras_busca_e_preenche_TFIDF((*item), posicao);
    }

    if (opt == TRAIN)
    {
        p_Documentos *doc_item = (p_Documentos *)bsearch(&doc,
                                                         table->class_table,
                                                         table->qtd_class,
                                                         sizeof(p_Documentos),
                                                         compara_classe_documentos);

        if (doc_item == NULL)
        {
            hash_free(table);
            exit(printf("ALGO DE MUITO ERRADO ACONTECEU, NAO ENCONTREI UM CENTROIDE DE CLASSE QUE ERA PRA ESTAR REGISTRADO\n"));
        }

        documentos_preenche_centroide((*doc_item), palavras, tfidf, qtd);
    }
    free(palavras);

    documentos_preenche_TFIDF(doc, tfidf);
}

void hash_calcula_centroides(p_HashTable table)
{
    int i;

    for (i = 0; i < table->qtd_class; i++)
    {
        documentos_calcula_media_centroide(table->class_table[i]);
        // documentos_imprime(table->class_table[i]);
    }
}

void hash_escrever_arquivo_bin(p_HashTable table, const char *caminho_bin)
{
    int i = 0, qtd_pal_local = 0;

    FILE *arq = fopen(caminho_bin, "wb");

    if (arq == NULL)
    {
        hash_free(table);
        exit(printf("ERRO: FALHA AO CRIAR ARQUIVO BINARIO.\n"));
    }

    fwrite(&(table->qtd_pal), 1, sizeof(int), arq); // qtd de palavras - int

    // pra cada palavra na tabela
    for (i = 0; i < SIZE_OF_TABLE; i++)
    {
        qtd_pal_local = table->pal_table[i].qtd_palavras_no_indice;

        fwrite(&(qtd_pal_local), 1, sizeof(int), arq); // qtd de palavras - int (para montar a HashIndex)

        palavras_escrever_arquivo_bin(arq, table->pal_table[i].vet_indice, qtd_pal_local);
    }

    fwrite(&(table->qtd_doc), 1, sizeof(int), arq); // qtd de documentos -int

    documentos_escrever_arquivo_bin(arq, table->doc_table, table->qtd_doc);

    fwrite(&(table->qtd_class), 1, sizeof(int), arq); // qtd de classes - int

    documentos_escrever_arquivo_bin(arq, table->class_table, table->qtd_class);

    fclose(arq);
}

void hash_le_arquivo_bin(p_HashTable table, const char *caminho_bin)
{

    int i = 0, qtd_pal_local = 0;

    FILE *arq = fopen(caminho_bin, "rb");

    if (arq == NULL)
    {
        hash_free(table);
        exit(printf("ERRO: FALHA AO ABRIR ARQUIVO BINARIO. VERIFIQUE O NOME DO ARQUIVO INFORMADO.\n"));
    }

    fread(&(table->qtd_pal), 1, sizeof(int), arq); // qtd_pal - int

    for (i = 0; i < SIZE_OF_TABLE; i++)
    {
        fread(&(qtd_pal_local), 1, sizeof(int), arq); // qtd_palavras_indice - int

        table->pal_table[i].qtd_palavras_no_indice = qtd_pal_local;

        if (qtd_pal_local != 0) // pula o hash atual caso nao haja palavras nele
        {
            table->pal_table[i].vet_indice = (p_Palavras *)calloc(qtd_pal_local, sizeof(p_Palavras));

            palavras_le_arquivo_bin(arq, table->pal_table[i].vet_indice, qtd_pal_local);
        }
    }

    fread(&(table->qtd_doc), 1, sizeof(int), arq); // qtd_doc - int

    table->doc_table = (p_Documentos *)realloc(table->doc_table, table->qtd_doc * sizeof(p_Documentos));

    documentos_le_arquivo_bin(arq, table->doc_table, table->qtd_doc);

    fread(&(table->qtd_class), 1, sizeof(int), arq); // qtd_cls - int

    table->class_table = (p_Documentos *)realloc(table->class_table, table->qtd_class * sizeof(p_Documentos));

    documentos_le_arquivo_bin(arq, table->class_table, table->qtd_class);

    table->doc_allcd = table->qtd_doc;
    table->class_allcd = table->qtd_class;

    fclose(arq);
}

int hash_palavra_verfica_existencia(p_HashTable table, char *palavra_alvo)
{
    // possivel indice armazena o indice da tabela hash onde a palavra alvo pode estar contida
    HashIndex possivel_indice = table->pal_table[hash_get_index(palavra_alvo)];

    return palavras_verifica_existencia(possivel_indice.vet_indice, possivel_indice.qtd_palavras_no_indice, palavra_alvo);
}

void hash_buscar_noticias(p_HashTable table, char *texto)
{

    int qtd_org_doc = 0, qtd_org_pal = 0, indice_pal_hash = -1, i = 0;

    char *palavra_atual;

    p_Palavras palavra_tabela;

    // identificador == idx_doc | fator == TF-IDF acumulado
    Organizador *vet_org_doc = (Organizador *)calloc(qtd_org_doc, sizeof(Organizador));

    // identificador == ponteiro da palavra na tabela | fator == 0 (desnecessario nesse caso)
    Organizador *vet_org_pal = (Organizador *)calloc(qtd_org_pal, sizeof(Organizador));

    palavra_atual = strtok(texto, " ");

    while (palavra_atual != NULL)
    {
        indice_pal_hash = hash_palavra_verfica_existencia(table, palavra_atual);
        // verifica se a plavra existe na hash
        // verifica se a palavra ja foi adicionada

        if (indice_pal_hash != -1)
        {
            // pega a palavra da tabela
            palavra_tabela = table->pal_table[hash_get_index(palavra_atual)].vet_indice[indice_pal_hash];

            // verifica se a palavra já foi lida em alguma iteracao anterior
            if (organizador_verifica_existencia(vet_org_pal, qtd_org_pal, (long int)palavra_tabela) == -1)
            {
                // adiciona no vetor para impedir que a mesma palavra seja lida duas vezes
                vet_org_pal = organizador_adiciona(vet_org_pal, &qtd_org_pal, (long int)palavra_tabela, 0, "");

                // extrai informacoes dos documentos associados com a plavra (necessita liberacao manual)
                int *palavra_docs_ids = palavras_retorna_docs_ids(palavra_tabela);
                double *palavra_docs_TFIDFs = palavras_retorna_docs_TFIFDs(palavra_tabela);

                // adiciona no vetor os dados dos documentos
                for (i = 0; i < palavras_retorna_docs_quantidade(palavra_tabela); i++)
                {
                    // verifica se o documento associados a palavra estao contido dentro dos limites dos documentos
                    if (palavra_docs_ids[i]<table->qtd_doc)
                    {
                        vet_org_doc = organizador_adiciona(vet_org_doc, &qtd_org_doc, palavra_docs_ids[i], palavra_docs_TFIDFs[i], "");
                    }
                }

                // libera os vetores alocados dentro da função de retorno
                free(palavra_docs_ids);
                free(palavra_docs_TFIDFs);
            }
        }
        palavra_atual = strtok(NULL, " ");
    }

    // verifica se os termos foram encontrados
    if (qtd_org_doc != 0)
    {
        qsort(vet_org_doc, qtd_org_doc, sizeof(Organizador), organizador_sorteia_decrescente);

        printf("\nResultado da busca:\n\nIDX_DOC\t\t|TF-IDF\n");
        organizador_imprime(vet_org_doc, qtd_org_doc, 10, 'n');
    }
    else
    {
        printf("\nNenhum termo foi encontrado!\n");
    }

    // libera organizadores
    organizador_free(vet_org_doc, qtd_org_doc);
    organizador_free(vet_org_pal, qtd_org_pal);
}

void hash_registra_noticia_do_terminal(p_HashTable table, p_Documentos doc, char *texto)
{
    int indice_pal_hash;

    char *palavra_atual;

    palavra_atual = strtok(texto, " ");

    while (palavra_atual != NULL)
    {
        indice_pal_hash = hash_get_index(palavra_atual);
        // verifica se a plavra existe na hash
        // verifica se a palavra ja foi adicionada

        hash_register_new_item(table, doc, palavra_atual, indice_pal_hash);

        palavra_atual = strtok(NULL, " ");
    }
}

char *hash_classificar_noticias(p_HashTable table, p_Documentos doc, TIPOS_DISPONIVEIS opcao, int k_vizinhos)
{

    Classificador modelo = classificadores_retorna_tipo(opcao);
    if (modelo == NULL)
    {
        documentos_free(doc);
        hash_free(table);
        exit(printf("ERRO: Opcao invalida!\n"));
    }
    p_Documentos *dataset;
    int qtd_dados;

    if (opcao == K_NEAREST_NEIGHBOURS)
    {

        dataset = table->doc_table;
        qtd_dados = table->qtd_doc; // pegar somente os textos que vieram da base de treino
    }
    else
    {
        dataset = table->class_table;
        qtd_dados = table->qtd_class;
    }

    return modelo(dataset, qtd_dados, doc, k_vizinhos);
}

void hash_relatorio_palavras(p_HashTable table, char *palavra_relatorio)
{
    int qtd_org_doc = 0, qtd_org_cls = 0, i = 0;
    p_Palavras palavra_tabela;
    // identificador == idx_doc | fator == frquencia
    Organizador *vet_org_doc = (Organizador *)calloc(qtd_org_doc, sizeof(Organizador));

    // identificador == ponteiro para nome da classe | fator == frequencia
    Organizador *vet_org_cls = (Organizador *)calloc(qtd_org_cls, sizeof(Organizador));

    int indice_pal_hash = hash_palavra_verfica_existencia(table, palavra_relatorio);

    if (indice_pal_hash != -1)
    {
        // pega a palavra da tabela
        palavra_tabela = table->pal_table[hash_get_index(palavra_relatorio)].vet_indice[indice_pal_hash];

        // extrai informacoes dos documentos associados com a plavra (necessita liberacao manual)
        int *palavra_docs_ids = palavras_retorna_docs_ids(palavra_tabela);
        int *palavras_docs_frequencia = palavras_retorna_docs_frequencia(palavra_tabela);

        int qtd_docs_palavra = palavras_retorna_docs_quantidade(palavra_tabela);
        // adiciona no vetor os dados dos documentos
        for (i = 0; i < qtd_docs_palavra; i++)
        {
            // verifica se o documento associados a palavra estao contido dentro dos limites dos documentos
            if (palavra_docs_ids[i] < table->qtd_doc)
            {
                vet_org_doc = organizador_adiciona(vet_org_doc, &qtd_org_doc, palavra_docs_ids[i], palavras_docs_frequencia[i], "");
            }
        }

        for (i = 0; i < qtd_org_doc; i++)
        { // usamos os ids no pirmeiro organizador para pegar a frequencia dos docs em que a palavra aparece junto de sua classe
            p_Documentos doc_atual = table->doc_table[vet_org_doc[i].identificador];
            vet_org_cls = organizador_adiciona(vet_org_cls, &qtd_org_cls, (long int)classe_retorna_string_unica(table, doc_atual), vet_org_doc[i].fator, "");
        }

        // libera os vetores alocados dentro da função de retorno
        free(palavra_docs_ids);
        free(palavras_docs_frequencia);

        qsort(vet_org_doc, qtd_org_doc, sizeof(Organizador), organizador_sorteia_decrescente);
        qsort(vet_org_cls, qtd_org_cls, sizeof(Organizador), organizador_sorteia_decrescente);

        // imprime o resultado da busca
        printf("\nResultado da busca:\n\nIDX_DOC\t\t|Frequencia\n");
        organizador_imprime(vet_org_doc, qtd_org_doc, 10, 'n');

        printf("\n\nClasse\t\t|Frequencia\n");
        organizador_imprime(vet_org_cls, qtd_org_cls, 10, 's');
    }
    else
    {
        printf("\nTermo nao encontrado!\n");
    }
    // libera os organizadores
    organizador_free(vet_org_doc, qtd_org_doc);
    organizador_free(vet_org_cls, qtd_org_cls);
}

void hash_relatorio_documentos(p_HashTable table)
{
    int qtd_org_doc = 0, i = 0;
    p_Documentos documento_atual;
    // identificador == idx_doc | fator == frquencia
    Organizador *vet_org_doc = (Organizador *)calloc(qtd_org_doc, sizeof(Organizador));

    //Adiciona os documentos no organizador
    for (i = 0; i < table->qtd_doc; i++)
    {
        documento_atual = table->doc_table[i];
        vet_org_doc = organizador_adiciona(vet_org_doc, &qtd_org_doc, documentos_retorna_id(documento_atual), documentos_retorna_quantidade_palavras(documento_atual), documentos_retorna_classe(documento_atual));
    }

    //Organiza e imprime os documentos em ordem decrscente
    qsort(vet_org_doc, qtd_org_doc, sizeof(Organizador), organizador_sorteia_decrescente);
    printf("\nResultado da busca:\n\nIDX_DOC\t\t|Qtd Palavras\t|Classe\n");
    organizador_imprime(vet_org_doc, qtd_org_doc, 10, 'N');

    //Organiza e imprime os documentos em ordem crscente
    qsort(vet_org_doc, qtd_org_doc, sizeof(Organizador), organizador_sorteia_crescente);
    printf("\n\nIDX_DOC\t\t|Qtd Palavras\t|Classe\n");
    organizador_imprime(vet_org_doc, qtd_org_doc, 10, 'N');

    organizador_free(vet_org_doc, qtd_org_doc);
}

char *classe_retorna_string_unica(p_HashTable table, p_Documentos doc)
{
    int i;
    for (i = 0; i < table->qtd_class; i++)
    {
        if (compara_classe_documentos(&(table->class_table[i]), &(doc)) == 0)
        {
            return documentos_retorna_classe(table->class_table[i]);
        }
    }

    return NULL;
}

int organizador_verifica_existencia(Organizador *vet_org, int tam_vet, long int identificador)
{
    int i;
    for (i = 0; i < tam_vet; i++)
    {
        if (vet_org[i].identificador == identificador)
        {
            return i;
        }
    }
    return -1;
}

Organizador *organizador_adiciona(Organizador *vet_org, int *tam_vet, long int identificador, double fator, char *rotulo)
{
    //Verifica se o identificador ja esta presente no vetor
    int id = organizador_verifica_existencia(vet_org, *tam_vet, identificador);

    //se não estiver presente, adiciona; se estiver presente, aumenta o fator dele 
    if (id == -1)
    {
        vet_org = (Organizador *)realloc(vet_org, ((*tam_vet) + 1) * sizeof(Organizador));

        vet_org[*tam_vet].fator = fator;
        vet_org[*tam_vet].identificador = identificador;
        vet_org[*tam_vet].rotulo = calloc(strlen(rotulo) + 1, sizeof(char));
        strcpy(vet_org[*tam_vet].rotulo, rotulo);
        *tam_vet += 1;

        return vet_org;
    }
    else
    {
        vet_org[id].fator += fator;

        return vet_org;
    }
}

void organizador_imprime(Organizador *vet_org, int tam_vet, int limitador, char modo)
{
    int i = 0, qtd_impressao = 0;
    //Verifica se o limitador ta dentro dos limites do vetor
    if (limitador > 0 && limitador < tam_vet)
    {
        qtd_impressao = limitador;
    }
    else
    {
        qtd_impressao = tam_vet;
    }

    //Verifica o modo de impressao
    if (modo == 'n')
    {
        for (i = 0; i < qtd_impressao; i++)
        {
            printf("%ld\t\t|%.6f\n", vet_org[i].identificador, vet_org[i].fator);
        }
    }
    else if (modo == 's')
    {
        for (i = 0; i < qtd_impressao; i++)
        {
            printf("%s\t\t|%.6f\n", (char *)vet_org[i].identificador, vet_org[i].fator);
        }
    }
    else if (modo == 'N')
    {
        for (i = 0; i < qtd_impressao; i++)
        {
            printf("%ld\t\t|%.6f\t|%s\n", vet_org[i].identificador, vet_org[i].fator, vet_org[i].rotulo);
        }
    }
    else if (modo == 'S')
    {
        for (i = 0; i < qtd_impressao; i++)
        {
            printf("%s\t\t|%.6f\t|%s\n", (char *)vet_org[i].identificador, vet_org[i].fator, vet_org[i].rotulo);
        }
    }
}

int organizador_sorteia_crescente(const void *elemento_1, const void *elemento_2)
{
    Organizador *org_1 = (Organizador *)elemento_1;
    Organizador *org_2 = (Organizador *)elemento_2;

    if (org_1->fator > org_2->fator)
    {
        return 1;
    }
    else if (org_1->fator < org_2->fator)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int organizador_sorteia_decrescente(const void *elemento_1, const void *elemento_2)
{
    Organizador *org_1 = (Organizador *)elemento_1;
    Organizador *org_2 = (Organizador *)elemento_2;

    if (org_1->fator > org_2->fator)
    {
        return -1;
    }
    else if (org_1->fator < org_2->fator)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void organizador_free(Organizador *vet_org, int tam_vet)
{
    int i;
    for (i = 0; i < tam_vet; i++)
    {
        if (vet_org[i].rotulo != NULL)
        {
            free(vet_org[i].rotulo);
        }
    }
    free(vet_org);
}

void hash_preenche_tabela_classes(p_HashTable table, char ***tabela_classes)
{
    (*tabela_classes) = (char **)realloc((*tabela_classes), table->qtd_class * sizeof(char *));

    int i;
    for (i = 0; i < table->qtd_class; i++)
    {
        (*tabela_classes)[i] = strdup(documentos_retorna_classe(table->class_table[i]));
    }
}

int hash_retorna_qtd_classes(p_HashTable table)
{
    return table->qtd_class;
}

void hash_free(p_HashTable table)
{
    int i, j;
    for (i = 0; i < table->pal_allcd; i++)
    {
        for (j = 0; j < table->pal_table[i].qtd_palavras_no_indice; j++)
        {
            palavras_free(table->pal_table[i].vet_indice[j]);
        }
        free(table->pal_table[i].vet_indice);
    }
    for (i = 0; i < table->qtd_doc; i++)
    {
        documentos_free(table->doc_table[i]);
    }

    for (i = 0; i < table->qtd_class; i++)
    {
        documentos_free(table->class_table[i]);
    }

    free(table->pal_table);
    free(table->doc_table);
    free(table->class_table);
    free(table);
}
