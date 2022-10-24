#include "quadtree.h"

int main(int argc, char **argv){
    if(strcmp(argv[1], "-c") == 0){
        PPM_image *img = PPM_get_image(argv[3]);
        if(img != NULL){
            CompressedQT *compQt = CompQT_create(img, atoi(argv[2]));
            if(compQt != NULL){
                CompQT_save(compQt, argv[4]);
                CompQT_free(&compQt);
            }
            PPM_free_image(&img);
        }
    }
    if(strcmp(argv[1], "-d") == 0){
        CompressedQT *compQt = CompQT_get_image(argv[2]);
        CompQT_decompress(compQt, argv[3]);
        if(compQt != NULL)
            CompQT_free(&compQt);
    }
    if(strcmp(argv[1], "-m") == 0){
        if(strcmp(argv[2], "h") == 0){
            PPM_image *img = PPM_get_image(argv[4]);
            if(img != NULL){
                Mirror_h(img, atoi(argv[3]));
                PPM_save_image(img, argv[5]);
                PPM_free_image(&img);
            }
        }
        if(strcmp(argv[2], "v") == 0){
            PPM_image *img = PPM_get_image(argv[4]);
            if(img != NULL){
                Mirror_v(img, atoi(argv[3]));
                PPM_save_image(img, argv[5]);
                PPM_free_image(&img);
            }
        }
    }

    return 0;
}