#ifndef DEFINE_H
#define DEFINE_H

// 256 8x8 tiles for 128x128 screen.
#define TILE_BYTE_SIZE 8
#define BIT_PER_BYTE 8
#define NUM_TILES 16
#define HEIGHT 128
#define WIDTH 128

#define BIT0 0b00000001
#define BIT1 0b00000010
#define BIT2 0b00000100
#define BIT3 0b00001000
#define BIT4 0b00010000
#define BIT5 0b00100000
#define BIT6 0b01000000
#define BIT7 0b10000000

#define MAX_PHYS_OBJ 16
#define MAX_OBJ 32

#define N_BIT(x, n) ((x >> (n)) & 0x01)
#define IN_Y_RANGE(x) (x >= 0 && x <= HEIGHT)
#define IN_X_RANGE(x) (x >= 0 && x <= WIDTH)

#define NOT_JUMPING 0x00
#define START_JUMP 0x10
#define END_JUMP 0x20
#define MAX_STACKED_JUMPS 2

#define SPACE_KEY 0x20
#define A_KEY 0x41
#define D_KEY 0x44

#endif /* DEFINE_H */