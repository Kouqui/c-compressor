#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>  // Garantindo que o tipo FILE seja reconhecido

#include "arvore.h"

typedef struct {
    unsigned int tamanho;
    unsigned char bits[256];
} Codigo;

typedef struct fila_prioridade {
    No* nos[TAM_ALFABETO];
    int tamanho;
} FilaPrioridade;

void calcular_frequencia(FILE* arquivo, unsigned int frequencias[TAM_ALFABETO]);
void inicializar_fila(FilaPrioridade* fila, unsigned int frequencias[TAM_ALFABETO]);
void gerar_tabela_codigos(No* raiz, Codigo tabela[TAM_ALFABETO], Codigo atual);
void compactar(const char* nome_entrada, const char* nome_saida);
void descompactar(const char* nome_entrada, const char* nome_saida);

#endif
