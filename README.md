# Projeto Sistema de Processamento Paralelo de Requisições a um Banco de Dados

## Descrição
Sistema que simula um gerenciador de banco de dados utilizando threads e concorrência.

## Disciplina:
Sistemas Operacionais

## Acadêmicas:
- Beatriz Pimentel Bagesteiro Alves
- Maria Eduarda Santos
- Yasmin Tarnovski Faccin

## Tecnologias
- Linguagem C
- Threads (pthread)
- Mutex (controle de concorrência)
- IPC (FIFO / Named Pipe)

## Requisitos de Execução
Este projeto deve ser executado em ambiente Linux ou no GitHub Codespaces, pois utiliza:
- FIFO (Named Pipes)
- Threads POSIX (pthread)
- Chamadas de sistema específicas do Linux

Obs: Não é garantido funcionamento em Windows sem adaptações.

---

## Como compilar
```bash
cd src
gcc servidor.c -o servidor -lpthread
gcc cliente.c -o cliente
```

## Como executar
Em um terminal:
```bash
./servidor
```
Em outro(s) terminal(is):
```bash
cd src
./cliente
```

---

## Teste de Concorrência 
Para demonstrar o processamento concorrente do servidor, é possível executar múltiplos clientes simultaneamente enviando várias requisições automaticamente.

---

### Passo 1: Compilar e executar o servidor

```bash
cd src
gcc servidor.c -o servidor -lpthread
./servidor
```

---

### Passo 2: Compilar e executar múltiplos clientes
Em outro terminal:
```bash
cd src
gcc cliente.c -o cliente

# Teste de estresse (500 requisições)
./cliente 3

# Executa 5 clientes simultaneamente
for i in {1..5}; do ./cliente 2 & done

# ou
for i in {1..5}; do ./cliente 2 & done; wait
```
Obs: O comando `wait` faz o terminal aguardar a finalização de todos os processos em background, evitando que o prompt fique “preso” ou desatualizado após a execução concorrente. Nele não precisa dar enter para o prompt voltar ao normal.

---


## Observações sobre concorrência
Devido ao uso de threads, o sistema executa as requisições de forma concorrente, ou seja, a ordem de execução não é garantida.

Isso significa que:
- Algumas operações podem ser executadas fora da ordem em que foram enviadas;
- Um SELECT pode ocorrer antes de um INSERT ou após um DELETE, dependendo do escalonamento das threads;
- Por isso, em alguns casos, operações podem retornar "Falha" mesmo estando corretas logicamente.

→ Exemplo:
- Após um DELETE, um SELECT do mesmo ID irá retornar falha, pois o registro não existe mais.
- Além disso, mesmo sem DELETE, em execução concorrente: 
  - Um SELECT pode ocorrer antes do INSERT correspondente;
  - Isso é esperado em sistemas concorrentes e **faz parte do comportamento do projeto**.

---

## Personalização das requisições
O arquivo `requisicoes.h` contém listas de requisições prontas para testes, como:
`lista_teste`,
`lista_insert`,
`lista_select`,
`lista_update`,
`lista_delete`.

→ O usuário pode:
- Alterar as listas existentes;
- Criar novas combinações de operações;
- Simular diferentes cenários de concorrência.

Isso permite testar facilmente:
- Inserções em massa;
- Consultas simultâneas;
- Atualizações concorrentes;
- Remoções e seus impactos.

---

## Registro de Execução `auditoria.log`
O sistema gera automaticamente um arquivo chamado `auditoria.log`, que registra todas as operações processadas pelo servidor.

Cada linha do log contém:
- ID da thread que executou a operação;
- PID do cliente que enviou a requisição;
- Tipo da operação (INSERT, SELECT, UPDATE, DELETE);
- Status da execução (Sucesso ou Falha).

Exemplo de saída:
```bash
Thread 12345 | PID Cliente: 21644 | Operação: INSERT | Status: Sucesso
Thread 12346 | PID Cliente: 21645 | Operação: DELETE | Status: Falha
```
---

## Funcionamento do sistema
1. O cliente envia uma requisição (INSERT, SELECT, UPDATE, DELETE);
2. A requisição é enviada via FIFO (pipe nomeado);
3. O servidor recebe a requisição;
4. O servidor adiciona a requisição a uma fila compartilhada;
5. Um pool de threads consome essa fila e processa as requisições;
6. As threads acessam uma estrutura de dados em memória que simula o banco de dados, utilizando mutex para garantir exclusão mútua.

## Estrutura:
```bash
src/
│
├── cliente.c        # Envia requisições para o servidor
├── servidor.c       # Recebe requisições e usa threads
├── banco.h          # Structs e defines      
└── requisicoes.h    # Lista com requisições já prontas
```
