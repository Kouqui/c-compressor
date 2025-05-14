#include "arvore.h"
#include "util.h"  
#include <stdlib.h>

No* criar_no(unsigned char caractere, unsigned int frequencia, No* esquerda, No* direita) { // aloca dinamicamente um novo nó da árvore com os dados fornecidos
    No* novo = (No*)malloc(sizeof(No));
    novo->caractere = caractere;
    novo->frequencia = frequencia; //Pode ser uma folha (se esquerda e direita forem NULL) ou um nó interno (se for resultado da fusão de dois nós menores).
    novo->esquerda = esquerda;
    novo->direita = direita;
    return novo;
}

No* remover_minimo(FilaPrioridade* fila) { //busca o índice do nó com menor frequência na fila de prioridade
    int menor = 0;
    for (int i = 1; i < fila->tamanho; i++) {
        if (fila->nos[i]->frequencia < fila->nos[menor]->frequencia)
            menor = i;
    }
    No* minimo = fila->nos[menor];
    for (int i = menor; i < fila->tamanho - 1; i++) { //remove esse nó da fila, reordenando os elementos
        fila->nos[i] = fila->nos[i + 1];
    }
    fila->tamanho--;
    return minimo; //retorna o nó com menor frequência — usado para criar um nó pai com dois filhos
}

No* construir_arvore(unsigned int frequencias[TAM_ALFABETO]) {  //cria e preenche a fila com os nós folha (um para cada caractere com frequência > 0)
    FilaPrioridade fila;
    inicializar_fila(&fila, frequencias);
    while (fila.tamanho > 1) {
        //remove os dois com menor frequência
        No* esq = remover_minimo(&fila);
        No* dir = remover_minimo(&fila);

        //cria um novo nó pai com frequência somada
        No* pai = criar_no(0, esq->frequencia + dir->frequencia, esq, dir);
        fila.nos[fila.tamanho++] = pai;
    }
    return fila.tamanho > 0 ? fila.nos[0] : NULL; //retorna a raiz da árvore final (único nó restante)
} 

void liberar_arvore(No* raiz) { //liberar memória da árvore
    if (raiz) {
        liberar_arvore(raiz->esquerda);
        liberar_arvore(raiz->direita);
        free(raiz);
    }
}
