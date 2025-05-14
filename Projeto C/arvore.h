#ifndef ARVORE_H
#define ARVORE_H

#define TAM_ALFABETO 256

typedef struct no {
    unsigned char caractere;
    unsigned int frequencia;
    struct no *esquerda, *direita;
} No;
    
No* criar_no(unsigned char caractere, unsigned int frequencia, No* esquerda, No* direita); //cria e retorna um novo nó da árvore (folha ou interno) 
No* construir_arvore(unsigned int frequencias[TAM_ALFABETO]); //cria a árvore de huffman usando as frequências dos caracteres
void liberar_arvore(No* raiz);//libera recursivamente a memória alocada para a árvore


#endif
