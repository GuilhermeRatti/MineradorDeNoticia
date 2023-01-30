#include "HashTable.h"
#include "Palavras.h"
#include "Documentos.h"
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
EX: IDX_DOC em relacao a TF-IDF;
    Classes em relacao a aparicoes;
    IDX_DOX em relacao a tamanho;
*/
typedef struct
{
    long int identificador;
    double fator;
    char * rotulo;
} Organizador;

int organizador_verifica_existencia(Organizador *vet_org, int tam_vet, long int identificador);
Organizador *organizador_adiciona(Organizador *vet_org, int *tam_vet, long int identificador, double fator, char* rotulo);
void organizador_imprime(Organizador *vet_org, int tam_vet, int limitador, char modo);
int organizador_sorteia_crescente(const void *elemento_1, const void *elemento_2);
int organizador_sorteia_decrescente(const void *elemento_1, const void *elemento_2);
void organizador_free(Organizador *vet_org, int tam_vet);
char * classe_retorna_string_unica(p_HashTable table, p_Documentos doc);

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

p_HashTable hash_register_new_doc(p_HashTable table, char *classe, char *diretorio)
{
    if (table->qtd_doc == table->doc_allcd)
    {
        table->doc_allcd *= 2;
        table->doc_table = (p_Documentos *)realloc(table->doc_table, table->doc_allcd * sizeof(p_Documentos));
    }

    table->doc_table[table->qtd_doc] = documentos_cria(classe, diretorio, table->qtd_doc);
    table->qtd_doc++;

    return table;
}

p_HashTable hash_register_new_item(p_HashTable table, char *palavra, int p_index)
{
    p_Palavras p = palavras_cria(palavra, p_index);

    int doc = table->qtd_doc - 1;
    int i, pos_no_indice = table->pal_table[p_index].qtd_palavras_no_indice;
    table->doc_table[doc] = documentos_registra_frequencia(table->doc_table[doc], palavra);

    for (i = 0; i < pos_no_indice; i++)
    {
        int match = compara_palavras(&(table->pal_table[p_index].vet_indice[i]), &p);

        if (!match)
        {
            table->pal_table[p_index].vet_indice[i] = palavras_registra_frequencia(table->pal_table[p_index].vet_indice[i], doc);
            palavras_free(p);

            return table;
        }
    }

    table->qtd_pal++;
    table->pal_table[p_index].qtd_palavras_no_indice++;
    table->pal_table[p_index].vet_indice = (p_Palavras *)realloc(table->pal_table[p_index].vet_indice,
                                                                 table->pal_table[p_index].qtd_palavras_no_indice * sizeof(p_Palavras));
    p = palavras_registra_frequencia(p, doc);
    table->pal_table[p_index].vet_indice[pos_no_indice] = p;

    return table;
}

void hash_print_amount_of_items(p_HashTable table)
{
    printf("QTD DE DOC E PALAVRA: %d %d\n", table->qtd_doc, table->qtd_pal);
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

p_HashTable hash_calcula_tfidf(p_HashTable table)
{
    int i, j, qtd_tfidf, qtd;

    for (i = 0; i < table->pal_allcd; i++)
    {
        HashIndex id_desejado = table->pal_table[i];
        qtd = id_desejado.qtd_palavras_no_indice;
        for (j = 0; j < qtd; j++)
        {
            double *vet_tfidf = (double *)malloc(sizeof(double));
            int *vet_docs = (int *)malloc(sizeof(int));

            id_desejado.vet_indice[j] = palavras_preenche_tfidf(id_desejado.vet_indice[j], table->qtd_doc, &vet_tfidf, &vet_docs, &qtd_tfidf);
            free(vet_docs);
            free(vet_tfidf);
        }
    }

    table = hash_preenche_tfidf_docs(table);

    return table;
}

p_HashTable hash_preenche_tfidf_docs(p_HashTable table)
{
    int i;

    for (i = 0; i < table->qtd_doc; i++)
    {
        table->doc_table[i] = documentos_preenche_tfidf(table, table->doc_table[i]);
    }

    return table;
}

double hash_return_tfidf(p_HashTable table, int doc, char *palavra)
{
    int i, match, hash = hash_get_index(palavra);
    p_Palavras p = palavras_cria(palavra, 0); // Encapsulando a palavra pra fazer comparacao

    for (i = 0; i < table->pal_table[hash].qtd_palavras_no_indice; i++)
    {
        match = compara_palavras(&p, &(table->pal_table[hash].vet_indice[i]));
        if (!match)
        {
            palavras_free(p);
            break;
        }
    }

    return palavras_busca_TFIDF(table->pal_table[hash].vet_indice[i], doc);
}

p_HashTable hash_calcula_centroides(p_HashTable table)
{
    int i, k, match = 1;

    for (i = 0; i < table->qtd_doc; i++)
    {
        for (k = 0; k < table->qtd_class; k++)
        {
            match = compara_documentos(&(table->class_table[k]), &(table->doc_table[i]));
            if (!match)
                break;
        }

        if (match != 0)
        {
            if (table->qtd_class == table->class_allcd)
            {
                table->class_allcd *= 2;
                table->class_table = (p_Documentos *)realloc(table->class_table, table->class_allcd * sizeof(p_Documentos));
            }

            char *class_name = documentos_retorna_classe(table->doc_table[i]);
            table->class_table[k] = documentos_cria(class_name, class_name, k);
            table->qtd_class++;
        }

        table->class_table[k] = documentos_preenche_centroide(table->doc_table[i], table->class_table[k]);
        documentos_organiza_ordem(table->class_table, table->qtd_class);
    }

    for (i = 0; i < table->qtd_class; i++)
    {
        table->class_table[i] = documentos_calcula_media_centroide(table->class_table[i]);
        documentos_imprime(table->class_table[i]);
    }

    return table;
}

/*Ordem de escrita:
    qtd_pal                                 - int
    HashIndex{                              - HashIndex
        OBS: escreve todos os indices da tabela hash, inclusive vazios
        OBS: chama a funcao 'palavras_escrever_arquivo_bin'
        Ordem de escritura:
        qtd_palavras_indice                 - int
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
    }
    qtd_doc                                 - int
    p_Documentos e centroides{              - p_Documentos
        OBS: chama a funcao 'documentos_escrever_arquivo_bin'
        Ordem de escritura:
        idx de doc                          - int
        tam_vet                             - int
        tam_nome_doc                        - int
        nome doc                            - char (* tam_nome_doc)
        tam_nome_classe                     - int
        classe                              - char (* tam_nome_classe)
        vet[tam_vet]:                       - IndiceDocumentos
            tam_palavra                     - int
            palavra                         - char *
            freq                            - int
            TFIDF                           - double
    }
*/
void hash_escrever_arquivo_bin(p_HashTable table, const char *caminho_bin)
{
    int i = 0, qtd_pal_local = 0;

    FILE *arq = fopen(caminho_bin, "wb");

    if (arq == NULL)
    {
        hash_free(table);
        exit(printf("ERRO: FALHA AO CRIAR ARQUIVO BINARIO.\n"));
    }

    fwrite(&(table->qtd_pal), 1, sizeof(int), arq); // qtd de palavras -int

    for (i = 0; i < SIZE_OF_TABLE; i++)
    {
        qtd_pal_local = table->pal_table[i].qtd_palavras_no_indice;

        fwrite(&(qtd_pal_local), 1, sizeof(int), arq); // qtd de palavras - int (para montar a HashIndex)

        palavras_escrever_arquivo_bin(arq, table->pal_table[i].vet_indice, qtd_pal_local);
    }

    fwrite(&(table->qtd_doc), 1, sizeof(int), arq); // qtd de documentos -int

    documentos_escrever_arquivo_bin(arq, table->doc_table, table->qtd_doc);

    fwrite(&(table->qtd_doc), 1, sizeof(int), arq); // qtd de classes - int

    documentos_escrever_arquivo_bin(arq, table->class_table, table->qtd_class);

    fclose(arq);
}

/*Ordem de escrita:
        qtd_pal                                 - int
        HashIndex{                              - HashIndex
            OBS: escreve todos os indices da tabela hash, inclusive vazios
            OBS: chama a funcao 'palavras_escrever_arquivo_bin'
            Ordem de escritura:
            qtd_palavras_indice                 - ints
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
        }
        qtd_doc                                 - int
        p_Documentos e centroides{              - p_Documentos
            OBS: chama a funcao 'documentos_escrever_arquivo_bin'
            Ordem de escritura:
            qtd de documetos                    - int
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
        }
*/
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

    fread(&(table->qtd_class), 1, sizeof(int), arq);

    table->class_table = (p_Documentos *)realloc(table->class_table, table->qtd_class * sizeof(p_Documentos));

    documentos_le_arquivo_bin(arq, table->class_table, table->qtd_class);

    fclose(arq);
}

int hash_palavra_verfica_existencia(p_HashTable table, char *palavra_alvo)
{
    // possivel indice armazena o indice da tabela hash onde a palavra alvo pode estar contida
    HashIndex possivel_indice = table->pal_table[hash_get_index(palavra_alvo)];

    return palavras_verifica_existencia(possivel_indice.vet_indice, possivel_indice.qtd_palavras_no_indice, palavra_alvo);
}


/* criar um vetor de Organizadores com identificador => IDX_DOC e fator => TF-IDF
    onde:
    para cada palavra:
        verifica existencia na tabela
        se existir, pega o indx_doc e TF-IDF para cada doc
        ve se o idx bate com algum idx no identificador e se bater add no fator
    depois organiza a matriz usando a função de sorteamento do organizador
    imprime os 10 primeiros docs
*/
void hash_buscar_noticias(p_HashTable table, char *texto)
{
    // mostrar as 10 noticias em que as palavras mais aparecem
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
            //pega a palavra da tabela
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
                    vet_org_doc = organizador_adiciona(vet_org_doc, &qtd_org_doc, palavra_docs_ids[i], palavra_docs_TFIDFs[i], "");
                }

                // libera os vetores alocados dentro da função de retorno
                free(palavra_docs_ids);
                free(palavra_docs_TFIDFs);
            }
        }
        palavra_atual = strtok(NULL, " ");
    }

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

    organizador_free(vet_org_doc, qtd_org_doc);
    organizador_free(vet_org_pal, qtd_org_pal);
}


/*
O usuario deve digitar uma palavra e o programa deve exibir o número total de
documentos em que a palavra aparece, os 10 em que ela aparece com mais frequência e a frequência da
palavra por classe. Ambas as listas devem ser ordenadas com os itens de maior contagem aparecendo
primeiro. A ordenação deve ser feita ao selecionar a opção usando a função qsort.
*/
void hash_relatorio_noticias(p_HashTable table, char *palavra_relatorio)
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
        //pega a palavra da tabela
        palavra_tabela = table->pal_table[hash_get_index(palavra_relatorio)].vet_indice[indice_pal_hash];

        // extrai informacoes dos documentos associados com a plavra (necessita liberacao manual)
        int *palavra_docs_ids = palavras_retorna_docs_ids(palavra_tabela);
        int *palavras_docs_frequencia = palavras_retorna_docs_frequencia(palavra_tabela);

        int qtd_docs_palavra = palavras_retorna_docs_quantidade(palavra_tabela);
        // adiciona no vetor os dados dos documentos
        for (i = 0; i < qtd_docs_palavra; i++)
        {
            vet_org_doc = organizador_adiciona(vet_org_doc, &qtd_org_doc, palavra_docs_ids[i], palavras_docs_frequencia[i],  "");
        }

        for (i = 0; i < qtd_docs_palavra; i++)
        {   p_Documentos doc_atual = table->doc_table[vet_org_doc[i].identificador];
            vet_org_cls = organizador_adiciona(vet_org_cls, &qtd_org_cls, (long int)classe_retorna_string_unica(table, doc_atual), vet_org_doc[i].fator, "");
        }
        
        // libera os vetores alocados dentro da função de retorno
        free(palavra_docs_ids);
        free(palavras_docs_frequencia);

        qsort(vet_org_doc, qtd_org_doc, sizeof(Organizador), organizador_sorteia_decrescente);
        qsort(vet_org_cls, qtd_org_cls, sizeof(Organizador), organizador_sorteia_decrescente);

        printf("\nResultado da busca:\n\nIDX_DOC\t\t|Frequencia\n");
        organizador_imprime(vet_org_doc, qtd_org_doc, 10, 'n');

        printf("\n\nClasse\t\t|Frequencia\n");
        organizador_imprime(vet_org_cls, qtd_org_cls, 10, 's');
    }
    else
    {
        printf("\nTermo nao encontrado!\n");
    }
    
    organizador_free(vet_org_doc, qtd_org_doc);
    organizador_free(vet_org_cls, qtd_org_cls);
}

/*
Exibe os 10 documentos mais longos e os 10 mais curtos com o número de
palavras e as respectivas classes. As listas devem ser ordenadas, a primeira do maior para o menor e a
segunda do menor para o maior. A ordenação deve ser feita ao selecionar a opção usando a função qsort.
*/
void hash_relatorio_documentos(p_HashTable table)
{
    int qtd_org_doc = 0, i = 0;
    p_Documentos documento_atual;
    // identificador == idx_doc | fator == frquencia
    Organizador *vet_org_doc = (Organizador *)calloc(qtd_org_doc, sizeof(Organizador));

    for (i = 0; i < table->qtd_doc; i++)
    {
        documento_atual = table->doc_table[i];
        vet_org_doc = organizador_adiciona(vet_org_doc, &qtd_org_doc, documentos_retorna_id(documento_atual), documentos_retorna_quantidade_palavras(documento_atual),  documentos_retorna_classe(documento_atual));
    }

    qsort(vet_org_doc, qtd_org_doc, sizeof(Organizador), organizador_sorteia_decrescente);
    printf("\nResultado da busca:\n\nIDX_DOC\t\t|Qtd Palavras\t|Classe\n");
    organizador_imprime(vet_org_doc, qtd_org_doc, 10, 'N');
    
    qsort(vet_org_doc, qtd_org_doc, sizeof(Organizador), organizador_sorteia_crescente);
    printf("\n\nIDX_DOC\t\t|Qtd Palavras\t|Classe\n");
    organizador_imprime(vet_org_doc, qtd_org_doc, 10, 'N');


    organizador_free(vet_org_doc, qtd_org_doc);
}

char * classe_retorna_string_unica(p_HashTable table, p_Documentos doc)
{
    int i;
    for (i = 0; i < table->qtd_class; i++)
    {
        if (compara_documentos(&(table->class_table[i]), &(doc)) == 0)
        {
            return documentos_retorna_classe(table->class_table[i]);
        }
    }
    
    return NULL;
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
    int id = organizador_verifica_existencia(vet_org, *tam_vet, identificador);

    if (id == -1)
    {
        vet_org = (Organizador *)realloc(vet_org, ((*tam_vet) + 1) * sizeof(Organizador));

        vet_org[*tam_vet].fator = fator;
        vet_org[*tam_vet].identificador = identificador;
        vet_org[*tam_vet].rotulo =  calloc(strlen(rotulo)+1,sizeof(char));
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
/*Recebe: 
- Um vetor de Organizador
- Um inteiro do tamanho do vetor
- Um inteiro para limita a quantidade de impressoes (ignorado caso passe do tamnho do vetor)
- Um char que pode assumir possibilita 4 metodos de impressao:
    - 'n' -> Numerico:          imprime identificador e o fator         | trata o identificador como um numero
    - 's' -> String:            imprime identificador e o fator         | trata o identificador como uma string    
    - 'N' -> Numerico Triplo:   imprime identificador ,fator e o rotulo | trata o identificador como um numero    
    - 'S' -> String Triplo:     imprime identificador ,fator e o rotulo | trata o identificador como uma string    
*/
void organizador_imprime(Organizador *vet_org, int tam_vet, int limitador, char modo)
{
    int i=0, qtd_impressao=0;
    if (limitador > 0 && limitador < tam_vet)
    {
        qtd_impressao = limitador;    
    }
    else
    {
        qtd_impressao = tam_vet;
    }

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
    } else if (modo == 'N')
    {
        for (i = 0; i < qtd_impressao; i++)
        {
            printf("%ld\t\t|%.6f\t|%s\n", vet_org[i].identificador, vet_org[i].fator, vet_org[i].rotulo);
        }
    } else if (modo == 'S')
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