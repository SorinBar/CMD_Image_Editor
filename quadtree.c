#include "quadtree.h"

/* COMPRESS */
// Return new dynamic quadtree node with given starting position
// (top left corner position), width and height
DQTNode *new_tree_node(uint32_t x, uint32_t y, uint32_t width, uint32_t height){
    DQTNode *root = (DQTNode*)malloc(sizeof(DQTNode));
    if(root == NULL)
        return NULL;
    root->start.x = x;
    root->start.y = y;
    root->width = width;
    root->height = height;
    root->top_left = NULL;
    root->top_right = NULL;
    root->bottom_right = NULL;
    root->bottom_left = NULL;
}
// Initialize root based on given image
void DQT_init(DQTNode **root, PPM_image *img){
    *root = new_tree_node(0, 0, img->width, img->height);
}
// Return pixel average data from given area
Pixel get_average(Position start, uint32_t width, uint32_t height, Pixel **grid){
    Pixel avg;
    uint64_t red, green, blue;
    int i, j;
    uint32_t max_x, max_y;
    red = 0;
    green = 0;
    blue = 0;
    max_x = start.x + width;
    max_y = start.y + height;
    for(i = start.y; i < max_y; i++)
        for(j = start.x; j < max_x; j++){
            red += grid[i][j].red;
            green += grid[i][j].green;
            blue += grid[i][j].blue;
        }
    red /= width * height;
    green /= width * height;
    blue /= width * height;

    avg.red = (unsigned char)red;
    avg.green = (unsigned char)green;
    avg.blue = (unsigned char)blue;
    
    return avg;
}
// Return mean of a given area
uint64_t get_mean(Position start, uint32_t width, uint32_t height, Pixel **grid, Pixel avg){
    uint64_t mean;
    uint32_t max_x, max_y;
    int i, j;
    mean = 0;
    max_x = start.x + width;
    max_y = start.y + height;
    for(i = start.y; i < max_y; i++)
        for(j = start.x; j < max_x; j++){
            mean += (avg.red - grid[i][j].red) * (avg.red - grid[i][j].red);
            mean += (avg.green - grid[i][j].green) * (avg.green - grid[i][j].green);
            mean += (avg.blue - grid[i][j].blue) * (avg.blue - grid[i][j].blue);
        }
    mean /= (3 * width * height);
    return mean;
}
// Create the image tree
DQTNode *DQT_create_tree(DQTNode *root, uint32_t maxError, Pixel **grid){
    Pixel avg = get_average(root->start, root->width, root->height, grid);
    uint64_t mean = get_mean(root->start, root->width, root->height, grid, avg);
    root->color = avg;
    if(mean <= maxError){
        root->top_left = NULL;
        root->top_right = NULL;
        root->bottom_left = NULL;
        root->bottom_right = NULL;
        return root;
    }
    uint32_t new_width = root->width / 2;
    uint32_t new_height = root->height / 2;
    root->top_left = new_tree_node(root->start.x, root->start.y, new_width, new_height);
    root->top_left = DQT_create_tree(root->top_left, maxError, grid);
    root->top_right = new_tree_node(root->start.x + new_width, root->start.y, new_width, new_height);
    root->top_right = DQT_create_tree(root->top_right, maxError, grid);
    root->bottom_right = new_tree_node(root->start.x + new_width, root->start.y + new_height, new_width, new_height);
    root->bottom_right = DQT_create_tree(root->bottom_right, maxError, grid);
    root->bottom_left = new_tree_node(root->start.x, root->start.y + new_height, new_width, new_height);
    root->bottom_left = DQT_create_tree(root->bottom_left, maxError, grid);

    return root;
}
// Count the number of colors and nodes in the quadtree
void DQT_count(DQTNode *root, uint32_t *colors, uint32_t *nodes){
    if(root->top_left == NULL && root->top_right == NULL &&
       root->bottom_right == NULL && root->bottom_left == NULL){
        (*colors)++;
        (*nodes)++;
        return;     
    }
    (*nodes)++;
    DQT_count(root->top_left, colors, nodes);
    DQT_count(root->top_right, colors, nodes);
    DQT_count(root->bottom_right, colors, nodes);
    DQT_count(root->bottom_left, colors, nodes);
}
// Free dynamic quadtree
DQTNode *DQT_free(DQTNode *root){
    if(root == NULL)
        return NULL;
    root->top_left = DQT_free(root->top_left);
    root->top_right = DQT_free(root->top_right);
    root->bottom_right = DQT_free(root->bottom_right);
    root->bottom_left = DQT_free(root->bottom_left);
    free(root);
    return NULL;
}
// Create new item for the compression array
QuadtreeNode new_array_item(Pixel color, uint32_t width, uint32_t height){
    QuadtreeNode item;
    item.area = width * height;
    item.red = color.red;
    item.green = color.green;
    item.blue = color.blue;
    item.top_left = -1;
    item.top_right = -1;
    item.bottom_right = -1;
    item.bottom_left = -1;
    return item;
}
// Fill the array with items based on generated tree
void fill_array(DQTNode *root, QuadtreeNode *array, int fixedIndex, int *index){
    if(root->top_left == NULL && root->top_right == NULL &&
       root->bottom_right == NULL && root->bottom_left == NULL){
        array[*index] = new_array_item(root->color, root->width, root->height);
        (*index)++;
        return;     
    }
    fixedIndex = *index;
    array[*index] = new_array_item(root->color, root->width, root->height);
    (*index)++;
    array[fixedIndex].top_left = *index;
    fill_array(root->top_left, array, fixedIndex, index);
    array[fixedIndex].top_right = *index;
    fill_array(root->top_right, array, fixedIndex, index);
    array[fixedIndex].bottom_right = *index;
    fill_array(root->bottom_right, array, fixedIndex, index);
    array[fixedIndex].bottom_left = *index;
    fill_array(root->bottom_left, array, fixedIndex,  index);
}
// Generate vector for compressed structure
QuadtreeNode *create_array(DQTNode *root, uint32_t nodes){
    QuadtreeNode *array = (QuadtreeNode*)malloc(nodes * sizeof(QuadtreeNode));
    int index = 0;
    fill_array(root, array, 0, &index);
    return array;
}
// Create compressed structure
CompressedQT *CompQT_create(PPM_image *img, u_int32_t maxErr){
    CompressedQT *compQt;
    compQt = (CompressedQT*)malloc(sizeof(CompressedQT));
    if(compQt == NULL)
        return NULL;
    DQTNode *root;
    uint32_t colors, nodes;
    QuadtreeNode *array;
    colors = 0;
    nodes = 0;
    DQT_init(&root, img);
    root = DQT_create_tree(root, maxErr, img->grid);
    DQT_count(root, &colors, &nodes);
    array = create_array(root, nodes);
    compQt->colors = colors;
    compQt->nodes = nodes;
    compQt->array = array;
    // Free memory
    root = DQT_free(root);
    return compQt;
}
// Save compressed quadtree as binary file
void CompQT_save(CompressedQT *compQt, char *filename){
    FILE *write = fopen(filename, "wb");
    fwrite(&(compQt->colors), sizeof(uint32_t), 1, write);
    fwrite(&(compQt->nodes), sizeof(uint32_t), 1, write);
    fwrite(compQt->array, sizeof(QuadtreeNode), compQt->nodes, write);
    fclose(write);
}
// Free compressed quadtree
void CompQT_free(CompressedQT **compQt){
    free((*compQt)->array);
    free(*compQt);
}

/* DECOMPRESS */
// Read compressed image data from a file with the given name (filename)
// and return CompressedQT struct that contain the data
CompressedQT *CompQT_get_image(char *filename){
    FILE *read = fopen(filename, "rb");
    if(read == NULL)
        return NULL;
    CompressedQT *compQt = (CompressedQT*)malloc(sizeof(CompressedQT));
    uint32_t colors, nodes;
    fread(&colors, sizeof(uint32_t), 1, read);
    fread(&nodes, sizeof(uint32_t), 1, read);
    compQt->colors = colors;
    compQt->nodes = nodes;
    compQt->array = (QuadtreeNode*)malloc(nodes * sizeof(QuadtreeNode));
    fread(compQt->array, sizeof(QuadtreeNode), nodes, read);
    // Free memory
    fclose(read);
    return compQt;
}
// Square root of type int
u_int32_t int_sqrt(u_int32_t x){
    u_int32_t res = 0;
    while(res * res < x)
        res++;
    if(res * res > x)
        res--;
    return res;
}
// Decompress quadtree
DQTNode *DQT_decompress_tree(DQTNode *root, u_int32_t x, u_int32_t y, CompressedQT *compQt, int index){
    if(index == -1)
        return NULL;
    u_int32_t width, height;
    width = int_sqrt(compQt->array[index].area);
    height = width;
    root = new_tree_node(x, y, width, height);
    root->color.red = compQt->array[index].red;
    root->color.green = compQt->array[index].green;
    root->color.blue = compQt->array[index].blue;
    root->top_left = 
    DQT_decompress_tree(root->top_left, x, y, compQt, compQt->array[index].top_left);
    root->top_right = 
    DQT_decompress_tree(root->top_right, x + width / 2, y, compQt, compQt->array[index].top_right);
    root->bottom_right = 
    DQT_decompress_tree(root->bottom_right, x + width / 2, y + height / 2, compQt, compQt->array[index].bottom_right);
    root->bottom_left = 
    DQT_decompress_tree(root->bottom_left, x, y + height / 2, compQt, compQt->array[index].bottom_left);

    return root;
}
// Generate grid based on quadtree
void DQT_gen_grid(DQTNode *root, Pixel **grid){
    if(root->top_left == NULL && root->top_right == NULL &&
       root->bottom_right == NULL && root->bottom_left == NULL){
        int i, j;
        u_int32_t max_x, max_y;
        max_x = root->start.x + root->width;
        max_y = root->start.y + root->height;
        for(i = root->start.y; i < max_y; i++)
            for(j = root->start.x; j < max_x; j++){
                grid[i][j].red = root->color.red;
                grid[i][j].green = root->color.green;
                grid[i][j].blue = root->color.blue;
            }
        return;
    }
    DQT_gen_grid(root->top_left, grid);
    DQT_gen_grid(root->top_right, grid);
    DQT_gen_grid(root->bottom_right, grid);
    DQT_gen_grid(root->bottom_left, grid);
}
// Decompress image saved in CompressedQT stucture into a file with
// given name
void CompQT_decompress(CompressedQT *compQt, char *filename){
    if(compQt == NULL)
        return ;
    FILE *write = fopen(filename, "wb");
    PPM_image *img = (PPM_image*)malloc(sizeof(PPM_image));
    strcpy(img->type, "P6");
    img->maxval = 255;
    img->width = int_sqrt(compQt->array[0].area);
    img->height = img->width;
    img->grid = generate_grid(img->width, img->height);
    // Generate grid
    DQTNode *root = DQT_decompress_tree(root, 0, 0, compQt, 0);
    DQT_gen_grid(root, img->grid);
    // Save image
    PPM_save_image(img, filename);
    // Free memory
    root = DQT_free(root);
    PPM_free_image(&img);
    fclose(write);
}

/* Mirror */
void gen_grid_mirror(DQTNode *root, Pixel **grid, uint32_t x, uint32_t y, u_int32_t w_h){
    if(root->top_left == NULL && root->top_right == NULL &&
       root->bottom_right == NULL && root->bottom_left == NULL){
        int i, j;
        u_int32_t max_x, max_y;
        max_x = x + w_h;
        max_y = y + w_h;
        for(i = y; i < max_y; i++)
            for(j = x; j < max_x; j++){
                grid[i][j].red = root->color.red;
                grid[i][j].green = root->color.green;
                grid[i][j].blue = root->color.blue;
            }
        return;
    }
    uint32_t new_w_h = w_h / 2;
    gen_grid_mirror(root->top_left, grid, x, y, new_w_h);
    gen_grid_mirror(root->top_right, grid, x + new_w_h, y, new_w_h);
    gen_grid_mirror(root->bottom_right, grid, x + new_w_h, y + new_w_h, new_w_h);
    gen_grid_mirror(root->bottom_left, grid, x, y + new_w_h, new_w_h);
}
// Swap top left node with top right node and
// bottom left node with bottom right node
void DQT_mirror_h(DQTNode *root){
    if(root->top_left == NULL && root->top_right == NULL &&
       root->bottom_right == NULL && root->bottom_left == NULL)
        return;

    DQTNode *nodeAux;
    nodeAux = root->top_left;
    root->top_left = root->top_right;
    root->top_right= nodeAux;

    nodeAux = root->bottom_left;
    root->bottom_left = root->bottom_right;
    root->bottom_right = nodeAux;

    DQT_mirror_h(root->top_left);
    DQT_mirror_h(root->top_right);
    DQT_mirror_h(root->bottom_right);
    DQT_mirror_h(root->bottom_left);
}
// Mirror PPM image horizontally
void Mirror_h(PPM_image *img, u_int32_t maxErr){
    DQTNode *root;
    DQT_init(&root, img);
    root = DQT_create_tree(root, maxErr, img->grid);
    DQT_mirror_h(root);
    gen_grid_mirror(root, img->grid, 0, 0, root->width);
    //Free memory
    root = DQT_free(root);
}
// Swap top left node with bottom left node and
// top right node with bottom right node
void DQT_mirror_v(DQTNode *root){
    if(root->top_left == NULL && root->top_right == NULL &&
       root->bottom_right == NULL && root->bottom_left == NULL)
        return;

    DQTNode *nodeAux;
    nodeAux = root->top_left;
    root->top_left = root->bottom_left;
    root->bottom_left= nodeAux;

    nodeAux = root->top_right;
    root->top_right = root->bottom_right;
    root->bottom_right = nodeAux;

    DQT_mirror_v(root->top_left);
    DQT_mirror_v(root->top_right);
    DQT_mirror_v(root->bottom_right);
    DQT_mirror_v(root->bottom_left);
}
// Mirror PPM image vertically
void Mirror_v(PPM_image *img, u_int32_t maxErr){
    DQTNode *root;
    DQT_init(&root, img);
    root = DQT_create_tree(root, maxErr, img->grid);
    DQT_mirror_v(root);
    gen_grid_mirror(root, img->grid, 0, 0, root->width);
    //Free memory
    root = DQT_free(root);
}