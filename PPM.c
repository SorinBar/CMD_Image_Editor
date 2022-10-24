#include "PPM.h"
// Return adress of a Pixel matrix (width x height)
Pixel **generate_grid(uint32_t width, uint32_t height){
    int i;
    Pixel **grid = (Pixel**)malloc(height * sizeof(Pixel*));
    for(i = 0; i < height; i++)
        grid[i] = (Pixel*)malloc(width * sizeof(Pixel));
    return grid;
}
// Read PPM image data from a file with the given name (filename)
// and return PPM_image struct that contain the data
PPM_image *PPM_get_image(char *filename){
    FILE *read = fopen(filename, "rb");
    if(read == NULL)
        return NULL;

    PPM_image *img = (PPM_image*)malloc(sizeof(PPM_image));
    char buffer[10];
    int i, j;

    // Read type
    fread(img->type, sizeof(char), 3, read);
    img->type[2] = '\0';
    // Read width
    i = 0;
    while(1){
        fread(&(buffer[i]), sizeof(char), 1, read);
        if(!isdigit(buffer[i]))
            break;
        i++;
    }
    i++;
    buffer[i] = '\0';
    img->width = (unsigned int)atoi(buffer);
    // Read height
    i = 0;
    while(1){
        fread(&(buffer[i]), sizeof(char), 1, read);
        if(!isdigit(buffer[i]))
            break;
        i++;
    }
    i++;
    buffer[i] = '\0';
    img->height = (unsigned int)atoi(buffer);
    // Read maxval
    i = 0;
    while(1){
        fread(&(buffer[i]), sizeof(char), 1, read);
        if(!isdigit(buffer[i]))
            break;
        i++;
    }
    i++;
    buffer[i] = '\0';
    img->maxval = (unsigned int)atoi(buffer);
    // Create grid
    img->grid = generate_grid(img->width, img->height);
    // Read grid
    for(i = 0; i < img->height; i++)
        for(j = 0; j < img->width; j++){
            fread(&(img->grid[i][j].red), sizeof(unsigned char), 1, read);
            fread(&(img->grid[i][j].green), sizeof(unsigned char), 1, read);
            fread(&(img->grid[i][j].blue), sizeof(unsigned char), 1, read);
        }
    // Free memory
    fclose(read);
    // Return image
    return img;
}
// Save image in local directory with given name (filename)
void PPM_save_image(PPM_image *img, char *filename){
    FILE *write = fopen(filename, "wb");
    char buffer[10];
    // Write type
    fwrite(img->type, sizeof(char), strlen(img->type), write);
    fwrite("\n", sizeof(char), 1, write);
    // Write width
    sprintf(buffer, "%d", img->width);
    fwrite(buffer, sizeof(char), strlen(buffer), write);
    fwrite(" ", sizeof(char), 1, write);
    // Write height
    sprintf(buffer, "%d", img->height);
    fwrite(buffer, sizeof(char), strlen(buffer), write);
    fwrite("\n", sizeof(char), 1, write);
    // Write maxval
    sprintf(buffer, "%d", img->maxval);
    fwrite(buffer, sizeof(char), strlen(buffer), write);
    fwrite("\n", sizeof(char), 1, write);
    // Write pixels
    int i, j;
    for(i = 0; i < img->height; i++)
        for(j = 0; j < img->width; j++){
            fwrite(&(img->grid[i][j].red), sizeof(unsigned char), 1, write);
            fwrite(&(img->grid[i][j].green), sizeof(unsigned char), 1, write);
            fwrite(&(img->grid[i][j].blue), sizeof(unsigned char), 1, write);
        }
    // Free memory
    fclose(write);
}
// Free the memory allocated for grid
void free_grid(Pixel **grid, uint32_t height){
    int i;
    for(i = 0; i < height; i++)
        free(grid[i]);
    free(grid);
}
// Free PPM_image structure
void PPM_free_image(PPM_image **img){
    free_grid((*img)->grid, (*img)->height);
    free(*img);
    *img = NULL;
}