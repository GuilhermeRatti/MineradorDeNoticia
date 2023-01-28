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
}HashIndex;

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

p_HashTable hash_initialize_table()
{
    p_HashTable hash_table = (p_HashTable)calloc(1,sizeof(struct HashTable));
    hash_table->doc_allcd = 10;
    hash_table->pal_allcd = SIZE_OF_TABLE; //Um numero primo maior que 20000
    hash_table->class_allcd = 1;

    hash_table->doc_table = (p_Documentos*)calloc(hash_table->doc_allcd,sizeof(p_Documentos));
    hash_table->pal_table = (HashIndex*)calloc(hash_table->pal_allcd,sizeof(HashIndex));
    hash_table->class_table = (p_Documentos*)calloc(hash_table->class_allcd,sizeof(p_Documentos));
    
    return hash_table;
}

int hash_get_index(char* palavra)
{
    int i, hash = 0, tam = strlen(palavra);

    for(i=0;i<tam;i++)
    {
        hash += palavra[i]*(i+1);
    }

    return hash%SIZE_OF_TABLE;
}

p_HashTable hash_register_new_doc(p_HashTable table, char* classe, char* diretorio)
{
    if(table->qtd_doc==table->doc_allcd)
    {
        table->doc_allcd*=2;
        table->doc_table = (p_Documentos*)realloc(table->doc_table,table->doc_allcd*sizeof(p_Documentos));
    }

    table->doc_table[table->qtd_doc] = documentos_cria(classe,diretorio,table->qtd_doc);
    table->qtd_doc++;

    return table;
}

p_HashTable hash_register_new_item(p_HashTable table, char* palavra, int p_index)
{
    p_Palavras p = palavras_cria(palavra, p_index);

    int doc = table->qtd_doc-1;
    int i,pos_no_indice = table->pal_table[p_index].qtd_palavras_no_indice;
    table->doc_table[doc] = documentos_registra_frequencia(table->doc_table[doc],palavra);
    
    for(i=0;i<pos_no_indice;i++)
    {
        int match = compara_palavras(&(table->pal_table[p_index].vet_indice[i]),&p);
        
        if(!match)
        {
            table->pal_table[p_index].vet_indice[i] = palavras_registra_frequencia(table->pal_table[p_index].vet_indice[i],doc);
            palavras_free(p);

            return table;
        }
    }

    table->qtd_pal++;
    table->pal_table[p_index].qtd_palavras_no_indice++;
    table->pal_table[p_index].vet_indice = (p_Palavras*)realloc(table->pal_table[p_index].vet_indice,
                                                                table->pal_table[p_index].qtd_palavras_no_indice*sizeof(p_Palavras));
    p = palavras_registra_frequencia(p,doc);
    table->pal_table[p_index].vet_indice[pos_no_indice] = p;

    return table;
}

void hash_print_amount_of_items(p_HashTable table)
{
    printf("QTD DE DOC E PALAVRA: %d %d\n", table->qtd_doc, table->qtd_pal);
}

void hash_imprime_palavra(p_HashTable table,char* palavra)
{
    int hash = hash_get_index(palavra),i,match=0;
    HashIndex desired_index = table->pal_table[hash];

    p_Palavras p = palavras_cria(palavra,0); // Encapsulando a palavra pra fazer comparacao

    for(i=0;i< desired_index.qtd_palavras_no_indice;i++)
    {
        match = compara_palavras(&(desired_index.vet_indice[i]),&p);

        if(!match)
        {
            palavras_imprime_informacoes(desired_index.vet_indice[i]);
            break;
        }
    }

    palavras_free(p);

    if(match!=0)
    {
        printf("PALAVRA '%s' NAO ENCONTRADA!!\nERA PRA ELA ESTAR REGISTRADA!?!?\n\n",palavra);
    }
}

void hash_imprime_documento(p_HashTable table,int posicao)
{
    documentos_imprime(table->doc_table[posicao]);
    documentos_imprime(table->class_table[posicao]);
}

p_HashTable hash_calcula_tfidf(p_HashTable table)
{
    int i,j,qtd_tfidf,qtd;

    for(i=0;i<table->pal_allcd;i++)
    {
        HashIndex id_desejado = table->pal_table[i];
        qtd = id_desejado.qtd_palavras_no_indice;
        for(j=0;j<qtd;j++)
        {
            double *vet_tfidf = (double*)malloc(sizeof(double));
            int *vet_docs = (int*)malloc(sizeof(int));

            id_desejado.vet_indice[j] = palavras_preenche_tfidf(id_desejado.vet_indice[j],table->qtd_doc, &vet_tfidf, &vet_docs,&qtd_tfidf);
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

    for(i=0;i<table->qtd_doc;i++)
    {
        table->doc_table[i] = documentos_preenche_tfidf(table,table->doc_table[i]);
    }

    return table;
}

double hash_return_tfidf(p_HashTable table, int doc, char*palavra)
{
    int i,match,hash = hash_get_index(palavra);
    p_Palavras p = palavras_cria(palavra,0); // Encapsulando a palavra pra fazer comparacao

    for(i=0;i<table->pal_table[hash].qtd_palavras_no_indice;i++)
    {
        match = compara_palavras(&p,&(table->pal_table[hash].vet_indice[i]));
        if(!match)
        {
            palavras_free(p);
            break;
        }
    }

    return palavras_busca_TFIDF(table->pal_table[hash].vet_indice[i],doc);
}

p_HashTable hash_calcula_centroides(p_HashTable table)
{
    int i,k,match=1;
    
    for(i=0;i<table->qtd_doc;i++)
    {
        for(k=0;k<table->qtd_class;k++)
        {
            match = compara_documentos(&(table->class_table[k]),&(table->doc_table[i]));
            if(!match)
                break;
        }

        if(match!=0)
        {
            if(table->qtd_class==table->class_allcd)
            {
                table->class_allcd*=2;
                table->class_table = (p_Documentos*)realloc(table->class_table,table->class_allcd*sizeof(p_Documentos));
            }
            char *class_name;
            class_name = (char*)calloc(5,sizeof(char));

            documentos_retorna_classe(table->doc_table[i],&class_name);
            table->class_table[k] = documentos_cria(class_name,class_name,k);
            table->qtd_class++;
            free(class_name);
        }

        table->class_table[k] = documentos_preenche_centroide(table->doc_table[i],table->class_table[k]);
        documentos_organiza_ordem(table->class_table,table->qtd_class);
    }

    for(i=0;i<table->qtd_class;i++)
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
    p_Documentos{                           - p_Documentos
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
void hash_escrever_arquivo_bin(p_HashTable table, FILE *arq)
{
    int i=0, qtd_pal_local = 0;

    fwrite(&(table->qtd_pal), 1, sizeof(int), arq);//qtd de palavras -int

    for ( i = 0; i < SIZE_OF_TABLE; i++)
    {
        qtd_pal_local = table->pal_table[i].qtd_palavras_no_indice;

        fwrite(&(qtd_pal_local), 1, sizeof(int), arq);//qtd de palavras - int (para montar a HashIndex)
    
        palavras_escrever_arquivo_bin(arq, table->pal_table[i].vet_indice, qtd_pal_local);
    }

    fwrite(&(table->qtd_doc), 1, sizeof(int), arq);//qtd de documentos -int

    documentos_escrever_arquivo_bin(arq, table->doc_table, table->qtd_doc);

    fwrite(&(table->qtd_doc),1,sizeof(int),arq); //qtd de classes - int

    documentos_escrever_arquivo_bin(arq,table->class_table,table->qtd_class);
    
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
        p_Documentos{                           - p_Documentos
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
void hash_le_arquivo_bin(p_HashTable table, FILE *arq)
{
    int i=0, qtd_pal_local = 0;

    fread(&(table->qtd_pal), 1, sizeof(int), arq); //qtd_pal - int
    
    for ( i = 0; i < SIZE_OF_TABLE; i++)
    {
        fread(&(qtd_pal_local), 1, sizeof(int), arq); // qtd_palavras_indice - int
        
        table->pal_table[i].qtd_palavras_no_indice = qtd_pal_local;
        
        if (qtd_pal_local != 0)//pula o hash atual caso nao haja palavras nele
        {
            table->pal_table[i].vet_indice = (p_Palavras *)calloc(qtd_pal_local, sizeof(p_Palavras));

            palavras_le_arquivo_bin(arq, table->pal_table[i].vet_indice, qtd_pal_local);
        }
    }
        
    fread(&(table->qtd_doc), 1, sizeof(int), arq); //qtd_doc - int
    
    table->doc_table = (p_Documentos*)realloc(table->doc_table, table->qtd_doc*sizeof(p_Documentos));

    documentos_le_arquivo_bin(arq, table->doc_table, table->qtd_doc);

    fread(&(table->qtd_class),1,sizeof(int),arq);

    table->class_table = (p_Documentos*)realloc(table->class_table,table->qtd_class*sizeof(p_Documentos));

    documentos_le_arquivo_bin(arq,table->class_table,table->qtd_class);
}


void hash_free(p_HashTable table)
{
    int i,j;
    for(i=0;i<table->pal_allcd;i++)
    {
        for(j=0;j<table->pal_table[i].qtd_palavras_no_indice;j++)
        {
            palavras_free(table->pal_table[i].vet_indice[j]);
        }
        free(table->pal_table[i].vet_indice);
    }
    for(i=0;i<table->qtd_doc;i++)
    {
        documentos_free(table->doc_table[i]);
    }

    for(i=0;i<table->qtd_class;i++)
    {
        documentos_free(table->class_table[i]);
    }

    free(table->pal_table);
    free(table->doc_table);
    free(table->class_table);
    free(table);
}