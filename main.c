#include "visual.h"
#include <allegro5/allegro.h>
#include "jogo.h"
#include "structs.h"

int main() {
    if (!inicializar_visual()) {
        printf("Falha ao inicializar visual.\n");
        return -1;
    }

    distribuir_colunas();
    desenhar_tabuleiro_inicial();

    ALLEGRO_EVENT_QUEUE *fila = al_create_event_queue();
    al_register_event_source(fila, al_get_display_event_source(obter_display()));
    al_install_mouse();
    al_register_event_source(fila, al_get_mouse_event_source());

    bool rodando = true;
    while (rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
        }

        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int x = evento.mouse.x;
            int y = evento.mouse.y;

            // clique de novo jogo
            int botao_x = LARGURA_TELA / 2 - 60;
            int botao_y = ALTURA_TELA - 50;
            int botao_largura = 120;
            int botao_altura = 35;

            if (x >= botao_x && x <= botao_x + botao_largura &&
                y >= botao_y && y <= botao_y + botao_altura) {
                reiniciar_jogo();
                desenhar_tabuleiro_inicial();
                continue;
            }

            // verifica clique no estoque
            if (!jogo_ganho) {
                int estoque_x = ESPACO;
                int estoque_y = ESPACO;

                if (x >= estoque_x && x <= estoque_x + CARTA_LARGURA &&
                    y >= estoque_y && y <= estoque_y + CARTA_ALTURA) {

                    revelar_carta_do_estoque();
                    desenhar_tabuleiro_inicial();
                } else {
                    processar_inicio_arrasto(x, y);
                }
            }
        }

        if (evento.type == ALLEGRO_EVENT_MOUSE_AXES && arrastando) {
            mouse_x = evento.mouse.x;
            mouse_y = evento.mouse.y;
            desenhar_tabuleiro_inicial();
        }

        if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && arrastando) {
            int x = evento.mouse.x;
            int y = evento.mouse.y;
            processar_fim_arrasto(x, y);
            desenhar_tabuleiro_inicial();
        }
    }

    al_destroy_event_queue(fila);
    finalizar_visual();
    return 0;
}
