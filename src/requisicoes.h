// Lista com requisições já prontas

#ifndef REQUISICOES_H
#define REQUISICOES_H

#include "banco.h"

// lista de teste em ordem lógica: 5 INSERT, 3 SELECT, 3 UPDATE, 3 SELECT, 2 DELETE e 3 SELECT
Requisicao lista_teste[20] = {
    {OP_INSERT, -1,{1, "Ana"}},
    {OP_INSERT, -1,{2, "Carlos"}},
    {OP_INSERT, -1,{3, "Beatriz"}},
    {OP_INSERT, -1,{4, "Lucas"}},
    {OP_INSERT, -1,{5, "Fernanda"}},

    {OP_SELECT, -1,{1, ""}},
    {OP_SELECT, -1,{3, ""}},
    {OP_SELECT, -1,{5, ""}},

    {OP_UPDATE, -1,{1, "Ana_Silva"}},
    {OP_UPDATE, -1,{2, "Carlos_Edu"}},
    {OP_UPDATE, -1,{3, "Bia_Mendes"}},

    // verifica updates
    {OP_SELECT, -1,{1, ""}},
    {OP_SELECT, -1,{2, ""}},
    {OP_SELECT, -1,{3, ""}},

    {OP_DELETE, -1,{2, ""}},
    {OP_DELETE, -1,{4, ""}},

    {OP_SELECT, -1,{2, ""}}, // Status: Falha - registro com id 2 foi deletado antes
    {OP_SELECT, -1,{4, ""}}, // Status: Falha - registro com id 4 foi deletado antes
    {OP_SELECT, -1,{5, ""}}  // pode falhar em execução concorrente, já que a ordem das threads é não garantida
};

// requisições apenas de INSERT
Requisicao lista_insert[10] = {
    {OP_INSERT, 0,{1, "Ana"}},
    {OP_INSERT, 0,{2, "Carlos"}},
    {OP_INSERT, 0,{3, "Beatriz"}},
    {OP_INSERT, 0,{4, "Lucas"}},
    {OP_INSERT, 0,{5, "Maria"}},
    {OP_INSERT, 0,{6, "Rafael"}},
    {OP_INSERT, 0,{7, "Joao"}},
    {OP_INSERT, 0,{8, "Mariana"}},
    {OP_INSERT, 0,{9, "Yasmin"}},
    {OP_INSERT, 0,{10, "Juliana"}}
};

// requisições apenas de SELECT 
Requisicao lista_select[10] = {
    {OP_SELECT, 0,{1, ""}},
    {OP_SELECT, 0,{2, ""}},
    {OP_SELECT, 0,{3, ""}},
    {OP_SELECT, 0,{4, ""}},
    {OP_SELECT, 0,{5, ""}},
    {OP_SELECT, 0,{6, ""}},
    {OP_SELECT, 0,{7, ""}},
    {OP_SELECT, 0,{8, ""}},
    {OP_SELECT, 0,{9, ""}},
    {OP_SELECT, 0,{10, ""}}
};

// requisições apenas de UPDATE
Requisicao lista_update[10] = {
    {OP_UPDATE, 0,{1, "Ana_Silva"}},
    {OP_UPDATE, 0,{2, "Carlos_Edu"}},
    {OP_UPDATE, 0,{3, "Bia_Mendes"}},
    {OP_UPDATE, 0,{4, "Lucas_M"}},
    {OP_UPDATE, 0,{5, "Fer_Costa"}},
    {OP_UPDATE, 0,{6, "Rafa_Dias"}},
    {OP_UPDATE, 0,{7, "Joao_Pedro"}},
    {OP_UPDATE, 0,{8, "Mari_Alves"}},
    {OP_UPDATE, 0,{9, "Pedro_H"}},
    {OP_UPDATE, 0,{10, "Ju_Lima"}}
};

// requisições apenas de DELETE
Requisicao lista_delete[10] = {
    {OP_DELETE, 0,{1, ""}},
    {OP_DELETE, 0,{2, ""}},
    {OP_DELETE, 0,{3, ""}},
    {OP_DELETE, 0,{4, ""}},
    {OP_DELETE, 0,{5, ""}},
    {OP_DELETE, 0,{6, ""}},
    {OP_DELETE, 0,{7, ""}},
    {OP_DELETE, 0,{8, ""}},
    {OP_DELETE, 0,{9, ""}},
    {OP_DELETE, 0,{10, ""}}
};

#endif