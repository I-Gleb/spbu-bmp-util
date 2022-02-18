#pragma once

#include <stdio.h>
#include <bmp.h>
#include <stdbool.h>

struct Stego_Info {
	int x, y;
	char component;
};

void insert_bit(struct BMP * bmp, struct Stego_Info * stego, bool message);

bool extract_bit(struct BMP * bmp, struct Stego_Info * stego);

bool verify_stego(struct Stego_Info * stego, struct BMP * bmp);
