#include <iostream>
#include <iomanip>
#include <set>
#include "halfedge.h"
#include "triangle.h"
#include "vertex.h"

Triangle::Triangle(Mesh* mesh, unsigned tri_id, Vertex* v0,
        Vertex* v1, Vertex* v2) : MeshAttribute(mesh, tri_id) {
    v0_ = v0;
    v1_ = v1;
    v2_ = v2;
    // add ourselves to each of the vertices
    v0->add_tri(this);
    v1->add_tri(this);
    v2->add_tri(this);
    // build the new halfedges
    he0_ = new Halfedge(mesh, v0, v1, v2, this, 0);
    he1_ = new Halfedge(mesh, v1, v2, v0, this, 1);
    he2_ = new Halfedge(mesh, v2, v0, v1, this, 2);

    // if the edges are now flipped, we can correct it
    //bool e0_flip = he0_->get_edge()->is_flipped_edge();
    //bool e1_flip = he1_->get_edge()->is_flipped_edge();
    //bool e2_flip = he2_->get_edge()->is_flipped_edge();

    // deal with any that need flipping
    //if (e0_flip || e1_flip || e2_flip)
     //   resolveChirality(e0_flip, e1_flip, e2_flip);

    // now, we should definitely have a well formed triangle.
    // lets check...
    if (!(v0_->legal_attachment_to_tri(*this) &&
          v1_->legal_attachment_to_tri(*this) &&
          v2_->legal_attachment_to_tri(*this)))
        std::cout << this << " has a vertex error." << std::endl;
}

Triangle::~Triangle() {}


Vertex* Triangle::get_v0() const {
    return v0_;
}

Vertex* Triangle::get_v1() const {
    return v1_;
}

Vertex* Triangle::get_v2() const {
    return v2_;
}

void Triangle::set_v0(Vertex* value) {
    v0_ = value;
}

void Triangle::set_v1(Vertex* value) {
    v1_ = value;
}

void Triangle::set_v2(Vertex* value) {
    v2_ = value;
}

Halfedge* Triangle::get_he0() const {
    return he0_;
}

Halfedge* Triangle::get_he1() const {
    return he1_;
}

Halfedge* Triangle::get_he2() const {
    return he2_;
}

void Triangle::set_he0(Halfedge* value) {
    he0_ = value;
}

void Triangle::set_he1(Halfedge* value) {
    he1_ = value;
}

void Triangle::set_he2(Halfedge* value) {
    he2_ = value;
}

Edge* Triangle::e0() const {
    return get_he0()->get_edge();
}

Edge* Triangle::e1() const {
    return get_he1()->get_edge();
}

Edge* Triangle::e2() const {
    return get_he2()->get_edge();
}

void Triangle::resolveChirality(bool e0_bad, bool e1_bad, bool e2_bad) {
    std::cout << this << " - resolving chirality error against ";
    if (e0_bad)
        std::cout << get_he0()->paired_tri() << " with " << get_he0() << "/" << get_he0()->paired_he();
    if (e1_bad)
        std::cout << get_he1()->paired_tri() << " with " << get_he1() << "/" << get_he1()->paired_he();
    if (e2_bad)
        std::cout << get_he2()->paired_tri() << " with " << get_he2() << "/" << get_he2()->paired_he();
    std::cout << std::endl;

    // we don't want to spread onto wrongly flipped neighbours..
    auto visited_tris = new std::set<Triangle*>;
    if (e0_bad) {
        visited_tris->insert(t0());
    } if (e1_bad) {
        visited_tris->insert(t1());
    } if (e2_bad) {
        visited_tris->insert(t2());
    }
    // call flip on myself, flipping myself and all my neighbours.
    recursiveFlip(*visited_tris);
    delete visited_tris;
}

Triangle* Triangle::t0() const {
    return he0_->paired_tri();
}

Triangle* Triangle::t1() const {
    return he1_->paired_tri();
}

Triangle* Triangle::t2() const {
    return he2_->paired_tri();
}

std::set<Triangle *> Triangle::adjacent_triangles() {
    std::set<Triangle *> triangles;
    triangles.insert(t0());
    triangles.insert(t1());
    triangles.insert(t2());
    triangles.erase(NULL);
    return triangles;
}

void Triangle::flipContiguousRegion() {
    auto visited_tris = new std::set<Triangle*>;
    // call recursive flip on ourselves
    recursiveFlip(*visited_tris);
    // all done - clean up the memory
    delete visited_tris;
}

void Triangle::recursiveFlip(std::set<Triangle*>& visited_tris) {
    visited_tris.insert(this);  // add myself to the list
    //std::cout << this << " - recursive flip" << std::endl;
    // flip the meaning of each half edge around.
    he0_->flip();
    he1_->flip();
    he2_->flip();
    // now the state of halfedges and vertices are fixed up, but this triangle
    // is all wrong.
    // make sure v0, v1, v2 are correct in meaning by flipping v0, v1_
    Vertex *v0_temp = v0_;
    v0_ = v1_;
    v1_ = v0_temp;
    // make sure e0, e1, e2 have the correct meaning by flipping he1_, he2_
    Halfedge *he1_temp = he1_;
    he1_ = he2_;
    he2_ = he1_temp;
    // update the get_id()'s on the halfedges themselves to be correct
    he1_->set_id(he1_->get_id() + 1); // e1_ -> he2_
    he2_->set_id(he1_->get_id() - 1); // e2 -> e1
    // get each neighbouring triangle, other than the one who called us
    for (auto tri : adjacent_triangles()) {
        if (!visited_tris.count(tri))
            tri->recursiveFlip(visited_tris);  // haven't been here before
    }
}

void Triangle::reduce_scalar_to_vertices(double* triangle_scalar,
        double* vertex_scalar) {
    vertex_scalar[v0_->get_id()] += triangle_scalar[get_id()];
    vertex_scalar[v1_->get_id()] += triangle_scalar[get_id() + 1];
    vertex_scalar[v2_->get_id()] += triangle_scalar[get_id() + 2];
}

void Triangle::reduce_scalar_per_vertex_to_vertices(
        double* triangle_scalar_per_vertex, double* vertex_scalar) {
    vertex_scalar[v0_->get_id()] += triangle_scalar_per_vertex[(get_id() * 3)];
    vertex_scalar[v1_->get_id()] += triangle_scalar_per_vertex[(get_id() * 3) + 1];
    vertex_scalar[v2_->get_id()] += triangle_scalar_per_vertex[(get_id() * 3) + 2];
}

void Triangle::status() {
    std::cout << "    TRIANGLE " << get_id() << "        " << std::endl;
    Halfedge* h01 = v0_->halfedge_on_tri(this);
    Halfedge* h12 = v1_->halfedge_on_tri(this);
    Halfedge* h20 = v2_->halfedge_on_tri(this);
    unsigned width = 12;
    std::cout  << std::setw(width) << "V0(" << v0_->get_id() << ")";
    if (h01->is_part_of_fulledge()) {
        std::cout << "============";
    }
    else {
        std::cout << "------------";
    }
    std::cout  << std::setw(width) << "V1(" << v1_->get_id() << ")";
    if (h12->is_part_of_fulledge()) {
        std::cout << "============";
    }
    else {
        std::cout << "------------";
    }
    std::cout  << std::setw(width) << "V2(" << v2_->get_id() << ")";
    if (h20->is_part_of_fulledge()) {
        std::cout << "============";
    }
    else {
        std::cout << "------------";
    }
    std::cout << std::setw(width) << "V0(" << v0_->get_id() << ")" << std::endl;

    std::cout  << std::setw(width) << " ";
    if (h01->is_part_of_fulledge()) {
        std::cout  << std::setw(width) << h01->paired_tri()->get_id();
    }
    else {
        std::cout << " -- ";
    }
    std::cout  << std::setw(width) << " ";
    if (h12->is_part_of_fulledge()) {
        std::cout << std::setw(width) << h12->paired_tri()->get_id();
    }
    else {
        std::cout << " -- ";
    }
    std::cout  << std::setw(width) << " ";
    if (h20->is_part_of_fulledge()) {
        std::cout << std::setw(width) << h20->paired_tri()->get_id();
    }
    else {
        std::cout << " -- ";
    }
}

