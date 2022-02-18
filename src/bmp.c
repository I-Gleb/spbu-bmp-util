#include <bmp.h>
#include <stdio.h>
#include <stdlib.h>

static const int ALIGNMENT = 4;
static const unsigned char PADDING_ELEM = '\0';

static struct Pixel * * allocate(int h, int w) {
	struct Pixel * * res = malloc(h * sizeof(struct Pixel *));
	struct Pixel * all_memory = malloc(w * h * sizeof(struct Pixel));
	if (res == NULL || all_memory == NULL) {
		free(all_memory);
		free(res);
		return NULL;
	}
	for (int i = 0; i < h; ++i) {
		res[i] = all_memory + i * w;
	}
	return res;
}

struct BMP * load_bmp(FILE * file) {
	struct BMP * res = malloc(sizeof(struct BMP));
	if (res == NULL) {
		//fprintf(stderr, "Memory allocation failed\n");
                return NULL;
	}

	if (fread(&res->header, sizeof(struct BMP_Header), 1, file) != 1) {
		free(res);
		//fprintf(stderr, "Wrong format of header\n");
                return NULL;
	}

	res->pixels = allocate(res->header.height, res->header.width);
	if (res->pixels == NULL) {
		free(res);
		//fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}

	int padding = (ALIGNMENT - res->header.width * sizeof(struct Pixel) % ALIGNMENT) % ALIGNMENT;
	for (int i = res->header.height - 1; i >= 0; --i) {
		if (fread(res->pixels[i], sizeof(struct Pixel), res->header.width, file) != res->header.width || fseek(file, padding, 1) != 0) {
			free(res->pixels[0]);
			free(res->pixels);
			free(res);
			//fprintf(stderr, "Wrong format of file\n");
			return NULL;
		}

	}

	return res;
}

struct BMP * crop(struct BMP * bmp, int x, int y, int h, int w) {
	struct BMP * res = malloc(sizeof(struct BMP));
        if (res == NULL) {
                //fprintf(stderr, "Memory allocation failed\n");
                return NULL;
        }

	res->header = bmp->header;
	res->header.height = h;
	res->header.width = w;
	res->header.image_size = h * ((w * (int)sizeof(struct Pixel) + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT);
	res->header.file_size += res->header.image_size - bmp->header.image_size;
	res->pixels = allocate(res->header.height, res->header.width);
	if (res->pixels == NULL) {
		free(res);
                //fprintf(stderr, "Memory allocation failed\n");
                return NULL;
	}

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			res->pixels[i][j] = bmp->pixels[x + i][y + j];
		}
	}
	
	return res;
}

struct BMP * rotate(struct BMP * bmp) {
	struct BMP * res = malloc(sizeof(struct BMP));
        if (res == NULL) {
                //fprintf(stderr, "Memory allocation failed\n");
                return NULL;
        }

        res->header = bmp->header;
        res->header.height = bmp->header.width;
        res->header.width = bmp->header.height;
	res->header.X_pixel_per_meter = bmp->header.Y_pixel_per_meter;
	res->header.Y_pixel_per_meter = bmp->header.X_pixel_per_meter;
	res->header.image_size = res->header.height * ((res->header.width * (int)sizeof(struct Pixel) + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT);
        res->header.file_size += res->header.image_size - bmp->header.image_size;
        res->pixels = allocate(res->header.height, res->header.width);
	if (res->pixels == NULL) {
                free(res);
                //fprintf(stderr, "Memory allocation failed\n");
                return NULL;
        }

	for (int i = 0; i < bmp->header.height; ++i) {
		for (int j = 0; j < bmp->header.width; ++j) {
			res->pixels[j][bmp->header.height - 1 - i] = bmp->pixels[i][j];
		}
	}

	return res;
}

void save_bmp(FILE * file, struct BMP * bmp) {
	fwrite(&bmp->header, sizeof(struct BMP_Header), 1, file);
	
	int padding = (ALIGNMENT - bmp->header.width * sizeof(struct Pixel) % ALIGNMENT) % ALIGNMENT;
	for (int i = bmp->header.height - 1; i >= 0; --i) {
		fwrite(bmp->pixels[i], sizeof(struct Pixel), bmp->header.width, file);
		for (int pad = 0; pad < padding; ++pad) {
			fwrite(&PADDING_ELEM, sizeof(unsigned char), 1, file);
		}
	}
}

void free_bmp(struct BMP * bmp) {
	if (bmp != NULL) {
		if (bmp->pixels != NULL) {
			if (bmp->pixels[0] != NULL) {
				free(bmp->pixels[0]);
			}
			free(bmp->pixels);
		}
		free(bmp);
	}
}

struct BMP * load_bmp_from_file(char * file_name) {
        FILE * bmp_file = fopen(file_name, "rb");
        if (bmp_file == NULL) {
                //fprintf(stderr, "Can't open the file for reading\n");
                return NULL;
        }

        struct BMP * bmp = load_bmp(bmp_file);
        fclose(bmp_file);
        return bmp;
}
