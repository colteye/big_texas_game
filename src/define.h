#ifndef DEFINE_H
#define DEFINE_H

// 256 8x8 tiles for 128x128 screen.
#define TILE_BYTE_SIZE 8
#define BIT_PER_BYTE 8
#define NUM_TILES 16
#define HEIGHT 128
#define WIDTH 128

// Try to keep even offscreen coordinates positive.
#define HEIGHT_OFFSET 64
#define WIDTH_OFFSET 64

#define BIT0 0b00000001
#define BIT1 0b00000010
#define BIT2 0b00000100
#define BIT3 0b00001000
#define BIT4 0b00010000
#define BIT5 0b00100000
#define BIT6 0b01000000
#define BIT7 0b10000000

// Where a collision is happening.
#define COLLIDE_UP BIT0
#define COLLIDE_DOWN BIT1
#define COLLIDE_LEFT BIT2
#define COLLIDE_RIGHT BIT3

#define VISIBLE BIT0
#define MODIFIED BIT1

// Collision layer.
#define COLLIDE_LAYER_1 BIT7 // - Player.
#define COLLIDE_LAYER_2 BIT6
#define COLLIDE_LAYER_3 BIT5 // - NPCs.
#define COLLIDE_LAYER_4 BIT4 // - Environment objects.

// Mask which determines which collider layers can collide with each other.
#define COLLIDE_MATRIX_1 0b10010000 // 1- Collide with layer 1 and layer 4 - Player.
#define COLLIDE_MATRIX_2 0b01100000 // 2 - Collide with layer 2 and 3
#define COLLIDE_MATRIX_3 0b01110000 // 3 - Collide with layer, 2, 3, 4 - NPCs.
#define COLLIDE_MATRIX_4 0b10110000 // 4 - Collide with layer 1, 3, 4 - Environment objects.

#define COLLIDE_MATRIX_MASK 0b11110000 // Mask for easier usage.

#define MAX_PHYS_OBJ 16
#define MAX_OBJ 32

#define ABS(x) (x >= 0) ? x : -(x)
#define ZERO_CLAMP(x) (x >= 0) ? x : 0
#define N_BIT(x, n) ((x >> (n)) & 0x01)
#define IN_Y_RANGE(x) (x >= HEIGHT_OFFSET && x <= HEIGHT + HEIGHT_OFFSET)
#define IN_X_RANGE(x) (x >= WIDTH_OFFSET && x <= WIDTH + WIDTH_OFFSET)

#endif /* DEFINE_H */