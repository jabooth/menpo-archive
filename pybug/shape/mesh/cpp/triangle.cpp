#include <iostream>
#include <iomanip>
#include <set>
#include "halfedge.h"
#include "triangle.h"
#include "vertex.h"

Triangle::Triangle(Mesh* mesh_in, unsigned tri_id, Vertex* v0_in,
        Vertex* v1_in, Vertex* v2_in) : MeshAttribute(mesh_in, tri_id) {
    v0 = v0_in;
    v1 = v1_in;
    v2 = v2_in;
    v0->add_triangle(this);
    v1->add_triangle(this);
    v2->add_triangle(this);
    v0->add_vertex(v1);
    v0->add_vertex(v2);
    v1->add_vertex(v0);
    v1->add_vertex(v2);
    v2->add_vertex(v0);
    v2->add_vertex(v1);
    // we need to know if any of the edges are going to be flipped when
    // connected up - i.e. do any of these halfedges already exist?
    bool e0_bad = v0->halfedge_to_vertex(v1) ? 1 : 0;
    bool e1_bad = v1->halfedge_to_vertex(v2) ? 1 : 0;
    bool e2_bad = v2->halfedge_to_vertex(v0) ? 1 : 0;
    // create all the new halfedges regardless
    e0 = add_halfedge_between(v0, v1, v2, 0);
    e1 = add_halfedge_between(v1, v2, v0, 1);
    e2 = add_halfedge_between(v2, v0, v1, 2);
    // deal with any that need flipping
    if (e0_bad || e1_bad || e2_bad)
        this->resolve_chirality(e0_bad, e1_bad, e2_bad);
}

HalfEdge* Triangle::add_halfedge_between(Vertex* v0, Vertex* v1, Vertex* v2,
                                         unsigned v0_id) {
    HalfEdge* halfedge = new HalfEdge(this->mesh, v0, v1, v2, this, v0_id);
    v0->add_halfedge(halfedge);
    return halfedge;
}

void Triangle::resolve_chirality(bool e0_bad, bool e1_bad, bool e2_bad) {
//    std::cout << "T:" << id << " is chirally inconsistent with ";
//    if (e0_bad) std::cout << "T:" << t0()->id << " ";
//    if (e1_bad) std::cout << "T:" << t1()->id << " ";
//    if (e2_bad) std::cout << "T:" << t2()->id << " ";
//    std::cout << " Repairing...";
    // need to temp store the connecting pointers to other regions
    HalfEdge *h0, *h1, *h2;
    // store out the current e0,e1,e2 (there meaning will change with the flip
    // - we just want to ensure that the correct edges get wired up at the
    // end!)
    HalfEdge *orig_e0, *orig_e1, *orig_e2;
    orig_e0 = e0;
    orig_e1 = e1;
    orig_e2 = e2;
    // detach any bad halfedges so we don't recursively flip onto a 'good' set
    // of triangles
    if (e0_bad) {
        h0 = e0->halfedge;
        e0->halfedge = NULL;
    } if (e1_bad) {
        h1 = e1->halfedge;
        e1->halfedge = NULL;
    } if (e2_bad) {
        h2 = e2->halfedge;
        e2->halfedge = NULL;
    }
    // call flip on myself, flipping myself and all my neighbours.
    this->flip_contiguous_region();
    // now myself and all my (non bad) neighbours are flipped, I reattach the
    // bad edges. Everything is now fixed
    if (e0_bad)
        orig_e0->halfedge = h0;
    if (e1_bad)
        orig_e1->halfedge = h1;
    if (e2_bad)
        orig_e2->halfedge = h2;
    //std::cout << "Done." << std::endl;
}

Triangle::~Triangle() {}

Triangle* Triangle::t0(){
    return e0->other_triangle();
}

Triangle* Triangle::t1(){
    return e1->other_triangle();
}

Triangle* Triangle::t2(){
    return e2->other_triangle();
}

std::set<Triangle *> Triangle::adjacent_triangles(){
    std::set<Triangle *> triangles;
    triangles.insert(t0());
    triangles.insert(t1());
    triangles.insert(t2());
    triangles.erase(NULL);
    return triangles;
}

void Triangle::flip_contiguous_region() {
    std::set<Triangle*> visited_tris;
    visited_tris.insert(this);
    // call recursive flip on ourselves
    this->recursive_flip(&visited_tris);
}

void Triangle::recursive_flip(std::set<Triangle*>* visited_tris){
    visited_tris->insert(this);  // add myself to the list
    //std::cout << "Calling recursive flip on T" << id << std::endl;
    // flip the meaning of each half edge around.
    e0->flip();
    e1->flip();
    e2->flip();
    // now the state of halfedges and vertices are fixed up, but this triangle
    // is all wrong.
    // make sure v0, v1, v2 are correct in meaning by flipping v0, v1
    Vertex *v0_temp = v0;
    v0 = v1;
    v1 = v0_temp;
    // make sure e0, e1, e2 have the correct meaning by flipping e1, e2
    HalfEdge *e1_temp = e1;
    e1 = e2;
    e2 = e1_temp;
    // update the id's on the halfedges themselves to be correct
    e1->id = 1;
    e2->id = 2;
    // get each neighbouring triangle, other than the one who called us
    std::set<Triangle*> adjacent_tris = adjacent_triangles();
    std::set<Triangle*>::iterator it;
    for (it = adjacent_tris.begin(); it != adjacent_tris.end(); it++)
        if (visited_tris->find(*it) == visited_tris->end())
            (*it)->recursive_flip(visited_tris);  // haven't been here before
}

void Triangle::reduce_scalar_to_vertices(double* triangle_scalar,
        double* vertex_scalar) {
    vertex_scalar[v0->id] += triangle_scalar[id];
    vertex_scalar[v1->id] += triangle_scalar[id + 1];
    vertex_scalar[v2->id] += triangle_scalar[id + 2];
}

void Triangle::reduce_scalar_per_vertex_to_vertices(
        double* triangle_scalar_per_vertex, double* vertex_scalar) {
    vertex_scalar[v0->id] += triangle_scalar_per_vertex[(id * 3)];
    vertex_scalar[v1->id] += triangle_scalar_per_vertex[(id * 3) + 1];
    vertex_scalar[v2->id] += triangle_scalar_per_vertex[(id * 3) + 2];
}

void Triangle::status() {
    std::cout << "    TRIANGLE " << id << "        " << std::endl;
    HalfEdge* h01 = v0->halfedge_on_triangle(this);
    HalfEdge* h12 = v1->halfedge_on_triangle(this);
    HalfEdge* h20 = v2->halfedge_on_triangle(this);
    unsigned width = 12;
    std::cout  << std::setw(width) << "V0(" << v0->id << ")";
    if (h01->part_of_fulledge()) {
        std::cout << "============";
    }
    else {
        std::cout << "------------";
    }
    std::cout  << std::setw(width) << "V1(" << v1->id << ")";
    if (h12->part_of_fulledge()) {
        std::cout << "============";
    }
    else {
        std::cout << "------------";
    }
    std::cout  << std::setw(width) << "V2(" << v2->id << ")";
    if (h20->part_of_fulledge()) {
        std::cout << "============";
    }
    else {
        std::cout << "------------";
    }
    std::cout << std::setw(width) << "V0(" << v0->id << ")" << std::endl;

    std::cout  << std::setw(width) << " ";
    if (h01->part_of_fulledge()) {
        std::cout  << std::setw(width) << h01->halfedge->triangle->id;
    }
    else {
        std::cout << " -- ";
    }
    std::cout  << std::setw(width) << " ";
    if (h12->part_of_fulledge()) {
        std::cout << std::setw(width) << h12->halfedge->triangle->id;
    }
    else {
        std::cout << " -- ";
    }
    std::cout  << std::setw(width) << " ";
    if (h20->part_of_fulledge()) {
        std::cout << std::setw(width) << h20->halfedge->triangle->id;
    }
    else {
        std::cout << " -- ";
    }
}

