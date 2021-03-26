#include "svpng.inc"
#include <math.h>   // fmaxf(), sinf(), cosf()
#include <stdlib.h> // abs()
#include <string.h> // memset()

#include "line_bresenham.h"

#define PI 3.14159265359f
// #define W 512
// #define H 512
// static unsigned char img[W * H * 3];
static unsigned char _line_r=255, _line_g=255, _line_b=0;	// default line color=yellow
static unsigned char *_p_image=NULL;
static int _W=0, _H=0;

// Modified from  https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
static void bresenham(int x0, int y0, int x1, int y1) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;

	while (draw_setpixel(x0, y0), x0 != x1 || y0 != y1) {
		int e2 = err;
		if (e2 > -dx) { err -= dy; x0 += sx; }
		if (e2 <  dy) { err += dx; y0 += sy; }
	}
}

void draw_bbox_xyxy(int x1, int y1, int x2, int y2)
{
	if (!_p_image || !_W || !_H) {
		printf("[FATAL] Bad image param !!\n");
		return;
	}

	x1 = (x1 > 0) ? x1 : 0;
	y1 = (y1 > 0) ? y1 : 0;
	x2 = (x2 >= _W) ? _W-1 : x2;
	y2 = (y2 >= _H) ? _H-1 : y2;

	bresenham(x1, y1, x2, y1);
	bresenham(x2, y1, x2, y2);
	bresenham(x2, y2, x1, y2);
	bresenham(x1, y2, x1, y1);
	return;
}

void draw_bbox_xywh(int x, int y, int w, int h)
{
	int x2, y2;
	x2 = x+w;
	y2 = y+h;

	draw_bbox_xyxy(x, y, x2, y2);
	return;
}

void draw_setcolor(unsigned char r, unsigned char  g, unsigned char  b)
{
	_line_r = r;	_line_g = g;	_line_b = b;
	return;
}

void draw_setpixel(int x, int y)
{
	unsigned char* p = _p_image + (y * _W + x) * 3;
	p[0] = _line_r; p[1] = _line_g; p[2] = _line_b;
}

void draw_setimage(unsigned char *image, int w, int h)
{
	_p_image = image;
	_W = (w > 0) ? w : 0;
	_H = (h > 0) ? h : 0;

	return;
}

#if 0 // CY

int main() {
	memset(img, 255, sizeof(img));
	float cx = W * 0.5f - 0.5f, cy = H * 0.5f - 0.5f;
	for (int j = 0; j < 5; j++) {
		float r1 = fminf(W, H) * (j + 0.5f) * 0.085f;
		float r2 = fminf(W, H) * (j + 1.5f) * 0.085f;
		float t = j * PI / 64.0f;
		for (int i = 1; i <= 64; i++, t += 2.0f * PI / 64.0f) {
			float ct = cosf(t), st = sinf(t);
			bresenham((int)(cx + r1 * ct), (int)(cy - r1 * st), (int)(cx + r2 * ct), (int)(cy - r2 * st));
		}
	}
	svpng(fopen("line_bresenham.png", "wb"), W, H, img, 0);
}

#endif