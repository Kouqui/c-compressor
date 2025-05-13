#ifndef ARVORE_H
#define ARVORE_H

#define TAM_ALFABETO 256

typedef struct no {
    unsigned char caractere;
    unsigned int frequencia;
    struct no *esquerda, *direita;
} No;

No* criar_no(unsigned char caractere, unsigned int frequencia, No* esquerda, No* direita);
No* construir_arvore(unsigned int frequencias[TAM_ALFABETO]);
void liberar_arvore(No* raiz);

#endif
