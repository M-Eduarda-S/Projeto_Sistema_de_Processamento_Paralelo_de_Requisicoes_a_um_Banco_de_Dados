# Projeto Sistema de Processamento Paralelo de Requisições a um Banco de Dados

## Descrição
Sistema que simula um gerenciador de banco de dados utilizando threads e concorrência.

## Acadêmicas:
- Beatriz Pimentel Bagesteiro Alves
- Maria Eduarda Santos
- Yasmin Tarnovski Faccin

## Tecnologias
- Python
- Threads (threading)
- Mutex (Lock)

## Como executar
```bash
python src/servidor.py

## Estrutura:
```bash
src/
│
├── cliente.py        # Envia requisições para o servidor
├── servidor.py       # Recebe requisições e usa threads
├── banco.py          # Structs e defines
├── dados.txt         # Simulação da base de dados
├── utils.py          
└── main.py           
