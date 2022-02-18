#include <stego.h>
#include <bmp.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

static uint8_t * get_component(struct Pixel * p, char component) {
	switch (component) {
		case 'R': return &p->r;
		case 'G': return &p->g;
		case 'B': return &p->b;
		default: assert(false);
	}
}

static void set_last_bit(uint8_t * n, bool bit) {
	*n = ((*n >> 1) << 1) + bit;
}

void insert_bit(struct BMP * bmp, struct Stego_Info * stego, bool message) {
	set_last_bit(get_component(&bmp->pixels[stego->x][stego->y], stego->component), message);
}

bool extract_bit(struct BMP * bmp, struct Stego_Info * stego) {
	return *get_component(&bmp->pixels[stego->x][stego->y], stego->component) % 2;
}

bool verify_stego(struct Stego_Info * stego, struct BMP * bmp) {
        return (stego->x >= 0 && stego->x < bmp->header.height && stego->y >= 0 && stego->y < bmp->header.width && (stego->component == 'R' || stego->component == 'G' || stego->component == 'B'));
}

