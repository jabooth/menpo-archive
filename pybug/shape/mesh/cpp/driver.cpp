#include "mesh.h"
#include <fstream>

int main() {
    std::fstream trilist_data("/home/jab08/squaretl.txt");
    unsigned n_triangles, n_vertices, tl_i;
    trilist_data >> n_triangles;
    trilist_data >> n_vertices;
    std::vector<unsigned> trilist;
    while (trilist_data >> tl_i)
        trilist.push_back(tl_i);
    Mesh mesh(&trilist[0], n_triangles, n_vertices);
    mesh.verify_mesh();
    return 0;
}

