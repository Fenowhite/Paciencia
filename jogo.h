#ifndef JOGO_H
#define JOGO_H

#include "structs.h"

extern Pilha colunas[7];
extern Pilha estoque;
extern Pilha descarte;
extern Pilha fundacoes[4];


extern int arrastando;
extern Carta carta_em_movimento;
extern int origem_coluna;
extern int origem_descarte;
extern int mouse_x, mouse_y;
extern int carta_removida_temporariamente;

extern Carta sequencia_movimento[52]; //array para múltiplas cartas
extern int tamanho_sequencia;
extern int jogo_ganho;

//funções
void embaralhar(Carta *baralho, int tamanho);
void distribuir_colunas();
void empilhar(Pilha *p, Carta c);
Carta desempilhar(Pilha *p);
int pode_mover(Carta origem, Carta destino);
int pode_mover_fundacao(Carta c, int fundacao_index);
void revelar_carta_do_estoque();
int verificar_vitoria();
void reiniciar_jogo();

//mouse
void processar_inicio_arrasto(int x, int y);
void processar_fim_arrasto(int x, int y);

#endif


