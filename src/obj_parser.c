#include "obj_parser.h"

#include <float.h>
#include <stdbool.h>

// ---------------------------------------------------------------------
// 5. CALCULATE EDGES
int* calculate_edges(int* v_mapping, int map_count) {
    // Note: make vvt_pairs non aggregative

    int* edges = (int*)malloc(sizeof(int)*1000);
    int edge_count  = 0;
    int* p1         = &v_mapping[0];
    int* p2         = &v_mapping[1];
    int* p3         = &v_mapping[2];
    int* end_of_map = &v_mapping[map_count];

    if (map_count < 3) {
        free(edges);
        return NULL;
    }

    // Triangle Fanning Loop
    while (p3 != end_of_map) {
        // Add Edge
        edges[edge_count++] = *p1;
        edges[edge_count++] = *p2;
        edges[edge_count++] = *p3;

        // Move ptr 2 and 3 forward
        p2++;
        p3++;
    }
    // Add end of line indicator
    edges[edge_count++] = -1;
    for (int i = 0; i<edge_count; i++) {
        printf("%d", edges[i]);
    }
    printf("\n");
    return edges;
}

// -------------------------------------------------------------------------
// PARSE
ObjParse parse(char* file_path) {

    // VARIABLES ----------------------------------------------------------

    // File
    FILE* obj_file = open_file(file_path);  // File pointer
    char  line_buffer[LINE_BUFFER_SIZE];    // Buffer for current line

    // Vertex
    Vec3  v_list[1000];                     // Vertex list
    int   v_count = 0;                      // Vertex count

    // Texture
    Vec2  uv_list[1000];                     // Vertex list
    int   uv_count = 0;                      // Vertex count

    // Faces
    int face_count = 0;
    Vec2 vvt_pairs[1000];
    int vvt_count = 0;

    // Vertices (Aggregate)
    float vertices[1000]; // Master list of [Vertex], [Color], [UV Coord], per element
    int vert_count = 0;
    int vert_index = 0;

    // Indices (Edges)
    int indices[1000];
    int indices_count = 0;

    // Output
    ObjParse output;

    // Read File Loop ------------------------------------------------------
    if (line_buffer == NULL) {
        printf("\nLine buffer empty.\n");
    }
    // Loop while read line is valid
    while (read_line(obj_file, line_buffer)){

        // Read Line
        // strcpy(line_buffer, read_line(obj_file, line_buffer));

        // printf(line_buffer);

        // 1. Texture Coordinate Declaration -------------------------------------

        // If texture declaration
        if (line_buffer[0] == 'v' && line_buffer[1] == 't') {

            // Scan texture coordinates
            Vec2 tex_coord;
            int count = sscanf(line_buffer, "vt %f %f ", &tex_coord.u, &tex_coord.v);
            if (count != 2) printf("Error: texture has not 2 values.\n");
            printf("Texture Coordinate: %f, %f\n", tex_coord.u, tex_coord.v);

            // Add texture coordinate to UV list
            uv_list[uv_count] = tex_coord;
            uv_count++;
        }

        // 2. Vertex Declaration ------------------------------------------------

        // If vertex declaration
        else if (line_buffer[0] == 'v') {

            // Scan vertex coordinates
            Vec3 new_vertex;
            int count = sscanf(line_buffer, "v %f %f %f ", &new_vertex.x, &new_vertex.y, &new_vertex.z);
            if (count != 3) printf("Error: Vertex has non 3 values");
            printf("Vertex: %f, %f, %f\n", new_vertex.x, new_vertex.y, new_vertex.z);

            // Add vertex to vertex list
            v_list[v_count++] = new_vertex;
        }

        // 3. Face Declaration ------------------------------------------------------------------
        if (line_buffer[0] == 'f') {
            printf("\nFace #\n");

            Vec2 pair = {NAN, NAN};
            int v_mapping[1000];
            int map_count = 0;

            // Assemble vvt pairs
            for (int index = 0; index < LINE_BUFFER_SIZE; index++) {

                // If end of line, exit loop
                if (line_buffer[index] == '\0') {

                    // 5. Calculate edges
                    int* edges = calculate_edges(v_mapping, map_count);

                    if (edges == NULL) break;

                    // Add edges to indices
                    int edge_index = 0;
                    while (edges[edge_index] != -1) {
                        indices[indices_count++] = edges[edge_index++];
                    }

                    free(edges);
                    break;
                }

                // Skip if not digit
                if (!isdigit(line_buffer[index])) continue;

                // Get next integer
                int value = atoi(&line_buffer[index]);

                // Assign U if empty
                if (isnan(pair.u)) pair.u = value;
                // Assign V if empty
                else if (isnan(pair.v)) pair.v = value;

                // If U and V are full
                if (!isnan(pair.u) && !isnan(pair.v)) {
                    // Store in VVT List
                    vvt_pairs[vvt_count++] = pair;
                    printf("vvt pair: %f, %f\n", pair.u, pair.v);

                    // 4. Add to Vertices list
                    vertices[vert_index++] = v_list[(int)pair.u - 1].x;
                    vertices[vert_index++] = v_list[(int)pair.u - 1].y;
                    vertices[vert_index++] = v_list[(int)pair.u - 1].z;
                    vertices[vert_index++] = 0.0f;
                    vertices[vert_index++] = 0.0f;
                    vertices[vert_index++] = 0.0f;
                    vertices[vert_index++] = uv_list[(int)pair.v - 1].u;
                    vertices[vert_index++] = uv_list[(int)pair.v - 1].v;

                    // Add Pair-Vertices Mapping
                    v_mapping[map_count++] = vert_count++;

                    pair.u = NAN;
                    pair.v = NAN;
                }
                // Jump to next int
                while (isdigit(line_buffer[index]))
                    index++;
                index--;
            }
        }
    }
    close_file(obj_file);

    memcpy(output.vertices, vertices, sizeof(float)*vert_index);
    memcpy(output.indices, indices,  sizeof(int)*indices_count);

    output.vertices_count = vert_index;
    output.indices_count = indices_count;

    // // DEBUG: Print Output
    printf("\nIndices:\n");
    for (int i2=0; i2 < indices_count; i2++) printf("%d,", indices[i2]);
    printf("\n\nVertices:");
    for (int i3=0; i3 < vert_index; i3++) {
        if (i3 % 8 == 0) printf("\n");
        printf("%.3g, ", vertices[i3]);
    }
    return output;
}

//-------------------------------------------------------------------------------------------------
FILE* open_file(char* file_name) {

    FILE *file_ptr;

    // 1. Open the file for reading
    if (file_ptr = fopen(file_name, "r"))
    {
        printf("\nFile Opened\n");
        return file_ptr;
    }
    printf("Error: Could not open file.\n");
    return NULL;
}

// ---------------------------------------------------------------------
char* read_line(FILE *file_ptr, char* buffer)
{
    if (fgets(buffer, LINE_BUFFER_SIZE, file_ptr)) {
        return buffer;
    } else {
        printf("\nFailed to read line.\n");
        return NULL;
    }
}

// ---------------------------------------------------------------------
void close_file(FILE *file_ptr) {
    fclose(file_ptr);
    printf("\nFile Closing\n");

}
// ---------------------------------------------------------------------
