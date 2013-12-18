# distutils: language = c++
# distutils: extra_compile_args = -std=c++11
# distutils: sources = ./pybug/shape/mesh/cpp/mesh.cpp ./pybug/shape/mesh/cpp/vertex.cpp ./pybug/shape/mesh/cpp/halfedge.cpp ./pybug/shape/mesh/cpp/triangle.cpp

from libcpp.vector cimport vector
from libcpp.set cimport set
from cython.operator cimport dereference as deref, preincrement as inc
import numpy as np
cimport numpy as cnp

class MeshConstructionError(Exception):
    pass

# externally declare the C++ Mesh, Vertex, Triangle and HalfEdge classes
cdef extern from "./cpp/mesh.h":
    cdef cppclass Mesh:
        Mesh(unsigned *tri_index, unsigned n_tris, unsigned n_points) except +
        unsigned n_vertices
        unsigned n_triangles
        unsigned n_halfedges
        unsigned n_fulledges
        vector[Vertex*]* vertices
        vector[Triangle*]* triangles
        vector[HalfEdge*]* halfedges
        void laplacian(unsigned* i_sparse, unsigned* j_sparse,
                double* v_sparse)
        void cotangent_laplacian(unsigned* i_sparse, unsigned* j_sparse,
                double* v_sparse, double* cotangent_weights)
        void verify_mesh()
        void generate_edge_index(unsigned* edgeIndex)
        void reduce_tri_scalar_to_vertices(
                double* triangle_scalar, double* vertex_scalar)
        void reduce_tri_scalar_per_vertex_to_vertices(
                double* triangle_scalar_p_vert, double* vertex_scalar)

    cdef enum LaplacianWeightType:
        combinatorial
        distance

cdef extern from "./cpp/vertex.h":
    cdef cppclass Vertex:
        set[HalfEdge*] halfedges
        void status()

cdef extern from "./cpp/triangle.h":
    cdef cppclass Triangle:
        void status()

cdef extern from "./cpp/halfedge.h":
    cdef cppclass HalfEdge:
        pass

# Wrap the Mesh class to produce CppTriMesh
# TODO: document me
cdef class CppTriMesh:
    cdef Mesh* thisptr
    cdef points
    cdef trilist

    def __cinit__(self, points, unsigned[: , ::1] trilist not None):
        if points.shape[1] != 3:
            raise MeshConstructionError(
                "A CppTriMesh can only be in 3 dimensions "
                "(attempting with {})".format(str(points.shape[1])))
        self.thisptr = new Mesh(&trilist[0,0], trilist.shape[0],
                                points.shape[0])
        self.points = points
        self.trilist = trilist

    def __dealloc__(self):
        del self.thisptr

    def __reduce__(self):
        return self.__class__, (np.asarray(self.points),
                                np.asarray(self.trilist))

    def n_fulledges(self):
        return self.thisptr.n_fulledges

    def n_halfedges(self):
        return self.thisptr.n_halfedges

    def verify_mesh(self):
        self.thisptr.verify_mesh()

    def vertex_status(self, n_vertex):
        assert 0 <= n_vertex < self.thisptr.n_vertices
        deref(deref(self.thisptr.vertices)[n_vertex]).status()

    def tri_status(self, n_triangle):
        assert 0 <= n_triangle < self.thisptr.n_triangles
        deref(deref(self.thisptr.triangles)[n_triangle]).status()

    def reduce_t_scalar_per_v_to_v(self, double[:, ::1] tri_s not None):
        cdef cnp.ndarray[double, ndim=1, mode='c'] v_scalar = \
            np.zeros(self.thisptr.n_vertices)
        self.thisptr.reduce_tri_scalar_per_vertex_to_vertices(&tri_s[0,0],
                                                              &v_scalar[0])
        return v_scalar

    def reduce_t_scalar_to_vs(self, double[::1] t_scalar not None):
        cdef cnp.ndarray[double, ndim=1, mode='c'] v_scalar = \
            np.zeros(self.thisptr.n_vertices)
        self.thisptr.reduce_tri_scalar_to_vertices(&t_scalar[0], &v_scalar[0])
        return v_scalar
