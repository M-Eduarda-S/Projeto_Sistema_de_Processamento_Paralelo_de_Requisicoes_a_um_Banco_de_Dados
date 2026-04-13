#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include "banco.h"

#define THREAD_NUM = 4
//para a entrega parcial utilizaremos um vetor
Registro bd_simulado[1000];
int bd_count = 0;

Registro registroQueue[256];
int registroCount = 0;

pthread_mutex_t mutex_bd;
pthread_mutex_t mutex_fila;

void salvarRequisicao(Requisicao* req){
    switch(req->tipo){
        case OP_INSERT:
            printf("Executando INSERT...\n");
            //Impedir condicao de corrida
            pthread_mutex_lock(&mutex);
            // Salvando no banco simulado
            bd_simulado[bd_count] = req->reg;
            // Incrementando id do banco
            bd_count++;
            //liberando o mutex
            pthread_mutex_unlock(&mutex);
            break;
        case OP_DELETE:
            printf("Executando DELETE...\n");
            pthread_mutex_lock(&mutex_bd);
            for (int i = 0; i < bd_count; i++) {
                if (bd_simulado[i].id == req->reg.id) {
                    bd_simulado[i].id = -1;
                    printf("Registro deletado com sucesso!\n");
                    break;
                }
            }
            bd_count--; 
            pthread_mutex_unlock(&mutex_bd);
            break;
        case OP_SELECT:
            printf("Executando SELECT...\n");
            pthread_mutex_lock(&mutex_bd);
            for (int i = 0; i < bd_count; i++) {
                if (bd_simulado[i].id == req->reg.id) {
                    printf("Nome: %s\n", bd_simulado[i].nome);
                    break;
                }
            }
            pthread_mutex_unlock(&mutex_bd);
            break;
        case OP_UPDATE:
            printf("Executando UPDATE...\n");
            pthread_mutex_lock(&mutex_bd);
            for (int i = 0; i < bd_count; i++) {
                if (bd_simulado[i].id == req->reg.id) {
                    bd_simulado[i].nome = req->reg.nome;
                    break;
                }
            }
            pthread_mutex_unlock(&mutex_bd);
            break;
        default:
            printf("Operação Invalída!\n");
    }
}

void submitTask(Registro reg){
    pthread_mutex_lock(&mutex_fila);
    registroQueue[registroCount] = reg;
    registroCount++;
    pthread_mutex_unlock(&mutex_fila);
}

void* iniciarThread(void* args){
    while(1){
        Registro task;
        int existe = 0;

        pthread_mutex_lock(&mutex_fila);
        if(registroCount > 0){
            task = registroQueue[0];
            int i = 0;
            for(i = 0; i < registroCount-1; i++){
                registroQueue[i] = registroQueue[i+1];
            }
            registroCount--;
        }
        pthread_mutex_unlock(&mutex_fila);

        if(existe == 1){
            salvarRequisicao(&task);
        }
    }
}

int main(int argc, char* argv[]){
    pthread_mutex_init(&mutex_bd,NULL);
    pthread_mutex_init(&mutex_fila,NULL);

    //"portal" para a memória RAM, qualquer programa pode procura-lo
    // pelo nome e colocar dados la dentro
    if(mkfifo("bd_pipe", 0777) == -1){
        // verifica se o pipe existe
        if(eerno != EEXIST){
            printf("Não foi possível criar o arquivo pipe\n");
        }
    }

    pthread_t th[THREAD_NUM];
    int i;
    for(i = 0; i < THREAD_NUM; i++){
        if(pthread_create(&th[i], NULL, &iniciarThread, NULL) != 0){
            perror("Erro ao criar a thread!");
        }
    }
    
    // abre o pipe para leitura
    int fd = open("bd_pipe", O_RDONLY);

    
    Requisicao dado_recebido;
    
    while(1){
        int dados_bytes = read(fd, &dado_recebido, sizeof(Requisicao));
        if(dados_bytes > 0){
            submitTask(dado_recebido);
            printf("Main: Tarefa recebida e enviada para a fila!\n")
        }else if(dados_bytes == 0){
            printf("Main: Cliente desconectado");
            break;
        }
        
    }
    close(fd);
    
    for(i = 0; i < THREAD_NUM; i++){
        if(pthread_join(&th[i], NULL) != 0){
            perror("Erro ao entrar na thread!");
        }
    }

    pthread_mutex_destroy(&mutex_bd);
    pthread_mutex_destroy(&mutex_fila);

    return 0;
}