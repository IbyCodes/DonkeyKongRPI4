void fb_init();
void drawPixel(int x, int y, unsigned char attr);
void drawChar(unsigned char ch, int x, int y, unsigned char attr);
void drawString(int x, int y, char *s, unsigned char attr);
void drawRect(int x1, int y1, int x2, int y2, unsigned char attr, int fill);
void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill);
void drawLine(int x1, int y1, int x2, int y2, unsigned char attr);


// source: https://github.com/isometimes/rpi4-osdev/tree/master/part5-framebuffer/fb.h

// these are newly added method
void myDrawPixel(int x, int y, int argb_color);
void myDrawImage(unsigned char * img, int width, int height, int offx, int offy, int dontDrawBlack); // assumes image buffer has pixels in RGBA color space not argb
