#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>
#include <omp.h>

#define N 3
#define NTHREADS 3

typedef struct Image{
    int width;
    int height;
    int bitDepth;
    unsigned char header[54];
    unsigned char colorTable[1024];
}IMAGE;

int sizeImage(FILE * fpi,char * image){
    fpi = fopen(image,"rb");
    if(!fpi){
        printf("\nERRO AO ABRIR O ARQUIVO : %s\n",image);
        fclose(fpi);
        exit(0);
    }
    unsigned char tempHeader[54];
    fread(tempHeader,sizeof(unsigned char),54,fpi);

    int w = *(int *)&tempHeader[18],h = *(int *)&tempHeader[22];
    int size = w*h;
    fclose(fpi);

    return size;
}

void readImage(FILE * fpi,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    fpi = fopen(image,"rb");
    if(!fpi){
        printf("\nERRO AO ABRIR O ARQUIVO : %s\n",image);
        fclose(fpi);
        exit(0);
    }
    fread(IMG->header,sizeof(unsigned char),54,fpi);

    IMG->width = *(int *)&IMG->header[18];
    IMG->height = *(int *)&IMG->header[22];
    IMG->bitDepth = *(int *)&IMG->header[28];

    int size = IMG->height*IMG->width;

    if(IMG->bitDepth <= 8)
        fread(IMG->colorTable,sizeof(unsigned char),1024,fpi);     //Color Table if present

    int i;
    fread(buffer,sizeof(unsigned char),size*3,fpi);
    fclose(fpi);
}

void writeImage(FILE * fpo,char * image,IMAGE * IMG,unsigned char buffer[][3]){
    fpo = fopen(image,"wb");

    fwrite(IMG->header,sizeof(unsigned char),54,fpo);

    if(IMG->bitDepth <= 8)
        fwrite(IMG->colorTable,sizeof(unsigned char),1024,fpo);

    int size = IMG->height*IMG->width;
    int i;

    fwrite(buffer,sizeof(unsigned char),3*size,fpo);
    fclose(fpo);
}

int filterImage(char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int x,y,i,j,k,n,sum[3],ch,z, id;

    n = N;

    int a[(n+2) * (n+2)][3];
    float prod[3],b[(n+2) * (n+2)][3];

    for(y=n/2;y<IMG->height-n/2;y++){
        for(x=n/2;x<IMG->width-n/2;x++){

            omp_set_num_threads(NTHREADS);

            #pragma omp parallel private(id)
            {
                id = omp_get_thread_num();
                sum[id] = 0;
            }
            
            for(j=-n/2;j<=n/2;j++){
                for(i=-n/2;i<=n/2;i++){

                    omp_set_num_threads(NTHREADS);

                    #pragma omp parallel private(id)
                    {
                        id = omp_get_thread_num();

                        sum[id] += buffer[(y+j)*IMG->width + (x+i)][id];
                    }
                }
            }

            omp_set_num_threads(NTHREADS);

            #pragma omp parallel private(id)
            {
                id = omp_get_thread_num();
                buffer[y*IMG->width + x][id] = sum[id]/n/n;
            }
        }
     
    }
    
    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);

    return 1;
}

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

    (filterImage(imageOut,&IMG,buffer)) ? printf("\n...CONCLUIDO...\n") : printf("\n...ERRO...\n");

    return 0;
}

