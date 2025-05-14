#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>  

#include "arvore.h"

typedef struct {
    unsigned int tamanho;
    unsigned char bits[256];
} Codigo;

typedef struct fila_prioridade {
    No* nos[TAM_ALFABETO];
    int tamanho;
} FilaPrioridade;

void calcular_frequencia(FILE* arquivo, unsigned int frequencias[TAM_ALFABETO]); //Lê o arquivo e conta quantas vezes cada caractere aparece
void inicializar_fila(FilaPrioridade* fila, unsigned int frequencias[TAM_ALFABETO]); //Cria os nós da árvore com base nas frequências e os insere na fila de prioridade
void gerar_tabela_codigos(No* raiz, Codigo tabela[TAM_ALFABETO], Codigo atual); //Gera a tabela de códigos binários (Huffman) a partir da árvore construída
void compactar(const char* nome_entrada, const char* nome_saida);
void descompactar(const char* nome_entrada, const char* nome_saida);

#endif
