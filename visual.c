#include "visual.h"
#include "jogo.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>

static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_BITMAP *verso = NULL;
static ALLEGRO_BITMAP *cache_imagens[52] = {NULL};
static ALLEGRO_FONT *fonte = NULL;

int inicializar_visual() {
    if (!al_init()) {
        printf("Erro ao inicializar Allegro.\n");
        return 0;
    }

    if (!al_init_image_addon()) {
        printf("Erro ao inicializar o addon de imagem.\n");
        return 0;
    }

    if (!al_init_primitives_addon()) {
        printf("Erro ao inicializar addon de primitivas.\n");
        return 0;
    }

    al_init_font_addon();
    al_init_ttf_addon();

    display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!display) {
        printf("Erro ao criar janela.\n");
        return 0;
    }

    verso = al_load_bitmap("imagens/verso.png");
    if (!verso) {
        printf("Erro ao carregar imagem do verso.\n");
        return 0;
    }

    fonte = al_create_builtin_font();
    if (!fonte) {
        printf("Erro ao criar fonte.\n");
        return 0;
    }

    return 1;
}

int indice_carta(Carta c) {
    int naipe_offset;
    switch (c.naipe) {
        case 'C': naipe_offset = 0; break;
        case 'E': naipe_offset = 13; break;
        case 'O': naipe_offset = 26; break;
        case 'P': naipe_offset = 39; break;
        default: return -1;
    }
    return naipe_offset + (c.valor - 1);
}

ALLEGRO_BITMAP *obter_imagem_carta(Carta c) {
    if (c.virada == 0) return verso;

    int idx = indice_carta(c);
    if (idx < 0 || idx >= 52) return verso;

    if (!cache_imagens[idx]) {
        char caminho[32];
        snprintf(caminho, sizeof(caminho), "imagens/%d_%c.png", c.valor, c.naipe);
        cache_imagens[idx] = al_load_bitmap(caminho);
        if (!cache_imagens[idx]) return verso;
    }

    return cache_imagens[idx];
}

void desenhar_carta(ALLEGRO_BITMAP *img, int x, int y) {
    if (img) {
        al_draw_scaled_bitmap(img, 0, 0,
                              al_get_bitmap_width(img), al_get_bitmap_height(img),
                              x, y, CARTA_LARGURA, CARTA_ALTURA, 0);
    }
}

void desenhar_estoque() {
    int x = ESPACO;
    int y = ESPACO;

    if (estoque.topo) {
        al_draw_scaled_bitmap(verso, 0, 0,
                              al_get_bitmap_width(verso), al_get_bitmap_height(verso),
                              x, y, CARTA_LARGURA, CARTA_ALTURA, 0);
    } else {
        al_draw_rectangle(x, y, x + CARTA_LARGURA, y + CARTA_ALTURA, al_map_rgb(200, 200, 200), 2);
    }
}

void desenhar_descarte() {
    int x = ESPACO * 2 + CARTA_LARGURA;
    int y = ESPACO;

    if (descarte.topo && !(arrastando && origem_descarte)) {
        ALLEGRO_BITMAP *img = obter_imagem_carta(descarte.topo->carta);
        desenhar_carta(img, x, y);
    } else {
        al_draw_rectangle(x, y, x + CARTA_LARGURA, y + CARTA_ALTURA, al_map_rgb(255, 0, 0), 2);
    }
}

void desenhar_fundacoes() {
    for (int i = 0; i < 4; i++) {
        int x = LARGURA_TELA - (ESPACO + (4 - i) * (CARTA_LARGURA + ESPACO));
        int y = ESPACO;

        if (fundacoes[i].topo) {
            ALLEGRO_BITMAP *img = obter_imagem_carta(fundacoes[i].topo->carta);
            desenhar_carta(img, x, y);
        } else {
            al_draw_rectangle(x, y, x + CARTA_LARGURA, y + CARTA_ALTURA,
                            al_map_rgb(255, 255, 255), 2);
        }
    }
}

void desenhar_colunas_iniciais() {
    int base_y = CARTA_ALTURA + 2 * ESPACO;

    for (int col = 0; col < 7; col++) {
        int x = ESPACO + col * (CARTA_LARGURA + ESPACO);
        int y = base_y;

        if (!colunas[col].topo) {
            al_draw_rectangle(x, y, x + CARTA_LARGURA, y + CARTA_ALTURA,
                            al_map_rgb(255, 255, 255), 2);
            continue;
        }

        NoCarta *cartas[52];
        int n = 0;
        NoCarta *atual = colunas[col].topo;
        while (atual && n < 52) {
            cartas[n++] = atual;
            atual = atual->next;
        }

        for (int i = n - 1; i >= 0; i--) {
            Carta c = cartas[i]->carta;

            int eh_parte_sequencia = 0;
            if (arrastando && carta_removida_temporariamente && col == origem_coluna) {
                for (int s = 0; s < tamanho_sequencia; s++) {
                    if (c.valor == sequencia_movimento[s].valor &&
                        c.naipe == sequencia_movimento[s].naipe) {
                        eh_parte_sequencia = 1;
                        break;
                    }
                }
            }

            if (!eh_parte_sequencia) {
                ALLEGRO_BITMAP *img = obter_imagem_carta(c);
                desenhar_carta(img, x, y);
            }

            y += (c.virada ? 25 : 15);
        }
    }
}

void desenhar_botao_novo_jogo() {
    int botao_x = LARGURA_TELA / 2 - 60;
    int botao_y = ALTURA_TELA - 50;
    int botao_largura = 120;
    int botao_altura = 35;

    // desenha botao
    al_draw_filled_rectangle(botao_x, botao_y,
                             botao_x + botao_largura, botao_y + botao_altura,
                             al_map_rgb(100, 150, 100));
    al_draw_rectangle(botao_x, botao_y,
                     botao_x + botao_largura, botao_y + botao_altura,
                     al_map_rgb(255, 255, 255), 2);

    // texto botao
    al_draw_text(fonte, al_map_rgb(255, 255, 255),
                LARGURA_TELA / 2, botao_y + 12,
                ALLEGRO_ALIGN_CENTER, "NOVO JOGO");
}

void desenhar_vitoria() {

    // mensagem de vitória
    al_draw_text(fonte, al_map_rgb(255, 215, 0),
                LARGURA_TELA / 2, ALTURA_TELA /2 + 120,
                ALLEGRO_ALIGN_CENTER, "PARABENS!");
    al_draw_text(fonte, al_map_rgb(200, 200, 200),
                LARGURA_TELA / 2, ALTURA_TELA - 120,
                ALLEGRO_ALIGN_CENTER, "Clique em NOVO JOGO para jogar novamente");
}

void desenhar_tabuleiro_inicial() {
    al_clear_to_color(al_map_rgb(0, 128, 0));

    desenhar_estoque();
    desenhar_descarte();
    desenhar_fundacoes();
    desenhar_colunas_iniciais();

    // desenha sequencia em movimento
    if (arrastando && tamanho_sequencia > 0) {
        int offset_y = 0;
        for (int i = 0; i < tamanho_sequencia; i++) {
            ALLEGRO_BITMAP *img = obter_imagem_carta(sequencia_movimento[i]);
            desenhar_carta(img, mouse_x - CARTA_LARGURA / 2,
                          mouse_y - CARTA_ALTURA / 2 + offset_y);
            offset_y += 25;
        }
    }


    desenhar_botao_novo_jogo();

    if (jogo_ganho) {
        desenhar_vitoria();
    }

    atualizar_tela();
}

void atualizar_tela() {
    al_flip_display();
}

void finalizar_visual() {
    if (verso) {
        al_destroy_bitmap(verso);
        verso = NULL;
    }

    for (int i = 0; i < 52; i++) {
        if (cache_imagens[i]) {
            al_destroy_bitmap(cache_imagens[i]);
            cache_imagens[i] = NULL;
        }
    }

    // deletar a fonte
    if (fonte) {
        al_destroy_font(fonte);
        fonte = NULL;
    }

    if (display) {
        al_destroy_display(display);
        display = NULL;
    }
}

ALLEGRO_DISPLAY *obter_display() {
    return display;
}
