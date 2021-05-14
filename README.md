# Image-processing
Image processing from C scratch for educational purposes of digital image processing and pointers.


Compile and run with
```
gcc sobelconv2d.c -o sobelconv2d -lm
./sobelconv2d
```

# BGR 2D Convolution with 3x3 Sobel kernel
![image](https://user-images.githubusercontent.com/56324869/118221775-aa290680-b454-11eb-990e-7bff005d9b8d.png)

Ever thought about what Convolutional Neural Networks do?
Well I can show you a little bit of the process of convolution, keep in mind other image processing is also done like Maxpooling, averagepooling, ReLu on pixels etc...

## Process

First of all we'll load an  image, I'll be using bmp images because its header is quite simple.

First we'll create a pointer of type FILE, this is commonly called stream in C, you open files and you are responsable of writing properly here.
```c
int main(){
    FILE *stream;
    stream = fopen("echidna.bmp", "r");
}
```
.bmp works pretty simple:
- 54 header bytes
- 1024 colortable bytes
- image pixels
### Header structure
![image](https://user-images.githubusercontent.com/56324869/118219653-4f8dab80-b450-11eb-884f-24fe1a70c657.png)

From that, some information is important while we operate the image, which is **height**, **width** and **bitdepth**, we'll create an array to store this header and proper variables to hold this values
```c
    unsigned char header[54];
    fread(header,sizeof(unsigned char), 54,stream);
    int width  = *(int*)&header[18];
    int height = *(int*)&header[22];
    int bit_depth = *(int*)&header[28];
```
What happens here is that we create an unsigned char array of lenth 54 to hold the header, and use the function fread to pass 54 bytes to our stream, we specify sizeof(unsigned char) to take 1 byte per time, since unsigned char holds 1 byte.
Next we do 
```c
*(int*)&header[18]
```
and you should read that backwards like: we go into the memory location where header position 18 is stored, that means we're looking to a pointer (& operator), we'll cast that unsigned char pointer into a int pointer, because int holds 4 bytes, and as you can see in the table, width height and bit_depth are 4 bytes, and then we dereference the pointer (* operator) to get its 4 byte value. And we do that for height and bit_depth same way.
### Important:
Bmp files are little endian, so if you are debugging that to learn, you should read the 4 bytes backwards. Little Endian means the least significant bit is at our left when reading, and most significant on the right.

Next, we'll read the colortable, that's pretty simple and we don't hold any information from that
```c
unsigned char colortable[1024];
fread(colortable, sizeof(unsigned char), 1024, stream);
```
The bit depth is important because it holds how big each pixel expect to be, if you want a RGB image, each pixel will be 3 bytes long, 1 byte for each color channel, also since is little endian file, the right thing to say is BGR, and it expects the colors in that order. If you work with a grayscale image then bit depth will be 1 byte. In this case, bit depth is 24, and to get a way of knowing how many bytes bmp file is expecting we do:
```c
int pixel_size = bit_depth/8;
```
If grayscale 8/8 is 1 byte, if BGR 24/8 is 3 bytes.

Now let's store image data in an array, more precisely, a 2d matrix that will hold each color channel
```c
    int size = height*width;
    unsigned char bgr_buf[height*width][3];
    for(int i=0; i<size; i++)
    {
        bgr_buf[i][0] = getc(stream);
        bgr_buf[i][1] = getc(stream);
        bgr_buf[i][2] = getc(stream);
    }
```
We use the function getc, that gets a byte, and store each of in an 1d array, you can do this a lot of different ways, here in order to show other processes later I did 3 1d arrays in a 2d matrix, [0] holds the blue array, [1] for green and [2] for red.


Now we have a copy of the original image in our C code, now time to do something with that. Just in case, if you want to test if thats right you can save a copy of this image with:
```c
    unsigned char buf[height * width* pixel_size];
    FILE *new_img = fopen("copy.bmp", "wb");
    fwrite(header,sizeof(unsigned char), 54, new_img); //copy header
    fwrite(colortable,sizeof(unsigned char), 1024, new_img); //copy colortable
    fwrite(buf,sizeof(unsigned char), (height*width*pixel_size),new_img);//copy image data
    fclose(new_img); //close your streams
    fclose(stream);
```

Now lets write a function that operates this image, we'll do a 2d convolution using the sobel operator over a bgr image. Some people think Sobel operator only works on grayscale image which is not true, you can also do this for bgr as long as you do the operations for each color channel.

I'll write the function as following, but keep in mind this is not the most optimized way, but I did it that way to show other functions you can use:
```c
void sobelConv2d(FILE* image,unsigned char(*bgr)[3])
{

}
```
We receive as parameter the pointer to the image we're operating, and a pointer to the color channels with pixel data. If you pass 1d array you can pass the pointer directly on parameters, like unsigned char *bgr, if you pass "n" dimension array you need to write is as: __type(*var)[n]__



