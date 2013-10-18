#include <iostream>
#include <ostream>
#include <assert.h>
#include "vertex.h"
#include "halfedge.h"
#include "triangle.h"

Vertex::Vertex(Mesh* mesh_in, unsigned vertex_id):
               MeshAttribute(mesh_in, vertex_id) {
	//std::cout << this << " - constructor" << std::endl;
}

bool Vertex::legal_attachment_to_triangle(Triangle& t) {
    // There should be only one halfedge per vertex-triangle.
    // true iff this vertex has exactly one halfedge to t
    unsigned count = 0;
    std::set<Halfedge*>::iterator he;
    for(he = halfedges.begin(); he != halfedges.end(); he++)
        if ((*he)->tri_ == &t)
            count++;
    return (count == 1);
}

Vertex::~Vertex() {
	//std::cout << this << " - destructor" << std::endl;
    halfedges.clear();
}

void Vertex::add_halfedge(Halfedge* halfedge) {
	//std::cout << this << " - added " << halfedge << std::endl;
    halfedges.insert(halfedge);
}

void Vertex::add_triangle(Triangle* triangle) {
	//std::cout << this << " - added " << triangle << std::endl;
    triangles.insert(triangle);
}

void Vertex::add_vertex(Vertex* vertex) {
	//std::cout << this << " - added " << vertex << std::endl;
    vertices.insert(vertex);
}

void Vertex::remove_halfedge(Halfedge* halfedge) {
	//std::cout << this << " - removed " << halfedge << std::endl;
    halfedges.erase(halfedge);
}

Halfedge* Vertex::halfedge_on_triangle(Triangle* triangle) {
	//std::cout << this << " - trying to find " << triangle << ".. ";
    std::set<Halfedge*>::iterator he;
    for(he = halfedges.begin(); he != halfedges.end(); he++)
        if((*he)->tri_ == triangle) {
            //std::cout << "found! Joined by " << (*he) << std::endl;
            return *he;
        }
	//std::cout << "not found" << std::endl;
    return NULL;
}

Halfedge* Vertex::halfedge_to_vertex(Vertex* vertex) {
    std::set<Halfedge*>::iterator he;
    for(he = halfedges.begin(); he != halfedges.end(); he++)
        if((*he)->get_v_b() == vertex)
            return *he;
    return NULL;
}

Halfedge* Vertex::halfedge_to_or_from_vertex(Vertex* vertex) {
    Halfedge* he = halfedge_to_vertex(vertex);
    if (he == NULL)
        he = vertex->halfedge_to_vertex(this);  // try the other way...
    // he could be NULL or a genuine HalfEdge* - either way, return it
    return he;
}

void Vertex::laplacian(unsigned* i_sparse, unsigned* j_sparse,
        double* w_sparse, unsigned& sparse_pointer,
        LaplacianWeightType weight_type) {
    // sparse_pointer points into how far into the sparse_matrix structures
    // we should be recording results for this vertex
    unsigned i = get_id();
    std::set<Vertex*>::iterator v;
    for(v = vertices.begin(); v != vertices.end(); v++) {
        unsigned j = (*v)->get_id();
        //if(i < j)
        //{
        Halfedge* he = halfedge_to_or_from_vertex(*v);
        double w_ij = 0;
        switch(weight_type) {
            case distance:
                w_ij = laplacian_distance_weight(he);
                break;
            case combinatorial:
                w_ij = 1;
                break;
        }
        i_sparse[sparse_pointer] = i;
        j_sparse[sparse_pointer] = j;
        w_sparse[sparse_pointer] = -w_ij;
        sparse_pointer++;
        // and record the other way for free (Laplacian is symmetrical)
        //i_sparse[sparse_pointer] = j;
        //j_sparse[sparse_pointer] = i;
        //w_sparse[sparse_pointer] = -w_ij;
        //sparse_pointer++;
        w_sparse[i] += w_ij;
        // += cotOp to the i'th\'th position twice (for both times)
        //w_sparse[j] += w_ij;
        //}
        // else:no point calculating this point - as we know the Laplacian is symmetrical
    }
}

double Vertex::laplacian_distance_weight(Halfedge* he) {
    double length = he->length();
    return 1.0/(length*length);
}

void Vertex::cotangent_laplacian(unsigned* i_sparse, unsigned* j_sparse,
        double* w_sparse, unsigned& sparse_pointer,
        double* cot_per_tri_vertex) {
    unsigned i = get_id();
    std::set<Vertex*>::iterator v;
    for(v = vertices.begin(); v != vertices.end(); v++) {
        unsigned j = (*v)->get_id();
        Halfedge* he = halfedge_to_or_from_vertex(*v);
        double w_ij = cot_per_tri_vertex[(he->tri_->get_id()*3) + he->v2_tri_i];
        if (he->part_of_fulledge()) {
            w_ij += cot_per_tri_vertex[(he->get_paired_he()->tri_->get_id()*3) +
                he->get_paired_he()->v2_tri_i];
        }
        else {
            //w_ij += w_ij;
        }
        i_sparse[sparse_pointer] = i;
        j_sparse[sparse_pointer] = j;
        w_sparse[sparse_pointer] = -w_ij;
        sparse_pointer++;
        w_sparse[i] += w_ij;
    }
}

void Vertex::verify_halfedge_connectivity() {
    std::set<Halfedge*>::iterator he;
    for(he = halfedges.begin(); he != halfedges.end(); he++) {
        Triangle* triangle = (*he)->tri_;
        Vertex* t_v0 = triangle->get_v0();
        Vertex* t_v1 = triangle->get_v1();
        Vertex* t_v2 = triangle->get_v2();
        if(t_v0 != this && t_v1 != this && t_v2 != this)
            std::cout << "this halfedge does not live on it's triangle!"
                << std::endl;
        if((*he)->get_v_a() != this)
            std::cout << "half edge errornously connected" << std::endl;
        if((*he)->ccw_around_tri()->ccw_around_tri()->get_v_b() != (*he)->get_v_a())
            std::cout << "cannie spin raarnd the triangle like man!"
                << std::endl;
        if((*he)->part_of_fulledge()) {
            if((*he)->get_paired_he()->get_v_a() != (*he)->get_v_b() ||
               (*he)->get_paired_he()->get_v_b() != (*he)->get_v_a())
                std::cout << "T" << triangle->get_id() << " H:" << (*he)->get_id() << std::endl;
        }
    }
}

void Vertex::status() {
    std::cout << "V" << get_id() << std::endl;
    std::set<Halfedge*>::iterator he;
    for(he = halfedges.begin(); he != halfedges.end(); he++) {
        std::cout << "|" ;
        if ((*he)->part_of_fulledge())
            std::cout << "=";
        else
            std::cout << "-";
        std::cout << "V" << (*he)->get_v_b()->get_id();
        std::cout << " (T" << (*he)->tri_->get_id();
        if ((*he)->part_of_fulledge())
            std::cout << "=T" << (*he)->get_paired_he()->tri_->get_id();
        std::cout << ")" << std::endl;
    }
}

void Vertex::test_contiguous(std::set<Vertex*>* vertices_not_visited) {
    std::set<Vertex*>::iterator v;
    for (v = vertices.begin(); v != vertices.end(); v++)
        if (vertices_not_visited->erase(*v))
            (*v)->test_contiguous(vertices_not_visited);
}

