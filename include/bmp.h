#pragma once

#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)

struct Pixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

struct BMP_Header {
	int16_t bm_string;
	int32_t file_size;
	int16_t reserved1;
	int16_t reserved2;
	int32_t pixel_offset;

	int32_t header_size;
	int32_t width;
	int32_t height;
	int16_t planes;
	int16_t bits_for_pixel;
	int32_t compression;
	int32_t image_size;
	int32_t X_pixel_per_meter;
	int32_t Y_pixel_per_meter;
	int32_t color_count;
	int32_t important_color_count;
};

#pragma pack(pop)

struct BMP {
	struct BMP_Header header;
	struct Pixel * * pixels;
};

struct BMP *  load_bmp(FILE * file);

struct BMP * crop(struct BMP * bmp, int x, int y, int h, int w);

struct BMP * rotate(struct BMP * bmp);

void save_bmp(FILE * file, struct BMP * bmp);

void free_bmp(struct BMP * bmp);

struct BMP * load_bmp_from_file(char * file_name);
