#ifndef __LINE_BRESENHAM__H__
#define __LINE_BRESENHAM__H__


#if defined (__cplusplus) || defined (c_plusplus)
extern "C"{
#endif

#define draw_line(x0, y0, x1, y1)		bresenham(x0, y0, x1, y1)

void draw_setimage(unsigned char *image, int w, int h);
void draw_setcolor(unsigned char r, unsigned char  g, unsigned char  b);
void draw_bbox_xywh(int x, int y, int w, int h);
void draw_bbox_xyxy(int x1, int y1, int x2, int y2);




#if defined (__cplusplus) || defined (c_plusplus)
}
#endif

#endif // __LINE_BRESENHAM__H__