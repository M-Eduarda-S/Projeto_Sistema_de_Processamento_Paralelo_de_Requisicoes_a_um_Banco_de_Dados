// Esse banco.h define as informações que cliente e servidor usam em comum, como os dados e o formato das requisições

#ifndef BANCO_H
#define BANCO_H

#include <stdio.h>

#define TAMANHO_NOME 50
#define TAMANHO_MEMORIA_TEMPORARIA 256  // buffer temporário
#define CAMINHO_PIPE "/tmp/bd_pipe" // localização do FIFO servidor

// tipos de operações
typedef enum {OP_INSERT, OP_DELETE,OP_SELECT, OP_UPDATE} OperacaoTipo;

// estrutura de registro
typedef struct {
    int id;                   
    char nome[TAMANHO_NOME];   
} Registro;

// estrutura de requisição 
typedef struct {
    OperacaoTipo tipo; 
    pid_t pid;          // id do processo cliente
    Registro reg;       // dados do registro
} Requisicao;

#endif