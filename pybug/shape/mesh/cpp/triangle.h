#pragma once
#include <set>
#include "mesh.h"

class Vertex;
class Halfedge;

class Triangle : public MeshAttribute {
private:
    Vertex *v0_, *v1_, *v2_;
    Halfedge *he0_, *he1_, *he2_;

    // repairs damaged chiral meshes (ensures all faces point same way)
    void resolveChirality(bool e0_bad, bool e1_bad, bool e2_bad);
    Halfedge* createHalfedge(Vertex* v0, Vertex* v1, Vertex* v2,
                             unsigned halfedge_id);
    void recursiveFlip(std::set<Triangle*>* visited_tris);
    void set_v0(Vertex* value);
    void set_v1(Vertex* value);
    void set_v2(Vertex* value);
    void set_he0(Halfedge* value);
    void set_he1(Halfedge* value);
    void set_he2(Halfedge* value);


public:
    Triangle(Mesh* mesh, unsigned id, Vertex* v0, Vertex* v1, Vertex* v2);
    ~Triangle();

    // accessors
    Vertex* get_v0() const;
    Vertex* get_v1() const;
    Vertex* get_v2() const;

    // the corresponding half edges for this triangle
    // (note v0->-he0->-v1->-he1->v2->-he2->v0)
    Halfedge* get_he0() const;
    Halfedge* get_he1() const;
    Halfedge* get_he2() const;

    // the parent edges of the halfedges
    Edge* e0() const;
    Edge* e1() const;
    Edge* e2() const;

    // joined triangles. May raise Exception if there is no joint triangle
    Triangle* t0() const;
    Triangle* t1() const;
    Triangle* t2() const;
    std::set<Triangle*> adjacent_triangles();

    // repairing
    void flipContiguousRegion();

    // algorithms
    void reduce_scalar_per_vertex_to_vertices(
            double* triangle_scalar_per_vertex, double* vertex_scalar);
    void reduce_scalar_to_vertices(
            double* triangle_scalar, double* vertex_scalar);

    // utilities
    void trilist(double* trilist);
    void status();
    friend std::ostream& operator<<(std::ostream& out, const Triangle* t) {
            return out << "T" << t->get_id();
    }
};

// Little idea for making Triangle Iterable things..
//template <class T>
//class TriangleIterable{
//    std::vector<T> ijk;
//    T* cw_;
//    T* ccw_;
//
//    public:
//        TriangleIterator(T item, T* ccw, T* cw);
//        T ccw();
//        T cw();
//};

