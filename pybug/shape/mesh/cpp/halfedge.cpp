#include <iostream>
#include <cmath>
#include "halfedge.h"
#include "triangle.h"
#include "vertex.h"


HalfEdge::HalfEdge(Mesh* mesh_in, Vertex* v0_in, Vertex* v1_in,
                   Vertex* v2_in, Triangle* triangle_in,
                   unsigned tri_halfedge_id) :
                   MeshAttribute(mesh_in, tri_halfedge_id) {
    mesh->n_halfedges++;
    v0 = v0_in;
    v1 = v1_in;
    v2 = v2_in;
    triangle = triangle_in;
    // we need to change our id to be correct - hacky!
    id = (3 * triangle->id) + tri_halfedge_id;
    std::cout << this << " - constructor"<< std::endl;
    // attach up halfedges and increase mesh counts for edges/halfedges
    HalfEdge* halfedge = v1->halfedge_to_vertex(v0);
    if (!halfedge)
        // try the other way. Note that this is now a broken halfedge, which
        // the triangle who constructed me will have to detect and fix.
        halfedge = v0->halfedge_to_vertex(v1);
    if (halfedge) {
        // setting opposite halfedge to me, and inc global full edge count
        halfedge->set_paired_halfedge(this);
        mesh->n_fulledges++;
    } else {
        // this truly is the first time this edge exists
        mesh_in->add_edge(this);
    }
}

HalfEdge::~HalfEdge(){}

HalfEdge* HalfEdge::paired_halfedge() const {
    return paired_halfedge_;
}

void HalfEdge::set_paired_halfedge(HalfEdge* value) {
    paired_halfedge_ = value;
}

bool HalfEdge::part_of_fulledge() {
    return paired_halfedge() != NULL;
}

Triangle* HalfEdge::other_triangle() {
    return this->part_of_fulledge() ? paired_halfedge()->triangle : NULL;
}

HalfEdge* HalfEdge::ccw_around_tri() {
    return v1->halfedge_on_triangle(triangle);
}

void HalfEdge::flip() {
    // fix up the vertex halfedge connectivity
    v0->remove_halfedge(this);
    v1->add_halfedge(this);
    // flip vertex 0-1 on self
    Vertex* v_temp = v0;
    v0 = v1;
    v1 = v_temp;
}

double HalfEdge::length() {
    // TODO actually calculate this
    std::cout << "This isn't actually calculating the length" << std::endl;
    return 1.0;
}

