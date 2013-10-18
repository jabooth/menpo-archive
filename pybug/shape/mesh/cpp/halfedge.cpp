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
    v0 = v0_in;
    v1 = v1_in;
    v2 = v2_in;
    triangle = triangle_in;
    // we need to change our id to be correct - hacky!
    this->set_id(3 * triangle->get_id() + tri_halfedge_id);
    //std::cout << this << " - constructor"<< std::endl;
    // attach up halfedges and increase mesh counts for edges/halfedges
    Halfedge* halfedge = v1->halfedge_to_vertex(v0);
    if (!halfedge) {
        // try the other way. Note that this is now a broken halfedge, which
        // the triangle who constructed me will have to detect and fix.
        halfedge = v0->halfedge_to_vertex(v1);
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

Halfedge* Halfedge::paired_halfedge() const {
    return paired_halfedge_;
}

void Halfedge::set_paired_halfedge(Halfedge* value) {
    //if (value)
    //    std::cout << this << " - set paired HE to " << value << std::endl;
    //else
    //    std::cout << this << " - wiping paired HE" << std::endl;
    paired_halfedge_ = value;
}

bool Halfedge::part_of_fulledge() {
    return paired_halfedge() != NULL;
}

Triangle* Halfedge::other_triangle() {
    return this->part_of_fulledge() ? paired_halfedge()->triangle : NULL;
}

Halfedge* Halfedge::ccw_around_tri() {
    return v1->halfedge_on_triangle(triangle);
}

void Halfedge::flip() {
    //std::cout << this << " flipping" << std::endl;
    // fix up the vertex halfedge connectivity
    v0->remove_halfedge(this);
    v1->add_halfedge(this);
    // flip vertex 0-1 on self
    Vertex* v_temp = v0;
    v0 = v1;
    v1 = v_temp;
}

double Halfedge::length() {
    // TODO actually calculate this
    //std::cout << "This isn't actually calculating the length" << std::endl;
    return 1.0;
}

