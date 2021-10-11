#include <stdio.h>
#include <stdlib.h>

#define LOG_TIME
#ifdef LOG_TIME
#include <sys/time.h>
#include <time.h>
#endif

static int _chanel_num =1;

static void _set_channel_num(int n)
{
	_chanel_num = n;
	return;
}

static int _is_in_array(int x, int y, int height, int width)
{
	if (x >= 0 && x < width && y >= 0 && y < height)
		return 1;
	else
		return 0;
}

#if 0
static inline char _get_arrayXY(int ch, char *in, int width, int height, int x, int y)
{
	return *((char *)(in + (((width*y) + x)*_chanel_num) + ch));
}
#endif

#if 0
static void _set_arrayXY(int ch, char v, char *out, int width, int height, int x, int y)
{
	char *p;

	p = out + (((width*y) + x)*_chanel_num) + ch;
	*p = v;
}
#endif


#ifdef LOG_TIME
struct timeval tv;
long int start_time, end_time;
#endif

void fast_bilinera_interpolation(int n_ch, char* in_array, int width, int height, char* out_array, int out_width, int out_height)
{
	//printf("bilinera_interpolation(): [IN] %dx%d, [OUT] %dx%d ...\n", width, height, out_width, out_height);
	// float h_times, w_times;
	int  x1, y1, x2, y2, f11, f12, f21, f22;
	// float x, y;
	int is_in_x1y1, is_in_x1y2, is_in_x2y1, is_in_x2y2;

#ifdef LOG_TIME
	gettimeofday(&tv, NULL);
	start_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif

	float h_times = ((float)(out_height)) / height;		//--> h_times = H/h
	float w_times = ((float)(out_width)) / width;		//--> w_times = W/w

	if (!in_array || !out_array) {
		printf("null pointer!!\n");
		return;
	}

	_set_channel_num(n_ch);

	// printf("h_times=%f, w_times=%f\n", h_times, w_times);
	static int j_w, i_h, W_H;
	static int x1_W, x2_W, y1_H, y2_H;
	int i, j;
	W_H = out_width * out_height;
	char *pxout = out_array;
	for (i = 0, i_h=0; i < out_height; i++, i_h+=height) {
		//  printf("i=%d \n", i);
		float y = i / h_times;
		y1 = (int)(y+1);	// (int)(y + 1); --> (i*h + H)/H
		y2 = (int)(y-1);	// (int)(y - 1); --> (i*h - H)/H
		// y1 = (int)((i_h+out_height)/out_height);	// (int)(y + 1); --> (i*h + H)/H
		// y2 = (int)((i_h-out_height)/out_height);	// (int)(y - 1); --> (i*h - H)/H
		int offset_y1 = y1 * width * n_ch;
		int offset_y2 = y2 * width * n_ch;
		y1_H = y1 * out_height;
		y2_H = y2 * out_height;

		for (j = 0, j_w=0; j < out_width; j++, j_w+=width) {
			// printf("\tj=%d\n", j);
			// x = j / w_times;		--> x = (j*w)/W
			// y = i / h_times;		--> y = (i*h)/H

			// j_w = j * width;
			// i_h = i * height;
			// W_H = out_width * out_height;

			// x1 = (int)((j*width-out_width)/out_width);		// (int)(x - 1); --> (j*w - W)/W
			// x2 = (int)((j*width+out_width)/out_width);		// (int)(x + 1);  --> (j*w + W)/W
			// y1 = (int)((i*height+out_height)/out_height);	// (int)(y + 1); --> (i*h + H)/H
			// y2 = (int)((i*height-out_height)/out_height);	// (int)(y - 1); --> (i*h - H)/H

			float x = j / w_times;
			x1 = (int)(x-1);		// (int)(x - 1); --> (j*w - W)/W
			x2 = (int)(x+1);		// (int)(x + 1);  --> (j*w + W)/W
			// x1 = (int)((j_w-out_width)/out_width);		// (int)(x - 1); --> (j*w - W)/W
			// x2 = (int)((j_w+out_width)/out_width);		// (int)(x + 1);  --> (j*w + W)/W

			int offset_x1 = x1 * n_ch;
			int offset_x2 = x2 * n_ch;

// printf("---1---\n");
			is_in_x1y1 = !!_is_in_array(x1, y1, height, width);
			is_in_x1y2 = !!_is_in_array(x1, y2, height, width);
			is_in_x2y1 = !!_is_in_array(x2, y1, height, width);
			is_in_x2y2 = !!_is_in_array(x2, y2, height, width);
// printf("---2---\n");

			char *px_x1y1 = (char *)(in_array + offset_x1 + offset_y1);
			char *px_x1y2 = (char *)(in_array + offset_x1 + offset_y2);
			char *px_x2y1 = (char *)(in_array + offset_x2 + offset_y1);
			char *px_x2y2 = (char *)(in_array + offset_x2 + offset_y2);

			x1_W = x1 * out_width;
			x2_W = x2 * out_width;

			int denominator = ((x2 - x1) * (y2 - y1) * W_H);
			int f11_DD = (x2_W - j_w) * (y2_H - i_h);
			int f21_DD = (j_w - x1_W) * (y2_H - i_h);
			int f12_DD = (x2_W - j_w) * (i_h - y1_H);
			int f22_DD = (j_w - x1_W) * (i_h - y1_H);
			for (int c=0; c<n_ch; c++) {
				// printf("c=%d\n", c);
				#if 1
				f11 = is_in_x1y1 ? (int)(0xFF & px_x1y1[c]) /* in_array[y1][x1]*/ : 0;
				f12 = is_in_x1y2 ? (int)(0xFF & px_x1y2[c]) /* in_array[y2][x1]*/ : 0;
				f21 = is_in_x2y1 ? (int)(0xFF & px_x2y1[c]) /* in_array[y1][x2]*/ : 0;
				f22 = is_in_x2y2 ? (int)(0xFF & px_x2y2[c]) /* in_array[y2][x2]*/ : 0;
				#else
				f11 = is_in_x1y1 ? (int)(0xFF & _get_arrayXY(c, in_array, width, height, x1, y1)) /* in_array[y1][x1]*/ : 0;
				f12 = is_in_x1y2 ? (int)(0xFF & _get_arrayXY(c, in_array, width, height, x1, y2)) /* in_array[y2][x1]*/ : 0;
				f21 = is_in_x2y1 ? (int)(0xFF & _get_arrayXY(c, in_array, width, height, x2, y1)) /* in_array[y1][x2]*/ : 0;
				f22 = is_in_x2y2 ? (int)(0xFF & _get_arrayXY(c, in_array, width, height, x2, y2)) /* in_array[y2][x2]*/ : 0;
				#endif

				// int t = (int)(
				// 				(	(f11 * (x2 - x) * (y2 - y)) +
				// 					(f21 * (x - x1) * (y2 - y)) +
				// 					(f12 * (x2 - x) * (y - y1)) +
				// 					(f22 * (x - x1) * (y - y1))
				// 				)  /  ((x2 - x1) * (y2 - y1) )
				// 			);

				// int t = (int)(
				// 				(	(f11 * (x2*out_width - j*width) * (y2*out_height - i*height))/(out_width*out_height) +
				// 					(f21 * (j*width - x1*out_width) * (y2*out_height - i*height))/(out_width*out_height) +
				// 					(f12 * (x2*out_width - j*width) * (i+height - y1*out_height))/(out_width*out_height) +
				// 					(f22 * (j*width - x1*out_width) * (i*height - y1*out_height))/(out_width*out_height)
				// 				) / ((x2 - x1) * (y2 - y1) )
				// 			);

				int t;
				t = (int)(
							// (	(f11 * (x2_W - j_w) * (y2_H - i_h)) +
							// 	(f21 * (j_w - x1_W) * (y2_H - i_h)) +
							// 	(f12 * (x2_W - j_w) * (i_h - y1_H)) +
							// 	(f22 * (j_w - x1_W) * (i_h - y1_H))
							// ) / denominator /* ((x2 - x1) * (y2 - y1) * W_H) */
							(	(f11 * f11_DD) +
								(f21 * f21_DD) +
								(f12 * f12_DD) +
								(f22 * f22_DD)
							) / denominator /* ((x2 - x1) * (y2 - y1) * W_H) */
						);
				#if 1
				*pxout++ = (char)(t & 0xFF);
				#else
				char v = (char)(t & 0xFF);
				_set_arrayXY(c, v, out_array, out_width, out_height, j, i); /*out_array[i][j]*/
				#endif
			}	// channel
		} // j (column)
	}	// i (row)

	#ifdef LOG_TIME
	end_time = gettimeofday(&tv, NULL);
	end_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	printf("elapase_time = %ld\n", (end_time - start_time));
	#endif
}
