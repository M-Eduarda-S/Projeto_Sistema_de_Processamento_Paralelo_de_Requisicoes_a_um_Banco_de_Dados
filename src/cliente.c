// Envia requisições estruturadas para o servidor utilizando IPC (FIFO/named pipe)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "banco.h"
#include "requisicoes.h"

void enviar_requisicao(Requisicao req) {
    int pipe;

    pipe = open(CAMINHO_PIPE, O_WRONLY); // abre o FIFO para escrita

    if (pipe < 0) {
        perror("Erro ao abrir o pipe");
        return;
    }

    //gera um id para o cliente
    req.pid = getpid();

    write(pipe, &req, sizeof(Requisicao)); // envia a struct para o servidor

    close(pipe); // fecha o pipe

    printf("A requisição foi enviada!\n"); 
}

/*
Para estressar o servidor com a lista de requisição:
defina opcao = 5, comente o menu, mude o break para exit(0);
use os comandos:
1)
gcc servidor.c -o servidor
./servidor
2) 
gcc cliente.c -o cliente
for i in {1..5}; do ./cliente & done
*/

int main() {

    int opcao;

    printf("O cliente foi iniciado...\n");

    while (1) { // loop infinito
        
        printf("\nOperações disponíveis: \n");
        printf("1- Insert \n");
        printf("2- Select \n");
        printf("3- Delete \n");
        printf("4- Update \n");
        printf("5- Requisições fixas\n");
        printf("0- Sair\n");
        printf("Escolha uma opção: "); 

        scanf("%d", &opcao);
        

        if (opcao == 0) break;

        Requisicao req;

        switch (opcao) { 
            case 1:
                req.tipo = OP_INSERT;
                printf("ID: "); scanf("%d", &req.reg.id);
                printf("Nome: "); scanf("%s", req.reg.nome);
                enviar_requisicao(req);
                break;

            case 2:
                req.tipo = OP_SELECT;
                printf("ID: "); scanf("%d", &req.reg.id);
                enviar_requisicao(req);
                break;

            case 3:
                req.tipo = OP_DELETE;
                printf("ID: "); scanf("%d", &req.reg.id);
                enviar_requisicao(req);
                break;

            case 4:
                req.tipo = OP_UPDATE;
                printf("ID: "); scanf("%d", &req.reg.id);
                printf("Novo nome: "); scanf("%s", req.reg.nome);
                enviar_requisicao(req);
                break;

            case 5: // Aqui são requisições já prontas para teste
                printf("Executando lista de requisições fixas...\n");

                    for (int i = 0; i < 20; i++) {
                        enviar_requisicao(lista_teste[i]);

                        usleep(200000); // delay pra visualizar no servidor
                    }

                    printf("Execução finalizada.\n");
                    exit(0);

            default:
                printf("Opção inválida! Tente novamente\n");
        }
    }

    printf("O cliente foi encerrado.\n");
    return 0;
}