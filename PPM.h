#ifndef PPM
#define PPM

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <inttypes.h>

    typedef struct Pixel{
        unsigned char red, blue, green;
    }Pixel;

    typedef struct Position{
        uint32_t x, y;
    }Position;

    typedef struct PPM_image{
        char type[3];
        uint32_t width, height;
        uint32_t maxval;
        Pixel **grid;
    }PPM_image;

    PPM_image *PPM_get_image(char *filename);
    void PPM_save_image(PPM_image *img, char *filename);
    void PPM_free_image(PPM_image **img);
    Pixel **generate_grid(uint32_t width, uint32_t height);

#endif