/// \file ijktable.h
/// Class containing a table of isosurface patches in a given polyhedron.
/// All 2^numv +/- patterns are stored in the table 
///   where numv = # polyhedron vertices.
/// Version 0.3.0

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

#pragma once

#include <iostream>
#include "ijk.h"

/// Classes and routines for storing and manipulating isosurface lookup table.
namespace IJKTABLE {

  typedef unsigned char ISOSURFACE_VERTEX_INDEX;  ///< Index of isosurface vertex.
  typedef unsigned char EDGE_INDEX;    ///< Index of edge.
  typedef unsigned char FACET_INDEX;   ///< Index of facet.
  typedef uint  TABLE_INDEX;           ///< Index of entry in isosurface lookup table.
  typedef uint  FACET;                 ///< Bits representing vertices in facet.
  typedef uint  FACET_SET;             ///< Bits representing set of facets.

  const int NO_VERTEX = -1;

//**************************************************
// ISOSURFACE TABLE POLYHEDRON
//**************************************************

/// Isosurface table polyhedron.
class ISOSURFACE_TABLE_POLYHEDRON {
protected:
  uint              dimension;          /// Polyhedron dimension.
  uint              num_vertices;       /// Number of polyhedron vertices.
  uint              num_edges;          /// Number of polyhedron edges.
  uint              num_facets;         /// Number of polyhedron facets.
  fixedarray<uint>  vertex_coord;       /// Polyhedron vertex coordinates.
  fixedarray<uint>  edge_endpoint;      /// Polyhedron edge endpoints.
  fixedarray<uint>  num_facet_vertices; /// Number of vertices of each facet.
  uint            **facet_vertex_list;  /// List of vertices in each facet.
  fixedarray<FACET> facet;              /// Polyhedron facets.
  void Init();                          /// Initialize.
  void FreeFacets();                    /// Free all facet arrays.

public:
  ISOSURFACE_TABLE_POLYHEDRON(const uint  d);
  ~ISOSURFACE_TABLE_POLYHEDRON();
  ISOSURFACE_TABLE_POLYHEDRON(const ISOSURFACE_TABLE_POLYHEDRON &src);
  ISOSURFACE_TABLE_POLYHEDRON &operator=(const ISOSURFACE_TABLE_POLYHEDRON &src);

   // Polyhedron dimension.
  inline uint  Dimension() const  {
    return dimension;
  }
  // Number of polyhedron vertices.
  inline uint  NumVertices() const {
    return num_vertices;
  }
  // Number of polyhedron edges.
  inline uint  NumEdges() const {
    return num_edges;
  }
  // Number of polyhedron facets.
  inline uint  NumFacets() const {
    return num_facets;
  }
  // Number of facet vertices of facet jf.
  inline uint  NumFacetVertices(FACET_INDEX jf) const {
    Assert(jf < num_facets);
    return num_facet_vertices[jf];
  }
  // ic'th vertex coordinate of vertex iv.
  inline uint  VertexCoord(uint  iv, uint  ic) const {
    Assert((iv < num_vertices) && (ic < dimension));
    return vertex_coord[iv*dimension + ic];
  }
  // j'th endpoint of edge ie. j = 0 or 1.
  inline uint  EdgeEndpoint(EDGE_INDEX ie, uint  j) const {
    Assert((j < 2) && (ie < num_edges));
    return edge_endpoint[(uint )ie*2 + j];
  }
  // ic'th coordinate of midpoint of edge ie.
  uint  MidpointCoord(EDGE_INDEX ie, uint  ic) const;
  // Bits representing vertices in facet jf.
  FACET Facet(FACET_INDEX jf) const {
    Assert(jf < num_facets);
    return facet[jf];
  }
  // Return true if vertex iv is in facet jf.
  bool IsVertexInFacet(FACET_INDEX jf, uint  iv) const {
    Assert((jf < num_facets) && (iv < num_vertices));
    return facet[jf] & ((1L) << iv);
  }
  // Return k'th vertex in facet jf.
  uint  FacetVertex(FACET_INDEX jf, uint  k) const {
    Assert((jf < num_facets) && (k < num_facet_vertices[jf]));
    return facet_vertex_list[jf][k];
  }

  void SetDimension(  uint  d   ); // Set polyhedron dimension.
  void SetNumVertices(uint  numv); // Set number of polyhedron vertices.
  void SetNumEdges(   uint  nume); // Set number of polyhedron edges.
  void SetNumFacets(  uint  numf); // Set number of polyhedron facets.

  // Set number of polyhedron vertices, edges and facets.
  void SetSize(uint  numv, uint  nume, uint  numf) {
    SetNumVertices(numv);
    SetNumEdges(   nume);
    SetNumFacets(  numf);
  }

  // Set ic'th coordinate of vertex iv.
  // SetNumVertices or SetSize must be called before SetVertexCoord.
  void SetVertexCoord(uint  iv, uint  ic, uint  coord);

  // Set endpoints of edge ie.
  // SetNumEdges or SetSize must be called before SetEdge.
  void SetEdge(EDGE_INDEX ie, uint  iv0, uint  iv1);

  // Set number of vertices in facet jf.
  // SetNumFacets or SetSize must be called before SetNumFacetVertices.
  void SetNumFacetVertices(FACET_INDEX jf, uint  numv);

  // Set \a k'th facet vertex of facet jf to vertex iv.
  // SetNumFacetVertices(jf) must be called before SetFacetVertex.
  void SetFacetVertex(FACET_INDEX jf, uint  k, uint  iv);

  // Memory Management Functions
  void FreeAll();

  bool CheckDimension() const;
  void Check() const;

  // Generate a square, cube or hypercube.
  void GenCube(uint  cube_dimension); 

  // Generate a triangle, tetrahedron or simplex.      
  void GenSimplex(uint  simplex_dimension);

  // Generate a pyramid over a square, cube or hypercube base.
  void GenPyramid(uint  pyramid_dimension);

};

typedef ISOSURFACE_TABLE_POLYHEDRON * ISOSURFACE_TABLE_POLYHEDRON_PTR;

//**************************************************
// ISOSURFACE VERTEX
//**************************************************

/// Isosurface vertex class.
class ISOSURFACE_VERTEX {

public:
  typedef enum {
    VERTEX
   ,EDGE
   ,FACET
   ,POINT
  } ISOSURFACE_VERTEX_TYPE;
  typedef float COORD_TYPE;

protected:
  ISOSURFACE_VERTEX_TYPE vtype;
  uint                   face;
  uint                   num_coord;
  COORD_TYPE            *coord;
  string                 label;
  bool                   is_label_set;

public:
  ISOSURFACE_VERTEX();        // constructor
  ~ISOSURFACE_VERTEX();       // destructor

  // Get Functions
  /// Return isosurface vertex type.
  ISOSURFACE_VERTEX_TYPE Type() const { 
    return vtype;
  }

  /// Return index of face (vertex, edge, facet) containing isosurface.
  /// Valid only if vertex type is VERTEX, EDGE or FACET.
  uint  Face() const {
    return face;
  }

  /// Return coordinate of isosurface vertex.
  /// Valid only if vertex type is POINT.
  COORD_TYPE Coord(uint  d) const {
    return coord[d];
  }

  /// Return number of isosurface vertex coordinates.
  /// Valid only if vertex type is POINT.
  uint  NumCoord() const {
    return num_coord;
  }

  /// Return label of isosurface vertex.
  /// Used for extending vertex types.
  string Label() const {
    return label;
  }

  /// Return true if label is set.
  bool IsLabelSet() const {
    return is_label_set;
  }

  // Set Functions
  void SetType(ISOSURFACE_VERTEX_TYPE t) {
    vtype = t;
  }
  void SetFace(uint  index) {
    face = index;
  }
  void SetNumCoord(uint  numc);
  void SetCoord(uint  ic, COORD_TYPE c) {
    Assert(ic < num_coord);
    coord[ic] = c;
  }
  void SetLabel(const string &s) {
    label        = s;
    is_label_set = true;
  }
};

// **************************************************
// ISOSURFACE TABLE
// **************************************************

/// Isosurface lookup table.
/// Stores isosurface patches for each configuration 
///   of +/- labels at polyhedron vertices.
class ISOSURFACE_TABLE {
protected:

  /// Entry in the isosurface lookup table.
  class ISOSURFACE_TABLE_ENTRY {
  public:
    uint                      num_simplices;
    ISOSURFACE_VERTEX_INDEX  *simplex_vertex_list;
    ISOSURFACE_TABLE_ENTRY();
    ~ISOSURFACE_TABLE_ENTRY();

    void Check() const;
    void FreeAll();
  };

public:
  /// Configuration encodings.
  /// Standard Marching Cubes lookup table with "-/+" vertex labels 
  ///   uses binary encoding for "-/+".
  /// Interval Volume lookup table uses base 3 encoding for "-/*/+"
  ///   where '*' represents a scalar value between the two isovalues.
  /// NEP (negative-equals-positive) isosurface lookup tables
  ///   with "-/=/+" labels uses base 3 encoding for "-/=/+".
  typedef enum {BINARY, BASE3, NONSTANDARD} ENCODING;
  // Programmer's note: Update standard_encoding_name[] in ijktable.cxx
  //   whenever ENCODING is changed.

  /// Index of entry in isosurface lookup table.
  /// Define within ISOSURFACE_TABLE for use in templates.
  typedef IJKTABLE::TABLE_INDEX TABLE_INDEX;    

protected:
  ENCODING                    encoding;                // type of encoding
  string                      encoding_name;           // string storing encoding name

  ISOSURFACE_TABLE_POLYHEDRON polyhedron;              /// Mesh polyhedron.
  uint                        simplex_dimension;       /// Simplex dimension.
  ISOSURFACE_VERTEX          *isosurface_vertex;       /// Array of isosurface vertex descriptors.
  uint                        num_isosurface_vertices; /// Number of vertices in array isosurface_vertex[].
  ISOSURFACE_TABLE_ENTRY     *entry;                   ///< Array of isosurface table entries.
  ulong                       num_table_entries;       ///< Number of entries in table.

  /// Maximum number of vertices allowed for table polyhedron.
  uint                        max_num_vertices; 
  bool                        is_table_allocated;      ///< True, if array num_table_entries[] is allocated.

  /// Check if isosurface vertices are allocated.
  /// Throw error if not enough isosurface vertices are allocated.
  void CheckIsoVerticesAlloc(const char * procname, uint  vstart, uint  numv);

  /// Initialization routine.
  void Init(const uint  dimension, const uint  simplex_dimension);

 public:
  ISOSURFACE_TABLE();
  ISOSURFACE_TABLE(const uint  d);
  ISOSURFACE_TABLE(const uint  dimension, const uint  simplex_dimension);
  ~ISOSURFACE_TABLE();

  /// Return table encoding.
  ENCODING Encoding() const {
    return encoding;
  }

  /// Return string for table encoding.
  const string &EncodingName() const {
    return encoding_name;
  }

  /// Return polyhedron dimension.
  uint  Dimension() const {
    return polyhedron.Dimension();
  }

  /// Return isosurface simplex dimension.
  uint  SimplexDimension() const {
    return simplex_dimension;
  }

  /// Return number of vertices in each isosurface simplex.
  uint  NumVerticesPerSimplex() const {
    return SimplexDimension()+1;
  }

  /// Return number of isosurface vertices in polyhedron.
  uint  NumIsosurfaceVertices() const {
    return num_isosurface_vertices;
  }

  /// Return number of lookup table entries.
  uint  NumTableEntries() const {
    return num_table_entries;
  }

  /// Access isosurface table polyhedron.
  const ISOSURFACE_TABLE_POLYHEDRON &Polyhedron() const {
    return polyhedron;
  }

  /// Access i'th isosurface vertex.
  const ISOSURFACE_VERTEX & IsosurfaceVertex(uint  i) const {
    Assert(i < num_isosurface_vertices);
    return isosurface_vertex[i];
  }

  /// Return number of simplices in isosurface patch for table entry \a it.
  uint  NumSimplices(TABLE_INDEX it) const {
    return entry[it].num_simplices;
  }

  /// Return \a k'th vertex of isosurface simplex \a is, table entry \a it.
  /// @param it = Index of table entry.
  /// @param is = Simplex \a is of table entry \a it.
  /// @param k = Return \a k'th vertex of simplex \a is.
  ISOSURFACE_VERTEX_INDEX SimplexVertex(TABLE_INDEX it, uint  is, uint  k) const {
    return entry[it].simplex_vertex_list[is*NumVerticesPerSimplex()+k];
  }

  /// Return maximum number of polyhedron vertices permitted in any table.
  /// Note: Even tables for polyhedra of this size are probably impossible 
  ///   to compute/store.
  uint  MaxNumVertices() const {
    return max_num_vertices;
  }

  /// Return true if table memory is allocated.
  bool IsTableAllocated() const {
    return(is_table_allocated);
  }

  /// Return standard string for the encoding.
  static std::string StandardEncodingName(const ENCODING encoding);

  /// @name Set Polyhedron Functions
  void SetDimension(uint  d) {
    polyhedron.SetDimension(d);
  }
  void SetNumPolyVertices(uint  numv) {
    polyhedron.SetNumVertices(numv);
  }
  void SetNumPolyEdges(uint  nume) {
    polyhedron.SetNumEdges(nume);
  }
  void SetNumPolyFacets(uint  numf) {
    polyhedron.SetNumFacets(numf);
  }
  void SetPolySize(uint  numv, uint  nume, uint  numf) {
    SetNumPolyVertices(numv);
    SetNumPolyEdges(nume); 
    SetNumPolyFacets(numf);
  }
  void SetPolyVertexCoord(uint  iv, uint  ic, uint  coord) {
    polyhedron.SetVertexCoord(iv, ic, coord);
  }
  // Note: SetNumPolyVertices or SetPolySize must be called before 
  //   SetPolyVertexCoord
  void SetPolyEdge(uint  ie, uint  iv0, uint  iv1) {
    polyhedron.SetEdge(ie, iv0, iv1);
  }
  // Note: SetNumPolyEdges or SetPolySize must be called before SetPolyEdge
  void SetPolyNumFacetVertices(const int jf, const int numv) {
    polyhedron.SetNumFacetVertices(jf, numv);
  }
  void SetPolyFacetVertex(const int jf, const int k, const int iv) {
    polyhedron.SetFacetVertex(jf, k, iv);
  }
  // Note: SetPolyNumFacetVertices must be called before SetPolyFacetVertex
  void Set(const ISOSURFACE_TABLE_POLYHEDRON & polyhedron) {
    this->polyhedron = polyhedron;
  }

  /// @name Set Isosurface Vertices Functions
  void SetNumIsosurfaceVertices(uint  num_vertices);
  void SetIsoVertexType(uint  i, const ISOSURFACE_VERTEX::ISOSURFACE_VERTEX_TYPE t) {
    Assert(i < num_isosurface_vertices);
    isosurface_vertex[i].SetType(t);
  }
  void SetIsoVertexFace(uint  i, uint  index) {
    Assert(i < num_isosurface_vertices);
    isosurface_vertex[i].SetFace(index);
  }
  void SetIsoVertexNumCoord(uint  i, uint  numc) {
    Assert(i < num_isosurface_vertices);
    isosurface_vertex[i].SetNumCoord(numc);
  }
  void SetIsoVertexCoord(uint  i, uint  ic, ISOSURFACE_VERTEX::COORD_TYPE c) {
    Assert(i < num_isosurface_vertices);
    isosurface_vertex[i].SetCoord(ic, c);
  }
  void SetIsoVertexLabel(uint  i, const string &s) {
    Assert(i < num_isosurface_vertices);
    isosurface_vertex[i].SetLabel(s);
  }

  // store polyhedron vertices, edges or faces as isosurface vertices
  void StorePolyVerticesAsIsoVertices(uint  vstart);
  void StorePolyEdgesAsIsoVertices(   uint  vstart);
  void StorePolyFacetsAsIsoVertices(  uint  vstart);

  /// @name Set Isosurface Table Functions
  void SetSimplexDimension(const int d) {
    simplex_dimension = d;
  }
  void SetEncoding(const ENCODING encoding);
  void SetBinaryEncoding() { SetEncoding(BINARY); }
  void SetBase3Encoding()  { SetEncoding(BASE3);  }
  void SetNonstandardEncoding(const std::string & name);
  virtual void SetNumTableEntries(uint  num_table_entries);
  void SetNumSimplices( TABLE_INDEX it, uint  nums);
  void SetSimplexVertex(TABLE_INDEX it, uint  is, uint  iv, ISOSURFACE_VERTEX_INDEX isov);

  /// @name Generate Polyhedron Functions
  void GenCube(uint  cube_dimension) {
    polyhedron.GenCube(cube_dimension);
  }
  // Note: Cubes of dimension > 4 will have too many vertices
  void GenSimplex(uint  simplex_dimension) {
    polyhedron.GenSimplex(simplex_dimension);
  }
  void GenPyramid(const int pyramid_dimension) {
    polyhedron.GenPyramid(pyramid_dimension);
  }

  /// @name Check Functions
  bool CheckDimension(const int d) const;
  bool CheckDimension() const {
    return CheckDimension(Dimension());
  }
  void CheckTable() const;
  void Check() const;

  /// @name Memory Management Functions
  /// Free all memory.
  virtual void FreeAll();
};

typedef ISOSURFACE_TABLE * ISOSURFACE_TABLE_PTR;

// **************************************************
// ISOSURFACE EDGE TABLE
// **************************************************

/// Isosurface edge table.
/// Store list of edges containing isosurface vertices.
class ISOSURFACE_EDGE_TABLE:public ISOSURFACE_TABLE {
protected:

  /// Entry in isosurface edge table.
  class ISOSURFACE_EDGE_TABLE_ENTRY {
  public:
    uint        num_edges;
    EDGE_INDEX *edge_endpoint_list;

    ISOSURFACE_EDGE_TABLE_ENTRY();
    ~ISOSURFACE_EDGE_TABLE_ENTRY();

    void Check() const;
    void FreeAll();
  };

protected:
  ISOSURFACE_EDGE_TABLE_ENTRY *edge_entry;

  // initialization routine
  void Init(const int d);

public:
  ISOSURFACE_EDGE_TABLE(const int d);
  ~ISOSURFACE_EDGE_TABLE();

  // get functions
  uint  NumEdges(TABLE_INDEX it) const {
    return edge_entry[it].num_edges;
  }
  // it = table entry index. ie = edge index. iend = endpoint index (0 or 1)
  EDGE_INDEX EdgeEndpoint(TABLE_INDEX it, uint  ie, uint  iend) const {
    return edge_entry[it].edge_endpoint_list[2*ie+iend];
  }

  // set isosurface table functions
  virtual void SetNumTableEntries(uint  num_table_entries);

  // generate edge lists
  void GenEdgeLists();

  // check functions
  void CheckTable() const;
  void Check() const;

  // free memory
  // free all memory
  virtual void FreeAll();
};

typedef ISOSURFACE_EDGE_TABLE *ISOSURFACE_EDGE_TABLE_PTR;

//**************************************************
// AMBIGUITY INFORMATION FOR ISOSURFACE TABLE
//**************************************************

 /// Isosurface table with ambiguity information.
class ISOSURFACE_TABLE_AMBIG_INFO {
protected:
   bool         is_allocated;                 /// True if table is allocated
   uint         num_table_entries;            /// Number of table entries
   bool        *is_ambiguous;                 /// True for ambiguous configurations.
   FACET_INDEX *num_ambiguous_facets;         /// Number of ambiguous facts.
   FACET_SET   *ambiguous_facet;              /// k'th bit is 1 if facet k is ambiguous

   void Init();                               /// Initialization routine.
   void Alloc(uint  num_table_entries);       /// Allocate memory.
   void FreeAll();                            /// Free all memory.

 public:
   ISOSURFACE_TABLE_AMBIG_INFO() {
     Init();
   }
   ~ISOSURFACE_TABLE_AMBIG_INFO();

   // get functions
   bool IsAmbiguous(TABLE_INDEX it) const {
     return is_ambiguous[it];
   }
   FACET_INDEX NumAmbiguousFacets(TABLE_INDEX it) const {
     return num_ambiguous_facets[it];
   }
   // get functions
   bool IsFacetAmbiguous(TABLE_INDEX it, FACET_INDEX jf) const {
     return ambiguous_facet[it] & ((1L) << jf);
   }
   uint  NumTableEntries() const {
     return num_table_entries;
   }

   // compute functions
   void ComputeAmbiguityInformation(const ISOSURFACE_TABLE &isotable);
};

//**************************************************
// ISOSURFACE TABLE WITH AMBIGUITY INFORMATION
//**************************************************

 /// Isosurface table with ambiguity information.
class ISOSURFACE_TABLE_AMBIG: public ISOSURFACE_TABLE {
protected:
   bool        *is_ambiguous;            ///< True for ambiguous configurations.
   FACET_INDEX *num_ambiguous_facets;    ///< Number of ambiguous facts.
   FACET_SET   *ambiguous_facet;         ///< k'th bit is 1 if facet k is ambiguous

   void Init();                          ///< Initialization routine.
   void FreeAll();                       ///< Free all memory.

   /// Compute ambiguity information for all table entries.
   bool ComputeAmbiguous(const fixedarray<int> &vertex_sign) const;

   /// Compute number of ambiguous facets.
   FACET_INDEX ComputeNumAmbiguousFacets(const fixedarray<int> &vertex_sign) const;

   /// Compute set of ambiguous facets.
   void ComputeAmbiguousFacets(const fixedarray<int> &vertex_sign, FACET_SET &facet_set, FACET_INDEX &num_ambiguous_facets) const;

 public:
   ISOSURFACE_TABLE_AMBIG() : ISOSURFACE_TABLE() {
     Init();
   }
   ISOSURFACE_TABLE_AMBIG(uint  d) : ISOSURFACE_TABLE(d) {
     Init();
   }
   ISOSURFACE_TABLE_AMBIG(uint  dimension, uint  simplex_dimension)
     : ISOSURFACE_TABLE(dimension, simplex_dimension)
   {
     Init();
   }

   ~ISOSURFACE_TABLE_AMBIG();

   // get functions
   bool IsAmbiguous(TABLE_INDEX it) const {
     return is_ambiguous[it];
   }
   FACET_INDEX NumAmbiguousFacets(TABLE_INDEX it) const {
     return num_ambiguous_facets[it];
   }
   // get functions
   bool IsFacetAmbiguous(TABLE_INDEX it, FACET_INDEX jf) const {
     return ambiguous_facet[it] & ((1L) << jf);
   }

   // set functions
   virtual void SetNumTableEntries(uint  num_table_entries);

   // compute functions
   void ComputeAmbiguityInformation() const;
 };

//**************************************************
// UTILITY FUNCTIONS
//**************************************************

// calculate number of entries required in ISOSURFACE_TABLE
uint  calculate_num_entries(uint  num_vert, uint  num_colors);

// convert integer to base "base"
void convert2base(ulong  ival, uint  base, fixedarray<int> &digit, uint  max_num_digits);


//**************************************************
// ROUTINES FOR GENERATING POLYHEDRA
//**************************************************
void generate_prism(const ISOSURFACE_TABLE_POLYHEDRON &base_polyhedron, ISOSURFACE_TABLE_POLYHEDRON &prism);

//**************************************************
// AMBIGUITY ROUTINES
//**************************************************
/// Return true if isosurface topology is ambiguous
/// @param vertex_sign[i] = Sign of isosurface vertex i. (0 or 1);
bool is_poly_ambiguous(const ISOSURFACE_TABLE_POLYHEDRON &poly, const fixedarray<int> &vertex_sign);

/// Return true if facet jf is ambiguous.
/// @param vertex_sign[i] = Sign of isosurface vertex i.
bool is_facet_ambiguous(const ISOSURFACE_TABLE_POLYHEDRON &poly, FACET_INDEX jf, const fixedarray<int> &vertex_sign);

/// Return number of vertices connected by edges to iv with same sign as iv.
/// @param vertex_sign[i] = Sign of isosurface vertex i.
uint  compute_num_connected(const ISOSURFACE_TABLE_POLYHEDRON & poly, uint  iv, const fixedarray<int> &vertex_sign);

/// Return number of vertices connected by edges in facet jf to vertex iv with same sign as iv.
/// @param jf = Facet index.
/// @param iv = Vertex index.  Precondition: Vertex iv is in facet jf.
/// @param vertex_sign[i] = Sign of isosurface vertex i.
uint  compute_num_connected_in_facet(const ISOSURFACE_TABLE_POLYHEDRON &poly, FACET_INDEX jf, uint  iv, const fixedarray<int> &vertex_sign);

/// Compute ambiguous facets.
/// @param vertex_sign[i] = Sign of isosurface vertex i.
void compute_ambiguous_facets(const ISOSURFACE_TABLE_POLYHEDRON &poly, const fixedarray<int> &vertex_sign, FACET_SET &facet_set, FACET_INDEX &num_ambiguous_facets);

}; // namespace

