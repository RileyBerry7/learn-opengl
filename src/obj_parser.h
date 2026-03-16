//
// Created by Riley on 3/13/2026.
//

#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

// std
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

// ---------------------------------------------------------------------
#define LINE_BUFFER_SIZE 256

// ---------------------------------------------------------------------
typedef struct {
    float x, y, z;
} Vec3;


typedef struct {
    float u, v;
} Vec2;

typedef struct {
    float vertices[1000];
    int   indices[1000];

    int vertices_count;
    int indices_count;
} ObjParse;



// ---------------------------------------------------------------------
ObjParse parse(char* file_name);

FILE* open_file(char* file_name);
char* read_line(FILE *file_ptr, char* buffer);
void close_file(FILE* file_ptr);


#endif //OBJ_PARSER_H