#ifndef QUADTREE
#define QUADTREE
    #include "PPM.h"
    
    typedef struct QuadtreeNode{
        unsigned char blue, green, red;
        uint32_t area;
        int32_t top_left, top_right;
        int32_t bottom_left, bottom_right;
    }__attribute__((packed)) QuadtreeNode;

    typedef struct DQTNode{
        Position start; // Top Left corner position
        Pixel color;
        uint32_t width, height;
        struct DQTNode *top_left, *top_right;
        struct DQTNode *bottom_left, *bottom_right;
    }DQTNode;

    typedef struct CompressedQT{
        uint32_t colors, nodes;
        QuadtreeNode *array;
    }CompressedQT;

    CompressedQT *CompQT_create(PPM_image *img, u_int32_t maxErr);
    void CompQT_save(CompressedQT *compQT, char *filename);
    void CompQT_free(CompressedQT **compQt);
    CompressedQT *CompQT_get_image(char *filename);
    void CompQT_decompress(CompressedQT *compQt, char *filename);
    void Mirror_h(PPM_image *img, u_int32_t maxErr);
    void Mirror_v(PPM_image *img, u_int32_t maxErr);

#endif