#ifndef VISUAL_H
#define VISUAL_H
#include "jogo.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define CARTA_LARGURA 100
#define CARTA_ALTURA 125
#define ESPACO 10

extern int tamanho_sequencia;
int inicializar_visual();
void desenhar_tabuleiro_inicial();
void desenhar_carta(ALLEGRO_BITMAP *img, int x, int y);
void atualizar_tela();
void finalizar_visual();
ALLEGRO_DISPLAY *obter_display();

#endif


