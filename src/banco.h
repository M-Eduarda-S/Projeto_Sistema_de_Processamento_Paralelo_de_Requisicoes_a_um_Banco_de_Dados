// Esse banco.h é para 
#ifndef BANCO_H
#define BANCO_H

#include <stdio.h>

#define TAMANHO_NOME 50
#define TAMANHO_MEMORIA_TEMPORARIA 256
#define CAMINHO_PIPE "/tmp/bd_pipe"

// tipos de Operação
typedef enum { OP_INSERT, OP_DELETE, OP_SELECT, OP_UPDATE } OperacaoTipo;

// estrutura interna do "banco"
typedef struct {
 int id;
 char nome[TAMANHO_NOME];
} Registro;

// estrutura da Requisição via IPC
typedef struct {
    OperacaoTipo tipo;
    Registro reg;
} Requisicao;

#endif