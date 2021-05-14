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

I'll write the header and colortable from our new image in main function and leave the rest of the process to a function I'll create called sobelConv2d
```c
    FILE *new_img = fopen("copy.bmp", "wb");
    fwrite(header,sizeof(unsigned char), 54, new_img); //copy header
    fwrite(colortable,sizeof(unsigned char), 1024, new_img); //copy colortable
    sobelConv2d(new_img,bgr_buf);
    fclose(new_img);
    fclose(stream);
```

Now lets write a function that operates this image, we'll do a 2d convolution using the sobel operator over a bgr image. Some people think Sobel operator only works on grayscale image which is not true, you can also do this for bgr as long as you do the operations for each color channel.

I'll write the function as following, but keep in mind this is not the most optimized way, but I did it that way to show other functions you can use:
```c
void sobelConv2d(FILE* image,unsigned char(*bgr)[3])
{

```
We receive as parameter the pointer to the image we're operating, and a pointer to the color channels with pixel data. If you pass 1d array you can pass the pointer directly on parameters, like unsigned char *bgr, if you pass "n" dimension array you need to write is as: __type(*var)[n]__


Now that we're inside of the function, we'll do a few things that might be redundant, but you'll see fseek and ttell.
```c
    unsigned char header[54];
    long cur = ftell(image);
    fseek(image,0,SEEK_SET);
    fread(header, sizeof(unsigned char), 54, image);
    fseek(image,cur,SEEK_SET);
    int width  = *(int*)&header[18];
    int height = *(int*)&header[22];
```
So, again we create a header array, and again we'll read from the stream, but something important is to have notion of controling your pointer position in the stream.
Everytime you use fwrite or putc, you advance your pointer by the 1 in the case of putc and by the amount of bytes you passed in the function for fwrite.
ftell holds your current pointer position, and we just created a long variable to hold this value, now fseek takes our stream and sets the pointer cursor to whatever you want, in this case we go into position 0 to read the first 54 bytes we wrote in main function, which is our bmp header. After reading we set the pointer back to the position it was after we wrote for the last time, which was 1078(54 from header + 1024 from colortable). Now we can start to process the image data.

## Sobel : Approximate Component Derivatives

The sobel filter is used as an edge detector, to give you an intuition of its work, let's think what is an edge in an image?
Let's take this image for example:

![image](https://user-images.githubusercontent.com/56324869/118332473-86ab9d80-b4e0-11eb-80b9-95da77819008.png)

If you think of this without the colors only the lines (i.e. the edges) you would think in something ike this:

![image](https://user-images.githubusercontent.com/56324869/118332708-e3a75380-b4e0-11eb-81c6-8bccd1ca5a4e.png)

Which is pretty much just painting where the colors change, in this case you just go where the black turns into white and draw a line that separetes them.

That's what Sobel does, more specifically, in mathematics you have a tool to calculate differences, its called derivatives, calcutaling a derivative is pretty much calculating how much something changed from a point 1 to a point 2, in this case, how much colors are chanching from one pixel to another one, are they the same, or are they completely the opposite?

Sobel does that using the idea of convolution, and if you have never heard of it, its pretty simple.

Lets take an example:

![full_padding_no_strides_transposed](https://user-images.githubusercontent.com/56324869/118333250-c1fa9c00-b4e1-11eb-99ac-d221e285976a.gif)

Here, the blue pixels are our image, the green pixels are our filtered image after some process, and the 3x3 shadow you see going through our image, is called the Kernel.
For our output image (green) we look to the original images in blocks of 3x3, and do something with this 9 pixels, now what Sobel do with that?

## Sobel Kernel
![image](https://user-images.githubusercontent.com/56324869/118333675-7e546200-b4e2-11eb-9cbc-32a0300dbb98.png)

This is one of Sobel kernels, and you wil understand why we have more than one. What we do with these 9 numbers is that we'll hover them  in a 3x3 area in our image, like the animation above. Now we take element by element and multiply them with the values inside of Gx, like you can see, the area in the middle will be multiplied by zero, while left side will be negative, and right side will be positive.

- Important
These values 1,2 and 1 come from a little gaussian filter while detecting edges. Since we have like 255 tons to represent the color it would be kind of a mess if we take the image directly, cause everything can be considered different in this scenario, this gaussian filter blurs the image a little bit, which spread the pixel colors to its neighbours a bit .

Now that you have this values just sum all of them, and you have the difference between the left side and the right side of this 3x3 block, if they are the same we're just making a sum of a positive value with the same value as negative, resulting in 0, which means no edge in that block. As you can see, this analyses differences in the pixels horizontally only, we take diferences from left and right and do that for the entire image. But you know, we could also see edges vertically right? Well that why we also have a Gy Kernel that does the same job for us, but vertically:

![image](https://user-images.githubusercontent.com/56324869/118334477-1272f900-b4e4-11eb-9dfb-2d7d2e2e2173.png)

Ok that's good, now let's write this, first I'll transform the arrays into matrices so it becomes easier to operate and understand:
```c
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
```
Now that we have the b,g and r matrices, lets do the job, but as you saw in the animation, our output image doesn't have the same size as the input, and thats a convolution property, you normally calculate the output size as 

output = (input_size - filter_size + 1) / stride

where in this example, my bmp image is 717x717, my filter size is 3x3 and stride is how long you move the 3x3 shadow over the image, which we are doing one position, thus 1.

output = 717 - 3 + 1 /1

output = 715

Our new image will be 715x715, and you can just have an intuition from where this formula comes looking to the animation.

Since our new image will be 715x715, create proper matrices with this size:

```c
    int gx=0,gy=0; //gradients
    unsigned char conv_b[width-2][height-2], conv_g[width-2][height-2], conv_r[width-2][height-2];
```

And now write the 3x3 calculation over the entire image, for each color channel:
```c
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
```
Vertical and horizontal derivatives represents intensity, thus they are components of a vector in 2d space, the derivative is calculated as the module of that vector.

```c
conv_b[i][j]= sqrt((gx*gx) + (gy*gy));
```

We don't really care about vector direction since it doesnt really matter for us if colors are getting brighter or darker, we just want to draw a line where they change.

Repeat the process for green and red:
```c
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
    }//close for loops
```

And write it on our new image:
```c
for(int i=0;i<width;i++)
    {
        for(int j=0; j<height;j++)
        {
            putc(conv_b[i][j],image);
            putc(conv_g[i][j],image);
            putc(conv_r[i][j],image);
        }
    }
}//close void function
```
Result:

![image](https://user-images.githubusercontent.com/56324869/118221775-aa290680-b454-11eb-990e-7bff005d9b8d.png)

You will see that the images has some borders, and that's because in order to change the image resolution you would need to rewrite the image header by yourself, so its just easier to keep the 717x717 and just accept the 2 pixels in the border.

Hopefully you learned something from that, any questions feel free to ask :)
