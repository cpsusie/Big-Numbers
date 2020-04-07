/// \file ijktable.cxx
/// Class containing a table of isosurface patches in a given polyhedron.
/// Version 0.2.0

/*
  IJK: Isosurface Jeneration Kode
  Copyright (C) 2007, 2006, 2003, 2001 Rephael Wenger

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
#include <limits>
#include <limits.h>
#include <set>
#include "ijktable.h"

using namespace IJK;
using namespace IJKTABLE;

#ifndef LONG_BIT

#define LONG_BIT (CHAR_BIT * sizeof(long))

#endif

// local namespace
namespace {
  static const char * standard_encoding_name[] =    
    { "BINARY", "BASE3", "NONSTANDARD" };
}

//**************************************************
// ISOSURFACE_TABLE_POLYHEDRON
//**************************************************

ISOSURFACE_TABLE_POLYHEDRON::ISOSURFACE_TABLE_POLYHEDRON(uint  d)
: vertex_coord(0)
, edge_endpoint(0)
, num_facet_vertices(0)
, facet(0)
{
  Init();
  dimension = d;
}

ISOSURFACE_TABLE_POLYHEDRON::~ISOSURFACE_TABLE_POLYHEDRON() {
  FreeAll();
}

ISOSURFACE_TABLE_POLYHEDRON::ISOSURFACE_TABLE_POLYHEDRON(const ISOSURFACE_TABLE_POLYHEDRON &src)
  : vertex_coord(0)
  , edge_endpoint(0)
  , num_facet_vertices(0)
  , facet(0)
{
  Init();
  *this = src;
}

ISOSURFACE_TABLE_POLYHEDRON &ISOSURFACE_TABLE_POLYHEDRON::operator=(const ISOSURFACE_TABLE_POLYHEDRON &src) {
  if(&src != this) {         // avoid self-assignment
    FreeAll();
    dimension = src.Dimension();
    SetSize(src.NumVertices(), src.NumEdges(), src.NumFacets());

    // copy vertices
    for(uint  iv = 0; iv < NumVertices(); iv++) {
      for(uint  ic = 0; ic < Dimension(); ic++) {
        SetVertexCoord(iv, ic, src.VertexCoord(iv, ic));
      }
    }

    // copy edges
    for(uint  ie = 0; ie < NumEdges(); ie++) {
      SetEdge(ie, src.EdgeEndpoint(ie, 0), src.EdgeEndpoint(ie, 1));
    }
    /// **** NOTE: NEED TO MODIFY ***
    // copy facets
    for(uint  jf = 0; jf < NumFacets(); jf++) {
      facet[jf] = src.Facet(jf);
      uint  num_fv = src.NumFacetVertices(jf);
      SetNumFacetVertices(jf, num_fv);

      for(uint  k = 0; k < num_fv; k++) {
        uint  iv = src.FacetVertex(jf, k);
        SetFacetVertex(jf, k, iv);
      }
    }
  }
  return *this;
}

/// Free facet arrays.
void ISOSURFACE_TABLE_POLYHEDRON::FreeFacets() {
  if(facet_vertex_list != NULL) {
    for(uint  jf = 0; jf < num_facets; jf++) {
      SAFEDELETEARRAY(facet_vertex_list[jf]);
    }
  }
  SAFEDELETEARRAY(facet_vertex_list);
  num_facet_vertices.setSize(0);
  facet.setSize(0);
  num_facets         = 0;
}

/// Free all memory.
void ISOSURFACE_TABLE_POLYHEDRON::FreeAll() {
  FreeFacets();
  num_vertices = 0;
  num_edges    = 0;
  vertex_coord.setSize(0);
  edge_endpoint.setSize(0);
}

void ISOSURFACE_TABLE_POLYHEDRON::Init() {
  dimension          = 0;
  num_vertices       = 0;
  num_edges          = 0;
  num_facets         = 0;
  facet_vertex_list  = NULL;
}

// set polyhedron dimension
void ISOSURFACE_TABLE_POLYHEDRON::SetDimension(uint  d) {
  FreeAll();
  num_vertices = num_edges = 0;
  dimension    = d;
}

// set number of vertices
// Must be called before setting polyhedron vertices
void ISOSURFACE_TABLE_POLYHEDRON::SetNumVertices(uint  numv) {
  if(!CheckDimension()) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal polyhedron dimension");
  }
  FreeAll();
  num_vertices = num_edges = 0;

  if(numv == 0) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of vertices must be non-zero");
  }
  // Note that even if numv <= LONG_BIT, there may not be enough 
  //   memory to store the isosurface table.
  if(numv > LONG_BIT) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of polyhedron vertices is too large");
  }
  num_vertices = numv;
  vertex_coord.setSize(num_vertices*Dimension());
}

// set number of edges
// Must be called before setting polyhedron edges
void ISOSURFACE_TABLE_POLYHEDRON::SetNumEdges(uint  nume) {
  num_edges = 0;

  if(!CheckDimension()) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal dimension");
  }
  if(NumVertices() == 0) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of vertices must be set before number of edges");
  }
  if(nume < 1) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of edges must be non-zero");
  }
#ifdef max
#undef max
#endif
  if(nume > numeric_limits<EDGE_INDEX>::max()) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of polyhedron edges is too large");
  }
  num_edges     = nume;
  edge_endpoint.setSize(num_edges*2);
}

// set number of facets
// Must be called before setting polyhedron facets
void ISOSURFACE_TABLE_POLYHEDRON::SetNumFacets(uint  numf) {
  FreeFacets();

  if(!CheckDimension()) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal dimension");
  }
  if(NumVertices() == 0) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of vertices must be set before number of facets");
  }
  if(numf < 1) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of facets must be non-zero");
  }
  if(numf > numeric_limits<FACET_INDEX>::max()) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of polyhedron facets is too large");
  }
  num_facets         = numf;
  facet.setSize(numf);
  num_facet_vertices.setSize(numf);
  facet_vertex_list = new uint *[numf]; TRACE_NEW(facet_vertex_list);

  if(facet_vertex_list == NULL) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Unable to allocate memory for list of facets");
  }
  // initialize each facet to 0
  for(uint  jf = 0; jf < numf; jf++) {
    facet[jf] = 0;
    num_facet_vertices[jf] = 0;
    facet_vertex_list[jf] = NULL;
  }
}

/// Set number of vertices in facet \a jf.
void ISOSURFACE_TABLE_POLYHEDRON::SetNumFacetVertices(FACET_INDEX jf, uint  numv) {
  uint  jf_int = jf;
  Assert(jf_int < NumFacets());

  SAFEDELETEARRAY(facet_vertex_list[jf]);

  num_facet_vertices[jf] = numv;
  facet_vertex_list[jf]  = new uint[numv]; TRACE_NEW(facet_vertex_list[jf]);
}

// set polyhedron vertex coordinate
// iv = vertex index.  In range [0..NumVertices()-1].
// ic = coordinate index. In range [0..Dimension()-1].
// coord = coordinate.  Must be even.
void ISOSURFACE_TABLE_POLYHEDRON::SetVertexCoord(uint  iv, uint  ic, uint  coord) {
  Assert(iv < NumVertices());
  Assert(ic < Dimension());
  if(coord%2 != 0) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal vertex coordinate. Vertex coordinate must be even");
  }
  vertex_coord[iv*Dimension() + ic] = coord;
}

// set polyhedron edge coordinate
// ie = edge index.  In range [0..NumEdges()-1].
// iv0 = vertex 0 index.  In range [0..NumVertices()-1].
// iv1 = vertex 1 index.  In range [0..NumVertices()-1].
void ISOSURFACE_TABLE_POLYHEDRON::SetEdge(EDGE_INDEX ie, uint  iv0, uint  iv1) {
  uint  ie_int = ie;
  Assert(ie_int < NumEdges());
  Assert(iv0 < NumVertices() && iv1 < NumVertices());
  uint  index = 2 * ie;
  edge_endpoint[index++] = iv0;
  edge_endpoint[index  ] = iv1;
}

void ISOSURFACE_TABLE_POLYHEDRON::SetFacetVertex(FACET_INDEX jf, uint  k, uint  iv) {
  const uint  jf_int = jf;
  Assert(jf_int < NumFacets());
  Assert(k      < NumFacetVertices(jf));
  Assert(iv     < NumVertices());
  facet_vertex_list[jf_int][k] = iv;
  facet[jf_int] |= (1L << iv);
}

// return ic'th coordinate of midpoint of edge ie
// Note: vertice coordinates must all be even so midpoint coordinate is an integer
uint  ISOSURFACE_TABLE_POLYHEDRON::MidpointCoord(EDGE_INDEX ie, uint  ic) const {
  Assert((ie < num_edges) && (ic < dimension));
  uint  iv0    = EdgeEndpoint(ie, 0);
  uint  iv1    = EdgeEndpoint(ie, 1);
  uint  coord0 = VertexCoord(iv0, ic);
  uint  coord1 = VertexCoord(iv1, ic);
  return (coord0+coord1)/2;
}

// generate a polyhedron
void ISOSURFACE_TABLE_POLYHEDRON::GenCube(uint  cube_dimension) {
  dimension = cube_dimension;
  if(!CheckDimension()) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal cube dimension");
  }
  const uint  numv = 1L << Dimension();
  const uint  nume = (numv * Dimension()) / 2;
  const uint  numf = 2*Dimension();

  SetSize(numv, nume, numf);

  // set vertex coordinates
  for(uint  iv = 0; iv < NumVertices(); iv++) {
    uint  mask = 1L;
    for(uint  ic = 0; ic < Dimension(); ic++) {
      uint  bit   = mask & iv;
      uint  coord = 0;
      if(bit != 0) {
        coord = 2;
      }
      SetVertexCoord(iv, ic, coord);
      mask <<= 1;
    }
  }

  // generate edges in lexicographic order
  uint  ie = 0;
  uint  control = 0;
  while (ie < NumEdges()) {
    // find first 0 bit in control
    uint  ic    = 0;
    uint  mask = 1L;
    while((mask & control) != 0) {
      mask <<= 1;
      ic++;
    }

    // find start vertex by stripping ic bits from control
    uint  start = control;
    start = start >> ic;
    start = start << ic;
    const uint  icpow2 = (1L << ic);
    for(uint  i = 0; i < icpow2; i++) {
      uint  iv0 = start + i;
      uint  iv1 = iv0 + icpow2;
      SetEdge(ie, iv0, iv1);
      ie++;
    }
    control++;
  }

  if(control+1 != (1L << Dimension())) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Programming error in edge generation");
  }
  // generate facets
  const uint  num_vertices_per_facet = NumVertices()/2;

  for(uint  jf = 0; jf < numf; jf++) {
    SetNumFacetVertices(jf, num_vertices_per_facet);
  }

  uint  mask = 1L;
  for(uint  ic = 0; ic < Dimension(); ic++) {
    uint  jf0 = 2*ic;
    uint  jf1 = jf0+1;
    uint  k0  = 0;
    uint  k1  = 0;

    for(uint  iv = 0; iv < NumVertices(); iv++) {
      uint  bit = mask & iv;
      if(bit == 0) {
        SetFacetVertex(jf0, k0, iv);
        k0++;
      } else {
        SetFacetVertex(jf1, k1, iv);
        k1++;
      }
    }

    if(k0 != NumFacetVertices(jf0) || k1 != NumFacetVertices(jf1)) {
      throwPROCEDURE_ERROR(__FUNCTION__, "Programming error in facet generation");
    }
    mask <<= 1;
  }
}

// generate a simplex
void ISOSURFACE_TABLE_POLYHEDRON::GenSimplex(uint  simplex_dimension) {
  dimension = simplex_dimension;
  if(!CheckDimension())
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal simplex dimension");

  const uint  numv = Dimension() + 1;
  const uint  nume = (numv * Dimension()) / 2;
  const uint  numf = Dimension() + 1;

  SetSize(numv, nume, numf);

  // initialize all vertex coordinates to 0
  for(uint  iv = 0; iv < NumVertices(); iv++) {
    for(uint  ic = 0; ic < Dimension(); ic++) {
      SetVertexCoord(iv, ic, 0);
    }
  }

  // set vertex coordinates
  uint  ic = 0;
  for(uint  jv = 1; jv < NumVertices(); jv++) {
    SetVertexCoord(jv, ic, 2);
    ic++;
  }

  // generate edges in lexicographic order
  uint  ie = 0;
  for(uint  iv0 = 0; iv0 < NumVertices() - 1; iv0++) {
    for(uint  iv1 = iv0 + 1; iv1 < NumVertices(); iv1++) {
      SetEdge(ie, iv0, iv1);
      ie++;
    }
  }
  if(ie != nume) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Programming error in edge generation");
  }
  // generate facets
  const uint  num_vertices_per_facet = Dimension();

  for(uint  jf = 0; jf < numf; jf++) {
    SetNumFacetVertices(jf, num_vertices_per_facet);
  }

  for(uint  jf = 0; jf < numf; jf++) {
    uint  k = 0;
    for(uint  jv = 0; jv < num_vertices_per_facet; jv++) {
      uint  iv = (jf + jv) % numv;
      SetFacetVertex(jf, k, iv);
      k++;
    }

    if(k != NumFacetVertices(jf)) {
      throwPROCEDURE_ERROR(__FUNCTION__, "Programming error in facet generation");
    }
  }
}

// generate a pyramid
void ISOSURFACE_TABLE_POLYHEDRON::GenPyramid(uint  pyramid_dimension) {
  dimension = pyramid_dimension;
  CheckDimension();
  if(dimension < 2) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal pyramid dimension");
  }
  uint  numv = (1L << (Dimension()-1)) + 1;
  uint  num_base_edges = (numv-1) * (Dimension()-1)/2;
  uint  nume = num_base_edges + (numv-1);
  uint  numf = 2*Dimension()-1;

  uint  apex = numv-1;

  SetSize(numv, nume, numf);

  // set vertex coordinates
  for(uint  iv = 0; iv < NumVertices()-1; iv++) {
    uint  mask = 1L;
    for(uint  ic = 0; ic < Dimension(); ic++) {
      uint  bit = mask & iv;
      uint  coord = 0;
      if(bit != 0) {
        coord = 4;
      }
      SetVertexCoord(iv, ic, coord);
      mask <<= 1;
    }
  }

  // Set coordinate of pyramid
  for(uint  ic = 0; ic < Dimension(); ic++) {
    SetVertexCoord(apex, ic, 2);
  }

  // generate edges in lexicographic order
  uint  ie = 0;
  uint  control = 0;
  while(ie < num_base_edges) {
    // find first 0 bit in control
    uint  ic = 0;
    uint  mask = 1L;
    while((mask & control) != 0) {
      mask <<= 1;
      ic++;
    }

    // find start vertex by stripping ic bits from control
    uint  start = control;
    start = start >> ic;
    start = start << ic;
    const uint  icpow2 = (1L << ic);
    for(uint  i = 0; i < icpow2; i++) {
      const uint  iv0 = start + i;
      const uint  iv1 = iv0   + icpow2;
      SetEdge(ie, iv0, iv1);
      ie++;
    }
    control++;
  }

  if(control+1 != (1L << (Dimension()-1))) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Programming error in edge generation");
  }
  for(uint  iv = 0; iv < NumVertices()-1; iv++) {
    SetEdge(num_base_edges+iv, iv, apex);
  }

  // generate facets containing apex
  uint  num_vertices_per_facet = 1+(numv-1)/2;

  for(uint  jf = 0; jf+1 < numf; jf++) {
    SetNumFacetVertices(jf, num_vertices_per_facet);
  }

  uint  mask = 1L;
  for(uint  ic = 0; ic < Dimension()-1; ic++) {
    uint  jf0 = 2*ic;
    uint  jf1 = jf0+1;

    SetFacetVertex(jf0, 0, apex);
    SetFacetVertex(jf1, 0, apex);

    uint  k0 = 1;
    uint  k1 = 1;
    for(uint  iv = 0; iv < NumVertices()-1; iv++) {
      uint  bit = mask & iv;
      if(bit == 0) {
        SetFacetVertex(jf0, k0, iv);
        k0++;
      } else {
        SetFacetVertex(jf1, k1, iv);
        k1++;
      }
    }

    if(k0 != NumFacetVertices(jf0) || k1 != NumFacetVertices(jf1)) {
      throwPROCEDURE_ERROR(__FUNCTION__, "Programming error in facet generation");
    }
    mask <<= 1;
  }

  // generate base facet
  uint  base_facet = numf-1;
  uint  num_vertices_in_base_facet = numv-1;
  SetNumFacetVertices(base_facet, num_vertices_in_base_facet);

  for(uint  iv = 0; iv < NumVertices()-1; iv++) {
    SetFacetVertex(base_facet, iv, iv);
  }
}

// check dimension
bool ISOSURFACE_TABLE_POLYHEDRON::CheckDimension() const {
  return dimension >= 1;
}

// check polyhedron
void ISOSURFACE_TABLE_POLYHEDRON::Check() const {
  if(!CheckDimension()) {
    throwException("Illegal polyhedron dimension:%d", Dimension());
  }

  if(NumVertices() < 1) {
    throwException("Illegal number of vertices (=%d)", NumVertices());
  }

  if(NumEdges() < 1) {
    throwException("Illegal number of edges (=%d)", NumEdges());
  }

  for(uint  iv = 0; iv < NumVertices(); iv++) {
    for(uint  ic = 0; ic < Dimension(); ic++) {
      if((VertexCoord(iv, ic) % 2) != 0) {
        throwException("Vertex coordinates must be even integers");
      }
    }
  }

  for(uint  ie = 0; ie < NumEdges(); ie++) {
    for(uint  ip = 0; ip < 2; ip++) {
      uint  iv = EdgeEndpoint(ie, ip);
      if(iv >= NumVertices()) {
        throwException("Illegal edge endpoint %d for edge %d", iv, ie);
      }
    }
  }
}

//**************************************************
// ISOSURFACE_VERTEX
//**************************************************

ISOSURFACE_VERTEX::ISOSURFACE_VERTEX() { 
  coord = NULL; 
  num_coord = 0; 
  is_label_set = false; 
}

ISOSURFACE_VERTEX::~ISOSURFACE_VERTEX() {
  SAFEDELETEARRAY(coord);
  num_coord    = 0;
  is_label_set = false;
}

void ISOSURFACE_VERTEX::SetNumCoord(uint numc) {
  SAFEDELETEARRAY(coord);
  num_coord = numc;
  coord = new COORD_TYPE[numc];  TRACE_NEW(coord );
}

//**************************************************
// ISOSURFACE_TABLE
//**************************************************
ISOSURFACE_TABLE::ISOSURFACE_TABLE_ENTRY::ISOSURFACE_TABLE_ENTRY() {
  num_simplices = 0;
  simplex_vertex_list = NULL;
}

ISOSURFACE_TABLE::ISOSURFACE_TABLE_ENTRY::~ISOSURFACE_TABLE_ENTRY() {
  FreeAll();
}

void ISOSURFACE_TABLE::ISOSURFACE_TABLE_ENTRY::Check() const {
  if(num_simplices < 0) {
    throwException("Number of simplices in isosurface table entry must be non-negative");
  }

  if(num_simplices > 0 && simplex_vertex_list == NULL) {
    throwException("Memory for simplex vertex list not allocated");
  }
}

void ISOSURFACE_TABLE::ISOSURFACE_TABLE_ENTRY::FreeAll() {
  SAFEDELETEARRAY(simplex_vertex_list);
  num_simplices = 0;
}

// default constructor. dimension = 3
ISOSURFACE_TABLE::ISOSURFACE_TABLE() : polyhedron(3) {
  Init(3, 2);
}

// constructor
// d = dimension of space containing isosurface.  Should be 2, 3 or 4.
ISOSURFACE_TABLE::ISOSURFACE_TABLE(uint  d) : polyhedron(d) {
  Init(d, d-1);
}

// constructor
// d = dimension of space containing isosurface.  Should be 2, 3 or 4.
ISOSURFACE_TABLE::ISOSURFACE_TABLE(uint  dimension, uint  simplex_dimension):polyhedron(dimension) {
  Init(dimension, simplex_dimension);
}

// constructor
// d = dimension of space containing isosurface.  Should be 2, 3 or 4.
void ISOSURFACE_TABLE::Init(uint  dimension, uint  simplex_dimension) {
  this->simplex_dimension = simplex_dimension;

  max_num_vertices = 20;
  // Note: Even tables for polyhedron of this size are probably impossible 
  //   to compute/store

  num_isosurface_vertices = 0;
  isosurface_vertex       = NULL;

  encoding      = NONSTANDARD;
  encoding_name = StandardEncodingName(NONSTANDARD);

  num_table_entries  = 0;
  entry              = NULL;
  is_table_allocated = false;
  if(!CheckDimension()) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal polyhedron dimension");
  }
}

ISOSURFACE_TABLE::~ISOSURFACE_TABLE() {
  FreeAll();
}

string ISOSURFACE_TABLE::StandardEncodingName(const ENCODING encoding) {
  return standard_encoding_name[encoding];
}

void ISOSURFACE_TABLE::SetEncoding(const ENCODING encoding) {
  this->encoding = encoding;
  encoding_name = StandardEncodingName(encoding);
}

void ISOSURFACE_TABLE::SetNonstandardEncoding(const string & name) {
  this->encoding = NONSTANDARD;
  encoding_name = name;
}

void ISOSURFACE_TABLE::SetNumIsosurfaceVertices(uint  num_vertices) {
  SAFEDELETEARRAY(isosurface_vertex);
  num_isosurface_vertices = num_vertices;
  isosurface_vertex = new ISOSURFACE_VERTEX[num_vertices]; TRACE_NEW(isosurface_vertex);
}

// check allocation of array isosurface_vertices
// procname = calling procedure name, for error messages
// vstart = first vertex
// numv = number of vertices required
void ISOSURFACE_TABLE::CheckIsoVerticesAlloc(const char * procname, uint  vstart, uint  numv) {
  if(numv == 0) return;

  if(isosurface_vertex == NULL) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Set number of isosurface vertices before storing vertices");
  }

  if(numv+vstart > NumIsosurfaceVertices()) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Illegal isosurface vertex index");
  }
}

// store polyhedron vertices as isosurface vertices
// store polyhedron vertices starting at isosurface vertex vstart
void ISOSURFACE_TABLE::StorePolyVerticesAsIsoVertices(uint  vstart) {
  const uint  num_polyv = Polyhedron().NumVertices();
  CheckIsoVerticesAlloc(__FUNCTION__, vstart, num_polyv);

  for(uint  iv = 0; iv < num_polyv; iv++) {
    SetIsoVertexType(iv+vstart, ISOSURFACE_VERTEX::VERTEX);
    SetIsoVertexFace(iv+vstart, iv);
  }
}

// store polyhedron edges as isosurface vertices
// store polyhedron edges starting at isosurface vertex vstart
void ISOSURFACE_TABLE::StorePolyEdgesAsIsoVertices(uint  vstart) {
  const uint  num_polye = Polyhedron().NumEdges();
  CheckIsoVerticesAlloc(__FUNCTION__, vstart, num_polye);
  for(uint  ie = 0; ie < num_polye; ie++) {
    SetIsoVertexType(ie+vstart, ISOSURFACE_VERTEX::EDGE);
    SetIsoVertexFace(ie+vstart, ie);
  }
}

// store polyhedron facets as isosurface vertices
// store polyhedron facets starting at isosurface vertex vstart
void ISOSURFACE_TABLE::StorePolyFacetsAsIsoVertices(uint  vstart) {
  uint  num_polyf = Polyhedron().NumFacets();
  CheckIsoVerticesAlloc(__FUNCTION__, vstart, num_polyf);
  for(uint  jf = 0; jf < num_polyf; jf++) {
    SetIsoVertexType(jf+vstart, ISOSURFACE_VERTEX::FACET);
    SetIsoVertexFace(jf+vstart, jf);
  }
}

void ISOSURFACE_TABLE::SetNumTableEntries(uint  num_table_entries) {
  SAFEDELETEARRAY(entry);
  this->num_table_entries = 0;
  is_table_allocated = false;

  entry = new ISOSURFACE_TABLE_ENTRY[num_table_entries]; TRACE_NEW(entry);
  if(entry == NULL && num_table_entries > 0) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Unable to allocate memory for isosurface table");
  }
  this->num_table_entries = num_table_entries;
  is_table_allocated = true;
}


// set number of simplices in table entry it
// it = table entry
// nums = number of simplices
void ISOSURFACE_TABLE::SetNumSimplices(TABLE_INDEX it, uint  nums) {
  if(!IsTableAllocated() || entry == NULL) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Table must be allocated before entering table entries");
  }

  Assert(it < NumTableEntries());

  entry[it].num_simplices = 0;
  SAFEDELETE(entry[it].simplex_vertex_list);

  if(nums > 0) {
    entry[it].simplex_vertex_list = new ISOSURFACE_VERTEX_INDEX[nums*NumVerticesPerSimplex()]; TRACE_NEW(entry[it].simplex_vertex_list );
  }
  entry[it].num_simplices = nums;
}

// set simplex vertex
// it = index table entry.  In range [0..NumTableEntries()-1].
// is = index simplex.  
// k = k'th simplex vertex.  In range [0..NumVerticesPerSimplex()-1].
// isov = index of isosurface vertex
void ISOSURFACE_TABLE::SetSimplexVertex(TABLE_INDEX it, uint  is, uint  k, ISOSURFACE_VERTEX_INDEX isov) {
  entry[it].simplex_vertex_list[is*NumVerticesPerSimplex()+k] = isov;
}

// check dimension
bool ISOSURFACE_TABLE::CheckDimension(int d) const {
  return d >= 1;
}

void ISOSURFACE_TABLE::CheckTable() const {
  if(polyhedron.NumVertices() >= LONG_BIT) {
    throwException("Too many polyhedron vertices");
  }

  if(polyhedron.NumVertices() > MaxNumVertices()) {
    throwException("Too many polyhedron vertices");
  }

  if(polyhedron.NumVertices() < 1) {
    throwException("Polyhedron must have at least one vertex.");
  }

  if(entry == NULL) {
    throwException("Memory for isosurface table not allocated");
  }

  for(uint  it = 0; it < NumTableEntries(); it++) {
    try {
      entry[it].Check();
    } catch(Exception e) {
      throwException("Error detected at isosurface table entry %d:%s"
                    ,it, e.what());
    }
  }
  for(uint  jt = 0; jt < NumTableEntries(); jt++) {
    for(uint  is = 0; is < NumSimplices(jt); is++) {
      for(uint  iv = 0; iv < NumVerticesPerSimplex(); iv++) {
        int iso_v = int(SimplexVertex(jt, is, iv));
        if(iso_v < 0 || iso_v >= (int)NumIsosurfaceVertices()) {
          throwException("Illegal isosurface vertex %d in isosurface table entry %d", iso_v, jt);
        }
      }
    }
  }
}

void ISOSURFACE_TABLE::Check() const {
  Polyhedron().Check();
  CheckTable();
}

void ISOSURFACE_TABLE::FreeAll() {
  if(entry != NULL) {
    for(uint  i = 0; i < num_table_entries; i++) {
      entry[i].FreeAll();
    }
    SAFEDELETEARRAY(entry);
  }
  num_table_entries = 0;
  is_table_allocated = false;

  polyhedron.FreeAll();

  SAFEDELETEARRAY(isosurface_vertex);
  num_isosurface_vertices = 0;
}

//**************************************************
// ISOSURFACE_EDGE_TABLE
//**************************************************

ISOSURFACE_EDGE_TABLE::ISOSURFACE_EDGE_TABLE_ENTRY::ISOSURFACE_EDGE_TABLE_ENTRY() {
  num_edges = 0;
  edge_endpoint_list = NULL;
}

ISOSURFACE_EDGE_TABLE::ISOSURFACE_EDGE_TABLE_ENTRY::~ISOSURFACE_EDGE_TABLE_ENTRY() {
  FreeAll();
}

void ISOSURFACE_EDGE_TABLE::ISOSURFACE_EDGE_TABLE_ENTRY::Check() const {
  if(num_edges < 0) {
    throwException("Number of edges in isosurface table entry must be non-negative");
  }

  if(num_edges > 0 && edge_endpoint_list == NULL) {
    throwException("Memory for edge endpoint list not allocated");
  }
}

void ISOSURFACE_EDGE_TABLE::ISOSURFACE_EDGE_TABLE_ENTRY::FreeAll() {
  SAFEDELETEARRAY(edge_endpoint_list);
  num_edges = 0;
}

// constructor
// d = dimension of space containing isosurface.  Should be 2, 3 or 4.
ISOSURFACE_EDGE_TABLE::ISOSURFACE_EDGE_TABLE(const int d) : ISOSURFACE_TABLE(d) {
  Init(d);
}

// constructor
// d = dimension of space containing isosurface.  Should be 2, 3 or 4.
void ISOSURFACE_EDGE_TABLE::Init(const int d) {
  edge_entry = NULL;
}

ISOSURFACE_EDGE_TABLE::~ISOSURFACE_EDGE_TABLE() {
  FreeAll();
}

void ISOSURFACE_EDGE_TABLE::SetNumTableEntries(uint  num_table_entries) {
  ISOSURFACE_TABLE::SetNumTableEntries(num_table_entries);

  edge_entry = new ISOSURFACE_EDGE_TABLE_ENTRY[num_table_entries]; TRACE_NEW(edge_entry );
  if(edge_entry == NULL) {
    throwPROCEDURE_ERROR(__FUNCTION__, "Unable to allocate memory for isosurface edge table");
  }
}

void ISOSURFACE_EDGE_TABLE::CheckTable() const {
  ISOSURFACE_TABLE::CheckTable();

  if(edge_entry == NULL) {
    throwException("Memory for isosurface table edge entries not allocated");
  }

  for(uint  it = 0; it < NumTableEntries(); it++) {
    try {
      edge_entry[it].Check();
    } catch(Exception e) {
      throwException("Error detected at isosurface table entry %d:%s", it, e.what());
    }
  }
  for(uint  jt = 0; jt < NumTableEntries(); jt++) {
    for(uint  ie = 0; ie < NumEdges(jt); ie++) {
      for(uint  iend = 0; iend < 2; iend++) {
        int iso_v = int(EdgeEndpoint(jt, ie, iend));
        if(iso_v < 0 || iso_v >= (int)NumIsosurfaceVertices()) {
          throwException("Illegal isosurface vertex %d in isosurface table edge entry %d", iso_v, jt);
        }
      }
    }
  }
}

void ISOSURFACE_EDGE_TABLE::Check() const {
  Polyhedron().Check();
  CheckTable();
}

void ISOSURFACE_EDGE_TABLE::ISOSURFACE_EDGE_TABLE::FreeAll() {
  if(edge_entry != NULL) {
    for(uint  i = 0; i < num_table_entries; i++) {
      edge_entry[i].FreeAll();
    }
    SAFEDELETEARRAY(edge_entry);
  }
  ISOSURFACE_TABLE::FreeAll();
}

namespace {
  class EDGE_CONTAINER { 
  public:
    EDGE_INDEX v[2]; 

    EDGE_CONTAINER() {
    }
    inline bool operator<(const EDGE_CONTAINER &e) const {
      return (v[0] < e.v[0]) || (v[0] == e.v[0] && v[1] < e.v[1]);
    }
  };
};

// for each table entry, generate edge lists from simplex lists
void ISOSURFACE_EDGE_TABLE::GenEdgeLists() {
  fixedarray<EDGE_INDEX> vlist(Dimension());
  typedef set<EDGE_CONTAINER> EDGE_SET;
  EDGE_SET eset;

  if(!IsTableAllocated()) {
    throwPROCEDURE_ERROR(__FUNCTION__,"Programming error: Isosurface table not allocated");
  }

  for(uint  it = 0; it < NumTableEntries(); it++) {
    eset.clear();

    for(uint  is = 0; is < NumSimplices(it); is++) {
      // get simplex vertices
      for(uint  iv = 0; iv < Dimension(); iv++) {
        vlist[iv] = SimplexVertex(it, is, iv);
      }
      sort(&vlist[0], &vlist[0]+Dimension());

      // store simplex edges
      for(uint  i0 = 0; i0 < Dimension(); i0++) {
        for(uint  i1 = i0+1; i1 < Dimension(); i1++) {
          EDGE_CONTAINER e;
          e.v[0] = vlist[i0];
          e.v[1] = vlist[i1];
          eset.insert(e);
        }
      }
    }

    edge_entry[it].FreeAll();
    if(eset.size() > 0) {
      edge_entry[it].edge_endpoint_list = new EDGE_INDEX[2*eset.size()]; TRACE_NEW(edge_entry[it].edge_endpoint_list );
      if(edge_entry[it].edge_endpoint_list == NULL) {
        throwPROCEDURE_ERROR(__FUNCTION__, "Unable to allocate memory for edge list in table entry %d", it);
      }
      edge_entry[it].num_edges = eset.size();

      int ie = 0;
      for(EDGE_SET::iterator edge_iter = eset.begin(); edge_iter != eset.end(); edge_iter++) {
        edge_entry[it].edge_endpoint_list[2*ie  ] = edge_iter->v[0];
        edge_entry[it].edge_endpoint_list[2*ie+1] = edge_iter->v[1];
        ie++;
      }
    }
  }
}

//**************************************************
// AMBIGUITY INFORMATION FOR ISOSURFACE TABLE
//**************************************************

void ISOSURFACE_TABLE_AMBIG_INFO::Init() {
  num_table_entries    = 0;
  is_ambiguous         = NULL;
  num_ambiguous_facets = NULL;
  ambiguous_facet      = NULL;
}

ISOSURFACE_TABLE_AMBIG_INFO::~ISOSURFACE_TABLE_AMBIG_INFO() {
  FreeAll();
}

void ISOSURFACE_TABLE_AMBIG_INFO::FreeAll() {
  SAFEDELETEARRAY(is_ambiguous        );
  SAFEDELETEARRAY(num_ambiguous_facets);
  SAFEDELETEARRAY(ambiguous_facet     );
  num_table_entries    = 0;
}

/// Allocate memory.
/// Free any previously allocated memory.
void ISOSURFACE_TABLE_AMBIG_INFO::Alloc(uint  num_table_entries) {
  if(this->num_table_entries > 0) {
    FreeAll();
  }

  is_ambiguous         = new bool[num_table_entries];        TRACE_NEW(is_ambiguous         );
  num_ambiguous_facets = new FACET_INDEX[num_table_entries]; TRACE_NEW(num_ambiguous_facets );
  ambiguous_facet      = new FACET_SET[num_table_entries];   TRACE_NEW(ambiguous_facet      );

  this->num_table_entries = num_table_entries;
}

/// Compute ambiguity information.
void ISOSURFACE_TABLE_AMBIG_INFO::ComputeAmbiguityInformation(const ISOSURFACE_TABLE &isotable) {
  const int poly_numv = isotable.Polyhedron().NumVertices();
  fixedarray<int> vertex_sign(poly_numv);

  if(isotable.Encoding() != ISOSURFACE_TABLE::BINARY) {
    throwPROCEDURE_ERROR(__FUNCTION__,"Programming error. Only binary encoding is currently accepted");
  }

  // Allocate memory.
  Alloc(isotable.NumTableEntries());

  for(TABLE_INDEX i = 0; i < isotable.NumTableEntries(); i++) {
    convert2base(i, 2, vertex_sign, isotable.Polyhedron().NumVertices());
    is_ambiguous[i] = is_poly_ambiguous(isotable.Polyhedron(), vertex_sign);
    compute_ambiguous_facets(isotable.Polyhedron(), vertex_sign, ambiguous_facet[i], num_ambiguous_facets[i]);
  }
}

//**************************************************
// ISOSURFACE TABLE WITH AMBIGUITY INFORMATION
//**************************************************
void ISOSURFACE_TABLE_AMBIG::Init() {
  is_ambiguous         = NULL;
  num_ambiguous_facets = NULL;
  ambiguous_facet      = NULL;
}

ISOSURFACE_TABLE_AMBIG::~ISOSURFACE_TABLE_AMBIG() {
  FreeAll();
}

void ISOSURFACE_TABLE_AMBIG::FreeAll() {
  SAFEDELETEARRAY(is_ambiguous        );
  SAFEDELETEARRAY(num_ambiguous_facets);
  SAFEDELETEARRAY(ambiguous_facet     );
}

// allocate table
void ISOSURFACE_TABLE_AMBIG::SetNumTableEntries(uint  num_table_entries) {
  ISOSURFACE_TABLE::SetNumTableEntries(num_table_entries);
  FreeAll();

  is_ambiguous         = new bool[num_table_entries];        TRACE_NEW(is_ambiguous         );
  num_ambiguous_facets = new FACET_INDEX[num_table_entries]; TRACE_NEW(num_ambiguous_facets );
  ambiguous_facet      = new FACET_SET[num_table_entries];   TRACE_NEW(ambiguous_facet      );

  if(num_table_entries > 0) {
    if(is_ambiguous == NULL || num_ambiguous_facets == NULL || ambiguous_facet == NULL) {
      throwPROCEDURE_ERROR(__FUNCTION__,"Unable to allocate memory for ambiguity information");
    }
  }
}

/// Compute ambiguity information.
void ISOSURFACE_TABLE_AMBIG::ComputeAmbiguityInformation() const {
  fixedarray<int> vertex_sign(Polyhedron().NumVertices());

  if(Encoding() != BINARY) {
    throwPROCEDURE_ERROR(__FUNCTION__,"Programming error.  Only binary encoding is currently accepted");
  }

  for(TABLE_INDEX i = 0; i < NumTableEntries(); i++) {
    convert2base(i, 2, vertex_sign, Polyhedron().NumVertices());
    is_ambiguous[i] = ComputeAmbiguous(vertex_sign);
    ComputeAmbiguousFacets(vertex_sign, ambiguous_facet[i], num_ambiguous_facets[i]);
  }
}

bool ISOSURFACE_TABLE_AMBIG::ComputeAmbiguous(const fixedarray<int> &vertex_sign) const {
  return(is_poly_ambiguous(Polyhedron(), vertex_sign));
}

/// Compute set of ambiguous facets.
void ISOSURFACE_TABLE_AMBIG::ComputeAmbiguousFacets(const fixedarray<int> &vertex_sign, FACET_SET & facet_set, FACET_INDEX & num_ambiguous_facets) const {
  compute_ambiguous_facets(Polyhedron(), vertex_sign, facet_set, num_ambiguous_facets);
}

//**************************************************
// Routines for generating polyhedra
//**************************************************

// generate a prism with base base_polyhedron
// first numv vertices have last coordinate = 0
// last numv vertices have last coordinate = 2
// first nume edges connect first numv vertices
// second nume edges connect second numv vertices
// third numv edges connect first to second set of vertices
// (numv = # vertices in base_polyhedron; nume = # edges in base_polyhedron)
void IJKTABLE::generate_prism(const ISOSURFACE_TABLE_POLYHEDRON &base_polyhedron, ISOSURFACE_TABLE_POLYHEDRON &prism) {
  uint  dim  = base_polyhedron.Dimension();
  uint  numc = dim;
  uint  numv = base_polyhedron.NumVertices();
  uint  nume = base_polyhedron.NumEdges();
  uint  numf = base_polyhedron.NumFacets();
  uint  prism_dim   = dim + 1;
  uint  prism_numc  = prism_dim;
  uint  prism_lastc = prism_numc - 1;
  uint  prism_numv  = numv * 2;
  uint  prism_nume  = nume * 2 + numv;
  uint  prism_numf  = 2 + numf;
  prism.SetDimension(prism_dim);
  prism.SetSize(prism_numv, prism_nume, prism_numf);

  // set prism vertex coord
  for(uint  iv = 0; iv < numv; iv++) {
    for(uint  ic = 0; ic < prism_lastc; ic++) {
      uint  coord = base_polyhedron.VertexCoord(iv, ic);
      prism.SetVertexCoord(iv, ic, coord);
      prism.SetVertexCoord(iv+numv, ic, coord);
    }
    prism.SetVertexCoord(iv, prism_lastc, 0);
    prism.SetVertexCoord(iv+numv, prism_lastc, 2);
  }

  // set edges
  for(uint  ie = 0; ie < base_polyhedron.NumEdges(); ie++) {
    uint  iv0 = base_polyhedron.EdgeEndpoint(ie, 0);
    uint  iv1 = base_polyhedron.EdgeEndpoint(ie, 1);
    prism.SetEdge(ie, iv0, iv1);
    prism.SetEdge(ie+nume, iv0+numv, iv1+numv);
  }

  for(uint  iv = 0; iv < base_polyhedron.NumVertices(); iv++) {
    uint  ie = 2*nume + iv;
    prism.SetEdge(ie, iv, iv+numv);
  }

  // set facets
  prism.SetNumFacetVertices(0, numv);
  prism.SetNumFacetVertices(1, numv);

  for(uint  iv = 0; iv < numv; iv++) {
    // set two base facets
    prism.SetFacetVertex(0, iv, iv);
    prism.SetFacetVertex(1, iv, iv+numv);
  }

  for(uint  jf = 0; jf < numf; jf++) {
    // set prism facet corresponding to original facet jf
    uint  prism_jf = 2 + jf;
    uint  base_num_fv = base_polyhedron.NumFacetVertices(jf);
    prism.SetNumFacetVertices(prism_jf, 2*base_num_fv);

    for(uint  k = 0; k < base_num_fv; k++) {
      uint  iv = base_polyhedron.FacetVertex(jf, k);
      prism.SetFacetVertex(prism_jf, k, iv);
      prism.SetFacetVertex(prism_jf, k+base_num_fv, iv+numv);
    }
  }
}

//**************************************************
// UTILITY FUNCTIONS
//**************************************************

  // calculate num table entries = (num_colors)^(num_vert)
uint  IJKTABLE::calculate_num_entries(uint  num_vert, uint  num_colors) {
  uint  num_table_entries = 0;
  if(num_colors < 1)
    throwPROCEDURE_ERROR(__FUNCTION__, "Number of colors must be positive");

  const uint  max2 = ULONG_MAX/num_colors;

  num_table_entries = 1;
  for(uint  iv = 0; iv < num_vert; iv++) {
    if(num_table_entries > max2) {
      throwPROCEDURE_ERROR(__FUNCTION__, "Number of entries is too large");
    }
    num_table_entries = num_table_entries * num_colors;
  }
  return num_table_entries;
}

void IJKTABLE::convert2base(ulong  ival, uint  base, fixedarray<int> &digit, uint  max_num_digits) {
  ulong  jval = ival;
  for(uint  i = 0; i < max_num_digits; i++) {
    digit[i] = jval % base;
    jval = jval / base;
  }

  if(jval != 0) {
    throwException("Error converting %d to base %d", ival, base);
  }
}

//**************************************************
// AMBIGUITY ROUTINES
//**************************************************

/// Return true if isosurface topology is ambiguous
/// @param vertex_sign[i] = Sign of isosurface vertex i. (0 or 1);
bool IJKTABLE::is_poly_ambiguous(const IJKTABLE::ISOSURFACE_TABLE_POLYHEDRON & poly, const fixedarray<int> &vertex_sign) {
  const int numv = poly.NumVertices();
  int       num0 = compute_num_connected(poly, 0, vertex_sign);
  int       num1 = 0;
  for(int i = 0; i < numv; i++) {
    if(vertex_sign[i] != vertex_sign[0]) {
      num1 = compute_num_connected(poly, i, vertex_sign);
      break;
    }
  }
  return num0 + num1 != numv;
}

/// Return true if facet jf is ambiguous.
/// @param vertex_sign[i] = Sign of isosurface vertex i.
bool IJKTABLE::is_facet_ambiguous(const IJKTABLE::ISOSURFACE_TABLE_POLYHEDRON & poly, const IJKTABLE::FACET_INDEX ifacet, const fixedarray<int> &vertex_sign) {
  const int numv = poly.NumVertices();
  int       num0 = 0;
  int       num1 = 0;
  for(int iv = 0; iv < numv; iv++) {
    if(poly.IsVertexInFacet(ifacet, iv) && vertex_sign[iv] == 0) {
      num0 = compute_num_connected_in_facet(poly, ifacet, iv, vertex_sign);
      break;
    }
  }

  for(int iv = 0; iv < numv; iv++) {
    if(poly.IsVertexInFacet(ifacet, iv) && vertex_sign[iv] == 1) {
      num1 = compute_num_connected_in_facet(poly, ifacet, iv, vertex_sign);
      break;
    }
  }
  return num0 + num1 != poly.NumFacetVertices(ifacet);
}

/// Return number of vertices connected by edges to iv with same sign as iv.
/// @param vertex_sign[i] = Sign of isosurface vertex i.
uint  IJKTABLE::compute_num_connected(const IJKTABLE::ISOSURFACE_TABLE_POLYHEDRON &poly, uint  iv, const fixedarray<int> &vertex_sign) {
  const uint  numv  = poly.NumVertices();
  const uint  nume  = poly.NumEdges();
  const int   isign = vertex_sign[iv];
  fixedarray<bool> visited(numv);

  for(uint  j = 0; j < numv; j++) {
    visited[j] = false;
  }
  visited[iv] = true;

  bool found_next = false;
  do {
    found_next = false;
    for(uint  k = 0; k < nume; k++) {
      uint  iv0 = poly.EdgeEndpoint(k, 0);
      uint  iv1 = poly.EdgeEndpoint(k, 1);
      if(vertex_sign[iv0] == isign && vertex_sign[iv1] == isign) {
        if(visited[iv0] && !visited[iv1]) {
          visited[iv1] = true;
          found_next   = true;
        } else if(!visited[iv0] && visited[iv1]) {
          visited[iv0] = true;
          found_next   = true;
        }
      }
    }
  } while(found_next);
  
  uint  count = 0;
  for(uint  j = 0; j < numv; j++) {
    if(visited[j]) {
      count++;
    }
  }
  return count;
}

/// Compute ambiguous facets.
/// @param vertex_sign[i] = Sign of isosurface vertex i.
void IJKTABLE::compute_ambiguous_facets(const ISOSURFACE_TABLE_POLYHEDRON & poly, const fixedarray<int> &vertex_sign, FACET_SET & facet_set, FACET_INDEX & num_ambiguous_facets) {
  const int numf = poly.NumFacets();
  num_ambiguous_facets = 0;
  facet_set            = 0;

  for(int ifacet = 0; ifacet < numf; ifacet++) {
    if(is_facet_ambiguous(poly, ifacet, vertex_sign)) {
      facet_set = facet_set | ((1L) << ifacet);
      num_ambiguous_facets++;
    }
  }
}

/// Return number of vertices connected by edges in facet jf to vertex iv with same sign as iv.
/// @param jf = Facet index.
/// @param iv = Vertex index.  Precondition: Vertex iv is in facet jf.
/// @param vertex_sign[i] = Sign of isosurface vertex i.
uint  IJKTABLE::compute_num_connected_in_facet(const IJKTABLE::ISOSURFACE_TABLE_POLYHEDRON &poly, FACET_INDEX jf, uint  iv, const fixedarray<int> &vertex_sign) {
  const uint  numv  = poly.NumVertices();
  const uint  nume  = poly.NumEdges();
  const int   isign = vertex_sign[iv];
  fixedarray<bool> visited(numv);

  for(uint  j = 0; j < numv; j++) {
    visited[j] = false;
  }
  visited[iv] = true;

  bool found_next = false;
  do {
    found_next = false;
    for(uint  k = 0; k < nume; k++) {
      uint  iv0 = poly.EdgeEndpoint(k, 0);
      uint  iv1 = poly.EdgeEndpoint(k, 1);
      if(vertex_sign[iv0] == isign && vertex_sign[iv1] == isign) {
        if(poly.IsVertexInFacet(jf, iv0) && poly.IsVertexInFacet(jf, iv1)) {
          if(visited[iv0] && !visited[iv1]) {
            visited[iv1] = true;
            found_next   = true;
          } else if(!visited[iv0] && visited[iv1]) {
            visited[iv0] = true;
            found_next   = true;
          }
        }
      }
    }
  } while(found_next);
  
  uint  count = 0;
  for(uint  j = 0; j < numv; j++) {
    if(visited[j]) {
      count++;
    }
  }
  return count;
}
