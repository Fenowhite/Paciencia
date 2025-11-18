#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_CARTAS 52
#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define CARTA_LARGURA 100
#define CARTA_ALTURA 125
#define ESPACO 10

typedef struct {
    int valor;       // 1 a 13
    char naipe;      // 'C', 'E', 'O', 'P'
    int virada;      // 0 = para baixo, 1 = para cima
} Carta;

typedef struct NoCarta {
    Carta carta;
    struct NoCarta *next;
} NoCarta;

typedef struct {
    NoCarta *topo;
    int tamanho;
} Pilha;

#endif
