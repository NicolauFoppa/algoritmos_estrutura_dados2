#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 30
#define INTERVALO_INDICE 100
#pragma pack(1)

// ----------------------------
// ESTRUTURAS
// ----------------------------
typedef struct {
    char order_id[20];
    char user_id[20];
    int quantity;
    float price_usd;
    char date_time[20];
    char gender;
    char status; // 'A' = Ativo, 'R' = Removido
} Pedido;

typedef struct {
    char product_id[20];
    char category_id[20];
    char category_alias[20];
    char brand_id[20];
    float price_usd;
    char gender;
    char status; // 'A' = Ativo, 'R' = Removido
} Joia;

typedef struct {
    char chave[20];
    long posicao;
} Indice;

#pragma pack()


// ----------------------------
// FUNÇÕES UTILITÁRIAS
// ----------------------------
int compararPedidos(const void *a, const void *b) {
    return strncmp(((Pedido*)a)->order_id, ((Pedido*)b)->order_id, 20);
}
int compararJoias(const void *a, const void *b) {
    return strncmp(((Joia*)a)->product_id, ((Joia*)b)->product_id, 20);
}

void trim_whitespace(char *str) {
    if (str == NULL) return;
    int len = strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
    int start = 0;
    while (str[start] != '\0' && (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r')) {
        start++;
    }
    if (start > 0) {
        memmove(str, str + start, len - start + 1);
    }
}

// ------------------------------------------
// FUNÇÕES DE IMPORTAÇÃO (BUG DO SSCANF CORRIGIDO)
// ------------------------------------------
void processar_pedidos_IMPORTADOR() {
    const char *txt_file = "pedidos_unicos_corrigido_status.txt";
    const char *dat_file = "pedidos.dat";
    const char *idx_file = "indice_pedidos.idx";

    FILE *f_txt = fopen(txt_file, "r");
    if (f_txt == NULL) {
        printf("\nERRO: Arquivo '%s' nao encontrado.\n", txt_file);
        return;
    }
    char buffer[1024];
    int qtd = 0;
    fgets(buffer, sizeof(buffer), f_txt);
    while (fgets(buffer, sizeof(buffer), f_txt) != NULL) qtd++;
    printf("Encontrados %d registros de pedidos.\n", qtd);

    Pedido *vetor = malloc(qtd * sizeof(Pedido));
    if (vetor == NULL) {
        printf("Falha ao alocar memoria para pedidos.\n");
        fclose(f_txt);
        return;
    }

    rewind(f_txt);

    for (int i = 0; i < qtd; i++) {
        fgets(buffer, sizeof(buffer), f_txt);
        memset(&vetor[i], 0, sizeof(Pedido));

        sscanf(buffer, "%19[^,],%19[^,],%d,%f,%19[^,],%c%c",
               vetor[i].order_id, vetor[i].user_id, &vetor[i].quantity,
               &vetor[i].price_usd, vetor[i].date_time, &vetor[i].gender, &vetor[i].status);
        trim_whitespace(vetor[i].order_id);
    }
    fclose(f_txt);

    printf("Ordenando pedidos...\n");
    qsort(vetor, qtd, sizeof(Pedido), compararPedidos);

    FILE *f_dat = fopen(dat_file, "wb");
    FILE *f_idx = fopen(idx_file, "wb");
    if (f_dat == NULL || f_idx == NULL) {
        perror("Erro ao criar arquivos de saida de pedidos");
        free(vetor);
        return;
    }

    printf("Escrevendo '%s' (70 bytes/reg) e '%s'...\n", dat_file, idx_file);
    for (int i = 0; i < qtd; i++) {
        if (i % INTERVALO_INDICE == 0 && vetor[i].status == 'A') {
            long pos_atual = ftell(f_dat);
            Indice idx_rec;
            strcpy(idx_rec.chave, vetor[i].order_id);
            idx_rec.posicao = pos_atual;
            fwrite(&idx_rec, sizeof(Indice), 1, f_idx);
        }
        fwrite(&vetor[i], sizeof(Pedido), 1, f_dat);
    }
    fclose(f_dat);
    fclose(f_idx);
    free(vetor);
    printf("Processamento de pedidos concluido.\n");
}

void processar_joias_IMPORTADOR() {
    const char *txt_file = "joias_unicas_corrigido_status.txt";
    const char *dat_file = "joias.dat";
    const char *idx_file = "indice_joias.idx";

    FILE *f_txt = fopen(txt_file, "r");
    if (f_txt == NULL) {
        printf("\nERRO: Arquivo '%s' nao encontrado.\n", txt_file);
        return;
    }
    char buffer[1024];
    int qtd = 0;
    fgets(buffer, sizeof(buffer), f_txt);
    while (fgets(buffer, sizeof(buffer), f_txt) != NULL) qtd++;
    printf("\nEncontrados %d registros de joias.\n", qtd);

    Joia *vetor = malloc(qtd * sizeof(Joia));
    if (vetor == NULL) {
        printf("Falha ao alocar memoria para joias.\n");
        fclose(f_txt);
        return;
    }

    rewind(f_txt);

    for (int i = 0; i < qtd; i++) {
        fgets(buffer, sizeof(buffer), f_txt);
        memset(&vetor[i], 0, sizeof(Joia));

        sscanf(buffer, "%19[^,],%19[^,],%19[^,],%19[^,],%f,%c%c",
               vetor[i].product_id, vetor[i].category_id, vetor[i].category_alias,
               vetor[i].brand_id, &vetor[i].price_usd, &vetor[i].gender, &vetor[i].status);

        trim_whitespace(vetor[i].product_id);

    }
    fclose(f_txt);

    fclose(f_txt);

    printf("Ordenando joias...\n");
    qsort(vetor, qtd, sizeof(Joia), compararJoias);

    FILE *f_dat = fopen(dat_file, "wb");
    FILE *f_idx = fopen(idx_file, "wb");
    if (f_dat == NULL || f_idx == NULL) {
        perror("Erro ao criar arquivos de saida de joias");
        free(vetor);
        return;
    }

    printf("Escrevendo '%s' (86 bytes/reg) e '%s'...\n", dat_file, idx_file);
    for (int i = 0; i < qtd; i++) {
        if (i % INTERVALO_INDICE == 0 && vetor[i].status == 'A') {
            long pos_atual = ftell(f_dat);
            Indice idx_rec;
            strcpy(idx_rec.chave, vetor[i].product_id);
            idx_rec.posicao = pos_atual;
            fwrite(&idx_rec, sizeof(Indice), 1, f_idx);
        }
        fwrite(&vetor[i], sizeof(Joia), 1, f_dat);
    }
    fclose(f_dat);
    fclose(f_idx);
    free(vetor);
    printf("Processamento de joias concluido.\n");
}

// ----------------------------
// FUNÇÕES DE ORDENAÇÃO E REORGANIZAÇÃO
// ----------------------------
void ordenarPedidos(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    if (tamanho == 0) return;
    int qtd = tamanho / sizeof(Pedido);
    rewind(fp);

    Pedido *vetor = malloc(qtd * sizeof(Pedido));
    if (vetor == NULL) {
        printf("Erro de alocacao de memoria para ordenar pedidos.\n");
        return;
    }
    fread(vetor, sizeof(Pedido), qtd, fp);
    qsort(vetor, qtd, sizeof(Pedido), compararPedidos);

    rewind(fp);
    fwrite(vetor, sizeof(Pedido), qtd, fp);
    fflush(fp);
    free(vetor);
    printf("Pedidos ordenados.\n");
}

void ordenarJoias(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    if (tamanho == 0) return;
    int qtd = tamanho / sizeof(Joia);
    rewind(fp);

    Joia *vetor = malloc(qtd * sizeof(Joia));
    if (vetor == NULL) {
        printf("Erro de alocacao de memoria para ordenar joias.\n");
        return;
    }
    fread(vetor, sizeof(Joia), qtd, fp);
    qsort(vetor, qtd, sizeof(Joia), compararJoias);

    rewind(fp);
    fwrite(vetor, sizeof(Joia), qtd, fp);
    fflush(fp);
    free(vetor);
    printf("Joias ordenadas.\n");
}

// ----------------------------
// CRIAÇÃO DE ÍNDICES
// ----------------------------
void criarIndicePedidos(FILE *fp, FILE *fi) {
    Pedido p;
    Indice idx;
    long num_registro_ativo = 0;
    long pos_bytes_atual;

    rewind(fp);
    freopen(NULL, "wb+", fi);

    while (fread(&p, sizeof(Pedido), 1, fp)) {
        if (p.status == 'A') {
            if (num_registro_ativo % INTERVALO_INDICE == 0) {
                pos_bytes_atual = ftell(fp) - sizeof(Pedido);
                strcpy(idx.chave, p.order_id);
                idx.posicao = pos_bytes_atual;
                fwrite(&idx, sizeof(Indice), 1, fi);
            }
            num_registro_ativo++;
        }
    }
    if (num_registro_ativo > 0)
        printf("Indice parcial de pedidos (ativos) criado.\n");
    else
        printf("AVISO: Indice de pedidos criado, mas nenhum registro ativo ('A') foi encontrado no .dat.\n");
}

void criarIndiceJoias(FILE *fp, FILE *fi) {
    Joia j;
    Indice idx;
    long num_registro_ativo = 0;
    long pos_bytes_atual;

    rewind(fp);
    freopen(NULL, "wb+", fi);

    while (fread(&j, sizeof(Joia), 1, fp)) {
        if (j.status == 'A') {
            if (num_registro_ativo % INTERVALO_INDICE == 0) {
                pos_bytes_atual = ftell(fp) - sizeof(Joia);
                strcpy(idx.chave, j.product_id);
                idx.posicao = pos_bytes_atual;
                fwrite(&idx, sizeof(Indice), 1, fi);
            }
            num_registro_ativo++;
        }
    }
    if (num_registro_ativo > 0)
        printf("Indice parcial de joias (ativas) criado.\n");
    else
        printf("AVISO: Indice de joias criado, mas nenhum registro ativo ('A') foi encontrado no .dat.\n");
}

// ----------------------------
// PESQUISA BINÁRIA NO ÍNDICE
// ----------------------------
int pesquisaBinariaIndice(FILE *fi, const char *chave, Indice *resultado) {
    Indice idx;
    int inicio = 0, fim, meio;
    long n_registros_idx;

    if(fi == NULL) {
        printf("ERRO: Ponteiro de arquivo de índice nulo na pesquisa.\n");
        return 0;
    }

    clearerr(fi);
    fseek(fi, 0, SEEK_END);
    n_registros_idx = ftell(fi) / sizeof(Indice);
    fim = n_registros_idx - 1;

    if (n_registros_idx == 0) {
        resultado->posicao = 0;
        return 1;
    }

    rewind(fi);
    fread(resultado, sizeof(Indice), 1, fi);

    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        fseek(fi, meio * sizeof(Indice), SEEK_SET);
        if(fread(&idx, sizeof(Indice), 1, fi) != 1) {
            printf("ERRO ao ler o arquivo de índice no meio da busca.\n");
            return 0;
        }

        int cmp = strncmp(chave, idx.chave, 20);

        if (cmp == 0) {
            *resultado = idx;
            return 1;
        } else if (cmp < 0) {
            fim = meio - 1;
        } else {
            *resultado = idx;
            inicio = meio + 1;
        }
    }
    return 1;
}

// ------------------------------------------
// FUNÇÃO AUXILIAR DE BUSCA
// ------------------------------------------
long buscarPosicaoPedido(FILE *fp, FILE *fi, const char *order_id) {
    Indice idx_bloco_inicial;
    printf("\nDEBUG: Buscando pedido ID: %s\n", order_id);

    if (fp == NULL || fi == NULL) {
        printf("DEBUG: Erro - Ponteiros de arquivo nulos na busca.\n");
        return -1;
    }

    if (pesquisaBinariaIndice(fi, order_id, &idx_bloco_inicial)) {
        Pedido p;
        printf("DEBUG: Bloco encontrado no indice. Chave do bloco: %s, Posicao: %ld\n", idx_bloco_inicial.chave, idx_bloco_inicial.posicao);

        if (idx_bloco_inicial.posicao < 0) {
             printf("DEBUG: Erro - Posicao invalida retornada pelo indice.\n");
             return -1;
        }

        fseek(fp, idx_bloco_inicial.posicao, SEEK_SET);
        printf("DEBUG: Pulando para byte %ld no arquivo de dados.\n", idx_bloco_inicial.posicao);

        int registros_ativos_lidos_no_bloco = 0;
        while (fread(&p, sizeof(Pedido), 1, fp)) {

            char chave_lida[21];
            strncpy(chave_lida, p.order_id, 20);
            chave_lida[20] = '\0';


            int cmp = strcmp(chave_lida, order_id);

            if (cmp == 0 && p.status == 'A') {
                printf("DEBUG: Registro ENCONTRADO!\n");
                return ftell(fp) - sizeof(Pedido);
            }

            if (cmp > 0) {
                 printf("DEBUG: Chave lida ('%s') eh maior que a chave buscada. Parando busca sequencial.\n", chave_lida);
                 break;
            }

            if (p.status == 'A') {
                 registros_ativos_lidos_no_bloco++;
                 if (registros_ativos_lidos_no_bloco >= INTERVALO_INDICE) {
                    printf("DEBUG: Limite do bloco (%d registros ativos) atingido. Parando busca sequencial.\n", INTERVALO_INDICE);
                    break;
                 }
            }
        }

        printf("DEBUG: Fim da busca sequencial no bloco.\n");

    } else {
        printf("DEBUG: Erro na pesquisa binaria do indice.\n");
    }
    printf("DEBUG: Registro nao encontrado apos busca completa.\n");
    return -1;
}

long buscarPosicaoJoia(FILE *fp, FILE *fi, const char *product_id) {
    Indice idx_bloco_inicial;
    printf("\nDEBUG: Buscando joia ID: %s\n", product_id);

    if (fp == NULL || fi == NULL) {
         printf("DEBUG: Erro - Ponteiros de arquivo nulos na busca.\n");
        return -1;
    }

    if (pesquisaBinariaIndice(fi, product_id, &idx_bloco_inicial)) {
        Joia j;
         printf("DEBUG: Bloco encontrado no indice. Chave do bloco: %s, Posicao: %ld\n", idx_bloco_inicial.chave, idx_bloco_inicial.posicao);

        if (idx_bloco_inicial.posicao < 0) {
             printf("DEBUG: Erro - Posicao invalida retornada pelo indice.\n");
             return -1;
        }

        fseek(fp, idx_bloco_inicial.posicao, SEEK_SET);
        printf("DEBUG: Pulando para byte %ld no arquivo de dados.\n", idx_bloco_inicial.posicao);

        int registros_ativos_lidos_no_bloco = 0;
        while (fread(&j, sizeof(Joia), 1, fp)) {

            char chave_lida[21];
            strncpy(chave_lida, j.product_id, 20);
            chave_lida[20] = '\0';

            int cmp = strcmp(chave_lida, product_id);

            if (cmp == 0 && j.status == 'A') {
                 printf("DEBUG: Registro ENCONTRADO!\n");
                return ftell(fp) - sizeof(Joia);
            }

            if (cmp > 0) {
                 printf("DEBUG: Chave lida ('%s') eh maior que a chave buscada. Parando busca sequencial.\n", chave_lida);
                break;
            }

            if(j.status == 'A') {
                registros_ativos_lidos_no_bloco++;
                if (registros_ativos_lidos_no_bloco >= INTERVALO_INDICE) {
                     printf("DEBUG: Limite do bloco (%d registros ativos) atingido. Parando busca sequencial.\n", INTERVALO_INDICE);
                    break;
                }
            }
        }
         printf("DEBUG: Fim da busca sequencial no bloco.\n");
    } else {
         printf("DEBUG: Erro na pesquisa binaria do indice.\n");
    }

     printf("DEBUG: Registro nao encontrado apos busca completa.\n");
    return -1;
}

// ----------------------------
// CONSULTAS
// ----------------------------
void consultarPedido(FILE *fp, FILE *fi, const char *order_id) {
    long pos = buscarPosicaoPedido(fp, fi, order_id);

    if (pos != -1) {
        Pedido p;
        fseek(fp, pos, SEEK_SET);
        fread(&p, sizeof(Pedido), 1, fp);
        printf("\nPedido encontrado:\nID: %s\nJpoia: %s\nValor: %.2f\nQtde: %d\nData: %s\nGenero: %c\n\n",
               p.order_id, p.user_id, p.price_usd, p.quantity, p.date_time, p.gender);
    } else {
        printf("Pedido %s nao encontrado.\n", order_id);
    }
}

void consultarJoia(FILE *fp, FILE *fi, const char *product_id) {
    long pos = buscarPosicaoJoia(fp, fi, product_id);

    if (pos != -1) {
        Joia j;
        fseek(fp, pos, SEEK_SET);
        fread(&j, sizeof(Joia), 1, fp);
        printf("\nJoia encontrada:\nID: %s\nCategoria: %s\nMarca: %s\nValor: %.2f\nGênero: %c\n\n",
               j.product_id, j.category_alias, j.brand_id, j.price_usd, j.gender);
    } else {
        printf("Joia %s nao encontrada.\n", product_id);
    }
}

int contarPedidosInativos(FILE *fpPedidos) {
    if (fpPedidos == NULL) return 0;
    Pedido p;
    int contador = 0;
    long pos_atual = ftell(fpPedidos);
    rewind(fpPedidos);
    while (fread(&p, sizeof(Pedido), 1, fpPedidos) == 1) {
        if (p.status == 'R') {
            contador++;
        }
    }
    fseek(fpPedidos, pos_atual, SEEK_SET);
    return contador;
}

int contarJoiasInativas(FILE *fpJoias) {
    if (fpJoias == NULL) return 0;
    Joia j;
    int contador = 0;
    long pos_atual = ftell(fpJoias);
    rewind(fpJoias);
    while (fread(&j, sizeof(Joia), 1, fpJoias) == 1) {
        if (j.status == 'R') {
            contador++;
        }
    }
    fseek(fpJoias, pos_atual, SEEK_SET);
    return contador;
}

int contarPedidosPorJoia(FILE *fpPedidos, const char *product_id_busca) {
    if (fpPedidos == NULL) {
        printf("ERRO: Arquivo de pedidos nao esta aberto para contagem por joia.\n");
        return 0;
    }

    Pedido p;
    int contador = 0;
    long pos_atual = ftell(fpPedidos);
    rewind(fpPedidos);

    while (fread(&p, sizeof(Pedido), 1, fpPedidos) == 1) {
        if (p.status == 'A') {
            if (strncmp(p.user_id, product_id_busca, 20) == 0) {
                contador++;
            }
        }
    }

    fseek(fpPedidos, pos_atual, SEEK_SET);
    return contador;
}

// ----------------------------
// INSERÇÃO
// ----------------------------
void inserirPedido(FILE *fpPedidos, FILE *idxPedidos) {
    if(fpPedidos == NULL || idxPedidos == NULL) {
        printf("ERRO: Arquivos de pedido nao estao abertos. Tente importar os dados primeiro (Opcao 11).\n");
        return;
    }
    Pedido p;

    printf("\n--- INSERIR NOVO PEDIDO ---\n");
    printf("Order ID: "); scanf("%19s", p.order_id);
    printf("Joia ID: "); scanf("%19s", p.user_id);
    printf("Quantidade: "); scanf("%d", &p.quantity);
    printf("Valor: "); scanf("%f", &p.price_usd);
    printf("Data (YYYY-MM-DD HH:MM): "); scanf(" %[^\n]", p.date_time);
    printf("Genero (M/F): "); scanf(" %c", &p.gender);
    p.status = 'A';

    fseek(fpPedidos, 0, SEEK_END);
    fwrite(&p, sizeof(Pedido), 1, fpPedidos);

    ordenarPedidos(fpPedidos);
    criarIndicePedidos(fpPedidos, idxPedidos);
    printf("Pedido inserido e arquivos reordenados/reindexados.\n");
}

void inserirJoia(FILE *fpJoias, FILE *idxJoias) {
     if(fpJoias == NULL || idxJoias == NULL) {
        printf("ERRO: Arquivos de joia nao estao abertos. Tente importar os dados primeiro (Opcao 11).\n");
        return;
    }
    Joia j;

    printf("\n--- INSERIR NOVA JOIA ---\n");
    printf("Product ID: "); scanf("%19s", j.product_id);
    printf("Category ID: "); scanf("%19s", j.category_id);
    printf("Category Alias: "); scanf("%19s", j.category_alias);
    printf("Brand ID: "); scanf("%19s", j.brand_id);
    printf("Valor: "); scanf("%f", &j.price_usd);
    printf("Genero (M/F): "); scanf(" %c", &j.gender);
    j.status = 'A';

    fseek(fpJoias, 0, SEEK_END);
    fwrite(&j, sizeof(Joia), 1, fpJoias);

    ordenarJoias(fpJoias);
    criarIndiceJoias(fpJoias, idxJoias);
    printf("Joia inserida e arquivos reordenados/reindexados.\n");
}

// ----------------------------
// REMOÇÃO
// ----------------------------
void removerPedido(FILE *fpPedidos, FILE *idxPedidos, const char *order_id) {
    long pos = buscarPosicaoPedido(fpPedidos, idxPedidos, order_id);

    if (pos != -1) {
        Pedido p;
        fseek(fpPedidos, pos, SEEK_SET);
        fread(&p, sizeof(Pedido), 1, fpPedidos);
        p.status = 'R';
        fseek(fpPedidos, pos, SEEK_SET);
        fwrite(&p, sizeof(Pedido), 1, fpPedidos);
        criarIndicePedidos(fpPedidos, idxPedidos);
        printf("Pedido %s removido logicamente. Índice atualizado.\n", order_id);
    } else {
        printf("Pedido %s nao encontrado para remocao.\n", order_id);
    }
}

void removerJoia(FILE *fpJoias, FILE *idxJoias, const char *product_id) {
    long pos = buscarPosicaoJoia(fpJoias, idxJoias, product_id);

    if (pos != -1) {
        Joia j;
        fseek(fpJoias, pos, SEEK_SET);
        fread(&j, sizeof(Joia), 1, fpJoias);
        j.status = 'R';
        fseek(fpJoias, pos, SEEK_SET);
        fwrite(&j, sizeof(Joia), 1, fpJoias);
        criarIndiceJoias(fpJoias, idxJoias);
        printf("Joia %s removida logicamente. Indice atualizado.\n", product_id);
    } else {
        printf("Joia %s nao encontrada para remocao.\n", product_id);
    }
}

// ----------------------------
// ALTERAÇÃO
// ----------------------------
void alterarPedido(FILE *fpPedidos, FILE *idxPedidos, const char *order_id) {
    long pos = buscarPosicaoPedido(fpPedidos, idxPedidos, order_id);

    if (pos != -1) {
        Pedido p;
        fseek(fpPedidos, pos, SEEK_SET);
        fread(&p, sizeof(Pedido), 1, fpPedidos);

        printf("\n--- ALTERANDO PEDIDO %s ---\n", p.order_id);
        int nova_qtde;
        float novo_valor;
        char novo_product_id[20];
        char novo_gender;


        printf("Product id (Atual: %s): ", p.user_id);
        scanf("%19s", novo_product_id);
        getchar();

        strcpy(p.user_id, novo_product_id);

        printf("Quantidade (Atual: %d): ", p.quantity);
        scanf("%d", &nova_qtde);
        p.quantity = nova_qtde;
        getchar();

        printf("Valor (Atual: %.2f): ", p.price_usd);
        scanf("%f", &novo_valor);
        p.price_usd = novo_valor;
        getchar();

        printf("Genero (Atual: %c): ", p.gender);
        scanf(" %c", &novo_gender);
        p.gender = novo_gender;
        getchar();

        fseek(fpPedidos, pos, SEEK_SET);
        fwrite(&p, sizeof(Pedido), 1, fpPedidos);

        printf("Pedido %s alterado com sucesso.\n", order_id);
    } else {
        printf("Pedido %s nao encontrado para alteracao.\n", order_id);
    }
}

void alterarJoia(FILE *fpJoias, FILE *idxJoias, const char *product_id) {
    long pos = buscarPosicaoJoia(fpJoias, idxJoias, product_id);

    if (pos != -1) {
        Joia j;
        fseek(fpJoias, pos, SEEK_SET);
        fread(&j, sizeof(Joia), 1, fpJoias);

        char nova_category_id[20];
        char nova_category_alias[20];
        char nova_brand_id[20];
        float novo_valor;
        char novo_gender;

        printf("\n--- ALTERANDO JOIA %s ---\n", j.product_id);

        while(getchar() != '\n');

        printf("Valor (Atual: %.2f): ", j.price_usd);
        scanf("%f", &novo_valor);
        j.price_usd = novo_valor;
        while(getchar() != '\n');

        printf("Categoria ID (Atual: %s): ", j.category_id);
        scanf("%19s", nova_category_id);
        strcpy(j.category_id, nova_category_id);
        getchar();

        printf("Categoria Alias (Atual: %s): ", j.category_alias);
        scanf("%19s", nova_category_alias);
        strcpy(j.category_alias, nova_category_alias);
        getchar();

        printf("Marca (Atual: %s): ", j.brand_id);
        scanf("%19s", nova_brand_id);
        strcpy(j.brand_id, nova_brand_id);
        getchar();

        printf("Genero (Atual: %c): ", j.gender);
        scanf(" %c", &novo_gender);
        j.gender = novo_gender;
        getchar();

        fseek(fpJoias, pos, SEEK_SET);
        fwrite(&j, sizeof(Joia), 1, fpJoias);
        printf("Joia %s alterada com sucesso.\n", product_id);
    } else {
        printf("Joia %s nao encontrada para alteracao.\n", product_id);
    }
}

// ----------------------------
// FUNÇÕES DE LISTAGEM
// ----------------------------
void listarTodosPedidos(FILE *fpPedidos) {
    if (fpPedidos == NULL) {
        printf("ERRO: Arquivo de pedidos nao esta aberto para listagem.\n");
        return;
    }

    Pedido p;
    long pos_atual = ftell(fpPedidos);
    rewind(fpPedidos);
    int contador = 0;

    printf("\n--- LISTA DE TODOS OS PEDIDOS ATIVOS ---\n");
    while (fread(&p, sizeof(Pedido), 1, fpPedidos) == 1) {
        if (p.status == 'A') {
            printf("ID: %-20s | User/Joia: %-20s | Qtd: %-4d | Preço: %-8.2f | Data: %-19s | Gênero: %c\n",
                   p.order_id, p.user_id, p.quantity, p.price_usd, p.date_time, p.gender);
            contador++;
        }
    }

    if (contador == 0) {
        printf("Nenhum pedido ativo encontrado.\n");
    } else {
        printf("----------------------------------------\n");
        printf("Total de pedidos ativos listados: %d\n", contador);
    }

    fseek(fpPedidos, pos_atual, SEEK_SET);
}


void listarTodasJoias(FILE *fpJoias) {
    if (fpJoias == NULL) {
        printf("ERRO: Arquivo de joias nao esta aberto para listagem.\n");
        return;
    }

    Joia j;
    long pos_atual = ftell(fpJoias);
    rewind(fpJoias);
    int contador = 0;

    printf("\n--- LISTA DE TODAS AS JOIAS ATIVAS ---\n");
    while (fread(&j, sizeof(Joia), 1, fpJoias) == 1) {
        if (j.status == 'A') {
            printf("ID: %-20s | Cat ID: %-20s | Alias: %-20s | Marca: %-20s | Preço: %-8.2f | Gênero: %c\n",
                   j.product_id, j.category_id, j.category_alias, j.brand_id, j.price_usd, j.gender);
            contador++;
        }
    }

     if (contador == 0) {
        printf("Nenhuma joia ativa encontrada.\n");
    } else {
        printf("----------------------------------------\n");
        printf("Total de joias ativas listadas: %d\n", contador);
    }

    fseek(fpJoias, pos_atual, SEEK_SET);
}


// ----------------------------
// MAIN
// ----------------------------
int main() {
    FILE *fpPedidos = fopen("pedidos.dat", "rb+");
    FILE *fpJoias = fopen("joias.dat", "rb+");
    FILE *idxPedidos = fopen("indice_pedidos.idx", "rb+");
    FILE *idxJoias = fopen("indice_joias.idx", "rb+");

    if (fpPedidos == NULL || fpJoias == NULL || idxPedidos == NULL || idxJoias == NULL) {
        printf("AVISO: Arquivos .dat ou .idx nao encontrados.\n");
        printf("Por favor, execute a opcao 14 para (re)importar os dados dos arquivos .txt\n");

        if(fpPedidos) fclose(fpPedidos);
        if(fpJoias) fclose(fpJoias);
        if(idxPedidos) fclose(idxPedidos);
        if(idxJoias) fclose(idxJoias);

        fpPedidos = fopen("pedidos.dat", "wb+");
        fpJoias = fopen("joias.dat", "wb+");
        idxPedidos = fopen("indice_pedidos.idx", "wb+");
        idxJoias = fopen("indice_joias.idx", "wb+");

    } else {
         printf("Arquivos carregados. Sistema pronto.\n");
    }

    if (!fpPedidos || !fpJoias || !idxPedidos || !idxJoias) {
        printf("Erro fatal ao criar novos arquivos. Verifique as permissoes da pasta.\n");
        return 1;
    }

    int opcao = -1;
    char chave[30];

    do {
        printf("\n========== MENU ==========\n");
        printf("1. Inserir pedido\n");
        printf("2. Inserir joia\n");
        printf("3. Consultar pedido\n");
        printf("4. Consultar joia\n");
        printf("5. Alterar pedido\n");
        printf("6. Alterar joia\n");
        printf("7. Remover pedido\n");
        printf("8. Remover joia\n");
        printf("9. Reorganizar arquivos (Ordenar)\n");
        printf("10. Contar dados removidos\n");
        printf("11. Contar pedidos por Joia ID\n");
        printf("12. Listar todos os Pedidos Ativos\n");
        printf("13. Listar todas as Joias Ativas\n");
        printf("14. (RESET) Importar dados do .TXT\n");

        printf("0. Sair\n> ");

        if(scanf("%d", &opcao) != 1) {
             while(getchar() != '\n');
             opcao = -1;
        }
        getchar();

        switch (opcao) {
            case 1: inserirPedido(fpPedidos, idxPedidos); break;
            case 2: inserirJoia(fpJoias, idxJoias); break;
            case 3:
                printf("Order ID para consultar: ");
                scanf("%29s", chave);
                getchar();
                consultarPedido(fpPedidos, idxPedidos, chave);
                break;
            case 4:
                printf("Product ID para consultar: ");
                scanf("%29s", chave);
                getchar();
                consultarJoia(fpJoias, idxJoias, chave);
                break;
            case 5:
                printf("Order ID para alterar: ");
                scanf("%29s", chave);
                getchar();
                alterarPedido(fpPedidos, idxPedidos, chave);
                break;
            case 6:
                printf("Product ID para alterar: ");
                scanf("%29s", chave);
                getchar();
                alterarJoia(fpJoias, idxJoias, chave);
                break;
            case 7:
                printf("Order ID para remover: ");
                scanf("%29s", chave);
                getchar();
                removerPedido(fpPedidos, idxPedidos, chave);
                break;
            case 8:
                printf("Product ID para remover: ");
                scanf("%29s", chave);
                getchar();
                removerJoia(fpJoias, idxJoias, chave);
                break;
            case 9:
                printf("Reorganizando arquivos...\n");
                ordenarPedidos(fpPedidos);
                ordenarJoias(fpJoias);
                criarIndicePedidos(fpPedidos, idxPedidos);
                criarIndiceJoias(fpJoias, idxJoias);
                printf("Arquivos reorganizados e reindexados.\n");
                break;
            case 10:
                printf("\n--- Contagem de Registros Inativos ---\n");
                int inativos_pedidos = contarPedidosInativos(fpPedidos);
                int inativos_joias = contarJoiasInativas(fpJoias);
                printf("Total de Pedidos inativos (status 'R'): %d\n", inativos_pedidos);
                printf("Total de Joias inativas (status 'R'):   %d\n", inativos_joias);
                break;
            case 11:
                printf("Digite o ID da Joia para contar os pedidos: ");
                scanf("%29s", chave);
                getchar();
                int contagem = contarPedidosPorJoia(fpPedidos, chave);
                printf("-> A joia com ID '%s' aparece em %d pedido(s) ativo(s).\n", chave, contagem);
                break;

            case 12:
                listarTodosPedidos(fpPedidos);
                break;
            case 13:
                listarTodasJoias(fpJoias);
                break;
             case 14:
                printf("\n--- IMPORTANDO DADOS (RESET) ---\n");

                fclose(fpPedidos);
                fclose(fpJoias);
                fclose(idxPedidos);
                fclose(idxJoias);


                processar_pedidos_IMPORTADOR();
                processar_joias_IMPORTADOR();

                fpPedidos = fopen("pedidos.dat", "rb+");
                fpJoias = fopen("joias.dat", "rb+");
                idxPedidos = fopen("indice_pedidos.idx", "rb+");
                idxJoias = fopen("indice_joias.idx", "rb+");

                if (!fpPedidos || !fpJoias || !idxPedidos || !idxJoias) {
                    printf("ERRO FATAL: Falha ao recarregar arquivos apos importacao. Encerrando.\n");
                    return 1;
                }

                printf("Importacao concluida. Arquivos recarregados.\n");
                break;
            case 0:
                break;
            default:
                printf("Opção inválida.\n");
                break;
        }
    } while (opcao != 0);

    fclose(fpPedidos);
    fclose(fpJoias);
    fclose(idxPedidos);
    fclose(idxJoias);

    printf("\nEncerrado.\n");
    return 0;
}
