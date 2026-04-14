// Esse banco.h é para 
#ifndef BANCO_H
#define BANCO_H

#include <stdio.h>

/* ===== CONFIGURAÇÕES ===== */
#define TAMANHO_NOME 50                 // Tamanho máximo do campo nome em um registro
#define TAMANHO_MEMORIA_TEMPORARIA 256  // Tamanho buffer temporário
#define CAMINHO_PIPE "/tmp/bd_pipe"    // Localização do FIFO servidor
#define CAMINHO_PIPE_RES "/tmp/resposta_pipe"  // FIFO para respostas

/* ===== TIPOS DE OPERAÇÃO ===== */
// Definição de operações CRUD suportadas ao banco de dados
typedef enum { 
    OP_INSERT,  
    OP_DELETE,  
    OP_SELECT,  
    OP_UPDATE   
} OperacaoTipo;

/* ===== ESTRUTURA DE REGISTRO ===== */
// Representa um registro no banco de dados simulado
typedef struct {
    int id;                    // Identificador único do registro
    char nome[TAMANHO_NOME];   // Nome associado ao registro
} Registro;

/* ===== ESTRUTURA DE REQUISIÇÃO =====*/
// Pacote de dados enviado cliente -> servidor via FIFO
// Serializado binariamente para transmissão
typedef struct {
    OperacaoTipo tipo;  // Tipo de operação a executar
    pid_t pid;          // ID do processo cliente (para rastreamento)
    Registro reg;       // Dados do registro (ID e nome)
} Requisicao;

#endif