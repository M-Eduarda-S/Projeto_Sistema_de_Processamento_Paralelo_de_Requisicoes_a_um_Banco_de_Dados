// Envia requisições estruturadas para o servidor utilizando IPC (FIFO/named pipe)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "banco.h"
#include "requisicoes.h"

// Envia uma requisição para o servidor via FIFO
void enviar_requisicao(Requisicao req) {
    // Abre FIFO para escrita (bloqueia se servidor não está lendo)
    int pipe = open(CAMINHO_PIPE, O_WRONLY);
    if (pipe < 0) {
        perror("Erro ao abrir pipe");
        exit(1);
    }

    req.pid = getpid(); // Adiciona ID do cliente para rastreamento no servidor

    // Envia requisição como estrutura binária
    write(pipe, &req, sizeof(Requisicao));

    close(pipe);  // fecha o pipe após enviar a requisição

    printf("PID Cliente: %d - A requisição foi enviada!\n", getpid());
}

// Função principal do cliente: oferece menu para enviar requisições ou executar teste pré-definido
int main(int argQuantidade, char *argVetor[]) {

    int modo = 1;  // interativo

    printf("O cliente foi iniciado...\n");

    // Lê modo de operação se fornecido como argumento
    if (argQuantidade > 1) {
        modo = atoi(argVetor[1]); // Converte argumento para inteiro
    }

   // teste pré-definido: executa sequência de requisições para demonstrar funcionalidade do servidor
    if (modo == 2) {
        printf("\nExecutando teste de concorrência...\n");

        for (int i = 0; i < 20; i++) {
            enviar_requisicao(lista_teste[i]);
            usleep(200000); // pequeno delay
        }

        printf("Execução finalizada.\n");
        exit(0);
    }

    // Menu para escolher operações manualmente
    int opcao;

    while (1) { // Loop até usuário escolher sair
        
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
                printf("Nome: "); scanf("%99s", req.reg.nome);
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
                printf("Novo nome: "); scanf("%99s", req.reg.nome);
                enviar_requisicao(req);
                break;

            case 5: 
                printf("Executando lista de requisições fixas...\n");

                    for (int i = 0; i < 20; i++) {
                        enviar_requisicao(lista_teste[i]);

                        usleep(200000); // delay pra visualizar no servidor
                    }

                    printf("Execução finalizada.\n");
                    break; 

            default: 
                printf("Opção inválida! Tente novamente\n");
        }
    }

    printf("O cliente foi encerrado.\n");
    return 0;
}