// Lista com requisições já prontas
#ifndef REQUISICOES_H
#define REQUISICOES_H

#include "banco.h"

// lista organizada para testes
Requisicao lista_teste[20] = {
    // INSERT
    {OP_INSERT, {1, "Ana"}},
    {OP_INSERT, {2, "Carlos"}},
    {OP_INSERT, {3, "Beatriz"}},
    {OP_INSERT, {4, "Lucas"}},
    {OP_INSERT, {5, "Fernanda"}},

    // SELECT
    {OP_SELECT, {1, ""}},
    {OP_SELECT, {3, ""}},
    {OP_SELECT, {5, ""}},

    // UPDATE
    {OP_UPDATE, {1, "Ana_Silva"}},
    {OP_UPDATE, {2, "Carlos_Edu"}},
    {OP_UPDATE, {3, "Bia_Mendes"}},

    // SELECT (verifica updates)
    {OP_SELECT, {1, ""}},
    {OP_SELECT, {2, ""}},
    {OP_SELECT, {3, ""}},

    // DELETE
    {OP_DELETE, {2, ""}},
    {OP_DELETE, {4, ""}},

    // SELECT finais
    {OP_SELECT, {2, ""}},
    {OP_SELECT, {4, ""}},
    {OP_SELECT, {5, ""}}
};

Requisicao lista_insert[10] = {
    {OP_INSERT, {1, "Ana"}},
    {OP_INSERT, {2, "Carlos"}},
    {OP_INSERT, {3, "Beatriz"}},
    {OP_INSERT, {4, "Lucas"}},
    {OP_INSERT, {5, "Maria"}},
    {OP_INSERT, {6, "Rafael"}},
    {OP_INSERT, {7, "Joao"}},
    {OP_INSERT, {8, "Mariana"}},
    {OP_INSERT, {9, "Yasmin"}},
    {OP_INSERT, {10, "Juliana"}}
};

Requisicao lista_select[10] = {
    {OP_SELECT, {1, ""}},
    {OP_SELECT, {2, ""}},
    {OP_SELECT, {3, ""}},
    {OP_SELECT, {4, ""}},
    {OP_SELECT, {5, ""}},
    {OP_SELECT, {6, ""}},
    {OP_SELECT, {7, ""}},
    {OP_SELECT, {8, ""}},
    {OP_SELECT, {9, ""}},
    {OP_SELECT, {10, ""}}
};

Requisicao lista_update[10] = {
    {OP_UPDATE, {1, "Ana_Silva"}},
    {OP_UPDATE, {2, "Carlos_Edu"}},
    {OP_UPDATE, {3, "Bia_Mendes"}},
    {OP_UPDATE, {4, "Lucas_M"}},
    {OP_UPDATE, {5, "Fer_Costa"}},
    {OP_UPDATE, {6, "Rafa_Dias"}},
    {OP_UPDATE, {7, "Joao_Pedro"}},
    {OP_UPDATE, {8, "Mari_Alves"}},
    {OP_UPDATE, {9, "Pedro_H"}},
    {OP_UPDATE, {10, "Ju_Lima"}}
};

Requisicao lista_delete[10] = {
    {OP_DELETE, {1, ""}},
    {OP_DELETE, {2, ""}},
    {OP_DELETE, {3, ""}},
    {OP_DELETE, {4, ""}},
    {OP_DELETE, {5, ""}},
    {OP_DELETE, {6, ""}},
    {OP_DELETE, {7, ""}},
    {OP_DELETE, {8, ""}},
    {OP_DELETE, {9, ""}},
    {OP_DELETE, {10, ""}}
};

#endif