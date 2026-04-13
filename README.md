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

Obs: Este projeto deve ser executado em Linux ou GitHub Codespaces.

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

# Executa 5 clientes simultaneamente
for i in {1..5}; do ./cliente 2 & done
```

---

## Funcionamento do sistema
1. O cliente envia uma requisição (INSERT, SELECT, UPDATE, DELETE)
2. A requisição é enviada via FIFO (pipe nomeado)
3. O servidor recebe a requisição
4. O servidor cria uma thread para processá-la
5. A thread acessa o banco de dados com proteção de mutex

## Estrutura:
```bash
src/
│
├── cliente.c        # Envia requisições para o servidor
├── servidor.c       # Recebe requisições e usa threads
├── banco.txt        # Simulação da base de dados
├── banco.h          # Structs e defines      
└── requisicoes.h    # Lista com requisições já prontas
```
