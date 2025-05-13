#include <stdio.h>
#include <stdlib.h>
#include "util.h"

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
