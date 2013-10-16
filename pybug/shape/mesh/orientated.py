import numpy as np
from pybug.shape.mesh.base import TriMesh
from pybug.shape.mesh.cpptrimesh import CppTriMesh


#noinspection PyNoneFunctionAssignment
class OrientatedTriMesh(TriMesh):
    """A TriMesh with an underlying C++ data structure, allowing for efficient
    iterations around mesh vertices and triangles. This extra performance
    comes at a cost - the data provided to build the trimesh has to be
    ordered, such that contiguous regions are chirally consistent .
    """
    def __init__(self, points, trilist):
        points = np.require(points, dtype=np.float, requirements=['C'])
        trilist = np.require(trilist, dtype=np.uint32, requirements=['C'])
        # firstly, check the bounds of the trilist are within {0: n_points - 1}
        n_points = points.shape[0]
        if np.min(trilist) < 0 or np.max(trilist) > n_points - 1:
            raise ValueError("Trilist refers to missing data - cannot be "
                             "repaired.")
        tl_unique = np.unique(trilist)
        tl_diff = np.setdiff1d(tl_unique, np.arange(n_points))
        if len(tl_diff) != 0:
            raise ValueError("Unreferenced vertices - can be repaired.")
        super(OrientatedTriMesh, self).__init__(points, trilist)
        self._cpp_tri_mesh = CppTriMesh(points, trilist)

    @property
    def n_fulledges(self):
        return self._cpp_tri_mesh.n_fulledges()

    @property
    def n_halfedges(self):
        return self._cpp_tri_mesh.n_halfedges()

    @property
    def n_edges(self):
        return self.n_halfedges - self.n_fulledges

    def verify_mesh(self):
        self._cpp_tri_mesh.verify_mesh()

    def vertex_status(self, vertex_index):
        return self._cpp_tri_mesh.vertex_status(vertex_index)

    def tri_status(self, tri_index):
        return self._cpp_tri_mesh.vertex_status(tri_index)
