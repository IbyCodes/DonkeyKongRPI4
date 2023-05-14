#pragma once
struct Image
{
    int width;  // # of cols
    int height; // # of rows
    unsigned int *data;
};

struct Tilemap {
    int count;
    struct Image images[];
};

extern struct Tilemap tilemap;
