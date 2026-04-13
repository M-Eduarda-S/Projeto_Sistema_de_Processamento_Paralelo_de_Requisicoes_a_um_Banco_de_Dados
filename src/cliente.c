// Envia requisições estruturadas para o servidor utilizando IPC (FIFO/named pipe)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "banco.h"

int main() {   
    int identificaPipe;
    int opcao;

    printf("Cliente iniciado...\n");

    while (1) {
        Requisicao req;

        printf("\n1-INSERT | 2-SELECT | 3-DELETE | 4-UPDATE | 0-SAIR\n");
        printf("Escolha: ");
        scanf("%d", &opcao);

        if (opcao == 0) break;

        switch (opcao) {
            case 1:
                req.tipo = OP_INSERT;
                printf("ID: "); scanf("%d", &req.reg.id);
                printf("Nome: "); scanf("%s", req.reg.nome);
                break;

            case 2:
                req.tipo = OP_SELECT;
                printf("ID: "); scanf("%d", &req.reg.id);
                break;

            case 3:
                req.tipo = OP_DELETE;
                printf("ID: "); scanf("%d", &req.reg.id);
                break;

            case 4:
                req.tipo = OP_UPDATE;
                printf("ID: "); scanf("%d", &req.reg.id);
                printf("Novo Nome: "); scanf("%s", req.reg.nome);
                break;
        }

        identificaPipe = open(CAMINHO_PIPE, O_WRONLY); // abertura do IPC
        write(identificaPipe, &req, sizeof(Requisicao)); // envio dos dados
        close(identificaPipe); // fecha

        printf("Requisição enviada!\n");
    }

    return 0;
}