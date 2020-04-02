/// \file ijkgenpatch.cxx
/// generate isosurface patch

/*
  IJK: Isosurface Jeneration Kode
  Copyright (C) 2006, 2003, 2001 Rephael Wenger

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  (LGPL) as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "stdafx.h"
#include "ijktable.h"
#include <clarksonhull.h>

using namespace IJKTABLE;

//**************************************************
// Type Declarations
//**************************************************

typedef ISOSURFACE_VERTEX::ISOSURFACE_VERTEX_TYPE ISOSURFACE_VERTEX_TYPE;

//**************************************************
// Function Declarations
//**************************************************

void ijkgenpatch(              const ISOSURFACE_TABLE_POLYHEDRON        &polyhedron
                              ,const fixedarray<int>                    &vertex_sign
                              ,vector<ISOSURFACE_VERTEX_INDEX>          &edge_list
                              ,u_int                                    &num_simplices
                              ,fixedarray<u_int>                        &temp_plist
                              ,doublearray                              &temp_pcoord);

void ijkgenpatch_nep(          const ISOSURFACE_TABLE_POLYHEDRON        &polyhedron
                              ,const fixedarray<int>                    &vertex_sign
                              ,vector<ISOSURFACE_VERTEX_INDEX>          &isov_list
                              ,vector<ISOSURFACE_VERTEX_TYPE>           &isov_type
                              ,u_int                                    &num_simplices
                              ,fixedarray<u_int>                        &temp_plist
                              ,fixedarray<ISOSURFACE_VERTEX_TYPE>       &temp_type 
                              ,doublearray                              &temp_pcoord);

void add_simplex_to_isosurface(u_int                                     dimension
                              ,const fixedarray<u_int>                  &simplex_vert
                              ,u_int                                     is
                              ,const fixedarray<u_int>                  &point_list
                              ,vector<ISOSURFACE_VERTEX_INDEX>          &isov_list);

void add_simplex_to_isosurface(u_int                                     dimension
                              ,const fixedarray<u_int>                  &simplex_vert
                              ,u_int                                     is
                              ,const fixedarray<u_int>                  &point_list
                              ,const fixedarray<ISOSURFACE_VERTEX_TYPE> &point_type
                              ,vector<ISOSURFACE_VERTEX_INDEX>          &isov_list
                              ,vector<ISOSURFACE_VERTEX_TYPE>           &isov_type);

//**************************************************
// inline functions
//**************************************************

  // set bits in vertex_bits[]
  // polyhedron = isosurface table polyhedon
  // point_list[] = list of points
  //   Precondition: Polyhedron vertices are listed first in point list
  //                 followed by polyhedron edge midpoints
  // ip = set bits for point point_list[ip]
  // num_vert_in_point_list = number of polyhedron vertices in point_list[]
  // vertex_bits = bit vector of polyhedron vertices
inline void set_vertex_bits(const ISOSURFACE_TABLE_POLYHEDRON &polyhedron
                           ,const fixedarray<u_int>           &point_list
                           ,u_int                              ip
                           ,u_int                              num_vert_in_point_list
                           ,long                              &vertex_bits)
{
  if(ip < num_vert_in_point_list) {
    // point_list[ip] is a polyhedron vertex
    vertex_bits  = vertex_bits | (1L << point_list[ip]);
  } else {
    // point_list[ip] is a polyhedron vertex
    const u_int ie = point_list[ip];

    u_int iv0 = polyhedron.EdgeEndpoint(ie, 0);
    u_int iv1 = polyhedron.EdgeEndpoint(ie, 1);

    vertex_bits |= (1L << iv0);
    vertex_bits |= (1L << iv1);
  }
}

//**************************************************
// ijkgenpatch
//**************************************************

void ijkgenpatch(const ISOSURFACE_TABLE_POLYHEDRON &polyhedron
                ,const fixedarray<int>             &vertex_sign
                ,vector<ISOSURFACE_VERTEX_INDEX>   &edge_list
                ,u_int                             &num_simplices) {
// polyhedron = mesh polyhedron
// vertex_sign[iv] = vertex sign of vertex iv.  
//                   -1 = negative; 0 = zero; 1 = positive
// edge_list = return list of polyhedron edges
//   i'th simplex has vertices on edges
//      (edge_list[d*i], edge_list[d*i+1], ..., edge_list[d*i+d-1])
  u_int              nume         = polyhedron.NumEdges();
  u_int              numv         = polyhedron.NumVertices();
  u_int              dimension    = polyhedron.Dimension();
  fixedarray<u_int>  temp_plist(  nume+numv);
  doublearray        temp_pcoord((nume+numv)*dimension);

  ijkgenpatch(polyhedron, vertex_sign, edge_list, num_simplices, temp_plist, temp_pcoord);
}

void ijkgenpatch(const ISOSURFACE_TABLE_POLYHEDRON &polyhedron 
                ,const fixedarray<int>             &vertex_sign
                ,vector<ISOSURFACE_VERTEX_INDEX>   &edge_list
                ,u_int                             &num_simplices
                ,fixedarray<u_int>                 &temp_plist
                ,doublearray                       &temp_pcoord)
// polyhedron = mesh polyhedron
// vertex_sign[iv] = vertex sign of vertex iv.  
//                   -1 = negative; 0 = zero; 1 = positive
// edge_list = return list of polyhedron edges
//   i'th simplex has vertices on edges
//      (edge_list[d*i], edge_list[d*i+1], ..., edge_list[d*i+d-1])
// temp_plist = temporary array for point list
//   Precondition: Preallocated to size at least 
//     polyhedron.NumEdges()+polyhedron.NumVertices()
// temp_pcoord = temporary array for point coordinates
//   Precondition: Preallocated to size at least
//     (polyhedron.NumEdges()+polyhedron.NumVertices())*polyhedron.Dimension()
{
  const u_int         dimension   = polyhedron.Dimension();
  fixedarray<u_int>  &point_list  = temp_plist;
  doublearray        &point_coord = temp_pcoord;

  num_simplices = 0;
  edge_list.clear();

  // add '+' vertices to point_list[]
  u_int nump = 0;
  for(u_int jv = 0; jv < polyhedron.NumVertices(); jv++) {
    if(vertex_sign[jv] >= 0) {
      // vertex jv is zero or positive
      for(u_int ic = 0; ic < dimension; ic++) {
        u_int j = nump*dimension+ic;
        point_coord[j] = polyhedron.VertexCoord(jv, ic);
      }
      point_list[nump] = jv;
      nump++;
    }
  }
  u_int num_pos_vertices = nump;

  // add midpoints of bipolar edges to point_list[]
  for(u_int ie = 0; ie < polyhedron.NumEdges(); ie++) {
    u_int iv0 = polyhedron.EdgeEndpoint(ie, 0);
    u_int iv1 = polyhedron.EdgeEndpoint(ie, 1);

    if((vertex_sign[iv0] >= 0 && vertex_sign[iv1] < 0) || (vertex_sign[iv0] < 0 && vertex_sign[iv1] >= 0)) {
      // edge connects positive and negative vertex
      // add edge midpoint to point_list[]
      for(u_int ic = 0; ic < dimension; ic++) {
        u_int j = nump*dimension+ic;
        point_coord[j] = polyhedron.MidpointCoord(ie, ic);
      }

      point_list[nump] = ie;
      nump++;
    }
  }

  Assert(nump <= polyhedron.NumEdges() + polyhedron.NumVertices());

  u_int convex_hull_dimension = 0;
  fixedarray<u_int> simplex_vert(0);
  u_int num_hull_simplices    = 0;
  ClarksonConvexHull hull(dimension, point_coord, nump);
  hull.clarkson_convex_hull(convex_hull_dimension, simplex_vert, num_hull_simplices);

  Assert(nump == 0 || convex_hull_dimension == dimension);

  for(u_int is = 0; is < num_hull_simplices; is++) {
    bool flag_isosurface_simplex = true;
    for(u_int ic = 0; ic < dimension; ic++) {
      if(simplex_vert[is*dimension + ic] < num_pos_vertices) {
        // ignore simplices sharing a polyhedron vertex
        flag_isosurface_simplex = false;
      }
    }

    // ignore simplices lying on polyhedron facets
    if (flag_isosurface_simplex) {
      long vertex_bits = 0L;
      for (u_int ic = 0; ic < dimension; ic++) {
        u_int ip = simplex_vert[is*dimension+ic];
        Assert (ip >= num_pos_vertices);
        set_vertex_bits(polyhedron, point_list, ip, num_pos_vertices,vertex_bits);
      }

      for(u_int jf = 0; jf < polyhedron.NumFacets(); jf++) {
        FACET f = polyhedron.Facet(jf);
        if((vertex_bits & (~f)) == 0) {
          flag_isosurface_simplex = false;
          break;
        }
      }
    }

    if(flag_isosurface_simplex) {
      add_simplex_to_isosurface(dimension, simplex_vert, is, point_list, edge_list);
    }
  }

  Assert(edge_list.size() % dimension == 0);
  num_simplices = edge_list.size()/dimension;
}

//**************************************************
// ijkgenpatch_nep
//**************************************************

void ijkgenpatch_nep(const ISOSURFACE_TABLE_POLYHEDRON &polyhedron 
                    ,const fixedarray<int>             &vertex_sign
                    ,vector<ISOSURFACE_VERTEX_INDEX>   &isov_list
                    ,vector<ISOSURFACE_VERTEX_TYPE>    &isov_type
                    ,u_int                             &num_simplices)
// polyhedron = mesh polyhedron
// vertex_sign[iv] = vertex sign of vertex iv.  
//                   -1 = negative; 0 = zero; 1 = positive
// isov_list = return list of isosurface vertices
//   i'th simplex has vertices
//      (isov_list[d*i], isov_list[d*i+1], ..., isov_list[d*i+d-1])
// isov_type[k] = isosurface vertex type corresponding to isov_list[k]
{
  u_int                                nume       = polyhedron.NumEdges();
  u_int                                numv       = polyhedron.NumVertices();
  u_int                                dimension  = polyhedron.Dimension();
  fixedarray<u_int>                    temp_plist(nume+numv);
  fixedarray<ISOSURFACE_VERTEX_TYPE>   temp_type( nume+numv);
  doublearray                          temp_pcoord((nume+numv)*dimension);

  ijkgenpatch_nep(polyhedron
                 ,vertex_sign
                 ,isov_list
                 ,isov_type
                 ,num_simplices
                 ,temp_plist
                 ,temp_type
                 ,temp_pcoord);
}

void ijkgenpatch_nep(const ISOSURFACE_TABLE_POLYHEDRON  &polyhedron
                    ,const fixedarray<int>              &vertex_sign
                    ,vector<ISOSURFACE_VERTEX_INDEX>    &isov_list
                    ,vector<ISOSURFACE_VERTEX_TYPE>     &isov_type
                    ,u_int                              &num_simplices
                    ,fixedarray<u_int>                  &temp_plist
                    ,fixedarray<ISOSURFACE_VERTEX_TYPE> &temp_type
                    ,doublearray                        &temp_pcoord)
// polyhedron = mesh polyhedron
// vertex_sign[iv] = vertex sign of vertex iv.  
//                   -1 = negative; 0 = zero; 1 = positive
// isov_list = return list of isosurface vertices
//   i'th simplex has vertices
//      (isov_list[d*i], isov_list[d*i+1], ..., isov_list[d*i+d-1])
//   if iv < polyhedron.NumVertices(), then isosurface vertex iv
//      equals polyhedron vertex iv
//   else iv represents an isosurface vertex on 
//      edge (iv-polyhedron.NumVertices())
// isov_type[k] = isosurface vertex type corresponding to isov_list[k]
// temp_plist = temporary array for point list
//   Precondition: Preallocated to size at least 
//     polyhedron.NumEdges()+polyhedron.NumVertices()
// temp_tlist = temporary array for isosurface vertex type
//   Precondition: Preallocated to size at least 
//     polyhedron.NumEdges()+polyhedron.NumVertices()
// temp_pcoord = temporary array for point coordinates
//   Precondition: Preallocated to size at least
//     (polyhedron.NumEdges()+polyhedron.NumVertices())*polyhedron.Dimension()
{
  const u_int                         dimension   = polyhedron.Dimension();
  const u_int                         poly_numv   = polyhedron.NumVertices();
  const u_int                         poly_nume   = polyhedron.NumEdges();
  fixedarray<u_int>                  &point_list  = temp_plist;
  fixedarray<ISOSURFACE_VERTEX_TYPE> &point_type  = temp_type;
  doublearray                        &point_coord = temp_pcoord;

  num_simplices = 0;

  isov_list.clear();
  isov_type.clear();

  if(poly_nume + poly_numv == 0) {
    return;
  }

  // add '+' or '0' vertices to point_list[]
  u_int nump = 0;
  for(u_int jv = 0; jv < poly_numv; jv++) {
    if(vertex_sign[jv] >= 0) {
      // vertex jv is zero or positive
      for(u_int ic = 0; ic < dimension; ic++) {
        const u_int j = nump*dimension+ic;
        point_coord[j] = polyhedron.VertexCoord(jv, ic);
      }

      point_list[nump] = jv;
      point_type[nump] = ISOSURFACE_VERTEX::VERTEX;
      nump++;
    }
  }
  u_int num_vert_in_point_list = nump;

  // add midpoints of bipolar edges to point_list[]
  for(u_int ie = 0; ie < polyhedron.NumEdges(); ie++) {
    u_int iv0 = polyhedron.EdgeEndpoint(ie, 0);
    u_int iv1 = polyhedron.EdgeEndpoint(ie, 1);

    if ((vertex_sign[iv0] > 0 && vertex_sign[iv1] < 0) || (vertex_sign[iv0] < 0 && vertex_sign[iv1] > 0)) {
      // edge connects positive and negative vertex
      // add edge midpoint to point_list[]
      for (u_int ic = 0; ic < dimension; ic++) {
        const u_int j = nump*dimension+ic;
        point_coord[j] = polyhedron.MidpointCoord(ie, ic);
      }

      point_list[nump] = ie;
      point_type[nump] = ISOSURFACE_VERTEX::EDGE;
      nump++;
    }
  }

  Assert(nump <= poly_nume + poly_numv);

  u_int  convex_hull_dimension = 0;
  fixedarray<u_int> simplex_vert(0);
  u_int  num_hull_simplices    = 0;
  ClarksonConvexHull hull(dimension, point_coord, nump);
  hull.clarkson_convex_hull(convex_hull_dimension, simplex_vert, num_hull_simplices);

  if(convex_hull_dimension == dimension) {
    for(u_int is = 0; is < num_hull_simplices; is++) {
      // ignore simplices lying on polyhedron facets
      long vertex_bits = 0L;
      for(u_int ic = 0; ic < dimension; ic++) {
        const u_int ip = simplex_vert[is*dimension+ic];
        set_vertex_bits(polyhedron, point_list, ip, num_vert_in_point_list, vertex_bits);
      }

      bool flag_isosurface_simplex = true;
      for(u_int jf = 0; jf < polyhedron.NumFacets(); jf++) {
        FACET f = polyhedron.Facet(jf);
        if((vertex_bits & (~f)) == 0) {
          flag_isosurface_simplex = false;
          break;
        }
      }

      if (flag_isosurface_simplex) {
        add_simplex_to_isosurface(dimension, simplex_vert, is, point_list, point_type, isov_list, isov_type);
      }
    }
  } else if(convex_hull_dimension+1 == dimension) {
    // check if points lie on polyhedron facet
    long vertex_bits = 0L;
    for(u_int ip = 0; ip < nump; ip++) {
      set_vertex_bits(polyhedron, point_list, ip, num_vert_in_point_list, vertex_bits);
    }

    bool flag_in_facet = false;
    for(u_int jf = 0; jf < polyhedron.NumFacets(); jf++) {
      FACET f = polyhedron.Facet(jf);
      if((vertex_bits & (~f)) == 0) {
        flag_in_facet = true;
        break;
      }
    }

    if(flag_in_facet && (nump < poly_numv + poly_nume)) {
      // points lie on a polyhedron facet
      // construct triangulation of the convex hull of the points
      //   (not just the boundary of the convex hull)

      // add another vertex to increase the hull dimension
      u_int lastp = nump;
      nump++;

      for(u_int jv = 0; jv < poly_numv; jv++) {
        if(vertex_sign[jv] < 0) {
          // vertex jv is negative
          for(u_int ic = 0; ic < dimension; ic++) {
            u_int j = lastp*dimension+ic;
            point_coord[j] = polyhedron.VertexCoord(jv, ic);
          }

          // add vertex jv to the point list
          point_list[lastp] = jv;
          point_type[lastp] = ISOSURFACE_VERTEX::VERTEX;

          simplex_vert.setSize(0);

          ClarksonConvexHull hull(dimension, point_coord, nump);
          hull.clarkson_convex_hull(convex_hull_dimension, simplex_vert, num_hull_simplices);

          if(convex_hull_dimension == dimension) {
            for(u_int is = 0; is < num_hull_simplices; is++) {
              // ignore simplices containing vertex point_list[lastp]
              bool flag_isosurface_simplex = true;
              for(u_int ic = 0; ic < dimension; ic++) {
                // ignore simplices incident on vertex point_list[lastp]
                u_int ip = simplex_vert[is*dimension+ic];
                if(ip == lastp) {
                  flag_isosurface_simplex = false;
                }
              }

              if(flag_isosurface_simplex) {
                add_simplex_to_isosurface(dimension, simplex_vert, is, point_list, point_type,isov_list, isov_type);
              }
            }

           break;
          }
        }
      }
    }
  }
  Assert(isov_list.size() % dimension == 0);
  num_simplices = isov_list.size()/dimension;
}

//**************************************************
// Subroutines
//**************************************************

  // add simplex "is" to isosurface
  // dimension = volume dimension
  // simplex_vert[] = array of simplex vertices
  // simplex_vert[is*dimension+j] = j'th vertex of simplex is
  // point_list[] = list of points
  // isov_list[] = array of isosurface simplex vertices
void add_simplex_to_isosurface(u_int                            dimension
                              ,const fixedarray<u_int>         &simplex_vert
                              ,u_int                            is
                              ,const fixedarray<u_int>         &point_list
                              ,vector<ISOSURFACE_VERTEX_INDEX> &isov_list) {
  for(u_int k = 0; k < dimension; k++) {
    u_int ip = simplex_vert[is*dimension+k];
    u_int isov = point_list[ip];
    isov_list.push_back(isov);
  }
}

// add simplex "is" to isosurface
// dimension = volume dimension
// simplex_vert[] = array of simplex vertices
// simplex_vert[is*dimension+j] = j'th vertex of simplex is
// point_list[] = list of points
// point_type[k] = type of point_list[k]
// isov_list[] = array of isosurface simplex vertices
// isov_type[k] = type of isov_list[k]
void add_simplex_to_isosurface(u_int                                     dimension
                              ,const fixedarray<u_int>                  &simplex_vert
                              ,u_int                                     is
                              ,const fixedarray<u_int>                  &point_list
                              ,const fixedarray<ISOSURFACE_VERTEX_TYPE> &point_type
                              ,vector<ISOSURFACE_VERTEX_INDEX>          &isov_list
                              ,vector<ISOSURFACE_VERTEX_TYPE>           &isov_type)
{
  for(u_int k = 0; k < dimension; k++) {
    u_int                  ip    = simplex_vert[is*dimension+k];
    u_int                  isov  = point_list[ip];
    ISOSURFACE_VERTEX_TYPE vtype = point_type[ip];

    isov_list.push_back(isov);
    isov_type.push_back(vtype);
  }
}
