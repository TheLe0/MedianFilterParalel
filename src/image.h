#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

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

/* void colorToGray(char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int i,y;
    if(IMG->bitDepth <= 8){

    }
    else{
        FILE * fpo = fopen(image,"wb");
        fwrite(IMG->header,sizeof(unsigned char),54,fpo);
        int size = IMG->height*IMG->width;
        if(IMG->bitDepth <= 8)
            fwrite(IMG->colorTable,sizeof(unsigned char),1024,fpo);
        for(i=0;i<size;i++){
            y = buffer[i][0]*0.3 + buffer[i][1]*0.59 + buffer[i][2]*0.11;
            buffer[i][0] = buffer[i][1] = buffer[i][2] = y;
            fputc(y,fpo);
            fputc(y,fpo);
            fputc(y,fpo);
        }
        fclose(fpo);
    }
} */

int filterImage(char * image,IMAGE * IMG,unsigned char buffer[][3]){
    int x,y,i,j,k,n,sum[3],ch,z;
    n = 5;
    int a[n*n][3];
    float prod[3],b[n*n][3];

    printf(" FILTER #1 : Blur\n FILTER #2 : Motion Blur\n FILTER #3 : Sharpen\nEnter choice : ");
    scanf("%d",&ch);

    if(ch == 1){
        for(y=n/2;y<IMG->height-n/2;y++){
            for(x=n/2;x<IMG->width-n/2;x++){
                for(k=0;k<3;k++)
                    sum[k] = 0;
                for(j=-n/2;j<=n/2;j++){
                    for(i=-n/2;i<=n/2;i++){
                        for(k=0;k<3;k++){
                            sum[k] += buffer[(y+j)*IMG->width + (x+i)][k];
                        }
                    }
                }
                for(k=0;k<3;k++){
                    buffer[y*IMG->width + x][k] = sum[k]/n/n;
                }
            }
        }
    }
    else if(ch == 2){
        int p = 9;

        double filter[9][9] =
        {
            1, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 1, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 1,
        };
        double factor = 1.0/9.0;
        double bias = 0.0;

        long w = IMG->height, h = IMG->width;
        int filterX,filterY;
        for(int x = 0; x < w; x++)
            for(int y = 0; y < h; y++){

                double red = 0.0, green = 0.0, blue = 0.0;

                for(filterY = 0; filterY < p; filterY++)
                    for(filterX = 0; filterX < p; filterX++)
                    {

                        int imageX = (x - p/2 + filterX + w) % w;
                        int imageY = (y - p/2 + filterY + h) % h;

                        red += buffer[imageY * w + imageX][0] * filter[filterY][filterX];
                        green += buffer[imageY * w + imageX][1] * filter[filterY][filterX];
                        blue += buffer[imageY * w + imageX][2] * filter[filterY][filterX];
                    }
                    if(fabs(factor*red + bias)>255){
                        buffer[y * w + x][0] = 255;
                    }else{
                        buffer[y * w + x][0] = fabs(factor*red + bias);
                    }
                    if(fabs(factor*green + bias)>255){
                        buffer[y * w + x][1] = 255;
                    }else{
                        buffer[y * w + x][1] = fabs(factor*green + bias);
                    }
                    if(fabs(factor*blue + bias)>255){
                        buffer[y * w + x][2] = 255;
                    }else{
                        buffer[y * w + x][2] = fabs(factor*blue + bias);
                    }
            }
    }else if(ch == 3){
        int p = 5;

        double filter[5][5] =
        {
            -1, -1, -1, -1, -1,
            -1,  2,  2,  2, -1,
            -1,  2,  8,  2, -1,
            -1,  2,  2,  2, -1,
            -1, -1, -1, -1, -1,
        };
        double factor = 1.0/8.0;
        double bias = 0.0;

        long w = IMG->height, h = IMG->width;
        int filterX,filterY;
        for(int x = 0; x < w; x++)
            for(int y = 0; y < h; y++){

                double red = 0.0, green = 0.0, blue = 0.0;

                for(filterY = 0; filterY < p; filterY++)
                    for(filterX = 0; filterX < p; filterX++)
                    {

                        int imageX = (x - p/2 + filterX + w) % w;
                        int imageY = (y - p/2 + filterY + h) % h;

                        red += buffer[imageY * w + imageX][0] * filter[filterY][filterX];
                        green += buffer[imageY * w + imageX][1] * filter[filterY][filterX];
                        blue += buffer[imageY * w + imageX][2] * filter[filterY][filterX];
                    }
                    if(fabs(factor*red + bias)>255){
                        buffer[y * w + x][0] = 255;
                    }else{
                        buffer[y * w + x][0] = fabs(factor*red + bias);
                    }
                    if(fabs(factor*green + bias)>255){
                        buffer[y * w + x][1] = 255;
                    }else{
                        buffer[y * w + x][1] = fabs(factor*green + bias);
                    }
                    if(fabs(factor*blue + bias)>255){
                        buffer[y * w + x][2] = 255;
                    }else{
                        buffer[y * w + x][2] = fabs(factor*blue + bias);
                    }
            }
    }else{
        printf("OPCAO INVALIDA!");
        return 0;
    }
    FILE * fpo;
    writeImage(fpo,image,IMG,buffer);
    return 1;
}

#endif // IMAGE_H_INCLUDED

