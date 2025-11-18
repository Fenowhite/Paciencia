#include "jogo.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Pilha colunas[7];
Pilha estoque;
Pilha descarte;
Pilha fundacoes[4];

int arrastando = 0;
Carta carta_em_movimento;
int origem_coluna = -1;
int origem_descarte = 0;
int mouse_x = 0, mouse_y = 0;
int carta_removida_temporariamente = 0;
Carta sequencia_movimento[52];
int tamanho_sequencia = 0;

int jogo_ganho = 0;

void embaralhar(Carta *baralho, int tamanho) {
    srand(time(NULL));
    for (int i = 0; i < tamanho; i++) {
        int j = rand() % tamanho;
        Carta temp = baralho[i];
        baralho[i] = baralho[j];
        baralho[j] = temp;
    }
}

void empilhar(Pilha *p, Carta c) {
    NoCarta *novo = malloc(sizeof(NoCarta));
    novo->carta = c;
    novo->next = p->topo;
    p->topo = novo;
    p->tamanho++;
}

Carta desempilhar(Pilha *p) {
    if (!p->topo) return (Carta){0, 'X', 0};
    NoCarta *remover = p->topo;
    Carta c = remover->carta;
    p->topo = remover->next;
    free(remover);
    p->tamanho--;
    return c;
}

int eh_vermelho(char naipe) {
    return (naipe == 'C' || naipe == 'O');
}

int pode_mover(Carta origem, Carta destino) {
    return origem.virada && destino.virada &&
           origem.valor == destino.valor - 1 &&
           eh_vermelho(origem.naipe) != eh_vermelho(destino.naipe);
}

int pode_mover_fundacao(Carta c, int fundacao_index) {
    Pilha *f = &fundacoes[fundacao_index];

    if (!f->topo) {
        return c.valor == 1;
    }

    Carta topo = f->topo->carta;
    return (c.naipe == topo.naipe && c.valor == topo.valor + 1);
}

void remover_carta_origem() {
    if (origem_coluna >= 0) {
        desempilhar(&colunas[origem_coluna]);
        if (colunas[origem_coluna].topo &&
            colunas[origem_coluna].topo->carta.virada == 0) {
            colunas[origem_coluna].topo->carta.virada = 1;
        }
    }
    if (origem_descarte) {
        desempilhar(&descarte);
    }
}

//remove sequencia se der certo
void remover_sequencia_origem() {
    if (origem_coluna >= 0) {
        // remove todas as cartas da sequência
        for (int i = 0; i < tamanho_sequencia; i++) {
            desempilhar(&colunas[origem_coluna]);
        }
        // vira a carta que ficou em cima
        if (colunas[origem_coluna].topo &&
            colunas[origem_coluna].topo->carta.virada == 0) {
            colunas[origem_coluna].topo->carta.virada = 1;
        }
    }
    if (origem_descarte) {
        desempilhar(&descarte);
    }
}
// verifica se jogador ganhou
int verificar_vitoria() {
    for (int i = 0; i < 4; i++) {
        if (fundacoes[i].tamanho != 13) {
            return 0;
        }
    }
    return 1;
}

//libera memória de uma pilha
void limpar_pilha(Pilha *p) {
    while (p->topo) {
        desempilhar(p);
    }
}

void reiniciar_jogo() {
    for (int i = 0; i < 7; i++) {
        limpar_pilha(&colunas[i]);
    }
    limpar_pilha(&estoque);
    limpar_pilha(&descarte);
    for (int i = 0; i < 4; i++) {
        limpar_pilha(&fundacoes[i]);
    }

    //resetar estados
    jogo_ganho = 0;
    arrastando = 0;
    origem_coluna = -1;
    origem_descarte = 0;
    carta_removida_temporariamente = 0;
    tamanho_sequencia = 0;
    distribuir_colunas();
}
void distribuir_colunas() {
    Carta baralho[MAX_CARTAS];
    int pos = 0;

    for (int i = 0; i < 4; i++) {
        for (int v = 1; v <= 13; v++) {
            baralho[pos++] = (Carta){v, "CEOP"[i], 0};
        }
    }

    embaralhar(baralho, MAX_CARTAS);

    pos = 0;

    for (int i = 0; i < 7; i++) {
        colunas[i].topo = NULL;
        colunas[i].tamanho = 0;

        for (int j = 0; j <= i; j++) {
            Carta c = baralho[pos++];
            c.virada = 0;
            empilhar(&colunas[i], c);
        }

        if (colunas[i].topo) {
            colunas[i].topo->carta.virada = 1;
        }
    }

    estoque.topo = NULL;
    estoque.tamanho = 0;
    descarte.topo = NULL;
    descarte.tamanho = 0;

    for (int i = 0; i < 4; i++) {
        fundacoes[i].topo = NULL;
        fundacoes[i].tamanho = 0;
    }

    while (pos < MAX_CARTAS) {
        Carta c = baralho[pos++];
        c.virada = 0;
        empilhar(&estoque, c);
    }
}

void revelar_carta_do_estoque() {
    if (estoque.topo) {
        Carta c = desempilhar(&estoque);
        c.virada = 1;
        empilhar(&descarte, c);
    } else if (descarte.topo) {
        while (descarte.topo) {
            Carta c = desempilhar(&descarte);
            c.virada = 0;
            empilhar(&estoque, c);
        }
    }
}

void processar_inicio_arrasto(int x, int y) {
    if (jogo_ganho) return;

    tamanho_sequencia = 0;


    int descarte_x = ESPACO * 2 + CARTA_LARGURA;
    int descarte_y = ESPACO;

    if (x >= descarte_x && x <= descarte_x + CARTA_LARGURA &&
        y >= descarte_y && y <= descarte_y + CARTA_ALTURA &&
        descarte.topo && descarte.topo->carta.virada) {

        arrastando = 1;
        carta_em_movimento = descarte.topo->carta;
        sequencia_movimento[0] = descarte.topo->carta;
        tamanho_sequencia = 1;
        origem_descarte = 1;
        carta_removida_temporariamente = 1;
        mouse_x = x;
        mouse_y = y;
        return;
    }

    // Verifica colunas
    int base_y = CARTA_ALTURA + 2 * ESPACO;

    for (int col = 0; col < 7; col++) {
        int col_x = ESPACO + col * (CARTA_LARGURA + ESPACO);

        // Copia a pilha para um array temporário
        NoCarta *cartas[52];
        int n = 0;
        NoCarta *atual = colunas[col].topo;
        while (atual && n < 52) {
            cartas[n++] = atual;
            atual = atual->next;
        }

        // Percorre de baixo para cima verificando cliques
        int carta_y = base_y;
        int indice_clicado = -1;

        for (int i = n - 1; i >= 0; i--) {
            Carta c = cartas[i]->carta;
            int offset = c.virada ? 25 : 15;
            int altura_clicavel = (i == 0) ? CARTA_ALTURA : offset;

            if (x >= col_x && x <= col_x + CARTA_LARGURA &&
                y >= carta_y && y < carta_y + altura_clicavel) {

                if (c.virada) {
                    indice_clicado = i;
                }
                break;
            }

            carta_y += offset;
        }

        // Se clicou em uma carta válida
        if (indice_clicado >= 0) {
            // Verifica se pode pegar múltiplas cartas (da clicada até o topo)
            int pode_pegar_sequencia = 1;

            // Valida a sequência do índice clicado até o topo (índice 0)
            for (int i = indice_clicado; i > 0; i--) {
                Carta atual_carta = cartas[i]->carta;
                Carta proxima = cartas[i - 1]->carta;

                if (!pode_mover(proxima, atual_carta)) {
                    pode_pegar_sequencia = 0;
                    break;
                }
            }

            if (pode_pegar_sequencia) {
                // Pega todas as cartas do índice clicado até o topo
                arrastando = 1;
                tamanho_sequencia = indice_clicado + 1;

                // Copia as cartas para o array (do fundo para o topo)
                for (int i = 0; i < tamanho_sequencia; i++) {
                    sequencia_movimento[i] = cartas[indice_clicado - i]->carta;
                }

                // A primeira carta da sequência é usada para validação
                carta_em_movimento = sequencia_movimento[0];

                origem_coluna = col;
                mouse_x = x;
                mouse_y = y;
                carta_removida_temporariamente = 1;
                return;
            }
        }
    }
}

void processar_fim_arrasto(int x, int y) {
    if (!arrastando) return;

    int base_y = CARTA_ALTURA + 2 * ESPACO;
    int movimento_valido = 0;

    if (tamanho_sequencia == 1) {
        for (int i = 0; i < 4; i++) {
            int fund_x = LARGURA_TELA - (ESPACO + (4 - i) * (CARTA_LARGURA + ESPACO));
            int fund_y = ESPACO;

            if (x >= fund_x && x <= fund_x + CARTA_LARGURA &&
                y >= fund_y && y <= fund_y + CARTA_ALTURA) {

                if (pode_mover_fundacao(carta_em_movimento, i)) {
                    empilhar(&fundacoes[i], carta_em_movimento);
                    remover_carta_origem();
                    movimento_valido = 1;

                    if (verificar_vitoria()) {
                        jogo_ganho = 1;
                    }
                    break;
                }
            }
        }
    }

    if (!movimento_valido) {
        for (int col = 0; col < 7; col++) {
            int col_x = ESPACO + col * (CARTA_LARGURA + ESPACO);

            if (x >= col_x && x <= col_x + CARTA_LARGURA && y >= base_y) {
                NoCarta *destino = colunas[col].topo;

                //rei em coluna vazia
                if (!destino && carta_em_movimento.valor == 13) {

                    for (int i = 0; i < tamanho_sequencia; i++) {
                        empilhar(&colunas[col], sequencia_movimento[i]);
                    }
                    remover_sequencia_origem();
                    movimento_valido = 1;
                    break;
                }

                //else
                if (destino && pode_mover(carta_em_movimento, destino->carta)) {

                    for (int i = 0; i < tamanho_sequencia; i++) {
                        empilhar(&colunas[col], sequencia_movimento[i]);
                    }
                    remover_sequencia_origem();
                    movimento_valido = 1;
                    break;
                }
            }
        }
    }

    arrastando = 0;
    origem_coluna = -1;
    origem_descarte = 0;
    carta_removida_temporariamente = 0;
    tamanho_sequencia = 0;
}
