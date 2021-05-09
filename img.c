#include<stdio.h>

int main(){
    FILE *stream;
    stream = fopen("echidna.bmp", "rb");
  
    unsigned char header[54];
    unsigned char colortable[1024];
    for(int i=0; i<54; i++){
        header[i] = getc(stream);
    }

    int width  = *(int*)&header[18];
    int height = *(int*)&header[22];
    int bit_depth = *(int*)&header[28];
    printf("width is %d\nheight is %d\nbit depth is %d\n", width,height,bit_depth);

    fread(colortable, sizeof(unsigned char), 1024, stream); //pass 1024 bytes into colortable array( after 54 byte header comes this 1024 colortable bytes
    int pixel_size = bit_depth/8;
    unsigned char buf[height * width* pixel_size]; //image buffer, my bitdepth is 24 which is 3 times bigger than normal bmp, so the image will be 3 times larger than its height*width to hold all the colors (because every byte that would hold one color now is 3 bytes holding the color)
    
    fread(buf,sizeof(unsigned char), (height*width*pixel_size), stream);
    
    FILE *new_img = fopen("new_echidna.bmp", "wb");
    fwrite(header,sizeof(unsigned char), 54, new_img);
    fwrite(colortable,sizeof(unsigned char), 1024, new_img);
    fwrite(buf,sizeof(unsigned char), (height*width*pixel_size),new_img);
    fclose(new_img);
    fclose(stream);
    
}
