/*==========================================================
  File:  tga.c
  Author:  unknown

  Description:  Gets TGA data from a TGA image file.

  Note:  I obtained this from somewhere on the OpenGL website
         and modified it to fit my needs.
  ==========================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tga.h"
#include "util.h"

/*
   The following is rather crude demonstration code to read 
   uncompressed and compressed TGA files of 16, 24, or 32 bit 
   TGA. 
   Hope it is helpful.
*/

unsigned char *TGA_getData(char *filename, int *w, int *h)
{
   int n=0,i,j;
   unsigned int bytes2read,skipover = 0;
   unsigned char p[5];
   FILE *fptr;
   TGA_HEADER header;
   TGA_PIXEL *pixels;
   unsigned char *data;

   if(!filename) return 0;

   if ((fptr = fopen(filename,"r")) == NULL)
     return 0;

   /* Display the header fields */
   header.idlength = fgetc(fptr);
   header.colourmaptype = fgetc(fptr);
   header.datatypecode = fgetc(fptr);
   fread(&header.colourmaporigin,2,1,fptr);
   fread(&header.colourmaplength,2,1,fptr);
   header.colourmapdepth = fgetc(fptr);
   fread(&header.x_origin,2,1,fptr);
   fread(&header.y_origin,2,1,fptr);
   fread(&header.width,2,1,fptr);
   fread(&header.height,2,1,fptr);
   header.bitsperpixel = fgetc(fptr);
   header.imagedescriptor = fgetc(fptr);

   /* Allocate space for the image */
   if ((pixels = malloc(header.width*header.height*sizeof(TGA_PIXEL))) == NULL) {
      fclose(fptr);
      perror("TGA_getData malloc");
      return 0;
   }
   for (i=0;i<header.width*header.height;i++) {
      pixels[i].r = 0;
      pixels[i].g = 0;
      pixels[i].b = 0;
      pixels[i].a = 0;
   }

   /* What can we handle */
   if (header.datatypecode != 2 && header.datatypecode != 10) {
     fclose(fptr);
     free(pixels);
     return 0;
   }

   if (header.bitsperpixel != 16 && 
       header.bitsperpixel != 24 && header.bitsperpixel != 32) {
     fclose(fptr);
     free(pixels);
     return 0;
   }
   if (header.colourmaptype != 0 && header.colourmaptype != 1) {
     fclose(fptr);
     free(pixels);
     return 0;
   }

   /* Skip over unnecessary stuff */
   skipover += header.idlength;
   skipover += header.colourmaptype * header.colourmaplength;
   fseek(fptr,skipover,SEEK_CUR);

   /* Read the image */
   bytes2read = header.bitsperpixel / 8;
   printf("bytes2read: %d\n", bytes2read);
   while (n < header.width * header.height) {
      if (header.datatypecode == 2) {                     /* Uncompressed */
         if (fread(p,1,bytes2read,fptr) != bytes2read) {
            fprintf(stderr,"TGA_getData: Unexpected end of file at pixel %d\n",i);
            free(pixels);
            fclose(fptr);
            return 0;
         }
         MergeBytes(&(pixels[n]),p,bytes2read);
         n++;
      } else if (header.datatypecode == 10) {             /* Compressed */
         if (fread(p,1,bytes2read+1,fptr) != bytes2read+1) {
            fprintf(stderr,"TGA_getData: Unexpected end of file at pixel %d\n",i);
            free(pixels);
            fclose(fptr);
            return 0;
         }
         j = p[0] & 0x7f;
         MergeBytes(&(pixels[n]),&(p[1]),bytes2read);
         n++;
         if (p[0] & 0x80) {         /* RLE chunk */
            for (i=0;i<j;i++) {
               MergeBytes(&(pixels[n]),&(p[1]),bytes2read);
               n++;
            }
         } else {                   /* Normal chunk */
            for (i=0;i<j;i++) {
              int wtf = fread(p, 1, bytes2read, fptr);
              printf("read: %d\n", wtf);
               if (wtf != bytes2read) {
                  fprintf(stderr,"TGA_loadData: Unexpected end of file at pixel %d\n",i);
                  free(pixels);
                  fclose(fptr);
                  return 0;
               }
               MergeBytes(&(pixels[n]),p,bytes2read);
               n++;
            }
         }
      }
   }
   fclose(fptr);

   data=(unsigned char *)malloc(header.height * header.width * 4);

   for (i=0,n=0;i<header.height*header.width;i++) 
   {
      data[n++] = pixels[i].r;
      data[n++] = pixels[i].g;
      data[n++] = pixels[i].b;
      data[n++] = pixels[i].a;
   }

   free(pixels);
   *w=header.width;
   *h=header.height;
   return data;
}

void MergeBytes(TGA_PIXEL *pixel,unsigned char *p,int bytes)
{
   if (bytes == 4) {
      pixel->r = p[2];
      pixel->g = p[1];
      pixel->b = p[0];
      pixel->a = p[4];
   } else if (bytes == 3) {
      pixel->r = p[2];
      pixel->g = p[1];
      pixel->b = p[0];
      pixel->a = 0;
   } else if (bytes == 2) {
      pixel->r = (p[1] & 0x7c) << 1;
      pixel->g = ((p[1] & 0x03) << 6) | ((p[0] & 0xe0) >> 2);
      pixel->b = (p[0] & 0x1f) << 3;
      pixel->a = (p[1] & 0x80);
   }
}



