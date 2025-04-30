#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_ALFABETO 256

typedef struct no {
    unsigned char caractere;
    unsigned int frequencia;
    struct no *esquerda, *direita;
} No;

typedef struct {
    unsigned int tamanho;
    unsigned char bits[256];
} Codigo;

typedef struct fila_prioridade {
    No* nos[TAM_ALFABETO];
    int tamanho;
} FilaPrioridade;

// Função para criar nó
No* criar_no(unsigned char caractere, unsigned int frequencia, No* esquerda, No* direita) {
    No* novo = (No*)malloc(sizeof(No));
    novo->caractere = caractere;
    novo->frequencia = frequencia;
    novo->esquerda = esquerda;
    novo->direita = direita;
    return novo;
}

// Calcula as frequências de cada caractere no arquivo
void calcular_frequencia(FILE* arquivo, unsigned int frequencias[TAM_ALFABETO]) {
    for (int i = 0; i < TAM_ALFABETO; i++) frequencias[i] = 0;
    int c;
    while ((c = fgetc(arquivo)) != EOF) frequencias[(unsigned char)c]++;
    rewind(arquivo);
}

// Inicializa a fila de prioridade com as frequências
void inicializar_fila(FilaPrioridade* fila, unsigned int frequencias[TAM_ALFABETO]) {
    fila->tamanho = 0;
    for (int i = 0; i < TAM_ALFABETO; i++) {
        if (frequencias[i] > 0) {
            fila->nos[fila->tamanho++] = criar_no(i, frequencias[i], NULL, NULL);
        }
    }
}

// Remove o nó com menor frequência
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

// Constrói a árvore de Huffman usando a fila de prioridade
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

// Gera a tabela de códigos binários a partir da árvore de Huffman
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

// Libera a memória da árvore
void liberar_arvore(No* raiz) {
    if (raiz) {
        liberar_arvore(raiz->esquerda);
        liberar_arvore(raiz->direita);
        free(raiz);
    }
}

// Compactação
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

// Descompactação
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

int main() {
    int opcao;
    char entrada[100], saida[100];

    printf("1 - Compactar\n2 - Descompactar\nEscolha: ");
    scanf("%d", &opcao);
    printf("Arquivo de entrada: ");
    scanf("%s", entrada);
    printf("Arquivo de saída: ");
    scanf("%s", saida);

    if (opcao == 1)
        compactar(entrada, saida);
    else if (opcao == 2)
        descompactar(entrada, saida);
    else
        printf("Opção inválida!\n");

    return 0;
}
