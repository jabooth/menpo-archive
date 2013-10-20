#include <iostream>
#include <cmath>
#include "halfedge.h"
#include "triangle.h"
#include "vertex.h"


Halfedge::Halfedge(Mesh* mesh, Vertex* a, Vertex* b, Vertex* opposite,
                   Triangle* tri, unsigned tri_he_id) :
                   MeshAttribute(mesh, tri_he_id) {
    get_mesh()->n_halfedges++;
    a_ = a;
    b_ = b;
    opp_ = opposite;
    tri_ = tri;
    // we need to change our id to be correct - hacky!
    set_id(3 * tri_->get_id() + tri_he_id);
    // attach up halfedges and increase mesh counts for edges/halfedges
    Halfedge* halfedge = b_->halfedge_to_vertex(a_);
    if (!halfedge) {
        // try the other way. Note that this is now a broken halfedge, which
        // the triangle who constructed me will have to detect and fix.
        halfedge = a_->halfedge_to_vertex(b_);
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
        halfedge->set_paired_he(this);
        set_paired_he(halfedge);
        get_mesh()->n_fulledges++;
    } else {
        // this truly is the first time this edge exists
        mesh->add_edge(this);
        set_paired_he(NULL);
    }
}

Halfedge::~Halfedge(){}

Vertex* Halfedge::get_a() const {
	return a_;
}

Vertex* Halfedge::get_b() const {
	return b_;
}

Vertex* Halfedge::get_opp() const {
	return opp_;
}

void Halfedge::set_a(Vertex* value) {
	a_ = value;
}

void Halfedge::set_b(Vertex* value) {
	b_ = value;
}

void Halfedge::set_opp(Vertex* value) {
	opp_ = value;
}

Halfedge* Halfedge::get_paired_he() const {
    return paired_halfedge_;
}

void Halfedge::set_paired_he(Halfedge* value) {
    paired_halfedge_ = value;
}

Triangle* Halfedge::get_tri() {
    return tri_;
}

void Halfedge::set_tri(Triangle* value) {
    tri_ = value;
}

bool Halfedge::part_of_fulledge() {
    return get_paired_he() != NULL;
}

Triangle* Halfedge::other_tri() {
    return this->part_of_fulledge() ? get_paired_he()->get_tri() : NULL;
}

Halfedge* Halfedge::ccw_around_tri() {
    return get_b()->halfedge_on_tri(get_tri());
}

void Halfedge::flip() {
    // fix up the vertex halfedge connectivity
    get_a()->rm_halfedge(this);
    get_b()->add_halfedge(this);
    // flip vertex 0-1 on self
    Vertex* v_a_temp = get_a();
    set_a(get_b());
    set_b(v_a_temp);
}

double Halfedge::length() {
    // TODO calculate this
    return 1.0;
}

