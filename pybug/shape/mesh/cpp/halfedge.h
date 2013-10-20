#pragma once
#include "mesh.h"
#include "triangle.h"

class Halfedge : public MeshAttribute
{
private:
    Halfedge* paired_halfedge_;
    Vertex* a_;  // start of HE
    Vertex* b_;  // end of HE
    Vertex* opp_; // opposite vertex on my triangle
    Triangle* tri_; // triangle I belong too
public:
    Halfedge(Mesh* mesh, Vertex* a, Vertex* b, Vertex* opposite,
             Triangle* tri, unsigned tri_he_id);
    ~Halfedge();

    Vertex* get_a() const;
    Vertex* get_b() const;
    Vertex* get_opp() const;
    void set_a(Vertex* value);
    void set_b(Vertex* value);
    void set_opp(Vertex* value);

    Halfedge* get_paired_he() const;
    void set_paired_he(Halfedge* value);

    Triangle* get_tri();
    void set_tri(Triangle* value);

    Triangle* other_tri();

    // TODO rename these to distinguish from v0 on triangle
    int v0_tri_i; // The position no. of each vertex on triangle
    int v1_tri_i; // i.e., if MY v0 is actually the triangle's v1,
    int v2_tri_i; // v0_tri_i == 1, v1_tri_i == 2 ... (always CCW)


    bool part_of_fulledge();
    void flip(); // flip this half edge, fixing up all vertex pointers
    // along the way
    Halfedge* ccw_around_tri();
    double length();
    friend std::ostream& operator<<(std::ostream& out, const Halfedge* h) {
        return out << "H" << ((h->get_id())/3) << ":" << h->get_id() % 3;
    }
    friend bool operator==(const Halfedge& he1, const Halfedge& he2) {
        return (he1.a_ == he2.a_ && he1.b_ == he2.b_ &&
                he1.opp_ == he2.opp_ && he1.tri_ == he2.tri_);
    }
};
