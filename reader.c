#include <stdint.h>
#include <stdlib.h>

#define N (180*55*4)
#define THICK (1)
#define THRESHOLD (100)
uint8_t img[N], tmp[N];
char res[7];
int symbol_n, symbol_dim[36][2];
uint8_t * symbol_img[36];
char symbol_name[36];

void add_symbol(char s, int w, int h, uint8_t * img){
    symbol_dim[symbol_n][0] = w;
    symbol_dim[symbol_n][1] = h;
    //symbol_img[symbol_n] = img;
    symbol_img[symbol_n] = malloc(w*h*4);
    for(int i=w*h*4-1; i>=0; --i)
        symbol_img[symbol_n][i] = img[i];
    symbol_name[symbol_n++] = s;
}

char * read(uint8_t * img){
    for(int i=0; i<N; i+=4){
        int r=i/4/180, c=i/4%180;
        if( r<THICK || c<THICK || r+THICK>=55 || c+THICK>=180 )
            continue;
        switch(
            (img[i-4+1]>=THRESHOLD)*1 +
            (img[i+4+1]>=THRESHOLD)*2 +
            (img[i-4*180+1]>=THRESHOLD)*4 +
            (img[i+4*180+1]>=THRESHOLD)*8
        ){
            case 15: img[i]=img[i+1]=img[i+2] = 255; break;
            case 0: img[i]=img[i+1]=img[i+2] = 0;
        }
    }
    for(int i=0; i<N; i+=4){
        int r=i/4/180, c=i/4%180;
        if( r<THICK || c<THICK || r+THICK>=55 || c+THICK>=180 )
            goto NONE;

        if( img[i+1]>=THRESHOLD && (img[i-4+1]>=THRESHOLD || img[i+4+1]>=THRESHOLD) && (img[i-4*180+1]>=THRESHOLD || img[i+4*180+1]>=THRESHOLD) )
            goto NONE;

        if( img[i-4+1]>=THRESHOLD && img[i+4+1]>=THRESHOLD )
            goto NONE;
        if( img[i-4*180+1]>=THRESHOLD && img[i+4*180+1]>=THRESHOLD )
            goto NONE;

        tmp[i] = tmp[i+1] = tmp[i+2] = 0;
        tmp[i+3] = 255;
        continue;

        NONE:
        tmp[i] = tmp[i+1] = tmp[i+2] = tmp[i+3] = 255;
    }
    for(int i=0; i<N; ++i)
        img[i] = tmp[i];

    for(int i=0; i<6; ++i)
        res[i] = 0;

    int begin = -1;
    int res_p = 0;
    int state = symbol_name[0];
    for(int x=0; x<180; ++x){
        int on = 0;
        for(int y=0; y<55; ++y)
            if( img[(y*180+x)*4]<100 ){
                on = 1;
                break;
            }
        if( on ){
            if( begin<0 )
                begin = x;
        }
        else{
            if( begin>=0 ){
                int end = x;
                int k = (end-begin)/15;
                if( k<1 ) k = 1;
                double kw = (double)(end-begin)/k;
                int fw = (end-begin+k-1)/k;
                int fh = 23;
                for(int i=0; i<k; ++i, ++res_p){
                    double best_score = -9;
                    int best_symbol = -1;
                    for(int s=0; s<symbol_n; ++s){
                        double score = 0;
                        for(int r=15; r<38; ++r){
                            for(int c=begin+i*fw, p=(r-15)*symbol_dim[s][0]*4; c<begin+i*fw+fw/2; ++c, p+=4){
                                double bg = *(uint32_t*)&symbol_img[s][p]%16777216 / 16777215. * 2 - 1;
                                if( img[(r*180+c)*4+1] < 100 )
                                    score += bg;
                                else
                                    score -= bg;
                            }
                            for(int c=begin+(i+1)*fw-1, p=((r-15+1)*symbol_dim[s][0]-1)*4; c>=begin+i*fw+fw/2; --c, p-=4){
                                double bg = *(uint32_t*)&symbol_img[s][p]%16777216 / 16777215. * 2 - 1;
                                if( img[(r*180+c)*4+1] < 100 )
                                    score += bg;
                                else
                                    score -= bg;
                            }
                        }
                        score /= (38-15) * fw;
                        if( score > best_score ){
                            best_score = score;
                            best_symbol = s;
                        }
                    }
                    res[res_p] = symbol_name[best_symbol];
                }
                begin = -1;
            }
        }
    }
    return res;
}
