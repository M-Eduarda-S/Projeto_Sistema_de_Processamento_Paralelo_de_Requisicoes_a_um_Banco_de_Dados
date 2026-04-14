/*
 * ============================================================================
 * SERVIDOR DE BANCO DE DADOS COM PROCESSAMENTO PARALELO
 * ============================================================================
 * Responsabilidades:
 *  - Recebe requisições (INSERT, SELECT, UPDATE, DELETE) via FIFO (IPC)
 *  - Gerencia um pool de 4 threads para processamento paralelo
 *  - Protege acesso à base de dados com mutex
 *  - Mantém fila de requisições para processamento ordenado
 *  - Gera arquivo de auditoria (log) com rastreamento de operações
 * 
 * Fluxo:
 *  1. Servidor inicia e cria 4 threads worker
 *  2. Abre pipe nomeado (FIFO) para receber requisições
 *  3. Ao receber requisição, adiciona à fila de processamento
 *  4. Threads processam requisições da fila em paralelo
 *  5. Cada operação é registrada em arquivo de log com ID da thread e PID
 * ============================================================================
 */

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

/* ========== CONFIGURAÇÕES DO SERVIDOR ========== */
#define THREAD_NUM 4              // Pool de 4 threads para processamento paralelo
#define TAM_MAX_BD 10000          // Capacidade máxima do banco simulado

/* ========== ESTRUTURAS DE DADOS ========== */
Registro bd_simulado[TAM_MAX_BD];  // Simulação da base de dados em memória
int bd_count = 0;                   // Contador de registros no banco

Requisicao registroQueue[256];      // Fila de requisições a processar
int registroCount = 0;              // Quantidade de requisições na fila

/* ========== MUTEXES PARA SINCRONIZAÇÃO ========== */
pthread_mutex_t mutex_bd;           // Protege acesso à base de dados
pthread_mutex_t mutex_fila;         // Protege acesso à fila de requisições
pthread_mutex_t mutex_log;          // Protege escrita no arquivo de auditoria

// Protótipos de funções
// Processa uma requisição e atualiza o banco de dados simulado
// Retorna 0 para sucesso, -1 para falha (ex: banco cheio, registro não encontrado)
//
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

// Adiciona uma requisição à fila de processamento
// Sincroniza acesso à fila com mutex para evitar condições de corrida
// Recebe uma requisição e a adiciona à fila de tarefas para as threads processarem
/
void submitTask(Requisicao reg){
    pthread_mutex_lock(&mutex_fila);    // Adquire exclusividade sobre a fila
    registroQueue[registroCount] = reg;  // Adiciona requisição
    registroCount++;                     // Incrementa contador
    pthread_mutex_unlock(&mutex_fila);  // Libera acesso à fila
}

// Função executada por cada thread worker para processar requisições da fila
void* iniciarThread(void* args){
    while(1){
        Requisicao task;
        int existe = 0;

        pthread_mutex_lock(&mutex_fila);  // Protege leitura da fila
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

            // Registra operação no arquivo de log 
            pthread_mutex_lock(&mutex_log);  // Protege escrita no arquivo
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

// Função principal do servidor: inicializa mutexes, cria threads e gerencia recebimento de requisições
int main(int argc, char* argv[]){
    // ===== INICIALIZAÇÃO DE MUTEXES =====
    // Mutex para sincronizar acesso ao banco de dados
    pthread_mutex_init(&mutex_bd,NULL);
    // Mutex para sincronizar acesso à fila de requisições
    pthread_mutex_init(&mutex_fila,NULL);
    // Mutex para sincronizar escrita no arquivo de log
    pthread_mutex_init(&mutex_log,NULL);

    // ===== CONFIGURAÇÃO DO PIPE (FIFO) =====
    // FIFO (Named Pipe) para comunicação entre cliente e servidor
    // Permite que múltiplos clientes enviem requisições ao servidor
    //"portal" para a memória RAM, qualquer programa pode procura-lo
    // pelo nome e colocar dados la dentro
    if(mkfifo(CAMINHO_PIPE, 0777) == -1){
        // verifica se o pipe existe
        if(errno != EEXIST){
            printf("Não foi possível criar o arquivo pipe\n");
        }
    }

    // ===== CRIAÇÃO DO POOL DE THREADS =====
    pthread_t th[THREAD_NUM];  // Array para armazenar IDs das threads
    int i;
    for(i = 0; i < THREAD_NUM; i++){
        // Cria 4 threads workers que processarão requisições em paralelo
        if(pthread_create(&th[i], NULL, &iniciarThread, NULL) != 0){
            perror("Erro ao criar a thread!");
        }
    }
    
    // ===== LOOP PRINCIPAL: Recebimento de Requisições =====
    // Mantém o servidor em funcionamento permanente
    while(1){
        printf("Servidor aberto...\n");
        // Abre o FIFO para leitura (bloqueia até que um cliente conecte)
        int fd = open(CAMINHO_PIPE, O_RDONLY);
        if(fd == -1){
            return 1;
        }
        
        Requisicao dado_recebido;
        
        // Recebe requisições enquanto o cliente estiver conectado
        while(1){
            // Lê dados do pipe - requisição estruturada (struct Requisicao)
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