#pragma once
#include <set>
#include "mesh.h"
#include "triangle.h"

/** Abstract representation of an edge in the triangle topology.
 *
 *
 *
 */

class AbstractEdge: public MeshAttribute {
protected:
    std::set<Vertex*>* vertices_;  /**< the two vertices the edge involves */

public:
    AbstractEdge(Mesh* mesh, Vertex* a, Vertex* b);
    ~AbstractEdge();
    // TODO returning internal state, seems pretty bad.
    std::set<Vertex*>* get_vertices() const;  //*< the set of vertices */
    double length() const; /**< presently unimplimented */
};

/** For any triangle mesh there is exactly one Edge between any two joined
 *  vertices. As such an Edge is non-directional - it represents connectivity.
 *
 */

class Edge : public AbstractEdge {
private:
    std::set<Halfedge *>* halfedges_;  /**! an Edge has many Halfedges */

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

/** A directional Edge from a vertex to another vertex. All Halfedges belong
 *  to an Edge (which is non-directional).
 *
 *  As Halfedges have a sense of direction, they also belong to a single
 *  Triangle. There are four possible states of an Edge/HalfEdge arrangement:
 *
 *  1. A "half edge". An Edge that has one and only one Halfedge (and hence one
 *     Triangle) attached. Such an Edge occurs on the edge of a triangle mesh
 *     manifold.
 *
 *  2. A "full edge". An Edge that owns exactly two Halfedges. The first a->b,
 *     the second, b->a. Each Halfedge owns a Triangle, and the two triangles
 *     are "stitched together" along this Edge. As the Halfedge's are the
 *     compliment of each other (they join the same two vertices but in
 *     opposite directions) the two Triangles are therefore orientated
 *     in a consistent manor - that is two say the surface normals of the
 *     triangles both point in a consisent "outward" direction. This is the
 *     Edge type found in the middle of a well ordered Triangle Mesh.
 *
 *  3. A "flipped edge". As a full edge, only the two Halfedge's share the same
 *     vertices (a->b and a->b), but different Triangles (t0 and t1). This is
 *     only possible if the Triangles are orientated in opposite directions to
 *     each other - that is to say that we have a chiral inconsistency in the
 *     mesh. This case is seen in the middle of a badly ordered Triangle Mesh
 *     where two regions of well-ordered Triangles meet to create a flipped
 *     edge. Generally this is a problematic state that should be resolved.
 *     If the problem is resolved (by flipping triangles) the mesh can still
 *     be completely chirially consistent.
 *
 *
 *  4. An "overdetermined edge". An Edge that has three or more Halfedges. This
 *     cannot be repaired without the use of some huristics and destruction
 *     - the mesh is simply no longer a manifold. An attempt to repair by
 *     removing offending triangles is possible but dangerous.
 *
 *  In a poorly constructed trilist,
 *  there may be 3 or more Halfedges on one Edge - in such a case the Edge is
 *  deemed to be overdetermined, and cannot topologically be considered a
 *  manifold.
 *
 */

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
