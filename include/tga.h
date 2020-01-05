/*
   The following is rather crude demonstration code to read 
   uncompressed and compressed TGA files of 16, 24, or 32 bit 
   TGA. 
   Hope it is helpful.
*/

typedef struct {
   char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
} TGA_HEADER;

typedef struct {
   unsigned char r,g,b,a;
} TGA_PIXEL;

void MergeBytes(TGA_PIXEL *,unsigned char *,int);
unsigned char *TGA_getData(char *filename, int *w, int *h);
