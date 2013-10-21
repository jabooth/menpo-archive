#pragma once
#include <set>
#include "mesh.h"
#include "triangle.h"

class AbstractEdge: public MeshAttribute {
protected:
    std::set<Vertex*>* vertices_;  // the two vertices the edge involves

public:
    AbstractEdge(Mesh* mesh, Vertex* a, Vertex* b);
    ~AbstractEdge();
    std::set<Vertex*>* get_vertices() const;
    double length() const;
};


class Edge : public AbstractEdge {
private:
    std::set<Halfedge *>* halfedges_;

public:
    Edge(Mesh* mesh, Vertex* a, Vertex* b);
    ~Edge();
    unsigned n_halfedges() const;
    std::set<Halfedge*>* get_halfedges() const;
    void add_halfedge(Halfedge* halfedge);

    // edge status
    bool is_halfedge() const;
    bool is_fulledge() const;
    bool is_overdetermined_edge() const;
    bool is_flipped_edge() const;
    bool includes_vertex(Vertex* vertex) const;
    bool includes_triangle(Triangle* t) const;
    // TODO includes halfedge
};


class Halfedge : public AbstractEdge
{
private:
    Edge* edge_;  // edge I belong to
    Vertex* a_;  // start of HE
    Vertex* b_;  // end of HE
    Vertex* opp_;  // opposite vertex on my triangle
    Triangle* tri_;  // triangle I belong to
    void set_tri(Triangle* value);

public:
    Halfedge(Mesh* mesh, Vertex* a, Vertex* b, Vertex* opposite,
             Triangle* tri, unsigned tri_he_id);
    ~Halfedge();

    Vertex* get_a() const;
    Vertex* get_b() const;
    Vertex* get_opp() const;
    Edge* get_edge() const;
    Triangle* get_tri() const;
    void set_a(Vertex* value);
    void set_b(Vertex* value);
    void set_opp(Vertex* value);

    Halfedge* paired_he() const;
    Triangle* paired_tri() const;

    // status of edge
    bool is_isolated_edge() const;
    bool is_part_of_fulledge() const;
    bool is_part_of_overdetermined_edge() const;
    bool is_part_of_flipped_edge() const;

    void flip(); // flip this half edge, fixing up all vertex pointers
    // along the way
    Halfedge* ccw_around_tri();
    friend std::ostream& operator<<(std::ostream& out, const Halfedge* h) {
        return out << "H" << ((h->get_id())/3) << ":" << h->get_id() % 3;
    }
    friend bool operator==(const Halfedge& he1, const Halfedge& he2) {
        return (he1.a_ == he2.a_ && he1.b_ == he2.b_ &&
                he1.opp_ == he2.opp_ && he1.tri_ == he2.tri_);
    }
};
