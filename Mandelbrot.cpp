#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Bmp_header.h"
#include "specrend.c"

#define dim_max_nume 25

#define wid 1920

#define hei 1080

#define max_iteration 1000

#define val_max (1024*1024)

#define max_color 1000

struct bitmap* getColorPalette() {
    struct bitmap *colorPalette = (struct bitmap*)malloc(max_color * sizeof(struct bitmap));
    if(!colorPalette)
        return NULL;
    int i = 0;
    double t, x, y, z, r, g, b;
    struct colourSystem *cs = &SMPTEsystem;
    for (t = 1000; t < 11000; t+= 10) {
        bbTemp = t;
        spectrum_to_xyz(bb_spectrum, &x, &y, &z);
        xyz_to_rgb(cs, x, y, z, &r, &g, &b);
        colorPalette[i].blue = (int)(b * 255.0);
        colorPalette[i].green = (int)(g * 255.0);
        colorPalette[i].red = (int)(r * 255.0);
        i++;
    }
    return colorPalette;
}

int main() {
	struct bmp_fileheader file_header;
	struct bmp_infoheader info_header;
	char nume_output[dim_max_nume] = "Mandelbrot.bmp";
	char nume_input[dim_max_nume] = "input.bmp";
	FILE *input = fopen(nume_input, "rb");
	if (!input) {
		printf("Nu s-a putut deschide fisierul de intrare specificat!\n");
		return -1;
	}
	FILE *output = fopen(nume_output, "wb");
	if (!output) {
		printf("Nu s-a putut deschide fisierul de iesire specificat!\n");
		fclose(input);
		return -2;
	}
	fread(&file_header, sizeof(struct bmp_fileheader), 1, input);
	fwrite(&file_header, sizeof(struct bmp_fileheader), 1, output);
	fread(&info_header, sizeof(struct bmp_infoheader), 1, input);
	info_header.width = wid;
	info_header.height = hei;
	info_header.biSizeImage = sizeof(bmp_fileheader) + sizeof(bmp_infoheader) + wid * hei;
	fwrite(&info_header, sizeof(struct bmp_infoheader), 1, output);
	fclose(input);
	int i, j, iteration;
	struct bitmap curent;
	double x[wid], y[hei], x0, y0, x_curent, y_curent, log_zn, nu;
	struct bitmap color1, color2, color_rez;
	struct bitmap* colorPalette = getColorPalette();
	for (j = hei - 1; j >= 0; j--) {
		for (i = 0; i < wid; i++) {
			x0 = -2.5 + 3.5 * i / wid;
			y0 = -1.0 + 2.0 * j / hei;
			x[i] = 0;
			y[j] = 0;
			iteration = 0;
			while ((iteration < max_iteration) && (x[i] * x[i] + y[j] * y[j] <= val_max)) {
				x_curent = x[i] * x[i] - y[j] * y[j] + x0;
				y_curent = 2.0 * x[i] * y[j] + y0;
				x[i] = x_curent;
				y[j] = y_curent;
				iteration += 1;
			}
			if (iteration >= max_iteration) {
				curent.blue = 0;
				curent.green = 0;
				curent.red = 0;
			}
			else {
				/*log_zn = log(x[i] * x[i] + y[j] * y[j]) / 2;
				nu = log(log_zn / log(2)) / log(2);
				iteration = iteration + 1 - nu;
				color1 = colorPalette[iteration];
				color2 = colorPalette[iteration + 1];
				curent.blue = abs((1 - iteration) * color1.blue + iteration * color2.blue) % 256;
				curent.green = abs((1 - iteration) * color1.green + iteration * color2.green) % 256;
				curent.red = abs((1 - iteration) * color1.red + iteration * color2.red) % 256;*/
				curent.blue = colorPalette[iteration].blue;
				curent.green = colorPalette[iteration].green;
				curent.red = colorPalette[iteration].red;
			}
			fwrite(&curent, sizeof(struct bitmap), 1, output);
		}
	}
	fclose(output);
	return 0;
}
