#include<stdio.h>
#include<stdlib.h>

void soma(int *a, int b);


int main()
{
  int x=5;
  int y=10;
  soma(&x,y);
  printf("novo valor de x : %d", x);
  return 0;
}

void soma(int *a, int b)
{
    printf("%d", *a);
    *a = b + *a; //changes the value on "a" address, *a is 5, and is stored somewhere right? right! its stored on a, which is the pointer passed, thus the value will change for x on main function, because they are stored on the same place
}

