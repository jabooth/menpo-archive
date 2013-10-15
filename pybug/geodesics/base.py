import numpy as np
import collections
from pybug.geodesics import kirsanov
from pybug.geodesics.exceptions import TriMeshGeodesicsError


class Geodesics(object):
    r"""
    A number of geodesic distance algorithms for TriMeshes, including
    an exact geodesic method.

    Parameters
    ----------
    points : (N, D) ndarray
        The cartesian points that make up the mesh
    trilist : (M, 3) ndarray
        The triangulation of the given points
    """

    def __init__(self, mesh):
        points = np.require(mesh.points, requirements=['C'])
        trilist = np.require(mesh.trilist,
                             dtype=np.uint32, requirements=['C'])
        self._kirsanov = kirsanov.KirsanovGeodesics(points, trilist)
        self.mesh = mesh

    def gdists_to_indices(self, indices, method='exact'):
        r"""
        Calculate the geodesic distance for all points from the
        given ``indices``.

        Parameters
        -----------
        indices : (N,) list
            List of indexes to calculate the gdists_to_indices for
        method : {'exact'}
            The method using to calculate the gdists_to_indices. Only the 'exact'
            method is currently supported

            Default: exact

        Returns
        -------
        gdist: (n_points,)
            The geodesic distance between each point and it's closest source
            point
        best_source_index : (n_points,) ndarray
            For each point, which of the indices was the closest
            (and hence which the geodesic distance is relevant to). For
            example, if

                best_source[i] = 3252

            then

                gdist[i] = geodesic_between(points[i], points[3252])

            and 3252 is guaranteed to be one of the indices. If no
            geodesic could be found, to that vertex, then best_source[i] = -1.

        """
        indices = np.require(indices, dtype=np.uint32, requirements=['C'])
        if not np.all(0 <= indices < self.mesh.n_points):
            raise TriMeshGeodesicsError('Invalid indexes ' +
                                        '(all must be in range  '
                                        '0 <= i < n_points)')
        gdist, best_source = self._kirsanov.gdist_from_sources(indices,
                                                               method)
        # now we just clean the output of the raw output to make it more
        # useable.
        n_sources = indices.shape[0]
        best_source_index = np.ones_like(best_source) * -1
        # was the best source one of the sources we passed in? If not,
        # the geodesic will be incorrect.
        best_source_legal = np.logical_and(best_source < n_sources,
                                           best_source >= 0)
        # fix incorrect indices and gdists to -1.
        best_source_index[best_source_legal] = indices[
            best_source[best_source_legal]]
        gdist[~best_source_legal] = -1
        return gdist, best_source_index

    def individual_gdists_to_indices(self, indices, method='exact'):
        r"""
        Calculate the geodesic distance for all points from the
        given ``indices``.

        Parameters
        -----------
        indices : (N,) list like
            List of vertex indexes to calculate the gdists_to_indices from. The
            distance from all points on the mesh to each source vertex are
            calculated independently.
        method : {'exact'}
            The method using to calculate the gdists_to_indices. Only the 'exact'
            method is currently supported

            Default: exact

        Returns
        -------
        gdists: (n_sources, n_points) ndarray
            The geodesic distance between each point and and each source
            point
        gdist_found: (n_sources, n_points) boolean ndarray
            True iff a geodesic path was found between the point and source
            in question.

        Raises
        -------
        TriMeshGeodesicsError
            When indexes are out of the range of the number of points
        """
        indices = np.require(indices, dtype=np.uint32, requirements=['C'])
        return_shape = indices.shape[0], self.mesh.n_points
        gdists = np.zeros(return_shape)
        best_sources = np.zeros(return_shape)
        for i, index in enumerate(indices):
            gdists[i], best_sources[i] = self.gdists_to_indices([index],
                                                                method=method)
        # if a geodesic was found best_source should be 0 (only one source
        # point used)
        gdist_found = best_sources == 0
        return gdists, gdist_found

    def individual_gdists_to_points(self, source_points, method='exact'):
        r"""
        Calculate the geodesic distance for all points from the
        given ``source_points``.

        Parameters
        -----------
        source_points : :class:`pybug.shape.PointCloud`
            Source points to take gdists_to_indices from. The nearest vertex
            will be found for each source_point, and gdists calculated from
            there
        method : {'exact'}
            The method using to calculate the gdists_to_indices. Only the
            'exact'
            method is currently supported

            Default: exact

        Returns
        -------
        gdists: (n_sources, n_points) ndarray
            The geodesic distance between each point and and each source
            point
        gdist_found: (n_sources, n_points) boolean ndarray
            True iff a geodesic path was found between the point and source
            in question.
        source_indices: (n_sources,) ndarray
            The source indices used for comparison
        """
        source_indices = np.argmin(source_points.distance_to(self.mesh),
                                   axis=1)
        return self.individual_gdists_to_indices(
            source_indices, method=method) + (source_indices,)
