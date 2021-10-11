#include <stdio.h>
#include <stdlib.h>

static int _chanel_num =1;

static void _set_channel_num(int n)
{
	_chanel_num = n;
	return;
}

static inline int is_in_array(int x, int y, int height, int width)
{
	if (x >= 0 && x < width && y >= 0 && y < height)
		return 1;
	else
		return 0;
}


static inline char get_arrayXY(int ch, char *in, int width, int height, int x, int y)
{
	char *p;

	p = in + (((width*y) + x)*_chanel_num) + ch;
	return *p;
}


static inline void set_arrayXY(int ch, char v, char *out, int width, int height, int x, int y)
{
	char *p;

	p = out + (((width*y) + x)*_chanel_num) + ch;
	*p = v;
}


void bilinera_interpolation(int n_ch, char* in_array, int height, int width, char* out_array, int out_height, int out_width)
{
// printf("bilinera_interpolation(): [IN] %dx%d, [OUT] %dx%d ...\n", width, height, out_width, out_height);
	float	h_times, w_times;
	int  x1, y1, x2, y2, f11, f12, f21, f22;
	float x, y;
	int is_in_x1y1, is_in_x1y2, is_in_x2y1, is_in_x2y2;

	h_times = (float)out_height / (float)height;
	w_times = (float)out_width / (float)width;

	if (!in_array || !out_array) {
		printf("null pointer!!\n");
		return;
	}

	_set_channel_num(n_ch);

	// printf("h_times=%f, w_times=%f\n", h_times, w_times);
	for (int i = 0; i < out_height; i++){
		// printf("i=%d \n", i);
		for (int j = 0; j < out_width; j++){
			x = j / w_times;
			y = i / h_times;
			x1 = (int)(x - 1);
			x2 = (int)(x + 1);
			y1 = (int)(y + 1);
			y2 = (int)(y - 1);

			is_in_x1y1 = !!is_in_array(x1, y1, height, width);
			is_in_x1y2 = !!is_in_array(x1, y2, height, width);
			is_in_x2y1 = !!is_in_array(x2, y1, height, width);
			is_in_x2y2 = !!is_in_array(x2, y2, height, width);

			for (int c=0; c<n_ch; c++) {
				f11 = is_in_x1y1 ? (int)(0xFF & get_arrayXY(c, in_array, width, height, x1, y1)) /* in_array[y1][x1]*/ : 0;
				f12 = is_in_x1y2 ? (int)(0xFF & get_arrayXY(c, in_array, width, height, x1, y2)) /* in_array[y2][x1]*/ : 0;
				f21 = is_in_x2y1 ? (int)(0xFF & get_arrayXY(c, in_array, width, height, x2, y1)) /* in_array[y1][x2]*/ : 0;
				f22 = is_in_x2y2 ? (int)(0xFF & get_arrayXY(c, in_array, width, height, x2, y2)) /* in_array[y2][x2]*/ : 0;

				int t = (int)(
								(
									(f11 * (x2 - x) * (y2 - y)) +
									(f21 * (x - x1) * (y2 - y)) +
									(f12 * (x2 - x) * (y - y1)) +
									(f22 * (x - x1) * (y - y1))
								) / ((x2 - x1) * (y2 - y1))
							);

				char v = (char)(t & 0xFF);
				set_arrayXY(c, v, out_array, out_width, out_height, j, i); /*out_array[i][j]*/
			}
		}
	}
}
