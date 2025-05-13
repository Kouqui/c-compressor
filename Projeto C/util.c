#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void calcular_frequencia(FILE* arquivo, unsigned int frequencias[TAM_ALFABETO]) {
    for (int i = 0; i < TAM_ALFABETO; i++) frequencias[i] = 0;
    int c;
    while ((c = fgetc(arquivo)) != EOF) frequencias[(unsigned char)c]++;
    rewind(arquivo);
}

void inicializar_fila(FilaPrioridade* fila, unsigned int frequencias[TAM_ALFABETO]) {
    fila->tamanho = 0;
    for (int i = 0; i < TAM_ALFABETO; i++) {
        if (frequencias[i] > 0) {
            fila->nos[fila->tamanho++] = criar_no(i, frequencias[i], NULL, NULL);
        }
    }
}

void gerar_tabela_codigos(No* raiz, Codigo tabela[TAM_ALFABETO], Codigo atual) {
    if (!raiz->esquerda && !raiz->direita) {
        tabela[raiz->caractere] = atual;
        return;
    }
    if (raiz->esquerda) {
        Codigo esq = atual;
        esq.bits[esq.tamanho / 8] &= ~(1 << (7 - esq.tamanho % 8));
        esq.tamanho++;
        gerar_tabela_codigos(raiz->esquerda, tabela, esq);
    }
    if (raiz->direita) {
        Codigo dir = atual;
        dir.bits[dir.tamanho / 8] |= (1 << (7 - dir.tamanho % 8));
        dir.tamanho++;
        gerar_tabela_codigos(raiz->direita, tabela, dir);
    }
}

void compactar(const char* nome_entrada, const char* nome_saida) {
    FILE* entrada = fopen(nome_entrada, "rb");
    FILE* saida = fopen(nome_saida, "wb");
    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos!\n");
        exit(1);
    }

    unsigned int frequencias[TAM_ALFABETO];
    calcular_frequencia(entrada, frequencias);
    No* raiz = construir_arvore(frequencias);

    Codigo tabela[TAM_ALFABETO] = {0};
    Codigo atual = {0, {0}};
    gerar_tabela_codigos(raiz, tabela, atual);

    fwrite(frequencias, sizeof(unsigned int), TAM_ALFABETO, saida);

    unsigned int total_bits = 0;
    long pos_total_bits = ftell(saida);
    fwrite(&total_bits, sizeof(unsigned int), 1, saida);

    unsigned char buffer = 0;
    int bits_no_buffer = 0;
    int c;

    while ((c = fgetc(entrada)) != EOF) {
        Codigo codigo = tabela[c];
        total_bits += codigo.tamanho;
        for (unsigned int i = 0; i < codigo.tamanho; i++) {
            buffer <<= 1;
            buffer |= (codigo.bits[i / 8] >> (7 - (i % 8))) & 1;
            bits_no_buffer++;
            if (bits_no_buffer == 8) {
                fputc(buffer, saida);
                bits_no_buffer = 0;
                buffer = 0;
            }
        }
    }
    if (bits_no_buffer > 0) {
        buffer <<= (8 - bits_no_buffer);
        fputc(buffer, saida);
    }

    fclose(entrada);
    fclose(saida);

    saida = fopen(nome_saida, "rb+");
    if (!saida) {
        printf("Erro ao reabrir o arquivo para atualizar total_bits!\n");
        exit(1);
    }
    fseek(saida, pos_total_bits, SEEK_SET);
    fwrite(&total_bits, sizeof(unsigned int), 1, saida);
    fclose(saida);
    liberar_arvore(raiz);
}

void descompactar(const char* nome_entrada, const char* nome_saida) {
    FILE* entrada = fopen(nome_entrada, "rb");
    FILE* saida = fopen(nome_saida, "wb");
    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos!\n");
        exit(1);
    }

    unsigned int frequencias[TAM_ALFABETO];
    unsigned int total_bits;

    fread(frequencias, sizeof(unsigned int), TAM_ALFABETO, entrada);
    fread(&total_bits, sizeof(unsigned int), 1, entrada);

    No* raiz = construir_arvore(frequencias);
    if (!raiz) {
        printf("Erro ao construir a árvore de Huffman!\n");
        fclose(entrada);
        fclose(saida);
        return;
    }

    int folhas = 0;
    unsigned char caractere_unico = 0;
    for (int i = 0; i < TAM_ALFABETO; i++) {
        if (frequencias[i] > 0) {
            folhas++;
            caractere_unico = i;
        }
    }
    if (folhas == 1) {
        for (unsigned int i = 0; i < total_bits; i++) {
            fputc(caractere_unico, saida);
        }
        fclose(entrada);
        fclose(saida);
        liberar_arvore(raiz);
        return;
    }

    No* atual = raiz;
    int byte_lido;
    unsigned int bits_lidos = 0;

    while (bits_lidos < total_bits && (byte_lido = fgetc(entrada)) != EOF) {
        for (int i = 7; i >= 0 && bits_lidos < total_bits; i--) {
            int bit = (byte_lido >> i) & 1;
            atual = (bit == 0) ? atual->esquerda : atual->direita;
            bits_lidos++;
            if (atual->esquerda == NULL && atual->direita == NULL) {
                fputc(atual->caractere, saida);
                atual = raiz;
            }
        }
    }

    fclose(entrada);
    fclose(saida);
    liberar_arvore(raiz);
}
