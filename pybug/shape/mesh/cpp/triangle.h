#pragma once
#include <set>
#include "mesh.h"

class Vertex;
class HalfEdge;

class Triangle : public MeshAttribute
{
    private:
        // repairs damaged chiral meshes (ensures all faces point same way)
        void resolveChirality(bool e0_bad, bool e1_bad, bool e2_bad);
        HalfEdge* createHalfedge(Vertex* v0, Vertex* v1, Vertex* v2,
                                 unsigned halfedge_id);
        void recursiveFlip(std::set<Triangle*>* visited_tris);


    public:
        Vertex *v0, *v1, *v2;
        // the corresponding half edges for this triangle
        // (note v0->-e0->-v1->-e1->v2->-e2->v0)
        HalfEdge *e0, *e1, *e2;
        Triangle(Mesh* mesh, unsigned id, Vertex* v0, Vertex* v1, Vertex* v2);
        ~Triangle();

        // accessing
        Triangle* t0();
        Triangle* t1();
        Triangle* t2();
        std::set<Triangle*> adjacent_triangles();

        // repairing
        void flipContiguousRegion();

        // algorithms
        void reduce_scalar_per_vertex_to_vertices(
                double* triangle_scalar_per_vertex, double* vertex_scalar);
        void reduce_scalar_to_vertices(
                double* triangle_scalar, double* vertex_scalar);

        // utilities
        void status();
        friend std::ostream& operator<<(std::ostream& out, const Triangle* t) {
            if (t->get_id())
                return out << "T" << t->get_id();
            else
                return out << "TNULL";
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

