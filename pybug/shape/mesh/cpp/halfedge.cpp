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

std::set<Vertex*>* AbstractEdge::get_vertices() const {
    return vertices_;
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

std::set<Halfedge*>* Edge::get_halfedges() const {
    return halfedges_;
}

void Edge::add_halfedge(Halfedge* halfedge) {
    halfedges_->insert(halfedge);
}

bool Edge::is_halfedge() const {
    return n_halfedges() == 1;
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

bool Edge::is_overdetermined_edge() const {
    return n_halfedges() > 2;
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

bool Edge::includes_vertex(Vertex* vertex) const {
    for (auto v : *vertices_) {
        if (v == vertex)
            return true;
    }
    return false;
}

bool Edge::includes_triangle(Triangle* t) const {
    for (auto he : *halfedges_) {
        if (he->get_tri() == t)
            return true;
    }
    return false;
}


Halfedge::Halfedge(Mesh* mesh, Vertex* a, Vertex* b, Vertex* opposite,
                   Triangle* tri, unsigned tri_he_id) :
                        AbstractEdge(mesh, a, b) {
    edge_ = a->edge_to_vertex(b);
    if (!edge_) {
        std::cout << "no edge exists - building a new one" << std::endl;
        edge_ = new Edge(mesh, a, b);
    } else {
        std::cout << "using existing edge: " << edge_ << std::endl;
    }
    a_ = a;
    b_ = b;
    opp_ = opposite;
    tri_ = tri;
    // we need to change our id to be correct - hacky!
    set_id(3 * tri_->get_id() + tri_he_id);
    // add to the mesh, vertex, and edge
    mesh->add_halfedge(this);
    a->add_halfedge(this);
    edge_->add_halfedge(this);
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

Triangle* Halfedge::get_tri() const {
    return tri_;
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

void Halfedge::set_tri(Triangle* value) {
    tri_ = value;
}

Halfedge* Halfedge::paired_he() const {
    if (!(is_part_of_fulledge() || is_part_of_flipped_edge())) {
        return nullptr;
    } else {
        auto halfedges = get_edge()->get_halfedges();
        for (auto he : *halfedges) {
            if (he != this) {
                return he;
            }
        }
        throw 1; // should never happen
    }
}

Triangle* Halfedge::paired_tri() const {
    auto he = paired_he();
    if(he) {
        return he->get_tri();
    } else {
        return nullptr;
    }
}

bool Halfedge::is_isolated_edge() const {
    return get_edge()->is_halfedge();
}

bool Halfedge::is_part_of_fulledge() const {
    return get_edge()->is_fulledge();
}

bool Halfedge::is_part_of_overdetermined_edge() const {
    return get_edge()->is_overdetermined_edge();
}

bool Halfedge::is_part_of_flipped_edge() const {
    return get_edge()->is_flipped_edge();
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

