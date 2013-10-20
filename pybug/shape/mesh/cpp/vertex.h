#pragma once
#include <set>
#include <ostream>
#include "mesh.h"

class Halfedge;
class Triangle;
class Vertex;

class Vertex : public MeshAttribute
{
private:
    std::set<Halfedge*> edges_; //halfedges STARTING from this vertex
    std::set<Triangle*> tris_; //ALL triangles attached to this vertex
    std::set<Vertex*> verts_; //ALL other vertices attached to this vertex

public:
    Vertex(Mesh* mesh, unsigned id);
    ~Vertex();

    // mesh construction methods
    void add_halfedge(Halfedge* halfedge);
    void add_tri(Triangle* triangle);
    void add_vertex(Vertex* vertex);
    void rm_halfedge(Halfedge* halfedge);

    // halfedge retrieval methods about this vertex
    Halfedge* halfedge_on_tri(Triangle* triangle);
    Halfedge* halfedge_to_vertex(Vertex* vertex);
    Halfedge* halfedge_to_or_from_vertex(Vertex* vertex);

    std::set<Halfedge*> halfedges_to_vertex(Vertex* vertex);
    std::set<Halfedge*> halfedges_to_or_from_vertex(Vertex* vertex);

    // algorithms
    void laplacian(unsigned* i_sparse, unsigned* j_sparse, double* v_sparse,
            unsigned& sparse_pointer, LaplacianWeightType weight_type);
    double laplacian_distance_weight(Halfedge* he);
    void cotangent_laplacian(unsigned* i_sparse, unsigned* j_sparse,
            double* w_sparse, unsigned& sparse_pointer,
            double* cot_per_tri_vertex);

    // utility methods
    bool legal_attachment_to_tri(Triangle& t);
    void status();
    void verify_halfedge_connectivity();
    void test_contiguous(std::set<Vertex*>* vertices_visited);
    friend std::ostream& operator<<(std::ostream& out, const Vertex *v) {
        return out << "V" << v->get_id();
    }

};
