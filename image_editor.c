#include "image_editor.h"

// Loads an image file (PPM/PGM formats) into memory
void LOAD(rgb_t ***rgb, char s[100], char f[5], int *n, int *m, int *I,
		int ***mat, int *fl, int *x1, int *x2, int *y1, int *y2)
{
	// Free any previously loaded image
	free_mat(mat, n);
	free_rgb(rgb, n);
	// Read input filename
	scanf("%29s", s);

	// Read input filename
	FILE *input = fopen(s, "rb");
	if (!input) {
		printf("Failed to load %s\n", s);
		*fl = 0;
		return;
	}

	// Reset selection
	*x1 = 0; *x2 = 0;
	*y1 = 0; *y2 = 0;

	// Read magic word and image metadata
	fscanf(input, "%s", f);
	fscanf(input, "%d %d %d ", m, n, I);

	// ASCII grayscale
	if (strcmp(f, "P2") == 0) {
		*mat = malloc((*n) * sizeof(int*));
		for (int i = 0; i < *n; i++) {
			(*mat)[i] = malloc((*m) * sizeof(int));
		}
		for (int i = 0; i < *n; i++) {
			for (int j = 0; j < *m; j++) {
				fscanf(input, "%d", &(*mat)[i][j]);
			}
		}

	// ASCII rgb
	} else if (strcmp(f, "P3") == 0) {
		*rgb = malloc((*n) * sizeof(rgb_t *));
		for (int i = 0; i < *n; i++) {
			(*rgb)[i] = malloc((*m) * sizeof(rgb_t));
		}
		for (int i = 0; i < *n; i++) {
			for (int j = 0; j < *m; j++) {
				fscanf(input, "%d%d%d", &(*rgb)[i][j].r, &(*rgb)[i][j].g,
					&(*rgb)[i][j].b);
			}
		}

	// Binary grayscale
	} else if (strcmp(f, "P5") == 0) {
		*mat = malloc((*n) * sizeof(int*));
		for (int i = 0; i < *n; i++) {
			(*mat)[i] = malloc((*m) * sizeof(int));
		}
		unsigned char pixel;
		for (int i = 0; i < *n; i++) {
			for (int j = 0; j < *m; j++) {
				fread(&pixel, sizeof(unsigned char), 1, input);
				(*mat)[i][j] = pixel;
			}
		}

	// Binary rgb
	} else if (strcmp(f, "P6") == 0) {
		*rgb = malloc((*n) * sizeof(rgb_t *));
		for (int i = 0; i < *n; i++) {
			(*rgb)[i] = malloc((*m) * sizeof(rgb_t));
		}
		unsigned char r, g, b;
		for (int i = 0; i < *n; i++) {
			for (int j = 0; j < *m; j++) {
				fread(&r, sizeof(unsigned char), 1, input);
				fread(&g, sizeof(unsigned char), 1, input);
				fread(&b, sizeof(unsigned char), 1, input);
				(*rgb)[i][j].r = r;
				(*rgb)[i][j].g = g;
				(*rgb)[i][j].b = b;
			}
		}
	}
	printf("Loaded %s\n", s);
	*fl = 1;
	fclose(input);
}

// Frees a grayscale image matrix
void free_mat(int ***mat, int *n)
{
	if (*mat) {
		for (int i = 0; i < *n; i++) {
			if ((*mat)[i]) {
				free((*mat)[i]);
			}
		}
		free(*mat);
	}
	*mat = NULL;
}

// Frees an RGB image matrix
void free_rgb(rgb_t ***rgb, int *n)
{
	if (*rgb) {
		for (int i = 0; i < *n; i++) {
			if ((*rgb)[i]) {
				free((*rgb)[i]);
			}
		}
		free(*rgb);
	}
	*rgb = NULL;
}

// Saves current image to file (ASCII or binary)
void SAVE(rgb_t ***rgb, int ***mat, int *n, int *m, char s[100], char f[5],
		int *I)
{
	// Parse output filename and optional "ascii" flag
	char output[30], ascii[70];
	size_t i1 = 0, j1 = 0;
	while (i1 < strlen(s) && s[i1] != ' ') {
		output[i1] = s[i1];
		i1++;
	}
	output[i1] = '\0';
	i1++;
	while (i1 < strlen(s)) {
		ascii[j1] = s[i1];
		i1++;
		j1++;
	}
	ascii[j1] = '\0';

	// Save in ASCII mode
	if (strcmp(ascii, "ascii") == 0) {
		FILE *out = fopen(output, "w");
		if(strcmp(f, "P2") == 0) {
			fprintf(out, "%s\n%d %d\n%d\n", f, *m, *n, *I);
			for (int i = 0; i < *n; i++) {
				for (int j = 0; j < *m; j++) {
					fprintf(out, "%d ", (*mat)[i][j]);
				}
				fprintf(out, "\n");
			}
		} else if (strcmp(f, "P3") == 0) {
			fprintf(out, "%s\n%d %d\n%d\n", f, *m, *n, *I);
			for (int i = 0; i < *n; i++) {
				for (int j = 0; j < *m; j++) {
					fprintf(out, "%d %d %d ", (*rgb)[i][j].r, (*rgb)[i][j].g,
							(*rgb)[i][j].b);
				}
				fprintf(out, "\n");
			}

		// Conversions P5->P2 and P6->P3 handled below
		} else if (strcmp(f, "P5") == 0) {
			fprintf(out, "P2\n%d %d\n%d\n", *m, *n, *I);
			strcpy(f, "P2");
			for (int i = 0; i < *n; i++) {
				for (int j = 0; j < *m; j++) {
					fprintf(out, "%d ", (*mat)[i][j]);
				}
				fprintf(out, "\n");
			}
		} else if (strcmp(f, "P6") == 0) {
			fprintf(out, "P3\n%d %d\n%d\n", *m, *n, *I);
			strcpy(f, "P3");
			for (int i = 0; i < *n; i++) {
				for (int j = 0; j < *m; j++) {
					fprintf(out, "%d %d %d ", (*rgb)[i][j].r, (*rgb)[i][j].g,
						(*rgb)[i][j].b);
				}
				fprintf(out, "\n");
			}
		}
		fclose(out);

	// Save in binary mode
	} else {
		FILE *out = fopen(output, "wb");
		if (strcmp(f, "P2") == 0) {
			fprintf(out, "P5\n%d %d\n%d\n", *m, *n, *I);
			strcpy(f, "P5");
			for (int i = 0; i < *n; i++) {
				for (int j = 0; j < *m; j++) {
					fwrite(&(*mat)[i][j], sizeof(unsigned char), 1, out);
				}
			}
		} else if (strcmp(f, "P3") == 0) {
			fprintf(out, "P6\n%d %d\n%d\n", *m, *n, *I);
			strcpy(f, "P6");
			for (int i = 0; i < *n; i++) {
				for (int j = 0; j < *m; j++) {
					fwrite(&(*rgb)[i][j].r, sizeof(unsigned char), 1, out);
					fwrite(&(*rgb)[i][j].g, sizeof(unsigned char), 1, out);
					fwrite(&(*rgb)[i][j].b, sizeof(unsigned char), 1, out);
				}
			}
		} else if (strcmp(f, "P5") == 0) {
			fprintf(out, "P5\n%d %d\n%d\n", *m, *n, *I);
			for (int i = 0; i < *n; i++) {
				for (int j = 0; j < *m; j++) {
					fwrite(&(*mat)[i][j], sizeof(unsigned char), 1, out);
				}
			}
		} else if (strcmp(f, "P6") == 0) {
			fprintf(out, "P6\n%d %d\n%d\n", *m, *n, *I);
			for (int i = 0; i < *n; i++) {
				for (int j = 0; j < *m; j++) {
					fwrite(&(*rgb)[i][j].r, sizeof(unsigned char), 1, out);
					fwrite(&(*rgb)[i][j].g, sizeof(unsigned char), 1, out);
					fwrite(&(*rgb)[i][j].b, sizeof(unsigned char), 1, out);
				}
			}
		}
		fclose(out);
	}
	printf("Saved %s\n", output);
}

// Parses coordinates and updates current selection
void SELECT(char s[100], int *x1, int *x2, int *y1, int *y2, int *n, int *m)
{
	if (strcmp(s, "ALL") == 0 || strcmp(s, "ALL ") == 0) {
		printf("Selected ALL\n");
		*x1 = 0;
		*y1 = 0;
		*x2 = *n;
		*y2 = *m;
		return;
	}

	// Parse 4 coordinates
	int values[4] = {0};
	int count = 0;
	char *token = strtok(s, " ");
	while (token && count < 4) {
		char *endptr;
		int value = strtol(token, &endptr, 10);
		if (*endptr != '\0') {
			printf("Invalid command\n");
			return;
		}
		values[count++] = value;
		token = strtok(NULL, " ");
	}
	if (count != 4) {
		printf("Invalid command\n");
		return;
	}
	if (values[0] < 0 || values[2] < 0 || values[1] < 0 || values[3] < 0 ||
		values[1] > *n || values[3] > *n || values[0] > *m || values[2] > *m) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (values[1] == values[3] || values[0] == values[2]) {
		printf("Invalid set of coordinates\n");
		return;
	}
	int ok = 0;
	for (int i = 1; i < 4 && ok == 0; i++) {
		if (values[i] != values[i - 1])
			ok = 1;
	}
	if (ok == 0 ) {
		printf("Invalid set of coordinates\n");
		return;
	}
	*y1 = values[0];
	*x1 = values[1];
	*y2 = values[2];
	*x2 = values[3];
	if (*x1 > *x2) {
		int aux = *x1;
		*x1 = *x2;
		*x2 = aux;
	}
	if (*y1 > *y2) {
		int aux = *y1;
		*y1 = *y2;
		*y2 = aux;
	}
	printf("Selected %d %d %d %d\n", *y1, *x1, *y2, *x2);
}

// Crops image using the active selection
void CROP(rgb_t ***rgb, int ***mat, int *n, int *m, int *x1, int *x2,
		int *y1, int *y2, char f[5])
{
	if (*x1 < 0 || *x1 > *n || *x2 < 0 || *x2 > *n) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (*y1 < 0 || *y1 > *m || *y2 < 0 || *y2 > *m) {
		printf("Invalid set of coordinates\n");
		return;
	}
		if (*x1 > *x2) {
			int aux = *x1;
			*x1 = *x2;
			*x2 = aux;
		}
		if (*y1 > *y2) {
			int aux = *y1;
			*y1 = *y2;
			*y2 = aux;
		}
	int auxn = *x2 - *x1;
	int auxm = *y2 - *y1;

	// Grayscale crop
	if(strcmp(f, "P2") == 0 || strcmp(f, "P5") == 0) {
		int **aux_mat = malloc(auxn * sizeof(int *));
		for (int i = 0; i < auxn; i++) {
			aux_mat[i] = malloc(auxm * sizeof(int));
		}
		for (int i = 0; i < auxn; i++) {
			for (int j = 0; j < auxm; j++) {
				aux_mat[i][j] = (*mat)[*x1 + i][*y1 + j];
			}
		}
		for (int i = 0; i < *n; i++) {
			free((*mat)[i]);
		}
		free(*mat);
		*mat = aux_mat;

	// RGB crop
	} else if (strcmp(f, "P3") == 0 || strcmp(f, "P6") == 0) {
		rgb_t **aux_rgb = malloc(auxn * sizeof(rgb_t *));
		for (int i = 0; i < auxn; i++) {
			aux_rgb[i] = malloc(auxm * sizeof(rgb_t));
		}
		for (int i = 0; i < auxn; i++) {
			for (int j = 0; j < auxm; j++) {
				aux_rgb[i][j] = (*rgb)[*x1 + i][*y1 + j];
			}
		}
		for (int i = 0; i < *n; i++) {
			free((*rgb)[i]);
		}
		free(*rgb);
		*rgb = aux_rgb;
	}
	*n = auxn;
	*m = auxm;
	printf("Image cropped\n");
}

// Applies a convolution filter on the selected area of the image
// Supported filters: SHARPEN, EDGE, BLUR, GAUSSIAN_BLUR
void APPLY(rgb_t ***rgb, int *n, int *m, char f[5], char s[100],
		 int *x1, int *x2, int *y1, int *y2)
{
	// Save original selection in case we need to restore it after errors
	int auxx1 = *x1, auxx2 = *x2, auxy1 = *y1, auxy2 = *y2;

	// If no explicit selection exists, default to full interior (avoid borders)
	if (*x1 == 0 && *y1 == 0 && *x2 == 0 && *y2 == 0) {
		*x1 = 1; *x2 = *n - 1;
		*y1 = 1; *y2 = *m - 1;
	}

	// Clamp selection to valid interior limits
	if (*x1 == 0) *x1 = 1;
	if (*y1 == 0) *y1 = 1;
	if (*x2 == 0) *x2 = 1;
	if (*y2 == 0) *y2 = 1;
	if (*x1 == *n) *x1 = *n - 1;
	if (*y1 == *m) *y1 = *m - 1;
	if (*x2 == *n) *x2 = *n - 1;
	if (*y2 == *m) *y2 = *m - 1;

	// Ensure proper coordinate ordering
	if (*x1 > *x2) {
		int aux = *x1;
		*x1 = *x2;
		*x2 = aux;
	}
	if (*y1 > *y2) {
		int aux = *y1;
		*y1 = *y2;
		*y2 = aux;
	}

	// SHARPEN filter
	if (strcmp(s, "SHARPEN") == 0) {
		if (strcmp(f, "P3") == 0 || strcmp(f, "P6") == 0) {
			// Create temporary buffer for convolution result
			rgb_t **temp = malloc((*n) * sizeof(rgb_t *));
			for (int i = 0; i < *n; i++) {
				temp[i] = malloc((*m) * sizeof(rgb_t));
			}

			// Apply sharpening kernel
			for (int i = *x1; i < *x2; i++) {
				for (int j = *y1; j < *y2; j++) {
					int sumr = 0, sumg = 0, sumb = 0;
					sumr = 0 * (*rgb)[i - 1][j - 1].r +
					(-1) * (*rgb)[i - 1][j].r +
					0 * (*rgb)[i - 1][j + 1].r + (-1) * (*rgb)[i][j - 1].r +
					5 * (*rgb)[i][j].r + (-1) * (*rgb)[i][j + 1].r +
					0 * (*rgb)[i + 1][j - 1].r + (-1) * (*rgb)[i + 1][j].r +
					0 * (*rgb)[i + 1][j + 1].r;
					sumg = 0 * (*rgb)[i - 1][j - 1].g +
						(-1) * (*rgb)[i - 1][j].g +
						0 * (*rgb)[i - 1][j + 1].g +
						(-1) * (*rgb)[i][j - 1].g +
						5 * (*rgb)[i][j].g + (-1) * (*rgb)[i][j + 1].g +
						0 * (*rgb)[i + 1][j - 1].g +
						(-1) * (*rgb)[i + 1][j].g +
						0 * (*rgb)[i + 1][j + 1].g;
					sumb = 0 * (*rgb)[i - 1][j - 1].b +
						(-1) * (*rgb)[i - 1][j].b +
						0 * (*rgb)[i - 1][j + 1].b +
						(-1) * (*rgb)[i][j - 1].b +
						5 * (*rgb)[i][j].b + (-1) * (*rgb)[i][j + 1].b +
						0 * (*rgb)[i + 1][j - 1].b +
						(-1) * (*rgb)[i + 1][j].b +
						0 * (*rgb)[i + 1][j + 1].b;

					// Clamp color values to valid [0..255] range
					temp[i][j].r = sumr < 0 ? 0 : sumr > 255 ? 255 : sumr;
					temp[i][j].g = sumg < 0 ? 0 : sumg > 255 ? 255 : sumg;
					temp[i][j].b = sumb < 0 ? 0 : sumb > 255 ? 255 : sumb;
				}
			}

			// Copy filtered region back into original image
			for (int i = *x1; i < *x2; i++) {
				for (int j = *y1; j < *y2; j++) {
					(*rgb)[i][j] = temp[i][j];
				}
			}

			// Release temporary buffer
			for (int i = 0; i < *n; i++) {
				free(temp[i]);
			}
			free(temp);
		} else {
			// Filter not supported for grayscale images
			printf("Easy, Charlie Chaplin\n");
			*x1 = auxx1; *x2 = auxx2;
			*y1 = auxy1; *y2 = auxy2;
			return;
		}

	// EDGE filter
	} else if (strcmp(s, "EDGE") == 0) {
		if (strcmp(f, "P3") == 0 || strcmp(f, "P6") == 0) {
			rgb_t **temp = malloc((*n) * sizeof(rgb_t *));
			for (int i = 0; i < *n; i++) {
				temp[i] = malloc((*m) * sizeof(rgb_t));
			}
			for (int i = *x1; i < *x2; i++) {
				for (int j = *y1; j < *y2; j++) {
					int sumr = 0, sumg = 0, sumb = 0;
					sumr = (-1) * (*rgb)[i - 1][j - 1].r +
						(-1) * (*rgb)[i - 1][j].r +
						(-1) * (*rgb)[i - 1][j + 1].r +
						(-1) * (*rgb)[i][j - 1].r +
						8 * (*rgb)[i][j].r + (-1) * (*rgb)[i][j + 1].r +
						(-1) * (*rgb)[i + 1][j - 1].r +
						(-1) * (*rgb)[i + 1][j].r
						+ (-1) * (*rgb)[i + 1][j + 1].r;
					sumg = (-1) * (*rgb)[i - 1][j - 1].g +
						(-1) * (*rgb)[i - 1][j].g +
						(-1) * (*rgb)[i - 1][j + 1].g +
						(-1) * (*rgb)[i][j - 1].g + 8 * (*rgb)[i][j].g
						+ (-1) * (*rgb)[i][j + 1].g +
						(-1) * (*rgb)[i + 1][j - 1].g +
						(-1) * (*rgb)[i + 1][j].g +
						(-1) * (*rgb)[i + 1][j + 1].g;
					sumb = (-1) * (*rgb)[i - 1][j - 1].b +
						(-1) * (*rgb)[i - 1][j].b +
						(-1) * (*rgb)[i - 1][j + 1].b +
						(-1) * (*rgb)[i][j - 1].b +
						8 * (*rgb)[i][j].b + (-1) * (*rgb)[i][j + 1].b +
						(-1) * (*rgb)[i + 1][j - 1].b +
						(-1) * (*rgb)[i + 1][j].b +
						(-1) * (*rgb)[i + 1][j + 1].b;
					temp[i][j].r = sumr < 0 ? 0 : sumr > 255 ? 255 : sumr;
					temp[i][j].g = sumg < 0 ? 0 : sumg > 255 ? 255 : sumg;
					temp[i][j].b = sumb < 0 ? 0 : sumb > 255 ? 255 : sumb;
				}
			}
			for (int i = *x1; i < *x2; i++) {
				for (int j = *y1; j < *y2; j++) {
					(*rgb)[i][j] = temp[i][j];
				}
			}
			for (int i = 0; i < *n; i++) {
				free(temp[i]);
			}
			free(temp);
		} else {
			printf("Easy, Charlie Chaplin\n");
			*x1 = auxx1; *x2 = auxx2;
			*y1 = auxy1; *y2 = auxy2;
			return;
		}

	// BLUR filter
	} else if (strcmp(s, "BLUR") == 0) {
		if (strcmp(f, "P3") == 0 || strcmp(f, "P6") == 0) {
			rgb_t **temp = malloc((*n) * sizeof(rgb_t *));
			for (int i = 0; i < *n; i++) {
				temp[i] = malloc((*m) * sizeof(rgb_t));
			}
			for (int i = *x1; i < *x2; i++) {
				for (int j = *y1; j < *y2; j++) {
					double sumar = 0, sumag = 0, sumab = 0;
					sumar = ((*rgb)[i - 1][j - 1].r +
					(*rgb)[i - 1][j].r + (*rgb)[i - 1][j + 1].r +
					(*rgb)[i][j - 1].r + (*rgb)[i][j].r + (*rgb)[i][j + 1].r +
					(*rgb)[i + 1][j - 1].r + (*rgb)[i + 1][j].r +
					(*rgb)[i + 1][j + 1].r) / 9.0;
					sumag = ((*rgb)[i - 1][j - 1].g +
					(*rgb)[i - 1][j].g + (*rgb)[i - 1][j + 1].g +
					(*rgb)[i][j - 1].g + (*rgb)[i][j].g + (*rgb)[i][j + 1].g +
					(*rgb)[i + 1][j - 1].g + (*rgb)[i + 1][j].g +
					(*rgb)[i + 1][j + 1].g) / 9.0;
					sumab = ((*rgb)[i - 1][j - 1].b +
					(*rgb)[i - 1][j].b + (*rgb)[i - 1][j + 1].b +
					(*rgb)[i][j - 1].b + (*rgb)[i][j].b + (*rgb)[i][j + 1].b +
					(*rgb)[i + 1][j - 1].b + (*rgb)[i + 1][j].b +
					(*rgb)[i + 1][j + 1].b) / 9.0;
					temp[i][j].r = sumar < 0 ? 0 : sumar > 255 ? 255 : round(sumar);
					temp[i][j].g = sumag < 0 ? 0 : sumag > 255 ? 255 : round(sumag);
					temp[i][j].b = sumab < 0 ? 0 : sumab > 255 ? 255 : round(sumab);
				}
			}
			for (int i = *x1; i < *x2; i++) {
				for (int j = *y1; j < *y2; j++) {
					(*rgb)[i][j] = temp[i][j];
				}
			}
			for (int i = 0; i < *n; i++) {
				free(temp[i]);
			}
			free(temp);
		} else {
			printf("Easy, Charlie Chaplin\n");
			*x1 = auxx1; *x2 = auxx2;
			*y1 = auxy1; *y2 = auxy2;
			return;
		}

	// GAUSSIAN BLUR filter
	} else if (strcmp(s, "GAUSSIAN_BLUR") == 0) {
		if (strcmp(f, "P3") == 0 || strcmp(f, "P6") == 0) {
			rgb_t **temp = malloc((*n) * sizeof(rgb_t *));
			for (int i = 0; i < *n; i++) {
				temp[i] = malloc((*m) * sizeof(rgb_t));
			}
			for (int i = *x1; i < *x2; i++) {
				for (int j = *y1; j < *y2; j++) {
					double sumbr = 0, sumbg = 0, sumbb = 0;
					sumbr = ((*rgb)[i - 1][j - 1].r +
					2 * (*rgb)[i - 1][j].r + (*rgb)[i - 1][j + 1].r +
					2 * (*rgb)[i][j - 1].r + 4 * (*rgb)[i][j].r +
					2 * (*rgb)[i][j + 1].r +
					(*rgb)[i + 1][j - 1].r + 2 * (*rgb)[i + 1][j].r +
					(*rgb)[i + 1][j + 1].r) / 16.0;
					sumbg = ((*rgb)[i - 1][j - 1].g +
					2 * (*rgb)[i - 1][j].g + (*rgb)[i - 1][j + 1].g +
					2 * (*rgb)[i][j - 1].g + 4 * (*rgb)[i][j].g +
					2 * (*rgb)[i][j + 1].g +
					(*rgb)[i + 1][j - 1].g + 2 * (*rgb)[i + 1][j].g +
					(*rgb)[i + 1][j + 1].g) / 16.0;
					sumbb = ((*rgb)[i - 1][j - 1].b +
					2 * (*rgb)[i - 1][j].b + (*rgb)[i - 1][j + 1].b +
					2 * (*rgb)[i][j - 1].b + 4 * (*rgb)[i][j].b +
					2 * (*rgb)[i][j + 1].b +
					(*rgb)[i + 1][j - 1].b + 2 * (*rgb)[i + 1][j].b +
					(*rgb)[i + 1][j + 1].b) / 16.0;
					temp[i][j].r = sumbr < 0 ? 0 : sumbr > 255 ? 255 : round(sumbr);
					temp[i][j].g = sumbg < 0 ? 0 : sumbg > 255 ? 255 : round(sumbg);
					temp[i][j].b = sumbb < 0 ? 0 : sumbb > 255 ? 255 : round(sumbb);
				}
			}

			for (int i = *x1; i < *x2; i++) {
				for (int j = *y1; j < *y2; j++) {
					(*rgb)[i][j] = temp[i][j];
				}
			}

			for (int i = 0; i < *n; i++) {
				free(temp[i]);
			}
			free(temp);
		} else {
			printf("Easy, Charlie Chaplin\n");
			*x1 = auxx1; *x2 = auxx2;
			*y1 = auxy1; *y2 = auxy2;
			return;
		}
	} else if (strcmp(s, "\n") != 0) {
		printf("APPLY parameter invalid\n");
		*x1 = auxx1; *x2 = auxx2;
		*y1 = auxy1; *y2 = auxy2;
		return;
	} else {
		printf("Invalid command\n");
		*x1 = auxx1; *x2 = auxx2;
		*y1 = auxy1; *y2 = auxy2;
		return;
	}
	printf("APPLY %s done\n", s);
	*x1 = auxx1; *x2 = auxx2;
	*y1 = auxy1; *y2 = auxy2;
}

// Applies histogram equalization on grayscale images (P2 / P5)
void EQUALIZE(int ***mat, int *n, int *m, char f[5])
{
	int H[256] = {0}; // histogram bins

	// Only grayscale images are supported
	if (strcmp(f, "P2") == 0 || strcmp(f, "P5") == 0) {
		// Build histogram
		for (int i = 0; i < *n; i++) {
			for (int j = 0; j < *m; j++) {
				H[(*mat)[i][j]]++;
			}
		}

		double frac = 0, A = (*n) * (*m);

		// Apply equalization formula per pixel
		for (int i = 0; i < *n; i++) {
			for (int j = 0; j < *m; j++) {
				double sum = 0;
				frac = 0;

				// CDF calculation
				for (int k = 0; k <= (*mat)[i][j]; k++) {
					sum += H[k];
				}

				// Mapping to new intensity range
				frac = 255 * (1.0 / A) * sum;
				frac = round(frac);

				// Clamp to valid pixel limits
				(*mat)[i][j] = frac < 0 ? 0 : frac > 255 ? 255 : frac;
			}
		}
		printf("Equalize done\n");
	} else {
		printf("Black and white image needed\n");
	}
}

// Displays a textual histogram for grayscale images
void HISTOGRAM(int ***mat, int *n, int *m, char s[100], char f[5])
{
	// Only grayscale images are supported
	if (strcmp(f, "P2") == 0 || strcmp(f, "P5") == 0) {
		int H[256] = {0}, sum[256] = {0}, x, y, max1 = 0;
		int values[5] = {0};
		int count = 0;

		// Parse parameters: x and y
		char *token = strtok(s, " ");
		while (token) {
			if (count >= 2) {
				printf("Invalid command\n");
				return;
			}
			char *endptr;
			int value = strtol(token, &endptr, 10);
			if (*endptr != '\0') {
				printf("Invalid command\n");
				return;
			}
			values[count++] = value;
			token = strtok(NULL, " ");
		}
		if (count != 2) {
			printf("Invalid command\n");
			return;
		}

		x = values[0];
		y = values[1];

		// Build histogram
		for (int i = 0; i < *n; i++) {
			for (int j = 0; j < *m; j++) {
				H[(*mat)[i][j]]++;
			}
		}

		int nrbin = 256 / y;

		// Group histogram values per bin
   		for (int i = 0; i < y; i++) {
			sum[i] = 0;
			for (int j = i * nrbin; j < (i + 1) * nrbin; j++) {
				sum[i] += H[j];
			}
			if (sum[i] > max1) max1 = sum[i];
		}

		// Print scaled star bars
		for (int i = 0; i < y; i++) {
			int nrstar = sum[i] * x / max1;
			printf("%d\t|\t", nrstar);
			for (int j = 0; j < nrstar; j++) {
				printf("*");
			}
			printf("\n");
		}
	} else {
		printf("Black and white image needed\n");
	}
}

// Rotates a square selection of the image by 90, 180 or 270 degrees
void ROTATE(rgb_t ***rgb, int ***mat, int *n, int *m, char f[5], int *angle,
		int *x1, int *x2, int *y1, int *y2)
{
	// Handle trivial cases
	if (*angle == 0) {
		printf("Rotated 0\n");
		return;
	}
	if (*angle == -360) {
		printf("Rotated -360\n");
		return;
	}
	if (*angle == 360) {
		printf("Rotated 360\n");
		return;
	}

	// Normalize coordinates
	if (*x1 > *x2) {
		int aux = *x1;
		*x1 = *x2;
		*x2 = aux;
	}
	if (*y1 > *y2) {
		int aux = *y1;
		*y1 = *y2;
		*y2 = aux;
	}

	int auxangle = *angle;

	// Rotation only works on square selections
	if (*x2 - *x1 != *y2 - *y1) {
		printf("The selection must be square\n");
		return;
	}

	// Normalize negative angles
	if (*angle < 0) {
		*angle = 360 + (*angle % 360);
		if (*angle == 360) *angle = 0;
	}

	// Only multiples of 90 degrees are supported
	if (*angle % 90 != 0 || *angle < 90) {
		printf("Unsupported rotation angle\n");
		return;
	}

	// Temporary buffer for grayscale rotations
	int **temp = malloc((*n) * sizeof(int *));
	for (int i = 0; i < *n; i++) {
		temp[i] = malloc((*m) * sizeof(int));
	}

	int size = *x2 - *x1;

	// Compute rotated coordinates
	for (int i = *x1; i < *x2; i++) {
		for (int j = *y1; j < *y2; j++) {
			int newX, newY;
			if (*angle == 90) {
				newX = *x1 + size - 1 - (j - *y1);
				newY = *y1 + (i - *x1);
			} else if (*angle == 180) {
				newX = *x1 + size - 1 - (i - *x1);
				newY = *y1 + size - 1 - (j - *y1);
			} else if (*angle == 270) {
				newX = *x1 + (j - *y1);
				newY = *y1 + size - 1 - (i - *x1);
			}
			if (strcmp(f, "P2") == 0 || strcmp(f, "P5") == 0) {
				temp[newX][newY] = (*mat)[i][j];
			} else if (strcmp(f, "P3") == 0 || strcmp(f, "P6") == 0) {
				(*rgb)[newX][newY].r = (*rgb)[i][j].r;
				(*rgb)[newX][newY].g = (*rgb)[i][j].g;
				(*rgb)[newX][newY].b = (*rgb)[i][j].b;
			}
		}
	}

	// Copy rotated selection back
	for (int i = *x1; i < *x2; i++) {
		for (int j = *y1; j < *y2; j++) {
			if (strcmp(f, "P2") == 0 || strcmp(f, "P5") == 0) {
				(*mat)[i][j] = temp[i][j];
			} else if (strcmp(f, "P3") == 0 || strcmp(f, "P6") == 0) {
				(*rgb)[i][j].r = (*rgb)[i][j].r;
			   	(*rgb)[i][j].g = (*rgb)[i][j].g;
				(*rgb)[i][j].b = (*rgb)[i][j].b;
			 }
		}
	}

	// Free temporary memory
	for (int i = 0; i < *n; i++) {
		free(temp[i]);
	}
	free(temp);
	printf("Rotated %d\n", auxangle);
}