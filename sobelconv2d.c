#include<stdio.h>
#include<math.h>
// if you pass 1d array you can pass the pointer directly on parameters, like unsigned char *bgr, if you pass "n" dimension array you need to write is as: type(*var)[n]    
void negativeImage(FILE* image,int size,unsigned char(*bgr)[3]){ 
    
    for(int i=0;i<size;i++)
    {
        putc(255-bgr[i][0], image);
        putc(255-bgr[i][1], image);
        putc(255-bgr[i][2], image);
    }
}

void sobelConv2d(FILE* image,unsigned char(*bgr)[3]){
    
    unsigned char header[54];
    long cur = ftell(image);
    fseek(image,0,SEEK_SET);
    fread(header, sizeof(unsigned char), 54, image);
    fseek(image,cur,SEEK_SET);
    int width  = *(int*)&header[18];
    int height = *(int*)&header[22];
    int size = width*height;
    unsigned char b[width][height], g[width][height], r[width][height];
    // create 2d matrix for each color
    for(int i=0; i<width;i++)
    {
        for(int j=0; j<height;j++)
        {
            b[i][j]=bgr[j+(i*width)][0];
            g[i][j]=bgr[j+(i*width)][1];
            r[i][j]=bgr[j+(i*width)][2];
        }
    }
    //conv2d
    int gx=0,gy=0; //gradients
    unsigned char conv_b[width-2][height-2], conv_g[width-2][height-2], conv_r[width-2][height-2];
    for(int i=0;i<width-2;i++)
    {
        for(int j=0; j<height-2;j++)
        {
            //horizontal derivative for blue 
            gx = (    b[i][j+0]*(-1) 
                    + b[i][j+1]*0
                    + b[i][j+2]*1

                    + b[i+1][j+0]*(-2) 
                    + b[i+1][j+1]*0
                    + b[i+1][j+2]*2
                    
                    + b[i+2][j+0]*(-1) 
                    + b[i+2][j+1]*0
                    + b[i+2][j+2]*1);
            //vertical derivative for blue
             gy = (   b[i][j+0]*1 
                    + b[i][j+1]*2
                    + b[i][j+2]*1

                    + b[i+1][j+0]*0 
                    + b[i+1][j+1]*0
                    + b[i+1][j+2]*0
                    
                    + b[i+2][j+0]*(-1) 
                    + b[i+2][j+1]*(-2)
                    + b[i+2][j+2]*(-1));
            //vertical and horizontal derivatives represents intensity, thus they are components of a vertical/horizontal vector, the derivative is calculated as the module of that vector.
            conv_b[i][j]= sqrt((gx*gx) + (gy*gy));
            
            gx = (    g[i][j+0]*(-1) 
                    + g[i][j+1]*0
                    + g[i][j+2]*1

                    + g[i+1][j+0]*(-2) 
                    + g[i+1][j+1]*0
                    + g[i+1][j+2]*2
                    
                    + g[i+2][j+0]*(-1) 
                    + g[i+2][j+1]*0
                    + g[i+2][j+2]*1);
            
             gy = (   g[i][j+0]*1 
                    + g[i][j+1]*2
                    + g[i][j+2]*1

                    + g[i+1][j+0]*0 
                    + g[i+1][j+1]*0
                    + g[i+1][j+2]*0
                    
                    + g[i+2][j+0]*(-1) 
                    + g[i+2][j+1]*(-2)
                    + g[i+2][j+2]*(-1));

            conv_g[i][j]= sqrt((gx*gx) + (gy*gy));

            gx = (    r[i][j+0]*(-1) 
                    + r[i][j+1]*0
                    + r[i][j+2]*1

                    + r[i+1][j+0]*(-2) 
                    + r[i+1][j+1]*0
                    + r[i+1][j+2]*2
                    
                    + r[i+2][j+0]*(-1) 
                    + r[i+2][j+1]*0
                    + r[i+2][j+2]*1);
            
             gy = (   r[i][j+0]*1 
                    + r[i][j+1]*2
                    + r[i][j+2]*1

                    + r[i+1][j+0]*0 
                    + r[i+1][j+1]*0
                    + r[i+1][j+2]*0
                    
                    + r[i+2][j+0]*(-1) 
                    + r[i+2][j+1]*(-2)
                    + r[i+2][j+2]*(-1));

            conv_r[i][j]= sqrt((gx*gx) + (gy*gy));
        }
    }
 
    for(int i=0;i<width;i++)
    {
        for(int j=0; j<height;j++)
        {
            putc(conv_b[i][j],image);
            putc(conv_g[i][j],image);
            putc(conv_r[i][j],image);
        }
    }
}


int main(){
    FILE *stream;
    stream = fopen("echidna.bmp", "r");
  
    unsigned char header[54];
    unsigned char colortable[1024];
    fread(header,sizeof(unsigned char), 54,stream);
    
    int width  = *(int*)&header[18];
    int height = *(int*)&header[22];
    int bit_depth = *(int*)&header[28];
    printf("width is %d\nheight is %d\nbit depth is %d\n", width,height,bit_depth);

    fread(colortable, sizeof(unsigned char), 1024, stream); //pass 1024 bytes into colortable array( after 54 byte header comes this 1024 colortable bytes
    int pixel_size = bit_depth/8;
    int size = height*width;
    unsigned char bgr_buf[height*width][3];
    for(int i=0; i<size; i++)
    {
        bgr_buf[i][0] = getc(stream);
        bgr_buf[i][1] = getc(stream);
        bgr_buf[i][2] = getc(stream);
    }
    
    FILE *new_img = fopen("bgr_sobel.bmp", "w+");//why w+ ? The difference is that if you select the “w” mode you can only write to this file. The “w+” mode allows you to read from the file as well, remember we're declaring a pointer of type FILE here, and we'll use fseek and read from this pointer on our functions. We could just run this header and colortable directly in functions, but for educational purposes we'll do that way to demonstrate fseek and ftell functions.
    fwrite(header,sizeof(unsigned char), 54, new_img);
    fwrite(colortable,sizeof(unsigned char), 1024, new_img);
    sobelConv2d(new_img,bgr_buf);
    fclose(new_img);
    fclose(stream);
    return 0;
}
