#include <iostream>
#include <cmath>
#include "halfedge.h"
#include "triangle.h"
#include "vertex.h"


Halfedge::Halfedge(Mesh* mesh_in, Vertex* v0_in, Vertex* v1_in,
                   Vertex* v2_in, Triangle* triangle_in,
                   unsigned tri_halfedge_id) :
                   MeshAttribute(mesh_in, tri_halfedge_id) {
    get_mesh()->n_halfedges++;
    v_a_ = v0_in;
    v_b_ = v1_in;
    v_opp_ = v2_in;
    triangle = triangle_in;
    // we need to change our id to be correct - hacky!
    this->set_id(3 * triangle->get_id() + tri_halfedge_id);
    //std::cout << this << " - constructor"<< std::endl;
    // attach up halfedges and increase mesh counts for edges/halfedges
    Halfedge* halfedge = v_b_->halfedge_to_vertex(v_a_);
    if (!halfedge) {
        // try the other way. Note that this is now a broken halfedge, which
        // the triangle who constructed me will have to detect and fix.
        halfedge = v_a_->halfedge_to_vertex(v_b_);
    //    std::cout << this << " - no opposite halfedge to pair with" << std::endl;
    //    if(halfedge)
    //        std::cout << this << " - warning: existing he " << halfedge <<
    //           " already connects " << v0 << "-" << v1 << " chirality repair "
    //           << "should be triggered" << std::endl;
    //} else {
    //    //std::cout << this << " - has opposite halfedge " << halfedge
    //        << std::endl;
    }
    if (halfedge) {
        //std::cout << this << " - as a halfedge exists, pairing" << std::endl;
        // setting opposite halfedge to me, and inc global full edge count
        halfedge->set_paired_halfedge(this);
        set_paired_halfedge(halfedge);
        get_mesh()->n_fulledges++;
    } else {
        // this truly is the first time this edge exists
        mesh_in->add_edge(this);
        set_paired_halfedge(NULL);
    }
}

Halfedge::~Halfedge(){}

Vertex* Halfedge::get_v_a() const {
	return v_a_;
}

Vertex* Halfedge::get_v_b() const {
	return v_b_;
}

Vertex* Halfedge::get_v_opp() const {
	return v_opp_;
}

void Halfedge::set_v_a(Vertex* value) {
	v_a_ = value;
}

void Halfedge::set_v_b(Vertex* value) {
	v_b_ = value;
}

void Halfedge::set_v_opp(Vertex* value) {
	v_opp_ = value;
}

Halfedge* Halfedge::get_paired_halfedge() const {
    return paired_halfedge_;
}

void Halfedge::set_paired_halfedge(Halfedge* value) {
    paired_halfedge_ = value;
}

bool Halfedge::part_of_fulledge() {
    return get_paired_halfedge() != NULL;
}

Triangle* Halfedge::other_triangle() {
    return this->part_of_fulledge() ? get_paired_halfedge()->triangle : NULL;
}

Halfedge* Halfedge::ccw_around_tri() {
    return get_v_b()->halfedge_on_triangle(triangle);
}

void Halfedge::flip() {
    // fix up the vertex halfedge connectivity
    get_v_a()->remove_halfedge(this);
    get_v_b()->add_halfedge(this);
    // flip vertex 0-1 on self
    Vertex* v_a_temp = get_v_a();
    set_v_a(get_v_b());
    set_v_b(v_a_temp);
}

double Halfedge::length() {
    // TODO calculate this
    return 1.0;
}

