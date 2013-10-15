# distutils: language = c++
# distutils: sources = ./pybug/geodesics/cpp/exact/kirsanov_geodesic_wrapper.cpp

import numpy as np
cimport numpy as cnp

# externally declare the exact geodesic code
cdef extern from "./cpp/exact/kirsanov_geodesic_wrapper.h":
    cdef cppclass KirsanovGeodesicWrapper:
        KirsanovGeodesicWrapper(double* points, unsigned n_vertices,
                unsigned* tri_index, unsigned n_triangles) except +
        void all_exact_geodesics_from_source_vertices(unsigned* source_vertices,
                unsigned n_sources, double* phi, unsigned* best_source)
        #void all_dijkstra_geodesics_from_source_vertices(
        #        unsigned* source_vertices, unsigned n_sources, double* phi,
        #        unsigned* best_source)
        #void all_subdivision_geodesics_from_source_vertices(
        #        unsigned* source_vertices, unsigned n_sources,
        #        double* phi, unsigned* best_source,
        #        unsigned subdivision_level)

cdef class KirsanovGeodesics:
    r"""
    Cython wrapper for the cpp class used to calculated the Kirsanov Geodesics.

    Parameters
    ----------
    points : (N, D) c-contiguous double ndarray
        The cartesian points
    trilist : (M, 3) c-contiguous unsigned ndarray
        The triangulation of the given points

    References
    ----------
    .. [1] Surazhsky, Vitaly, et al.
        "Fast exact and approximate gdists_to_indices on meshes."
        ACM Transactions on Graphics (TOG). Vol. 24. No. 3. ACM, 2005.
    """
    cdef KirsanovGeodesicWrapper* kirsanov
    cdef int n_points, n_tris

    def __cinit__(self, double[:, ::1] points not None,
                  unsigned[:, ::1] trilist not None):
        self.kirsanov = new KirsanovGeodesicWrapper(
                &points[0,0], points.shape[0],
                &trilist[0,0], trilist.shape[0])


    def __init__(self, points, trilist, **kwargs):
        self.n_points = points.shape[0]
        self.n_tris = trilist.shape[0]

    def __dealloc__(self):
        del self.kirsanov

    def gdist_from_sources(self, source_indices, method='exact'):
        r"""
        Calculate the geodesic distance for all points on the mesh to a
        subset of the points.

        Parameters
        -----------
        source_indices : (n_sources,) ndarray
            indices into the mesh points, selecting which points are sources.
            Has to be in range [0:n_points-1]
        method : {'exact'}
            The method using to calculate the gdists_to_indices. Only the
            'exact' method is currently supported

            Default: exact

        Returns
        -------
        gdist : (n_points,) ndarray
            geodesic distance from each point on the mesh to the closest of
            the source index points.

        best_source : (n_points,) ndarray
            For each point, which of the indices was the closest
            (and hence which the geodesic distance is relevant to). This
            is a raw output from the algorithm, and if the geodesic wasn't
            calculable the output is ill-defined.

        """
        cdef cnp.ndarray[unsigned, ndim=1, mode='c'] indices = \
            np.require(source_indices, dtype=np.uint32, requirements=['C'])
        cdef cnp.ndarray[double, ndim=1, mode='c'] gdist = np.zeros(
                self.n_points)
        # best source is an index into the source_index array - note that
        # this is not what we want to return directly..
        cdef cnp.ndarray[unsigned, ndim=1, mode='c'] best_source = np.zeros(
                self.n_points, dtype=np.uint32)
        cdef int n_sources = indices.shape[0]
        if method == 'exact':
            self.kirsanov.all_exact_geodesics_from_source_vertices(
                    &indices[0], n_sources,
                    &gdist[0], &best_source[0])
        #elif method == 'dijkstra':
        #    self.kirsanov.all_dijkstra_geodesics_from_source_vertices(&np_sources[0],
        #            np_sources.size, &gdist[0], &best_source[0])
        #elif method == 'subdivision':
        #    self.kirsanov.all_subdivision_geodesics_from_source_vertices(
        #            &np_sources[0], np_sources.size, &gdist[0], &best_source[0], 3)
        else:
            raise ValueError("The '" + `method` + "' method for calculating "
                            "gdists_to_indices is not understood "
                            "(only 'exact' can be used at this time)")
        return gdist, best_source
