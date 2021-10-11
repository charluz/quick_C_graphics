#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdarg.h>			/*for valist */
#include "common.h"
// #include "user_util.h"

/*---------------------------------------------------------------------
 * Constants
 */
#define SZ_FNAME_LEN_MAX			(32)
#define CHANEL_NUM					(3)

/*------------------------------------------------------------------
 * Global Variables
 */
FILE *fd_bmp, *fd_out;
static char *psz_prog, *psz_fnbmp, sz_fnraw[SZ_FNAME_LEN_MAX];


/*---------------------------------------------------------------------
 * External reference
 */
void bilinera_interpolation(int n_ch, char* in_array, int width, int height, char* out_array, int out_width, int out_height);

void bitmap(char *pfbmp, unsigned int width, unsigned height, char *prgb);

/*------------------------------------------------------------------
 * Implementation
 */


/*---------------------------------------------------------------------
 * _print_prog_usage
 */
static void _print_prog_usage(char *psz_prog_name)
{
	printf("\n");
	printf("Usage:\n");
	printf("\t%s src.bmp out_width out_height [out.bmp]\n", psz_prog_name);
	printf("DESCRIPTION:\n");
	printf("\tScale Up/Down src.bmp to out_width * out_height...\n");
//	printf("\tBMP_FILENAME shall specify the filename without the extension part.\n");
//  printf("\tThe program automatically generates the raw file - fname.raw.\n");
//	printf("OPTION:\n");
//	printf(" -w     *width* : specify width of raw image\n");
//	printf(" -h     *height* : specify height of raw image\n");
//	printf(" -bayer *pattern_id* : bayer pattern\n");
//	printf("\t 1=R_GR_GB_B, 2=GR_R_B_GB, 3=B_GB_GR_R, 4=GB_B_R_GR\n");
//	printf(" -b     *bits* : specify bits per pixel\n");
//	printf(" -of    *raw_fname* : file name of output RAW image.\n");
//	printf(" -bs    *block_hsize* : specify the hsize of do block\n");
//	printf(" -cfg   *cfg_fname* : specify full name of cfg file\n");
}

/*---------------------------------------------------------------------
 * vertical_flip
 */
static void vertical_flip(int ch_num, char *inbuf, int hsize, int vsize, char *outbuf)
{
	int row, col;
	char *prgb;

	printf("%s() : hsize(%d) vsize(%d), channel=%d\n", __FUNCTION__, hsize, vsize, ch_num);

	for (row=vsize-1; row>=0; row--) {
		//printf("-- row=%d\n", row);
		for (col=0; col<hsize; col++) {
			//printf("\trow=%d col=%d\n", row, col);
			prgb = inbuf+(hsize*row + col)*ch_num;
			for (int c=0; c<ch_num; c++) {

				*outbuf++ = *prgb++;
			}
		}
	}
}

/*---------------------------------------------------------------------
 * swap RB
 */
static void swap_RB(char *inbuf, int hsize, int vsize)
{
	int row, col;
	char *prgb;

	for (row=vsize-1; row>=0; row--) {
		//printf("-- row=%d\n", row);
		for (col=0; col<hsize; col++) {
			//printf("\trow=%d col=%d\n", row, col);
			char temp;
			prgb = inbuf+(hsize*row + col)*3;
			temp = prgb[2];
			prgb[2] = prgb[0];
			prgb[0] = temp;
		}
	}
}


/*---------------------------------------------------------------------
 * _gray_2_bmp (BGR)
 */
static void _gray_2_bmp(char *pgray, unsigned hsize, unsigned vsize, char *pbmp)
{
	//--- bin2bmp function will store the image up-side donw, and
	// use BGR format, leave it to bin2bmp function !!
	int width=hsize, height=vsize;
	char *pdstbuf, *pdstsave, *psrcbuf;

	pdstsave = pdstbuf = pbmp;
	psrcbuf = pgray;

	for(int row=0; row<height; row++)  {
		pdstbuf = pdstsave + (row * width * 3);
		for (int col=0; col<width; col++) {
			char r, g, b;

			r = g = b = (char)*psrcbuf++;

			//-- BMP 是 BGR 排列, leave it to bin2bmp function !!
			*pdstbuf++ = r;
			*pdstbuf++ = g;
			*pdstbuf++ = b;
		}
	}

}

/*---------------------------------------------------------------------
 * _bmp_2_gray
 */
static void _bmp_2_gray(char *pbmp, int hsize, int vsize, char *pgray)
{
	int row, col;
	char *prgb;
	char rawdat[3];

	_assert(hsize && vsize && pbmp);

	printf("%s() : hsize(%d) vsize(%d)\n", __FUNCTION__, hsize, vsize);

	for (row=vsize-1; row>=0; row--) {
		//printf("-- row=%d\n", row);
		for (col=0; col<hsize; col++) {
			//printf("\trow=%d col=%d\n", row, col);
			unsigned b, g, r;
			unsigned gray;

			prgb = pbmp+(hsize*3*row + col*3);
			b = (unsigned)(*prgb);	prgb++;// B
			g = (unsigned)(*prgb);	prgb++;// G
			r = (unsigned)(*prgb);	// R
			// gray = (unsigned char)(r*0.3 + g*0.6 + b*0.1);
			gray = (unsigned)(r+ g + b)/3;
			*pgray = (unsigned char)gray;
			pgray++;
		}
	}
}


/*---------------------------------------------------------------------
 * _read_bmp_image
 */
static unsigned _read_bmp_image(FILE *fd, char *pbuf, unsigned imgsize)
{
	size_t fsize;

	_assert(pbuf && imgsize && fd);

	fseek(fd, 54, SEEK_SET);
	fsize = fread((void *)pbuf, sizeof(char), imgsize, fd);
	// printf("%s: fsize= %d\n", __FUNCTION__, fsize);

	return fsize;
}


/*---------------------------------------------------------------------
 * _parse_bmp_dimension
 */
static void _parse_bmp_dimension(FILE *fd, unsigned *phsize, unsigned *pvsize)
{
	char szsize[4];

	_assert(fd);

	/*-- get hsize */
	fseek(fd, 18, SEEK_SET);
	fread(szsize, sizeof(char), 4, fd);
	*phsize = *((unsigned *)szsize);
	// printf("hsize=%d\n", *phsize);

	/*-- get vsize */
	fseek(fd, 22,SEEK_SET);
	fread(szsize, sizeof(char), 4, fd);
	*pvsize = *((unsigned *)szsize);
	// printf("vsize=%d\n", *pvsize);
}


/*---------------------------------------------------------------------
 * main
 */
int main(int argc, char *argv[])
{
	size_t szlen;
	int err;
	char *ptemp;
	int out_width, out_height;

	psz_prog = argv[0];
	if (argc<4) {
		_print_prog_usage(psz_prog);
		return -1;
	}

	/*-- open bmp file */
	psz_fnbmp = argv[1];
//	printf("openning bmp fname : %s\n", psz_fnbmp);
	fd_bmp = fopen(psz_fnbmp, "rb");
	if (!fd_bmp) {
		printf("FATAL: error openning BMP [%s] !\n", psz_fnbmp);
		exit(1);
	}

	out_width = atoi(argv[2]);
	out_height = atoi(argv[3]);

	int width, height;
	int imgsize;
	char *pbmpbuf=NULL;

	/*-- bmp size parse */
	_parse_bmp_dimension(fd_bmp, &width, &height);
	printf("BMP[%s] size : %d x %d\n", psz_fnbmp, width, height);

	/*-- alloc & read BMP image */
	imgsize = width*height*CHANEL_NUM;
	pbmpbuf = (char *)malloc(imgsize);
	if (!pbmpbuf) {
		printf("error: insufficient memory for BMP image!\n");
		goto _exit;
	}

	// printf("read bmp image...\n");
	err = _read_bmp_image(fd_bmp, pbmpbuf, imgsize);
	if (err!=imgsize) {
		printf("error: incorrect image size! req(%d) but read(%d)\n", imgsize, err);
		goto _exit;
	}

	/*-- swap RB, bmp store image in BGR format */
	printf("swap RB ...\n");
	swap_RB(pbmpbuf, width, height);

	/*-- vertical flip, bmp image is vertical flipped */
	printf("vertical flip ...\n");
	char *pvfbuf = (char *)malloc(height*width*3);
	if (!pvfbuf) {
		printf("error: insufficient memory for vflip image!\n");
		goto _exit;
	}
	vertical_flip(3, pbmpbuf, width, height, pvfbuf);
	ptemp = pbmpbuf;
	pbmpbuf = pvfbuf;
	free(ptemp);
	pvfbuf = NULL;

	/*-- Upscaling  x1.5 */
	// printf("Scaling to %d x %d ...\n", out_width, out_height);
	char *pupbuf = (char *)malloc(out_width*out_height*CHANEL_NUM);
	if (!pupbuf) {
		printf("error: insufficient memory for OUT image!\n");
		goto _exit;
	}
	printf("Scaling to %d x %d ...\n", out_width, out_height);
	fast_bilinera_interpolation(CHANEL_NUM, pbmpbuf, width, height, pupbuf, out_width, out_height);
//	free(pbmpbuf); pbmpbuf = NULL;

	/*-- write output BMP file */
	/*-- open output file */
	printf("open outpuf file \n");
	char *psz_oext = (char *)"bmp";
	char postfix[32];
	snprintf(postfix, 32, "_%dx%d", out_width, out_height);
	char *psz_ofname = prepare_file_name(psz_fnbmp, psz_oext, postfix);

	printf("write bmp file, %s ...\n", psz_ofname);
	bitmap(psz_ofname, out_width, out_height, pupbuf);

	/*-- exiting program */
_exit:
	printf("clean up ...\n");
	if (fd_bmp) fclose(fd_bmp);
	if (pbmpbuf) free(pbmpbuf);
	if (pvfbuf) free(pvfbuf);
	if (pupbuf) free(pupbuf);
	if (fd_out) fclose(fd_out);

	// if (!pbmpbuf) free(pbmpbuf);
    return -1;
}
