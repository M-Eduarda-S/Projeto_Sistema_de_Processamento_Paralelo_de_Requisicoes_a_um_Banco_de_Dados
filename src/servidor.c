#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "banco.h"

#define THREAD_NUM 4
#define TAM_MAX_BD 10000
Registro bd_simulado[TAM_MAX_BD];

int bd_count = 0;

Requisicao registroQueue[256];
int registroCount = 0;

pthread_mutex_t mutex_bd;
pthread_mutex_t mutex_fila;
pthread_mutex_t mutex_log;

int salvarRequisicao(Requisicao* req){
    int status = 0;
    switch(req->tipo){
        case OP_INSERT:
            printf("Executando INSERT...\n");
            //Impedir condicao de corrida
            pthread_mutex_lock(&mutex_bd);
            if(bd_count < TAM_MAX_BD){
                bd_simulado[bd_count] = req->reg;
                bd_count++;
                status = 0;
            }
            //liberando o mutex
            pthread_mutex_unlock(&mutex_bd);
            break;
        case OP_DELETE:
            printf("Executando DELETE...\n");
            pthread_mutex_lock(&mutex_bd);
            for (int i = 0; i < bd_count; i++) {
                if (bd_simulado[i].id == req->reg.id) {
                    bd_simulado[i].id = -1;
                    status = 0;
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
                status = 0;
            }
            pthread_mutex_unlock(&mutex_bd);
            break;
        case OP_UPDATE:
            printf("Executando UPDATE...\n");
            pthread_mutex_lock(&mutex_bd);
            for (int i = 0; i < bd_count; i++) {
                if (bd_simulado[i].id == req->reg.id) {
                    strcpy(bd_simulado[i].nome, req->reg.nome);
                    break;
                }
            }
            status = 0;
            pthread_mutex_unlock(&mutex_bd);
            break;
        default:
            printf("Operação Invalída!\n");
    }
    return status;
}

void submitTask(Requisicao reg){
    pthread_mutex_lock(&mutex_fila);
    registroQueue[registroCount] = reg;
    registroCount++;
    pthread_mutex_unlock(&mutex_fila);
}

void* iniciarThread(void* args){
    while(1){
        Requisicao task;
        int existe = 0;

        pthread_mutex_lock(&mutex_fila);
        if(registroCount > 0){
            task = registroQueue[0];
            int i = 0;
            for(i = 0; i < registroCount-1; i++){
                registroQueue[i] = registroQueue[i+1];
            }
            registroCount--;
            existe = 1; 
        }
        pthread_mutex_unlock(&mutex_fila);

        if(existe == 1){
            int status = salvarRequisicao(&task);
            char *status_txt = (status == 0) ? "Sucesso" : "Falha";
            char *op_texto;
            switch(task.tipo) {
                case OP_INSERT: op_texto = "INSERT"; break; 
                case OP_DELETE: op_texto = "DELETE"; break; 
                case OP_SELECT: op_texto = "SELECT"; break; 
                case OP_UPDATE: op_texto = "UPDATE"; break; 
                default:        op_texto = "DESCONHECIDA";
            }

            pthread_mutex_lock(&mutex_log);
            FILE *arquivo_log = fopen("auditoria.log","a");
            pthread_t idThread = pthread_self(); // pega o ID da Thread atual em execução
            if (arquivo_log != NULL) {
                fprintf(arquivo_log, "Thread %lu | PID Cliente: %d | Operação: %s | Status: %s\n", 
                        idThread, task.pid, op_texto, status_txt);
                
                fclose(arquivo_log);
            } else {
                perror("Erro ao abrir arquivo de log");
            }
            pthread_mutex_unlock(&mutex_log);
        }
    }
}

int main(int argc, char* argv[]){
    pthread_mutex_init(&mutex_bd,NULL);
    pthread_mutex_init(&mutex_fila,NULL);
    pthread_mutex_init(&mutex_log,NULL);

    //"portal" para a memória RAM, qualquer programa pode procura-lo
    // pelo nome e colocar dados la dentro
    if(mkfifo(CAMINHO_PIPE, 0777) == -1){
        // verifica se o pipe existe
        if(errno != EEXIST){
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
    
    // servidor aberto  
    while(1){
        printf("Servidor aberto...\n");
        // abre o pipe para leitura
        int fd = open(CAMINHO_PIPE, O_RDONLY);
        if(fd == -1){
            return 1;
        }
        
        Requisicao dado_recebido;
        
        //Enquanto o servidor estiver aberto, recebe requisição
        while(1){
            int dados_bytes = read(fd, &dado_recebido, sizeof(Requisicao));
            if(dados_bytes > 0){
                submitTask(dado_recebido);
                printf("Server: Tarefa recebida e enviada para a fila!\n");
            }else if(dados_bytes == 0){
                printf("Server: Cliente desconectado\n");
                break;
            }else{
                perror("Server: Erro grave na leitura do Pipe");
                break;
            }
            
        }
        close(fd);
    }
    
    for(i = 0; i < THREAD_NUM; i++){
        if(pthread_join(th[i], NULL) != 0){
            perror("Erro ao entrar na thread!");
        }
    }

    pthread_mutex_destroy(&mutex_bd);
    pthread_mutex_destroy(&mutex_fila);

    return 0;
}