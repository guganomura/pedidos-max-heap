#!/bin/sh
# Executa o sistema com entradas prontas e confere a saida de cada cenario.
# Uso: sh tests/test_sistema.sh [executavel]

programa=${1:-build/pedidos}
total=0
falhas=0

# testar ID CENARIO ESPERADO ENTRADA TRECHO_DA_SAIDA [TRECHO_PROIBIDO]
# Os trechos podem ter varias linhas.
testar() {
    total=$((total + 1))
    passou=0
    if saida=$(printf "$4" | "$programa" 2>&1); then
        case "$saida" in *"$5"*) passou=1 ;; esac
        if [ -n "$6" ]; then
            case "$saida" in *"$6"*) passou=0 ;; esac
        fi
    fi
    if [ "$passou" -eq 1 ]; then
        echo "[OK] $1 $2 -> $3"
    else
        falhas=$((falhas + 1))
        echo "[FALHA] $1 $2 -> $3"
    fi
}

# Os quatro cadastros da execucao do enunciado.
Q='1\n101\nPedido de 10 produtos\n2\n1\n102\nPedido de 5 produtos\n5\n'
Q="${Q}1\n103\nPedido de 20 produtos\n3\n1\n104\nPedido urgente\n8\n"

LISTA='Pedidos:
ID: 104 | Prioridade: 8
ID: 102 | Prioridade: 5
ID: 103 | Prioridade: 3
ID: 101 | Prioridade: 2'

ATENDIDO='Pedido atendido:
ID: 104
Descrição: Pedido urgente
Prioridade: 8

Pedidos restantes:
ID: 102 | Prioridade: 5
ID: 101 | Prioridade: 2
ID: 103 | Prioridade: 3'

ARVORE='[8]
|-- E: [5]
|   |-- E: [2]
|   `-- D: [1]
`-- D: [3]'

MENU='===== SISTEMA DE PEDIDOS =====

1 - Cadastrar pedido
2 - Atender pedido
3 - Exibir pedidos
4 - Exibir quantidade de pedidos
5 - Sair'

testar SIS-01 "menu do enunciado" "opcoes 1 a 5 iguais" '5\n' "$MENU"
testar SIS-02 "exibir no exemplo do enunciado" "104, 102, 103, 101" "${Q}3\n5\n" "$LISTA"
testar SIS-03 "atender no exemplo do enunciado" "104, restam 102, 101, 103" "${Q}2\n5\n" "$ATENDIDO"
testar SIS-04 "iniciar com heap vazio" "0 pedidos" '4\n5\n' "Pedidos aguardando atendimento: 0"
testar SIS-05 "exibir heap vazio" "avisa" '3\n5\n' "Nenhum pedido aguardando atendimento."
testar SIS-06 "cadastrar um pedido" "aparece na lista" '1\n7\nCaixa\n4\n3\n5\n' "ID: 7 | Prioridade: 4"
testar SIS-07 "descricao cadastrada" "devolvida ao atender" '1\n7\nCaixa grande\n4\n2\n5\n' "Descrição: Caixa grande"
testar SIS-08 "quantidade apos cadastros" "4" "${Q}4\n5\n" "Pedidos aguardando atendimento: 4"
testar SIS-09 "quantidade apos atender" "3" "${Q}2\n4\n5\n" "Pedidos aguardando atendimento: 3"
testar SIS-10 "segundo atendimento" "ID 102" "${Q}2\n2\n5\n" 'Pedido atendido:
ID: 102'
testar SIS-11 "terceiro atendimento" "ID 103" "${Q}2\n2\n2\n5\n" 'Pedido atendido:
ID: 103'
testar SIS-12 "atender ate esvaziar" "ultimo e o 101" "${Q}2\n2\n2\n2\n5\n" 'Pedido atendido:
ID: 101
Descrição: Pedido de 10 produtos
Prioridade: 2

Pedidos restantes:
Nenhum pedido aguardando atendimento.'
testar SIS-13 "atender heap vazio" "avisa" '2\n5\n' "Não há pedidos para atender."
testar SIS-14 "cadastrar apos esvaziar" "volta a funcionar" '1\n1\nA\n1\n2\n1\n2\nB\n9\n3\n5\n' "ID: 2 | Prioridade: 9"
testar SIS-15 "estado: vetor" "[8, 5, 3, 2]" "${Q}6\n5\n" "Vetor: [8, 5, 3, 2]"
testar SIS-16 "estado: tamanho e capacidade" "4 e 4" "${Q}6\n5\n" "Tamanho: 4 | Capacidade: 4"
testar SIS-17 "estado: capacidade apos crescer" "5 e 8" "${Q}1\n105\nE\n1\n6\n5\n" "Tamanho: 5 | Capacidade: 8"
testar SIS-18 "estado: arvore" "a do enunciado" '7\n1\n6\n5\n' "$ARVORE"
testar SIS-19 "estado do heap vazio" "arvore vazia" '6\n5\n' "(vazia)"
testar SIS-20 "conjunto de exemplo: antes" "[2, 5, 3, 8, 1]" '7\n1\n5\n' "Vetor recebido: [2, 5, 3, 8, 1]"
testar SIS-21 "conjunto de exemplo: depois" "[8, 5, 3, 2, 1]" '7\n1\n5\n' "Heap construído: [8, 5, 3, 2, 1]"
testar SIS-22 "conjunto digitado" "[9, 1, 4]" '7\n2\n3\n1\nA\n1\n2\nB\n9\n3\nC\n4\n5\n' "Heap construído: [9, 1, 4]"
testar SIS-23 "ID repetido no conjunto" "pede outro" '7\n2\n2\n1\nA\n1\n1\n2\nB\n2\n5\n' "Já existe um pedido com ID 1."
testar SIS-24 "cadastrar apos construir" "9 vai a raiz" '7\n1\n1\n6\nF\n9\n6\n5\n' "Vetor: [9, 5, 8, 2, 1, 3]"
testar SIS-25 "atender apos construir" "Pedido D" '7\n1\n2\n5\n' "Descrição: Pedido D"
testar SIS-26 "construir sobre pedidos, confirmando" "substitui" "${Q}7\ns\n1\n4\n5\n" "Pedidos aguardando atendimento: 5"
testar SIS-27 "construir sobre pedidos, cancelando" "mantem" "${Q}7\nn\n4\n5\n" "Pedidos aguardando atendimento: 4"
testar SIS-28 "inicializar vazio, confirmando" "0 pedidos" "${Q}8\ns\n4\n5\n" "Pedidos aguardando atendimento: 0"
testar SIS-29 "inicializar vazio, cancelando" "mantem" "${Q}8\nn\n4\n5\n" "Pedidos aguardando atendimento: 4"
testar SIS-30 "inicializar heap ja vazio" "sem confirmacao" '8\n5\n' "Heap vazio inicializado."
testar SIS-31 "confirmacao invalida" "pede de novo" "${Q}8\nx\nn\n5\n" "Responda com s ou n."
testar SIS-32 "ID repetido no cadastro" "pede outro" "${Q}1\n101\n105\nE\n1\n5\n" "Já existe um pedido com ID 101."
testar SIS-33 "ID de pedido atendido" "pode reutilizar" '1\n1\nA\n1\n2\n1\n1\nB\n7\n3\n5\n' "ID: 1 | Prioridade: 7" "Já existe"
testar SIS-34 "ID zero" "pede de novo" '1\n0\n1\nA\n1\n5\n' "Digite um número inteiro entre 1 e"
testar SIS-35 "descricao vazia" "pede de novo" '1\n1\n \nA\n1\n5\n' "A descrição não pode ficar vazia."
testar SIS-36 "prioridade com texto" "pede de novo" '1\n1\nA\nalta\n1\n5\n' "Digite um número inteiro entre -"
testar SIS-37 "prioridade fora de int" "pede de novo" '1\n1\nA\n99999999999999999999\n1\n5\n' "Digite um número inteiro entre -"
testar SIS-38 "prioridade negativa" "aceita" '1\n1\nA\n-3\n3\n5\n' "ID: 1 | Prioridade: -3"
testar SIS-39 "descricao com acentos" "preserva" '1\n1\nCafé e açúcar\n2\n2\n5\n' "Descrição: Café e açúcar"
testar SIS-40 "opcao invalida" "avisa e mostra o menu" '9\n5\n' "Opção inválida."
testar SIS-41 "sair" "programa termina" '5\n' "Sistema encerrado." "Fim da entrada"
testar SIS-42 "fim inesperado da entrada" "encerra sem travar" '1\n1\n' "Fim da entrada"

echo
echo "Sistema: $total testes, $falhas falhas"
[ "$falhas" -eq 0 ]
