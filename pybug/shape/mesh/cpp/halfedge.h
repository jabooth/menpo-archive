#pragma once
#include "mesh.h"
#include "triangle.h"

class Halfedge : public MeshAttribute
{
private:
        Halfedge* paired_halfedge_;
        Vertex* v_a_;  // start of HE
        Vertex* v_b_;  // end of HE
        Vertex* v_opp_; // opposite vertex on my triangle
public:
        // accessors
        Halfedge* get_paired_halfedge() const;
        void set_paired_halfedge(Halfedge* value);
        Vertex* get_v_a() const;
        Vertex* get_v_b() const;
        Vertex* get_v_opp() const;
        void set_v_a(Vertex* value);
        void set_v_b(Vertex* value);
        void set_v_opp(Vertex* value);

        // TODO rename these to distinguish from v0 on triangle
        int v0_tri_i; // The position no. of each vertex on triangle
        int v1_tri_i; // i.e., if MY v0 is actually the triangle's v1,
        int v2_tri_i; // v0_tri_i == 1, v1_tri_i == 2 ... (always CCW)
        Triangle* triangle;
        Triangle* other_triangle();
        Halfedge(Mesh* mesh, Vertex* v0, Vertex* v1, Vertex* v2,
                 Triangle* triangle, unsigned int tri_halfedge_id);
        ~Halfedge();
        bool part_of_fulledge();
        void flip(); // flip this half edge, fixing up all vertex pointers
        // along the way
        Halfedge* ccw_around_tri();
        double length();
        friend std::ostream& operator<<(std::ostream& out, const Halfedge* h) {
            return out << "H" << ((h->get_id())/3) << ":" << h->get_id() % 3;
        }
        friend bool operator==(const Halfedge& he1, const Halfedge& he2) {
            return (he1.v_a_ == he2.v_a_ && he1.v_b_ == he2.v_b_ &&
                    he1.v_opp_ == he2.v_opp_ && he1.triangle == he2.triangle);
        }
};
