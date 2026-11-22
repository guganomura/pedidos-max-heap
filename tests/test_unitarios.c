#include <limits.h>
#include <string.h>

#include "../src/heap.h"
#include "../src/visualizacao.h"

static int total, falhas;

static void verificar(const char *id, const char *cenario, const char *esperado, int condicao) {
    total++;
    if (!condicao) falhas++;
    printf("[%s] %s %s -> %s\n", condicao ? "OK" : "FALHA", id, cenario, esperado);
}

static Pedido pedido(int id, int prioridade) {
    Pedido p;
    p.id = id;
    snprintf(p.descricao, sizeof p.descricao, "Pedido %d", id);
    p.prioridade = prioridade;
    return p;
}

/* IDs de 1 a n, na ordem do vetor de prioridades. */
static void preencher(Pedido *v, const int *prioridades, int n) {
    int i;
    for (i = 0; i < n; i++) v[i] = pedido(i + 1, prioridades[i]);
}

/* Poe o vetor no heap exatamente como esta, sem construir. */
static void carregar(Heap *h, const int *prioridades, int n) {
    inicializar_heap(h, n, NULL, 0);
    preencher(h->dados, prioridades, n);
    h->tamanho = n;
}

static int vetor_igual(const Heap *h, const int *prioridades, int n) {
    int i;
    if (h->tamanho != n) return 0;
    for (i = 0; i < n; i++)
        if (h->dados[i].prioridade != prioridades[i]) return 0;
    return 1;
}

static int eh_max_heap(const Heap *h) {
    int i;
    if (h->tamanho < 0 || h->tamanho > h->capacidade) return 0;
    for (i = 1; i < h->tamanho; i++)
        if (h->dados[(i - 1) / 2].prioridade < h->dados[i].prioridade) return 0;
    return 1;
}

static int impresso(const Heap *h, const char *esperado) {
    char texto[256];
    FILE *f = tmpfile();
    size_t n;
    if (f == NULL) return 0;
    print_heap(h, f);
    rewind(f);
    n = fread(texto, 1, sizeof texto - 1, f);
    texto[n] = '\0';
    fclose(f);
    return strcmp(texto, esperado) == 0;
}

static void testar_indices(void) {
    int i, certo = 1;
    verificar("IDX-01", "pai dos indices 1, 2, 3 e 4", "0, 0, 1 e 1",
              pai(1) == 0 && pai(2) == 0 && pai(3) == 1 && pai(4) == 1);
    verificar("IDX-02", "filho esquerdo de 0 e 3", "1 e 7", esquerdo(0) == 1 && esquerdo(3) == 7);
    verificar("IDX-03", "filho direito de 0 e 3", "2 e 8", direito(0) == 2 && direito(3) == 8);
    for (i = 0; i < 100000; i++)
        if (pai(esquerdo(i)) != i || pai(direito(i)) != i) certo = 0;
    verificar("IDX-04", "pai de cada filho, 100000 indices", "volta ao proprio indice", certo);
}

static void testar_inicializar_heap(void) {
    const int v[] = {2, 5, 3, 8, 1}, esperado[] = {8, 5, 3, 2, 1};
    Pedido p[5];
    Heap h;
    preencher(p, v, 5);
    verificar("INI-01", "heap vazio com capacidade 4", "tamanho 0, capacidade 4",
              inicializar_heap(&h, 4, NULL, 0) && h.dados && h.tamanho == 0 && h.capacidade == 4);
    liberar_heap(&h);
    verificar("INI-02", "capacidade zero", "rejeita", !inicializar_heap(&h, 0, NULL, 0) && !h.dados);
    verificar("INI-03", "capacidade negativa", "rejeita",
              !inicializar_heap(&h, -1, NULL, 0) && !h.dados);
    verificar("INI-04", "a partir de um vetor", "[8, 5, 3, 2, 1]",
              inicializar_heap(&h, 10, p, 5) && vetor_igual(&h, esperado, 5) && h.capacidade == 10);
    verificar("INI-05", "vetor de origem", "nao e alterado",
              p[0].prioridade == 2 && p[3].prioridade == 8 && h.dados != p);
    liberar_heap(&h);
    verificar("INI-06", "vetor maior que a capacidade", "capacidade sobe para 5",
              inicializar_heap(&h, 2, p, 5) && h.capacidade == 5 && vetor_igual(&h, esperado, 5));
    liberar_heap(&h);
    verificar("INI-07", "tamanho sem vetor", "rejeita", !inicializar_heap(&h, 4, NULL, 3) && !h.dados);
    verificar("INI-08", "tamanho negativo", "rejeita", !inicializar_heap(&h, 4, p, -1) && !h.dados);
    inicializar_heap(&h, 4, NULL, 0);
    liberar_heap(&h);
    liberar_heap(&h);
    verificar("INI-09", "liberar duas vezes", "heap zerado, sem falha",
              !h.dados && !h.tamanho && !h.capacidade);
}

static void testar_inserir(void) {
    const int so2[] = {2}, fim[] = {9, 5, 3, 1}, sobe[] = {9, 5, 3}, um[] = {9, 7, 3, 5};
    const int v[] = {2, 5, 3, 8, 1}, exemplo[] = {8, 5, 3, 2, 1};
    Heap h;
    int i, certo = 1;
    inicializar_heap(&h, 4, NULL, 0);
    verificar("INS-01", "inserir no heap vazio", "fica na raiz",
              inserir(&h, pedido(1, 2)) && vetor_igual(&h, so2, 1));
    liberar_heap(&h);

    inicializar_heap(&h, 4, NULL, 0);
    inserir(&h, pedido(1, 5));
    inserir(&h, pedido(2, 3));
    inserir(&h, pedido(3, 9));
    verificar("INS-02", "maior de todos", "sobe ate a raiz", h.dados[0].prioridade == 9 && h.dados[2].prioridade == 5);
    liberar_heap(&h);

    carregar(&h, sobe, 3);
    verificar("INS-03", "menor que o pai", "permanece no final",
              inserir(&h, pedido(4, 1)) && vetor_igual(&h, fim, 4));
    liberar_heap(&h);

    carregar(&h, sobe, 3);
    verificar("INS-04", "maior que o pai, menor que a raiz", "sobe um nivel",
              inserir(&h, pedido(4, 7)) && vetor_igual(&h, um, 4));
    liberar_heap(&h);

    inicializar_heap(&h, 4, NULL, 0);
    for (i = 0; i < 5; i++) inserir(&h, pedido(i + 1, v[i]));
    verificar("INS-05", "exemplo do enunciado", "[8, 5, 3, 2, 1]", vetor_igual(&h, exemplo, 5));
    verificar("INS-06", "quinto pedido com capacidade 4", "capacidade dobra para 8", h.capacidade == 8);
    verificar("INS-07", "troca leva o pedido inteiro", "ID e descricao acompanham",
              h.dados[0].id == 4 && strcmp(h.dados[0].descricao, "Pedido 4") == 0);
    liberar_heap(&h);

    inicializar_heap(&h, 4, NULL, 0);
    inserir(&h, pedido(1, 5));
    inserir(&h, pedido(2, 5));
    verificar("INS-08", "prioridade igual a do pai", "nao troca", h.dados[0].id == 1);
    liberar_heap(&h);

    inicializar_heap(&h, 4, NULL, 0);
    inserir(&h, pedido(1, INT_MIN));
    inserir(&h, pedido(2, INT_MAX));
    inserir(&h, pedido(3, -1));
    verificar("INS-09", "extremos de int e negativos", "compara sem estouro",
              h.dados[0].id == 2 && eh_max_heap(&h));
    liberar_heap(&h);

    inicializar_heap(&h, 1, NULL, 0);
    for (i = 0; i < 100000; i++)
        if (!inserir(&h, pedido(i, i))) certo = 0;
    verificar("INS-10", "100000 pedidos em capacidade 1", "cresce e continua heap",
              certo && h.tamanho == 100000 && h.capacidade == 131072 && eh_max_heap(&h));
    liberar_heap(&h);

    inicializar_heap(&h, 1, NULL, 0);
    liberar_heap(&h);
    verificar("INS-11", "inserir em heap liberado", "aloca de novo",
              inserir(&h, pedido(1, 2)) && vetor_igual(&h, so2, 1));
    liberar_heap(&h);
}

static void testar_remover(void) {
    const int antes[] = {8, 5, 3, 2}, depois[] = {5, 2, 3}, direita[] = {9, 4, 7, 1}, dir2[] = {7, 4, 1};
    Heap h;
    Pedido p = pedido(77, 0);
    inicializar_heap(&h, 4, NULL, 0);
    verificar("REM-01", "remover de heap vazio", "falha, destino inalterado", !remover(&h, &p) && p.id == 77);
    inserir(&h, pedido(1, 4));
    verificar("REM-02", "remover o unico", "heap vazio",
              remover(&h, &p) && p.id == 1 && h.tamanho == 0);
    liberar_heap(&h);

    carregar(&h, antes, 4);
    verificar("REM-03", "remover de [8, 5, 3, 2]", "retorna o 8",
              remover(&h, &p) && p.prioridade == 8 && strcmp(p.descricao, "Pedido 1") == 0);
    verificar("REM-04", "vetor apos a remocao", "[5, 2, 3], como no enunciado", vetor_igual(&h, depois, 3));
    verificar("REM-05", "capacidade apos a remocao", "nao muda", h.capacidade == 4);
    liberar_heap(&h);

    carregar(&h, direita, 4);
    verificar("REM-06", "filho direito e o maior", "desce pela direita",
              remover(&h, NULL) && vetor_igual(&h, dir2, 3));
    remover(&h, NULL);
    remover(&h, NULL);
    remover(&h, NULL);
    verificar("REM-07", "inserir apos esvaziar", "volta a funcionar",
              h.tamanho == 0 && inserir(&h, pedido(9, 6)) && h.dados[0].id == 9);
    liberar_heap(&h);
}

static void testar_max_heapfy(void) {
    const int a[] = {1, 9, 3}, a2[] = {9, 1, 3}, b[] = {1, 3, 9}, b2[] = {9, 3, 1};
    const int c[] = {9, 5, 3}, d[] = {1, 9, 8, 7, 6}, d2[] = {9, 7, 8, 1, 6};
    const int e[] = {1, 4}, e2[] = {4, 1}, f[] = {5, 5, 5}, g[] = {9, 1, 8, 7, 6}, g2[] = {9, 7, 8, 1, 6};
    Heap h;
    carregar(&h, a, 3);
    max_heapfy(&h, 0);
    verificar("MAX-01", "esquerdo e o maior", "troca com o esquerdo", vetor_igual(&h, a2, 3));
    liberar_heap(&h);
    carregar(&h, b, 3);
    max_heapfy(&h, 0);
    verificar("MAX-02", "direito e o maior", "troca com o direito", vetor_igual(&h, b2, 3));
    liberar_heap(&h);
    carregar(&h, c, 3);
    max_heapfy(&h, 0);
    verificar("MAX-03", "pai ja e o maior", "nao troca", vetor_igual(&h, c, 3));
    liberar_heap(&h);
    carregar(&h, d, 5);
    max_heapfy(&h, 0);
    verificar("MAX-04", "precisa descer dois niveis", "[9, 7, 8, 1, 6]", vetor_igual(&h, d2, 5));
    liberar_heap(&h);
    carregar(&h, e, 2);
    max_heapfy(&h, 0);
    verificar("MAX-05", "apenas o filho esquerdo", "troca", vetor_igual(&h, e2, 2));
    liberar_heap(&h);
    carregar(&h, a, 3);
    h.tamanho = 1;
    max_heapfy(&h, 0);
    verificar("MAX-06", "filhos fora do tamanho", "sao ignorados", h.dados[0].prioridade == 1);
    liberar_heap(&h);
    carregar(&h, f, 3);
    max_heapfy(&h, 0);
    verificar("MAX-07", "prioridades iguais", "nao troca", h.dados[0].id == 1);
    liberar_heap(&h);
    carregar(&h, g, 5);
    max_heapfy(&h, 1);
    verificar("MAX-08", "chamar em indice interno", "corrige a subarvore", vetor_igual(&h, g2, 5));
    liberar_heap(&h);
}

static void testar_construir(void) {
    const int a[] = {2, 5, 3, 8, 1}, a2[] = {8, 5, 3, 2, 1}, b[] = {1, 2, 3, 4, 5, 6, 7};
    const int b2[] = {7, 5, 6, 4, 2, 1, 3}, c[] = {4}, d[] = {3, 3, 3, 3};
    Heap h;
    carregar(&h, a, 5);
    construir(&h);
    verificar("CON-01", "exemplo do enunciado", "[8, 5, 3, 2, 1]", vetor_igual(&h, a2, 5));
    construir(&h);
    verificar("CON-02", "vetor que ja e heap", "nao muda", vetor_igual(&h, a2, 5));
    verificar("CON-03", "inserir apos construir", "continua heap",
              inserir(&h, pedido(6, 9)) && h.dados[0].id == 6 && eh_max_heap(&h));
    liberar_heap(&h);
    carregar(&h, b, 7);
    construir(&h);
    verificar("CON-04", "vetor crescente", "[7, 5, 6, 4, 2, 1, 3]", vetor_igual(&h, b2, 7));
    liberar_heap(&h);
    inicializar_heap(&h, 4, NULL, 0);
    construir(&h);
    verificar("CON-05", "vetor vazio", "continua vazio", h.tamanho == 0);
    liberar_heap(&h);
    carregar(&h, c, 1);
    construir(&h);
    verificar("CON-06", "um elemento", "nao muda", vetor_igual(&h, c, 1));
    liberar_heap(&h);
    carregar(&h, d, 4);
    construir(&h);
    verificar("CON-07", "todos iguais", "nao troca", h.dados[0].id == 1 && h.dados[3].id == 4);
    liberar_heap(&h);
}

static void testar_print_heap(void) {
    const int v[] = {8, 5, 3};
    Heap h;
    inicializar_heap(&h, 4, NULL, 0);
    verificar("PRI-01", "heap vazio", "avisa que nao ha pedidos",
              impresso(&h, "Nenhum pedido aguardando atendimento.\n"));
    inserir(&h, pedido(104, 8));
    verificar("PRI-02", "um pedido", "ID e prioridade",
              impresso(&h, "ID: 104 | Prioridade: 8\n"));
    liberar_heap(&h);
    carregar(&h, v, 3);
    verificar("PRI-03", "varios pedidos", "ordem do vetor",
              impresso(&h, "ID: 1 | Prioridade: 8\nID: 2 | Prioridade: 5\nID: 3 | Prioridade: 3\n"));
    verificar("PRI-04", "exibir nao remove", "heap inalterado", vetor_igual(&h, v, 3));
    liberar_heap(&h);
}

/* Esvazia o heap conferindo a cada passo a ordem, a propriedade e os IDs. */
static int esvazia_certo(Heap *h, const int *prioridades, int n) {
    int i, anterior = INT_MAX, vistos = 0;
    Pedido p;
    for (i = 0; i < n; i++) {
        if (!eh_max_heap(h) || !remover(h, &p)) return 0;
        if (p.id < 1 || p.id > n || (vistos & (1 << p.id))) return 0;
        if (p.prioridade != prioridades[p.id - 1] || p.prioridade > anterior) return 0;
        vistos |= 1 << p.id;
        anterior = p.prioridade;
    }
    return h->tamanho == 0;
}

/* Todo vetor de n <= 7 posicoes com valores de 0 a n - 1, inclusive repetidos. */
static void testar_todos_os_vetores(void) {
    int n, i, j, combinacoes, resto, v[7];
    int casos = 0, por_construir = 1, por_inserir = 1;
    Pedido p[7];
    Heap a, b;
    for (n = 1; n <= 7; n++) {
        for (combinacoes = 1, i = 0; i < n; i++) combinacoes *= n;
        for (i = 0; i < combinacoes; i++) {
            for (j = 0, resto = i; j < n; j++, resto /= n) {
                v[j] = resto % n;
                p[j].id = j + 1;
                p[j].descricao[0] = '\0';
                p[j].prioridade = v[j];
            }
            if (!inicializar_heap(&a, n, p, n) || !esvazia_certo(&a, v, n)) por_construir = 0;
            inicializar_heap(&b, 1, NULL, 0);
            for (j = 0; j < n; j++)
                if (!inserir(&b, p[j]) || !eh_max_heap(&b)) por_inserir = 0;
            if (!esvazia_certo(&b, v, n)) por_inserir = 0;
            liberar_heap(&a);
            liberar_heap(&b);
            casos++;
        }
    }
    verificar("EXA-01", "construir os 873612 vetores de ate 7 itens", "atende em ordem decrescente",
              por_construir && casos == 873612);
    verificar("EXA-02", "inserir um a um os 873612 vetores", "propriedade mantida a cada passo",
              por_inserir && casos == 873612);
}

int main(void) {
    testar_indices();
    testar_inicializar_heap();
    testar_inserir();
    testar_remover();
    testar_max_heapfy();
    testar_construir();
    testar_print_heap();
    testar_todos_os_vetores();
    printf("\nUnitarios: %d testes, %d falhas\n", total, falhas);
    return falhas != 0;
}
