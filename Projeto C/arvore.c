#include "arvore.h"
#include "util.h"  // Incluindo util.h para usar a estrutura FilaPrioridade
#include <stdlib.h>

No* criar_no(unsigned char caractere, unsigned int frequencia, No* esquerda, No* direita) {
    No* novo = (No*)malloc(sizeof(No));
    novo->caractere = caractere;
    novo->frequencia = frequencia;
    novo->esquerda = esquerda;
    novo->direita = direita;
    return novo;
}

No* remover_minimo(FilaPrioridade* fila) {
    int menor = 0;
    for (int i = 1; i < fila->tamanho; i++) {
        if (fila->nos[i]->frequencia < fila->nos[menor]->frequencia)
            menor = i;
    }
    No* minimo = fila->nos[menor];
    for (int i = menor; i < fila->tamanho - 1; i++) {
        fila->nos[i] = fila->nos[i + 1];
    }
    fila->tamanho--;
    return minimo;
}

No* construir_arvore(unsigned int frequencias[TAM_ALFABETO]) {
    FilaPrioridade fila;
    inicializar_fila(&fila, frequencias);
    while (fila.tamanho > 1) {
        No* esq = remover_minimo(&fila);
        No* dir = remover_minimo(&fila);
        No* pai = criar_no(0, esq->frequencia + dir->frequencia, esq, dir);
        fila.nos[fila.tamanho++] = pai;
    }
    return fila.tamanho > 0 ? fila.nos[0] : NULL;
}

void liberar_arvore(No* raiz) {
    if (raiz) {
        liberar_arvore(raiz->esquerda);
        liberar_arvore(raiz->direita);
        free(raiz);
    }
}
