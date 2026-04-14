// O servidor.c recebe as requisições dos clientes via FIFO, armazena elas em uma fila e processa elas de forma concorrente usando threads, garantindo segurança com o uso de mutex

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

#define THREAD_NUM 4 // pool de 4 threads
#define TAM_MAX_BD 10000         

Registro bd_simulado[TAM_MAX_BD];  // simulação da base de dados em memória
int bd_count = 0; // contador de registros no banco

Requisicao registroQueue[256]; // fila de requisições a processar
int registroCount = 0; // quantidade de requisições na fila

// protege acesso a base de dados e a fila de requisições
pthread_mutex_t mutex_bd;
pthread_mutex_t mutex_fila;
pthread_mutex_t mutex_log; // protege escrita no arquivo de auditoria.log

// protótipos de funções
int salvarRequisicao(Requisicao* req){
    int status = 0;
    switch(req->tipo){
        case OP_INSERT:
            printf("Executando INSERT...\n");
            pthread_mutex_lock(&mutex_bd); // impedir condicao de corrida

            if(bd_count < TAM_MAX_BD){
                bd_simulado[bd_count] = req->reg;
                bd_count++;
                status = 0;
            } else {
                status = 1; // banco cheio
            }

            pthread_mutex_unlock(&mutex_bd); // liberando o mutex
            break;

        case OP_DELETE:
            printf("Executando DELETE...\n");
            pthread_mutex_lock(&mutex_bd);

            int encontrou_delete = 0; // se encontrou o id

            for (int i = 0; i < bd_count; i++) {
                if (bd_simulado[i].id == req->reg.id) {
                    bd_simulado[i].id = -1;
                    encontrou_delete = 1;
                    status = 0;
                    break;
                }
            }

            if (encontrou_delete) {
                bd_count--;
            } else {
                status = 1; // não encontrou
            }

            pthread_mutex_unlock(&mutex_bd);
            break;

        case OP_SELECT:
            printf("Executando SELECT...\n");
            pthread_mutex_lock(&mutex_bd);

            int encontrou_select = 0;

            for (int i = 0; i < bd_count; i++) {
                if (bd_simulado[i].id == req->reg.id) {
                    printf("Nome: %s\n", bd_simulado[i].nome);
                    encontrou_select = 1;
                    break;
                }
            }

            if (encontrou_select) {
                status = 0; 
            } else {
                status = 1; // não encontrou
            }

            pthread_mutex_unlock(&mutex_bd);
            break;

        case OP_UPDATE:
            printf("Executando UPDATE...\n");
            pthread_mutex_lock(&mutex_bd);

            int encontrou_update = 0;

            for (int i = 0; i < bd_count; i++) {
                if (bd_simulado[i].id == req->reg.id) {
                    strcpy(bd_simulado[i].nome, req->reg.nome);
                    encontrou_update = 1;
                    break;
                }
            }

            if (encontrou_update) {
                status = 0;
            } else {
                status = 1; // não encontrou
            }

            pthread_mutex_unlock(&mutex_bd);
            break;

        default:
            printf("Operação Invalída!\n");
    }
    return status;
}

// recebe uma requisição e a adiciona a fila de tarefas para as threads processarem
void submitTask(Requisicao reg){
    pthread_mutex_lock(&mutex_fila); 
    registroQueue[registroCount] = reg; // adiciona requisição
    registroCount++;                    
    pthread_mutex_unlock(&mutex_fila); // libera acesso à fila
}

// função executada por cada thread worker para processar requisições da fila
void* iniciarThread(void* args){
    while(1){
        Requisicao task;
        int existe = 0;

        pthread_mutex_lock(&mutex_fila); // protege leitura da fila
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

            // registra operação no arquivo de log 
            pthread_mutex_lock(&mutex_log); // protege escrita no arquivo
            FILE *arquivo_log = fopen("auditoria.log","a");
            pthread_t idThread = pthread_self(); // pega o id da thread atual em execução

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

int main(int argc, char* argv[]) {

    // mutex para sincronizar acessos e escrita 
    pthread_mutex_init(&mutex_bd,NULL); 
    pthread_mutex_init(&mutex_fila,NULL);
    pthread_mutex_init(&mutex_log,NULL);

    // FIFO para comunicação entre cliente e servidor
    if(mkfifo(CAMINHO_PIPE, 0777) == -1){ 
        if(errno != EEXIST){ // verifica se o pipe existe
            printf("Não foi possível criar o arquivo pipe\n");
        }
    }

    pthread_t th[THREAD_NUM];  // armazena ids das threads
    
    int i;
    for(i = 0; i < THREAD_NUM; i++){
        // cria 4 threads workers que processarão requisições em paralelo
        if(pthread_create(&th[i], NULL, &iniciarThread, NULL) != 0){
            perror("Erro ao criar a thread!");
        }
    }
    
    // mantém o servidor em funcionamento permanente
    while(1){
        printf("Servidor aberto...\n");
        
        int fd = open(CAMINHO_PIPE, O_RDONLY); // abre o FIFO para leitura
        if(fd == -1){
            return 1;
        }
        
        Requisicao dado_recebido;
        
        // recebe requisições enquanto o cliente estiver conectado
        while(1){
            // lê dados do pipe
            int dados_bytes = read(fd, &dado_recebido, sizeof(Requisicao));

            if (dados_bytes > 0) {
                submitTask(dado_recebido);
                printf("Server: Tarefa recebida e enviada para a fila!\n");
            } else if(dados_bytes == 0) {
                printf("Server: Cliente desconectado\n");
                break;
            } else {
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