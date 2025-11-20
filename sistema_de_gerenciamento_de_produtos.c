#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOME_ARQUIVO "produtos.dat"

typedef struct {
    int codigo;
    char nome[50];
    int quantidade;
    float preco;
} Produto;

void limpar_tela();

void exibir_menu();
void limpar_buffer();
int obter_novo_codigo(Produto* lista, int total);

void incluir_produto(Produto** lista, int* total);
void listar_produtos(Produto* lista, int total);
void buscar_produto(Produto* lista, int total);
void atualizar_produto(Produto* lista, int total);
int obter_indice_por_codigo(Produto* lista, int total, int codigo);

int carregar_dados(Produto** lista);
void salvar_dados(Produto* lista, int total);

int main() {
    Produto* lista_produtos = NULL;

    int total_produtos = 0;
    int opcao;

    printf("Iniciando Gerenciador de Cadastros de Produtos...\n");

    total_produtos = carregar_dados(&lista_produtos);
    printf("Foram carregados %d produtos da base de dados.\n", total_produtos);

    do {
        limpar_tela();
        exibir_menu();
        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            opcao = -1;
        }

        switch (opcao) {
            case 1:
                incluir_produto(&lista_produtos, &total_produtos);
                break;
            case 2:
                listar_produtos(lista_produtos, total_produtos);
                break;
            case 3:
                buscar_produto(lista_produtos, total_produtos);
                break;
            case 4:
                atualizar_produto(lista_produtos, total_produtos);
                break;
            case 0:
                printf("\nSalvando dados e encerrando o programa.\n");
                salvar_dados(lista_produtos, total_produtos);
                if (lista_produtos != NULL) {
                    free(lista_produtos);
                }
                break;
            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
        printf("\nPressione ENTER para continuar...");
        limpar_buffer();
        getchar();

    } while (opcao != 0);

    return 0;
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void limpar_tela() {
    system("cls");
}

void exibir_menu() {
    printf("\n-------- MENU PRINCIPAL --------\n");
    printf("1. Incluir Novo Produto\n");
    printf("2. Listar Todos os Produtos\n");
    printf("3. Buscar Produto por Codigo\n");
    printf("4. Atualizar Produto (Preco/Quantidade)\n");
    printf("0. Salvar e Sair\n");
    printf("\n--------------------------------\n");
    printf("Escolha uma opcao: ");
}


void incluir_produto(Produto** lista, int* total) {
    Produto* temp = realloc(*lista, (*total + 1) * sizeof(Produto));

    if (temp == NULL) {
        printf("\nERRO: Nao foi possivel alocar memoria.\n");
        return;
    }

    *lista = temp;
    int indice = *total;

    printf("\n--- INCLUSAO DE NOVO PRODUTO ---\n");

    (*lista)[indice].codigo = indice + 1;
    printf("Codigo gerado automaticamente: %d\n", (*lista)[indice].codigo);

    printf("Nome do Produto: ");
    limpar_buffer();
    fgets((*lista)[indice].nome, 50, stdin);

    size_t ln = strlen((*lista)[indice].nome) - 1;
    if ((*lista)[indice].nome[ln] == '\n') {
        (*lista)[indice].nome[ln] = '\0';
    }

    printf("Quantidade em estoque: ");
    scanf("%d", &(*lista)[indice].quantidade);

    printf("Preco unitario: ");
    scanf("%f", &(*lista)[indice].preco);

    (*total)++;

    printf("Produto cadastrado com sucesso!\n");

    printf("\nDeseja continuar cadastrando produtos?\n 1.     SIM \n Outro. NAO\nOpcao: ");
    int continuar;
    scanf("%d", &continuar);

    if (continuar == 1) {
        incluir_produto(lista, total);
    }
}

void listar_produtos(Produto* lista, int total) {
    if (total == 0 || lista == NULL) {
        printf("\n>> Nenhum produto cadastrado no sistema.\n");
        return;
    }

    printf("\n--- LISTA DE PRODUTOS (%d itens) ---\n", total);

    printf("%-6s | %-30s | %-10s | %-10s\n", "CODIGO", "NOME", "QTD", "PRECO");
    printf("----------------------------------------------------------------------\n");

    for (int i = 0; i < total; i++) {
        printf("%-06d | %-30s | %-10d | R$ %-9.2f\n",
               lista[i].codigo,
               lista[i].nome,
               lista[i].quantidade,
               lista[i].preco);
    }
    printf("----------------------------------------------------------------------\n");
}

int carregar_dados(Produto** lista) {
FILE* arquivo = fopen(NOME_ARQUIVO, "rb");
    if (arquivo == NULL) {
        return 0;
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho_bytes = ftell(arquivo);
    rewind(arquivo);

    int total_registros = tamanho_bytes / sizeof(Produto);

    if (total_registros > 0) {
        *lista = (Produto*) malloc(total_registros * sizeof(Produto));

        if (*lista == NULL) {
            printf("ERRO FATAL: Falha ao alocar memoria para carregar dados.\n");
            fclose(arquivo);
            return 0;
        }

        size_t lidos = fread(*lista, sizeof(Produto), total_registros, arquivo);

        if (lidos != total_registros) {
            printf("AVISO: O arquivo diz ter tamanho para %d, mas leu %zu produtos.\n", total_registros, lidos);
        }
    }

    fclose(arquivo);
    return total_registros;
}

void salvar_dados(Produto* lista, int total) {
    FILE* arquivo = fopen(NOME_ARQUIVO, "wb");

    if (arquivo == NULL) {
        printf("\nERRO CRÍTICO: Nao foi possivel abrir o arquivo para salvar!\n");
        return;
    }

    if (lista != NULL && total > 0) {
        size_t gravados = fwrite(lista, sizeof(Produto), total, arquivo);
        if (gravados == total) {
            printf("\nSucesso: %d produtos foram salvos no arquivo '%s'.\n", total, NOME_ARQUIVO);
        } else {
            printf("\nERRO: Diferenca na gravacao! Tentou salvar %d, mas gravou %zu.\n", total, gravados);
        }
    } else {
        printf("\nLista vazia. O arquivo foi atualizado (agora esta vazio).\n");
    }

    fclose(arquivo);
}

void buscar_produto(Produto* lista, int total) {
    if (total == 0 || lista == NULL) {
        printf("\n>> Base de dados vazia. Nada para buscar.\n");
        return;
    }

    int codigo_busca;
    int encontrado = 0;

    printf("\n--- BUSCA DE PRODUTO ---\n");
    printf("Digite o CODIGO do produto que deseja visualizar: ");
    scanf("%d", &codigo_busca);

    for (int i = 0; i < total; i++) {
        if (lista[i].codigo == codigo_busca) {

            printf("\n[!] Produto Encontrado:\n");
            printf("-------------------------------------\n");
            printf("Nome:       %s\n", lista[i].nome);
            printf("Codigo:     %d\n", lista[i].codigo);
            printf("Quantidade: %d unidades\n", lista[i].quantidade);
            printf("Preco:      R$ %.2f\n", lista[i].preco);
            printf("-------------------------------------\n");

            encontrado = 1;
            break;
        }
    }

    if (encontrado == 0) {
        printf("\n>> Produto com codigo %d NÃO foi encontrado.\n", codigo_busca);
    }
}

void atualizar_produto(Produto* lista, int total) {
    if (total == 0 || lista == NULL) {
        printf("\n>> Base de dados vazia. Nada para atualizar.\n");
        return;
    }

    int codigo_alvo;
    int indice_encontrado = -1;

    printf("\n--- ATUALIZACAO DE PRODUTO ---\n");
    printf("Digite o CODIGO do produto que deseja alterar: ");
    scanf("%d", &codigo_alvo);

    for (int i = 0; i < total; i++) {
        if (lista[i].codigo == codigo_alvo) {
            indice_encontrado = i;
            break;
        }
    }

    if (indice_encontrado == -1) {
        printf("\n>> Produto com codigo %d nao encontrado.\n", codigo_alvo);
        return;
    }

    Produto* p = &lista[indice_encontrado];

    printf("\nProduto Selecionado: %s\n", p->nome);
    printf("  1. Quantidade Atual: %d\n", p->quantidade);
    printf("  2. Preco Atual:      R$ %.2f\n", p->preco);
    printf("------------------------------\n");
    printf("O que voce deseja atualizar? (1-Qtd / 2-Preco / 0-Cancelar): ");

    int opcao_edit;
    scanf("%d", &opcao_edit);

    switch (opcao_edit) {
        case 1:
            printf("Digite a NOVA quantidade: ");
            scanf("%d", &p->quantidade);
            printf(">> Estoque atualizado com sucesso!\n");
            break;

        case 2:
            printf("Digite o NOVO preço: ");
            scanf("%f", &p->preco);
            printf(">> Preco atualizado com sucesso!\n");
            break;

        case 0:
            printf(">> Operacao cancelada.\n");
            break;

        default:
            printf(">> Opcao invalida.\n");
    }
}
