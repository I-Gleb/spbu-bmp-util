#include <bmp.h>
#include <stego.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const int BITS_FOR_STEGO = 5;

static int8_t get_code(char c) {
        if ('A' <= c && c <= 'Z')
                return c - 'A';
        else if (c == ' ')
                return 26;
        else if (c == '.')
                return 27;
        else if (c == ',')
                return 28;
	return 29;
}

static char get_char(uint8_t n) {
	if (n < 26)
		return 'A' + n;
	else if (n == 26)
		return ' ';
	else if (n == 27)
		return '.';
	else if (n == 28)
		return ',';
	return '0';
}

static void get_bits(uint8_t n, bool * bits) {
	for (int i = 0; i < BITS_FOR_STEGO; ++i) {
		bits[i] = n % 2;
		n /= 2;
	}
}

static int8_t get_code_from_bits(bool * bits) {
	uint8_t ans = 0;
	for (int i = BITS_FOR_STEGO - 1; i >= 0; --i) {
		ans = ans * 2 + bits[i];
	}
	return ans;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		return 1;
	}
	if (strcmp(argv[1], "crop-rotate") == 0) {
		if (argc != 8) {
			return 1;
		}

		struct BMP * bmp = load_bmp_from_file(argv[2]);
		if (bmp == NULL) {
			return 1;
		}

		int x = atoi(argv[5]);
		int y = atoi(argv[4]);
		int h = atoi(argv[7]);
		int w = atoi(argv[6]);

		if (x < 0 || h <= 0 || y < 0 || w <= 0 || x + h > bmp->header.height || y + w > bmp->header.width) {
			free_bmp(bmp);
			return 1;
		}

		struct BMP * cropped = crop(bmp, x, y, h, w);
		free_bmp(bmp);
		if (cropped == NULL) {
			return 1;
		}

		struct BMP * res = rotate(cropped);
		free_bmp(cropped);
		if (res == NULL) {
			return 1;
		}

		FILE * out_bmp = fopen(argv[3], "wb");
                if (out_bmp == NULL) {
			free_bmp(res);
                        //fprintf(stderr, "Can't open the file for saving\n");
                        return 1;
                }

		save_bmp(out_bmp, res);
		fclose(out_bmp);
		free_bmp(res);

	}
	else if (strcmp(argv[1], "insert") == 0) {
		if (argc != 6) {
			return 1;
		}

                struct BMP * bmp = load_bmp_from_file(argv[2]);
		if (bmp == NULL) {
			return 1;
		}

		FILE * message_file = fopen(argv[5], "rt");
		if (message_file == NULL) {
			free_bmp(bmp);
			//fprintf(stderr, "Can't open message file\n");
			return 1;
		}

		FILE * key_file = fopen(argv[4], "rt");
		if (key_file == NULL) {
			free_bmp(bmp);
			fclose(message_file);
			//fprintf(stderr, "Can't open key file\n");
			return 1;
		}

		char current;
		bool buffer[BITS_FOR_STEGO];
		struct Stego_Info stego;
		while (fscanf(message_file, "%c", &current) == 1 && current != '\n') {
			get_bits(get_code(current), buffer);
			for (int i = 0; i < BITS_FOR_STEGO; ++i) {
				if (fscanf(key_file, "%d %d %c", &stego.y, &stego.x, &stego.component) != 3 || !verify_stego(&stego, bmp)) {
					free_bmp(bmp);
					fclose(message_file);
					fclose(key_file);
					//fprintf(stderr, "Error while reading keys\n");
					return 1;
				}
				insert_bit(bmp, &stego, buffer[i]);
			}		
		}
		fclose(message_file);
		fclose(key_file);
		
		FILE * out_bmp = fopen(argv[3], "wb");
                if (out_bmp == NULL) {
                        free_bmp(bmp);
                        //fprintf(stderr, "Can't open the file for saving\n");
                        return 1;
                }

                save_bmp(out_bmp, bmp);
                fclose(out_bmp);
                free_bmp(bmp);
	}
	else if (strcmp(argv[1], "extract") == 0) {
		if (argc != 5) {
			return 1;
		}

                struct BMP * bmp = load_bmp_from_file(argv[2]);
		if (bmp == NULL) {
			return 1;
		}

                FILE * key_file = fopen(argv[3], "rt");
                if (key_file == NULL) {
                        free_bmp(bmp);
                        //fprintf(stderr, "Can't open key file for reading\n");
                        return 1;
                }

		FILE * message_file = fopen(argv[4], "wt");
		if (message_file == NULL) {
			free_bmp(bmp);
			fclose(key_file);
			//fprintf(stderr, "Can't open message file for saving\n");
			return 1;
		}

		struct Stego_Info stego;
		bool buffer[BITS_FOR_STEGO];
		int bits_in_buffer = 0;
                while (fscanf(key_file, "%d %d %c", &stego.y, &stego.x, &stego.component) == 3) {
			if (!verify_stego(&stego, bmp)) {
				fclose(message_file);
				fclose(key_file);
				free_bmp(bmp);
				return 1;
			}
			buffer[bits_in_buffer] = extract_bit(bmp, &stego);
			++bits_in_buffer;
			if (bits_in_buffer == BITS_FOR_STEGO) {
				fprintf(message_file, "%c", get_char(get_code_from_bits(buffer)));
				bits_in_buffer = 0;
			}
                }
		fprintf(message_file, "\n");
		fclose(message_file);
		free_bmp(bmp);
		if (ferror(key_file)) {
			fclose(key_file);
			return 1;
		}
                fclose(key_file);
	}
	else {
		return 1;
	}
	return 0;
}
