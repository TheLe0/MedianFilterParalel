#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>
#include "image.h"

int gcd(int a, int b){
        while(a!=b)
    {
        if(a > b)
            a -= b;
        else
            b -= a;
    }
    return a;
}

int lcm(int a,int b){
    return (a*b/gcd(a, b));
}

int main(){
    setlocale(LC_ALL,"Portuguese");
    IMAGE IMG;
    FILE * fpi;

    char imageIn[] = {"img.bmp"};
    char imageOut[] = {"new.bmp"};

    int size = sizeImage(fpi,imageIn);
    unsigned char buffer[size][3];

    restore:
    readImage(fpi,imageIn,&IMG,buffer);

    FILE * fpo;
    writeImage(fpo,imageOut,&IMG,buffer);

    int r,c,choice,factor,a,flag = 0;
    char horv,* image;

    while(1){
        printf("\n****** SELECIONE UMA OPCAO ******\n\n");
        printf(" 1. Filtrar\n");
        printf(" 0. SAIR\n");
        int ch;
        scanf("%d",&ch);
        printf("\n******************************\n");
        switch(ch){
        case 1:
            flag = filterImage(imageOut,&IMG,buffer);
            break;
        case 0:
            exit(0);
        default:
            printf("\nINVALIDO !!");
        }
        if(flag)
            printf("\n...CONCLUIDO...");
        getche();
        flag = 0;
        system("cls");
    }
    return 0;
}

