#include <iostream>
#include <cmath>
#include "halfedge.h"
#include "triangle.h"
#include "vertex.h"

AbstractEdge::AbstractEdge(Mesh* mesh, Vertex *a, Vertex *b) : MeshAttribute(mesh, 0) {
    vertices_ = new std::set<Vertex*>;
    vertices_->insert(a);
    vertices_->insert(b);
}

AbstractEdge::~AbstractEdge() {
    delete vertices_;
}

double AbstractEdge::length() const {
    // TODO calculate this
    return 1.0;
}


Edge::Edge(Mesh *mesh, Vertex *a, Vertex *b) : AbstractEdge(mesh, a, b) {
    halfedges_ = new std::set<Halfedge*>;
    a->add_edge(this);
    b->add_edge(this);
    a->add_vertex(b);
    b->add_vertex(a);
    set_id(mesh->n_edges());  // set my ID to the current edge count
    mesh->add_edge(this);
}

Edge::~Edge() {
    delete halfedges_;
}

unsigned Edge::n_halfedges() const {
    return halfedges_->size();
}

bool Edge::includes_vertex(Vertex* vertex) const {
    for (auto v : *vertices_) {
        if (v == vertex)
            return true;
    }
    return false;
}

bool Edge::is_flipped_edge() const {
    if (n_halfedges() != 2)
        return false;
    // we know there are only two he's - pull them off
    auto he_it = halfedges_->begin();
    Halfedge* he_a = *(he_it);
    he_it++;
    Halfedge* he_b = *(he_it);
    if (he_a->get_a() == he_b->get_a()) {
            return true;
    }
    return false;
}

bool Edge::is_overdetermined_edge() const {
    return n_halfedges() > 2;
}

bool Edge::is_fulledge() const {
    if (n_halfedges() != 2)
        return false;
    // we know there are only two he's - pull them off
    auto he_it = halfedges_->begin();
    Halfedge* he_a = *(he_it);
    he_it++;
    Halfedge* he_b = *(he_it);
    if (he_a->get_a() == he_b->get_b()) {
            return true;
    }
    return false;
}


Halfedge::Halfedge(Mesh* mesh, Vertex* a, Vertex* b, Vertex* opposite,
                   Triangle* tri, unsigned tri_he_id) :
                        AbstractEdge(mesh, a, b) {
    try {
        edge_ = a->edge_to_vertex(b);
    } catch (std::exception& e) {
        edge_ = new Edge(mesh, a, b);
    }
    a_ = a;
    b_ = b;
    opp_ = opposite;
    tri_ = tri;
    // we need to change our id to be correct - hacky!
    set_id(3 * tri_->get_id() + tri_he_id);
    mesh->add_halfedge(this);
    a->add_halfedge(this);
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

Edge* Halfedge::get_edge() const {
    return edge_;
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

Halfedge* Halfedge::paired_he() const {
    return paired_halfedge_;
}

void Halfedge::set_paired_he(Halfedge* value) {
    paired_halfedge_ = value;
}

Triangle* Halfedge::get_tri() const {
    return tri_;
}

void Halfedge::set_tri(Triangle* value) {
    tri_ = value;
}

bool Halfedge::part_of_fulledge() const {
    return paired_he() != NULL;
}

Triangle* Halfedge::paired_tri() const {
    return this->part_of_fulledge() ? paired_he()->get_tri() : NULL;
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
