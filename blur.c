#include<stdio.h>
// if you pass 1d array you can pass the pointer directly on parameters, like unsigned char *bgr, if you pass "n" dimension array you need to write is as: type(*var)[n]    
void negativeImage(FILE* image,int size,unsigned char(*bgr)[3]){ 
    
    for(int i=0;i<size;i++)
    {
        putc(255-bgr[i][0], image);
        putc(255-bgr[i][1], image);
        putc(255-bgr[i][2], image);
    }
}

void blurImage(FILE* image,unsigned char(*bgr)[3]){
    
    unsigned char header[54];
    long cur = ftell(image);//store our current position after colortable have been written
    fseek(image,0,SEEK_SET);//sets our cursor in the file to the start. if you print the line above you will see that our cursor is at 1078 (54+1024), we want the height and width so we'll go back to position 0 and take from 0 to 54 to write a header array here.
    fread(header, sizeof(unsigned char), 54, image);
    fseek(image,cur,SEEK_SET);//go back to our position after colortable to write image data
    int width  = *(int*)&header[18];
    int height = *(int*)&header[22];
    int size = width*height;
    
    // blur process: basically a blur (not gaussian blur) is the sum of values in a matrix, since we have a 1d array representing the image here, we'll do the process for a 3x3 matrix with value v, a 3x3 matrix is just 9 values, so we'll sum the values of the current position and the next 8 and write that sum as the pixel in our blurred image. Thats a blurred image in horizontal like, you could just do 2 for loops from 0 to 3 and do like a matrix adding width to the index for each time you reach 3 (because then you'll be looking the next line).
    float v = 1.0/9.0; //blur kernel value
    unsigned char filter_buf[size][3];
    float sum0=0.0,sum1=0.0,sum2=0.0;
    for(int i=0; i<size;i++)
    {
       for(int j=0; j<9; j++)
        {
            sum0 += (float)bgr[i+j][0]*v;
            sum1 += (float)bgr[i+j][1]*v;
            sum2 += (float)bgr[i+j][2]*v;
        }
        filter_buf[i][0]=sum0;
        filter_buf[i][1]=sum1;
        filter_buf[i][2]=sum2;
        sum0=0.0;
        sum1=0.0;
        sum2=0.0; 
    }

    //write that to fix last 8 pixels from row to be summed with next line
        //if (i%716==0)
        //{
         //   for(int j=0; j<9;j++){
                
          //      filter_buf[i-j][0]=bgr[i-j][0];
            //    filter_buf[i-j][1]=bgr[i-j][1];
            //    filter_buf[i-j][2]=bgr[i-j][2];
            //}
        //}
        
    //write on file
    for(int i=0;i<size;i++)
    {
        putc(filter_buf[i][0], image);
        putc(filter_buf[i][1], image);
        putc(filter_buf[i][2], image);
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
    
    FILE *new_img = fopen("blue_echidna.bmp", "w+");//why w+ ? The difference is that if you select the “w” mode you can only write to this file. The “w+” mode allows you to read from the file as well, remember we're declaring a pointer of type FILE here, and we'll use fseek and read from this pointer on our functions. We could just run this header and colortable directly in functions, but for educational purposes we'll do that way to demonstrate fseek and ftell functions.

    fwrite(header,sizeof(unsigned char), 54, new_img);
    fwrite(colortable,sizeof(unsigned char), 1024, new_img);
    blurImage(new_img,bgr_buf);
    fclose(new_img);
    fclose(stream);
    return 0;
}
