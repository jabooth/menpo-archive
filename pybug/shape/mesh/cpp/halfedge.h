#pragma once
#include "mesh.h"
#include "triangle.h"

class Halfedge : public MeshAttribute
{
    private:
        Halfedge* paired_halfedge_;
    public:
        // accessors

        // the opposite halfedge in the fulledge (or NULL)
        Halfedge* paired_halfedge() const;
        void set_paired_halfedge(Halfedge* value);

        // TODO rename these to distinguish from v0 on triangle
        Vertex* v0;
        Vertex* v1;
        Vertex* v2; // opposite vertex on my triangle
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
            return (he1.v0 == he2.v0 && he1.v1 == he2.v1 &&
                    he1.v2 == he2.v2 && he1.triangle == he2.triangle);
        }
};
