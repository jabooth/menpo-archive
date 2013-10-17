#include "mesh.h"

int main() {
    unsigned n_triangles = 2;
    unsigned n_vertices = 4;
    unsigned trilist[] = {0,1,3,2,1,3};
    Mesh mesh(trilist, n_triangles, n_vertices);
    return 0;
}
