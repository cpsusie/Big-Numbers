#include "pch.h"
#include <MyUtil.h>
#include <Math/Triangle/Triangle.h>
#include "MemoryPool.h"

//***************************************************************************
//
//      888888888        ,o,                          / 888
//         888    88o88o  "    o8888o  88o8888o o88888o 888  o88888o
//         888    888    888       88b 888  888 888 888 888 d888  88b
//         888    888    888  o88^o888 888  888 "88888" 888 8888oo888
//         888    888    888 C888  888 888  888  /      888 q888
//         888    888    888  "88o^888 888  888 Cb      888  "88oooo"
//                                              "8oo8D
//
//  A Two-Dimensional Quality Mesh Generator and Delaunay Triangulator.
//  (Triangle.c)
//
//  Version 1.6
//  July 28, 2005
//
//  Copyright 1993, 1995, 1997, 1998, 2002, 2005
//  Jonathan Richard Shewchuk
//  2360 Woolsey #H
//  Berkeley, California  94705-1927
//  jrs@cs.berkeley.edu
//
//  This program may be freely redistributed under the condition that the
//  copyright notices (including this entire header and the copyright
//  notice printed when the `-h' switch is selected) are not removed, and
//  no compensation is received. Private, research, and institutional
//  use is free. You may distribute modified versions of this code UNDER
//  THE CONDITION THAT THIS CODE AND ANY MODIFICATIONS MADE TO IT IN THE
//  SAME FILE REMAIN UNDER COPYRIGHT OF THE ORIGINAL AUTHOR, BOTH SOURCE
//  AND OBJECT CODE ARE MADE FREELY AVAILABLE WITHOUT CHARGE, AND CLEAR
//  NOTICE IS GIVEN OF THE MODIFICATIONS. Distribution of this code as
//  part of a commercial system is permissible ONLY BY DIRECT ARRANGEMENT
//  WITH THE AUTHOR. (If you are not directly supplying this code to a
//  customer, and you are instead telling them how they can obtain it for
//  free, then you are not required to make any arrangement with me.)
//
//  Hypertext instructions for Triangle are available on the Web at
//
//      http://www.cs.cmu.edu/~quake/triangle.html
//
//  Disclaimer:  Neither I nor Carnegie Mellon warrant this code in any way
//  whatsoever. This code is provided "as-is". Use at your own risk.
//
//  Some of the references listed below are marked with an asterisk. [*]
//  These references are available for downloading from the Web page
//
//      http://www.cs.cmu.edu/~quake/triangle.research.html
//
//  Three papers discussing aspects of Triangle are available. A short
//  overview appears in "Triangle:  Engineering a 2D Quality Mesh
//  Generator and Delaunay Triangulator," in Applied Computational
//  Geometry:  Towards Geometric Engineering, Ming C. Lin and Dinesh
//  Manocha, editors, Lecture Notes in Computer Science volume 1148,
//  pages 203-222, Springer-Verlag, Berlin, May 1996 (from the First ACM
//  Workshop on Applied Computational Geometry). [*]
//
//  The algorithms are discussed in the greatest detail in "Delaunay
//  Refinement Algorithms for Triangular Mesh Generation," Computational
//  Geometry:  Theory and Applications 22(1-3):21-74, May 2002. [*]
//
//  More detail about the data structures may be found in my dissertation:
//  "Delaunay Refinement Mesh Generation," Ph.D. thesis, Technical Report
//  CMU-CS-97-137, School of Computer Science, Carnegie Mellon University,
//  Pittsburgh, Pennsylvania, 18 May 1997. [*]
//
//  Triangle was created as part of the Quake Project in the School of
//  Computer Science at Carnegie Mellon University. For further
//  information, see Hesheng Bao, Jacobo Bielak, Omar Ghattas, Loukas F.
//  Kallivokas, David R. O'Hallaron, Jonathan R. Shewchuk, and Jifeng Xu,
//  "Large-scale Simulation of Elastic Wave Propagation in Heterogeneous
//  Media on Parallel Computers," Computer Methods in Applied Mechanics
//  and Engineering 152(1-2):85-102, 22 January 1998.
//
//  Triangle's Delaunay refinement algorithm for quality mesh generation is
//  a hybrid of one due to Jim Ruppert, "A Delaunay Refinement Algorithm
//  for Quality 2-Dimensional Mesh Generation," Journal of Algorithms
//  18(3):548-585, May 1995 [*], and one due to L. Paul Chew, "Guaranteed-
//  Quality Mesh Generation for Curved Surfaces," Proceedings of the Ninth
//  Annual Symposium on Computational Geometry (San Diego, California),
//  pages 274-280, Association for Computing Machinery, May 1993,
//  http://portal.acm.org/citation.cfm?id=161150 .
//
//  The Delaunay refinement algorithm has been modified so that it meshes
//  domains with small input angles well, as described in Gary L. Miller,
//  Steven E. Pav, and Noel J. Walkington, "When and Why Ruppert's
//  Algorithm Works," Twelfth International Meshing Roundtable, pages
//  91-102, Sandia National Laboratories, September 2003. [*]
//
//  My implementation of the divide-and-conquer and incremental Delaunay
//  triangulation algorithms follows closely the presentation of Guibas
//  and Stolfi, even though I use a triangle-based data structure instead
//  of their quad-edge data structure. (In fact, I originally implemented
//  Triangle using the quad-edge data structure, but the switch to a
//  triangle-based data structure sped Triangle by a factor of two.)  The
//  mesh manipulation primitives and the two aforementioned Delaunay
//  triangulation algorithms are described by Leonidas J. Guibas and Jorge
//  Stolfi, "Primitives for the Manipulation of General Subdivisions and
//  the Computation of Voronoi Diagrams," ACM Transactions on Graphics
//  4(2):74-123, April 1985, http://portal.acm.org/citation.cfm?id=282923 .
//
//  Their O(n log n) divide-and-conquer algorithm is adapted from Der-Tsai
//  Lee and Bruce J. Schachter, "Two Algorithms for Constructing the
//  Delaunay Triangulation," International Journal of Computer and
//  Information Science 9(3):219-242, 1980. Triangle's improvement of the
//  divide-and-conquer algorithm by alternating between vertical and
//  horizontal cuts was introduced by Rex A. Dwyer, "A Faster Divide-and-
//  Conquer Algorithm for Constructing Delaunay Triangulations,"
//  Algorithmica 2(2):137-151, 1987.
//
//  The incremental insertion algorithm was first proposed by C. L. Lawson,
//  "Software for C1 Surface Interpolation," in Mathematical Software III,
//  John R. Rice, editor, Academic Press, New York, pp. 161-194, 1977.
//  For point location, I use the algorithm of Ernst P. Mucke, Isaac
//  Saias, and Binhai Zhu, "Fast Randomized Point Location Without
//  Preprocessing in Two- and Three-Dimensional Delaunay Triangulations,"
//  Proceedings of the Twelfth Annual Symposium on Computational Geometry,
//  ACM, May 1996. [*]  If I were to randomize the order of Vertex
//  insertion (I currently don't bother), their result combined with the
//  result of Kenneth L. Clarkson and Peter W. Shor, "Applications of
//  Random Sampling in Computational Geometry II," Discrete &
//  Computational Geometry 4(1):387-421, 1989, would yield an expected
//  O(n^{4/3}) bound on running time.
//
//  The O(n log n) sweepline Delaunay triangulation algorithm is taken from
//  Steven Fortune, "A Sweepline Algorithm for Voronoi Diagrams",
//  Algorithmica 2(2):153-174, 1987. A random sample of edges on the
//  boundary of the triangulation are maintained in a splay tree for the
//  purpose of point location. Splay trees are described by Daniel
//  Dominic Sleator and Robert Endre Tarjan, "Self-Adjusting Binary Search
//  Trees," Journal of the ACM 32(3):652-686, July 1985,
//  http://portal.acm.org/citation.cfm?id=3835 .
//
//  The algorithms for exact computation of the signs of determinants are
//  described in Jonathan Richard Shewchuk, "Adaptive Precision Floating-
//  Point Arithmetic and Fast Robust Geometric Predicates," Discrete &
//  Computational Geometry 18(3):305-363, October 1997. (Also available
//  as Technical Report CMU-CS-96-140, School of Computer Science,
//  Carnegie Mellon University, Pittsburgh, Pennsylvania, May 1996.)  [*]
//  An abbreviated version appears as Jonathan Richard Shewchuk, "Robust
//  Adaptive Floating-Point Geometric Predicates," Proceedings of the
//  Twelfth Annual Symposium on Computational Geometry, ACM, May 1996. [*]
//  Many of the ideas for my exact arithmetic routines originate with
//  Douglas M. Priest, "Algorithms for Arbitrary Precision Floating Point
//  Arithmetic," Tenth Symposium on Computer Arithmetic, pp. 132-143, IEEE
//  Computer Society Press, 1991. [*]  Many of the ideas for the correct
//  evaluation of the signs of determinants are taken from Steven Fortune
//  and Christopher J. Van Wyk, "Efficient Exact Arithmetic for Computa-
//  tional Geometry," Proceedings of the Ninth Annual Symposium on
//  Computational Geometry, ACM, pp. 163-172, May 1993, and from Steven
//  Fortune, "Numerical Stability of Algorithms for 2D Delaunay Triangu-
//  lations," International Journal of Computational Geometry & Applica-
//  tions 5(1-2):193-213, March-June 1995.
//
//  The method of inserting new vertices off-center (not precisely at the
//  circumcenter of every poor-quality triangle) is from Alper Ungor,
//  "Off-centers:  A New Type of Steiner Points for Computing Size-Optimal
//  Quality-Guaranteed Delaunay Triangulations," Proceedings of LATIN
//  2004 (Buenos Aires, Argentina), April 2004.
//
//  For definitions of and results involving Delaunay triangulations,
//  constrained and conforming versions thereof, and other aspects of
//  triangular mesh generation, see the excellent survey by Marshall Bern
//  and David Eppstein, "Mesh Generation and Optimal Triangulation," in
//  Computing and Euclidean Geometry, Ding-Zhu Du and Frank Hwang,
//  editors, World Scientific, Singapore, pp. 23-90, 1992. [*]
//
//  The time for incrementally adding PSLG (planar straight line graph)
//  segments to create a constrained Delaunay triangulation is probably
//  O(t^2) per segment in the worst case and O(t) per segment in the
//  common case, where t is the number of triangles that intersect the
//  segment before it is inserted. This doesn't count point location,
//  which can be much more expensive. I could improve this to O(d log d)
//  time, but d is usually quite small, so it's not worth the bother.
//  (This note does not apply when the -s switch is used, invoking a
//  different method is used to insert segments.)
//
//  The time for deleting a Vertex from a Delaunay triangulation is O(d^2)
//  in the worst case and O(d) in the common case, where d is the degree
//  of the Vertex being deleted. I could improve this to O(d log d) time,
//  but d is usually quite small, so it's not worth the bother.
//
//  Ruppert's Delaunay refinement algorithm typically generates triangles
//  at a linear rate (constant time per triangle) after the initial
//  triangulation is formed. There may be pathological cases where
//  quadratic time is required, but these never arise in practice.
//
//  The geometric predicates (circumcenter calculations, segment
//  intersection formulae, etc.) appear in my "Lecture Notes on Geometric
//  Robustness" at http://www.cs.berkeley.edu/~jrs/mesh .
//
//  If you make any improvements to this code, please please please let me
//  know, so that I may obtain the improvements. Even if you don't change
//  the code, I'd still love to hear what it's being used for.
//
//***************************************************************************

// On some machines, my exact arithmetic routines might be defeated by the
// use of internal extended precision floating-point registers. The best
// way to solve this problem is to set the floating-point registers to use
// single or double precision internally. On 80x86 processors, this may
// be accomplished by setting the CPU86 symbol for the Microsoft C
// compiler, or the LINUX symbol for the gcc compiler running on Linux.
//
// An inferior solution is to declare certain values as `volatile', thus
// forcing them to be stored to memory and rounded off. Unfortunately,
// this solution might slow Triangle down quite a bit. To use volatile
// values, write "#define INEXACT volatile" below. Normally, however,
// INEXACT should be defined to be nothing. ("#define INEXACT".)
//
// For more discussion, see http://www.cs.cmu.edu/~quake/robust.pc.html .
// For yet more discussion, see Section 5 of my paper, "Adaptive Precision
// Floating-Point Arithmetic and Fast Robust Geometric Predicates" (also
// available as Section 6.6 of my dissertation).

#define CPU86
// #define LINUX

#define INEXACT /* Nothing */
// #define INEXACT volatile

#define HAS_DEBUG_FUNCTIONS

// Maximum number of characters in a file name (including the null).

#define FILENAMESIZE 2048

// For efficiency, a variety of data structures are allocated in bulk. The
// following constants determine how many of each structure is allocated at once.

#define TRIPERBLOCK    4092  /* Number of triangles allocated at once.       */
#define SUBSEGPERBLOCK  508  /* Number of subsegments allocated at once.     */
#define VERTEXPERBLOCK 4092  /* Number of vertices allocated at once.        */
#define VIRUSPERBLOCK  1020  /* Number of virus triangles allocated at once. */

// Number of encroached subsegments allocated at once.
#define BADSUBSEGPERBLOCK 252
// Number of skinny triangles allocated at once.
#define BADTRIPERBLOCK 4092
// Number of flipped triangles allocated at once.
#define FLIPSTACKERPERBLOCK 252
// Number of splay tree nodes allocated at once.
#define SPLAYNODEPERBLOCK 508

// The Vertex types. A DEADVERTEX has been deleted entirely. An
// UNDEADVERTEX is not part of the mesh, but is written to the output
//.node file and affects the node indexing in the other output files.

#define INPUTVERTEX 0
#define SEGMENTVERTEX 1
#define FREEVERTEX 2
#define DEADVERTEX -32768
#define UNDEADVERTEX -32767

// The next line is used to outsmart some very stupid compilers. If your
// compiler is smarter, feel free to replace the "int" with "void".
// Not that it matters.

//#define VOID int

// Two constants for algorithms based on random sampling. Both constants
// have been chosen empirically to optimize their respective algorithms.

// Used for the point location scheme of Mucke, Saias, and Zhu, to decide
// how large a random sample of triangles to inspect.

#define SAMPLEFACTOR 11

// Used in Fortune's sweepline Delaunay algorithm to determine what fraction
// of boundary edges should be maintained in the splay tree for point
// location on the front.

#define SAMPLERATE 10

// A number that speaks for itself, every kissable digit.

#define PI 3.141592653589793238462643383279502884197169399375105820974944592308

// Another fave.

#define SQUAREROOTTWO 1.4142135623730950488016887242096980785696718753769480732

// And here's one for those of you who are intimidated by math.

#define ONETHIRD 0.333333333333333333333333333333333333333333333333333333333333

#ifndef NO_TIMER
//#include <sys/time.h>
#endif // not NO_TIMER

#ifdef CPU86
#include <float.h>
#endif // CPU86

#ifdef LINUX
#include <fpu_control.h>
#endif // LINUX

// Labels that signify the result of point location. The result of a
// search indicates that the point falls in the interior of a triangle, on
// an edge, on a Vertex, or outside the mesh.

typedef enum {
  INTRIANGLE
 ,ONEDGE
 ,ONVERTEX
 ,OUTSIDE
} LocateResult;

// Labels that signify the result of Vertex insertion. The result indicates
// that the Vertex was inserted with complete success, was inserted but
// encroaches upon a subsegment, was not inserted because it lies on a
// segment, or was not inserted because another Vertex occupies the same
// location.

typedef enum {
  SUCCESSFULVERTEX
 ,ENCROACHINGVERTEX
 ,VIOLATINGVERTEX
 ,DUPLICATEVERTEX
} InserVvertexResult;

// Labels that signify the result of direction finding. The result
// indicates that a segment connecting the two query points falls within
// the direction triangle, along the left edge of the direction triangle,
// or along the right edge of the direction triangle.

typedef enum {
  WITHIN
 ,LEFTCOLLINEAR
 ,RIGHTCOLLINEAR
} FindDirectionResult;

//***************************************************************************
//
//  The basic mesh data structures
//
//  There are three:  vertices, triangles, and subsegments (abbreviated
//  `SubSeg'). These three data structures, linked by pointers, comprise
//  the mesh. A Vertex simply represents a mesh Vertex and its properties.
//  A triangle is a triangle. A subsegment is a special data structure used
//  to represent an impenetrable edge of the mesh (perhaps on the outer
//  boundary, on the boundary of a hole, or part of an internal boundary
//  separating two triangulated regions). Subsegments represent boundaries,
//  defined by the user, that triangles may not lie across.
//
//  A triangle consists of a list of three vertices, a list of three
//  adjoining triangles, a list of three adjoining subsegments (when
//  segments exist), an arbitrary number of optional user-defined
//  floating-point attributes, and an optional area constraint. The latter
//  is an upper bound on the permissible area of each triangle in a region,
//  used for mesh refinement.
//
//  For a triangle on a boundary of the mesh, some or all of the neighboring
//  triangles may not be present. For a triangle in the interior of the
//  mesh, often no neighboring subsegments are present. Such absent
//  triangles and subsegments are never represented by NULL pointers; they
//  are represented by two special records:  `dummytri', the triangle that
//  fills "outer space", and `dummysub', the omnipresent subsegment.
//  `dummytri' and `dummysub' are used for several reasons; for instance,
//  they can be dereferenced and their contents examined without violating
//  protected memory.
//
//  However, it is important to understand that a triangle includes other
//  information as well. The pointers to adjoining vertices, triangles, and
//  subsegments are ordered in a way that indicates their geometric relation
//  to each other. Furthermore, each of these pointers contains orientation
//  information. Each pointer to an adjoining triangle indicates which face
//  of that triangle is contacted. Similarly, each pointer to an adjoining
//  subsegment indicates which side of that subsegment is contacted, and how
//  the subsegment is oriented relative to the triangle.
//
//  The data structure representing a subsegment may be thought to be
//  abutting the edge of one or two triangle data structures:  either
//  sandwiched between two triangles, or resting against one triangle on an
//  exterior boundary or hole boundary.
//
//  A subsegment consists of a list of four vertices--the vertices of the
//  subsegment, and the vertices of the segment it is a part of--a list of
//  two adjoining subsegments, and a list of two adjoining triangles. One
//  of the two adjoining triangles may not be present (though there should
//  always be one), and neighboring subsegments might not be present.
//  Subsegments also store a user-defined integer "boundary marker".
//  Typically, this integer is used to indicate what boundary conditions are
//  to be applied at that location in a finite element simulation.
//
//  Like triangles, subsegments maintain information about the relative
//  orientation of neighboring objects.
//
//  Vertices are relatively simple. A Vertex is a list of floating-point
//  numbers, starting with the x, and y coordinates, followed by an
//  arbitrary number of optional user-defined floating-point attributes,
//  followed by an integer boundary marker. During the segment insertion
//  phase, there is also a pointer from each Vertex to a triangle that may
//  contain it. Each pointer is not always correct, but when one is, it
//  speeds up segment insertion. These pointers are assigned values once
//  at the beginning of the segment insertion phase, and are not used or
//  updated except during this phase. Edge flipping during segment
//  insertion will render some of them incorrect. Hence, don't rely upon
//  them for anything.
//
//  Other than the exception mentioned above, vertices have no information
//  about what triangles, subfacets, or subsegments they are linked to.
//
//***************************************************************************

//***************************************************************************
//
//  Handles
//
//  The oriented triangle (`otri') and oriented subsegment (`osub') data
//  structures defined below do not themselves store any part of the mesh.
//  The mesh itself is made of `triangle's, `subseg's, and `Vertex's.
//
//  Oriented triangles and oriented subsegments will usually be referred to
//  as "handles."  A handle is essentially a pointer into the mesh; it
//  allows you to "hold" one particular part of the mesh. Handles are used
//  to specify the regions in which one is traversing and modifying the mesh.
//  A single `triangle' may be held by many handles, or none at all. (The
//  latter case is not a memory leak, because the triangle is still
//  connected to other triangles in the mesh.)
//
//  An `otri' is a handle that holds a triangle. It holds a specific edge
//  of the triangle. An `osub' is a handle that holds a subsegment. It
//  holds either the left or right side of the subsegment.
//
//  Navigation about the mesh is accomplished through a set of mesh
//  manipulation primitives, further below. Many of these primitives take
//  a handle and produce a new handle that holds the mesh near the first
//  handle. Other primitives take two handles and glue the corresponding
//  parts of the mesh together. The orientation of the handles is
//  important. For instance, when two triangles are glued together by the
//  bond() primitive, they are glued at the edges on which the handles lie.
//
//  Because vertices have no information about which triangles they are
//  attached to, I commonly represent a Vertex by use of a handle whose
//  origin is the Vertex. A single handle can simultaneously represent a
//  triangle, an edge, and a Vertex.
//
//***************************************************************************

// The triangle data structure. Each triangle contains three pointers to
// adjoining triangles, plus three pointers to vertices, plus three
// pointers to subsegments (declared below; these pointers are usually
// `dummysub'). It may or may not also contain user-defined attributes
// and/or a floating-point "area constraint."  It may also contain extra
// pointers for nodes, when the user asks for high-order elements.
// Because the size and structure of a `triangle' is not decided until
// runtime, I haven't simply declared the type `triangle' as a struct.
typedef REAL **Triangle;            // Really:  typedef triangle *triangle

// An oriented triangle:  includes a pointer to a triangle and orientation.
// The orientation denotes an edge of the triangle. Hence, there are
// three possible orientations. By convention, each edge always points
// counterclockwise about the corresponding triangle.
typedef struct {
  Triangle *tri;
  int orient; // Ranges from 0 to 2.
} otri;

// The subsegment data structure. Each subsegment contains two pointers to
// adjoining subsegments, plus four pointers to vertices, plus two
// pointers to adjoining Triangles, plus one boundary marker, plus one
// segment number.
typedef REAL **SubSeg;                  // Really:  typedef subseg *subseg

// An oriented subsegment:  includes a pointer to a subsegment and an
// orientation. The orientation denotes a side of the edge. Hence, there
// are two possible orientations. By convention, the edge is always
// directed so that the "side" denoted is the right side of the edge.

typedef struct {
  SubSeg *ss;
  int ssorient; // Ranges from 0 to 1.
} osub;

// The Vertex data structure. Each Vertex is actually an array of REALs.
// The number of REALs is unknown until runtime. An integer boundary
// marker, and sometimes a pointer to a Triangle, is appended after the REALs
typedef REAL *Vertex;

String vertexToString(const Vertex v) {
  return format(_T("%.12g, %.12g)"), v[0], v[1]);
}

String edgeToString(const Vertex v1, const Vertex v2) {
  return format(_T("%s %s"), vertexToString(v1).cstr(), vertexToString(v2).cstr());
}

String triangleToString(const Vertex org, const Vertex dst, const Vertex apex) {
  return format(_T("%s %s %s"), vertexToString(org).cstr(), vertexToString(dst).cstr(), vertexToString(apex).cstr());
}

void duplicateVertexWarning(const Vertex v) {
 _tprintf(_T("Warning:A duplicate vertex at %s appeared and was ignored\n"), vertexToString(v).cstr());
}

void vertexFallsOnExistingWarning(const Vertex v) {
 _tprintf(_T("Warning:New vertex %s falls on existing vertex\n"), vertexToString(v).cstr());
}

// A queue used to store encroached subsegments. Each subsegment's vertices
// are stored so that we can check whether a subsegment is still the same.
typedef struct {
  SubSeg encsubseg;                             // An encroached subsegment.
  Vertex subsegorg, subsegdest;                 // Its two vertices.
} BadSubSeg;

// A queue used to store bad Triangles.The key is the square of the cosine
// of the smallest angle of the Triangle. Each Triangle's vertices are
// stored so that one can check whether a Triangle is still the same.
typedef struct _badtriang {
  Triangle poortri;                             // A skinny or too-large Triangle.
  REAL key;                                     // cos^2 of smallest (apical) angle.
  Vertex triangorg, triangdest, triangapex;     // Its three vertices.
  struct _badtriang *nexttriang;                // Pointer to next bad Triangle.
} BadTriangle;

// A stack of Triangles flipped during the most recent Vertex insertion.
// The stack is used to undo the Vertex insertion if the Vertex encroaches
// upon a subsegment.
typedef struct _flipstacker {
  Triangle flippedtri;                          // A recently flipped Triangle.
  struct _flipstacker *prevflip;                // Previous flip in the stack.
} FlipStacker;

static FlipStacker *FLIPSTACKER_MERGE4 = (FlipStacker*)((size_t)1);
                                                // we only need the address
                                                // different from NULL and all
                                                // others allocated


// A node in a heap used to store events for the sweepline Delaunay
// algorithm. Nodes do not point directly to their parents or children in
// the heap. Instead, each node knows its position in the heap, and can
// look up its parent and children in a separate array. The `eventptr'
// points either to a `Vertex' or to a Triangle (in encoded format, so
// that an orientation is included). In the latter case, the origin of
// the oriented Triangle is the apex of a "circle event" of the sweepline
// algorithm. To distinguish site events from circle events, all circle
// events are given an invalid (smaller than `xmin') x-coordinate `xkey'.
typedef struct {
  REAL xkey, ykey;                              // Coordinates of the event.
  VOID *eventptr;                               // Can be a Vertex or the location of a circle event.
  int heapposition;                             // Marks this event's position in the heap.
} Event;

// A node in the splay tree. Each node holds an oriented ghost Triangle
// that represents a boundary edge of the growing triangulation. When a
// circle event covers two boundary edges with a Triangle, so that they
// are no longer boundary edges, those edges are not immediately deleted
// from the tree; rather, they are lazily deleted when they are next
// encountered. (Since only a random sample of boundary edges are kept
// in the tree, lazy deletion is faster.)  `keydest' is used to verify
// that a Triangle is still the same as when it entered the splay tree; if
// it has been rotated (due to a circle event), it no longer represents a
// boundary edge and should be deleted.
typedef struct _splaynode {
  otri keyedge;                            // Lprev of an edge on the front.
  Vertex keydest;                          // Used to verify that splay node is still live.
  struct _splaynode *lchild, *rchild;      // Children in splay tree.
} SplayNode;

// Global constants.
static REAL splitter;                      // Used to split REAL factors for exact multiplication
static REAL epsilon;                       // Floating-point machine epsilon.
static REAL resulterrbound;
static REAL ccwerrboundA, ccwerrboundB, ccwerrboundC;
static REAL iccerrboundA, iccerrboundB, iccerrboundC;
static REAL o3derrboundA, o3derrboundB, o3derrboundC;


// Data structure for command line switches and file names. This structure
// is used (instead of global variables) to allow reentrancy.

class Behavior {
// Switches for the triangulator.
//   poly: -p switch. refine: -r switch.
//   quality: -q switch.
//     minangle: minimum angle bound, specified after -q switch.
//     goodangle: cosine squared of minangle.
//     offconstant: constant used to place off-center Steiner points.
//   vararea: -a switch without number.
//   fixedarea: -a switch with number.
//     maxarea: maximum area bound, specified after -a switch.
//   usertest: -u switch.
//   regionattrib: -A switch. convex: -c switch.
//   weighted: 1 for -w switch, 2 for -W switch. jettison: -j switch
//   firstnumber: inverse of -z switch. All items are numbered starting
//     from `firstnumber'.
//   edgesout: -e switch. voronoi: -v switch.
//   neighbors: -n switch. geomview: -g switch.
//   nobound: -B switch. nopolywritten: -P switch.
//   nonodewritten: -N switch. noelewritten: -E switch.
//   noiterationnum: -I switch. noholes: -O switch.
//   noexact: -X switch.
//   order: element order, specified after -o switch.
//   nobisect: count of how often -Y switch is selected.
//   steiner: maximum number of Steiner points, specified after -S switch.
//   incremental: -i switch. sweepline: -F switch.
//   dwyer: inverse of -l switch.
//   splitseg: -s switch.
//   conformdel: -D switch. docheck: -C switch.
//   quiet: -Q switch. verbose: count of how often -V switch is selected.
//   usesegments: -p, -r, -q, or -c switch; determines whether segments are
//     used at all.
//
// Read the instructions to find out the meaning of these switches.

public:
  int poly, refine, quality, vararea, fixedarea, usertest;
  int regionattrib, convex, weighted, jettison;
  int firstnumber;
  int edgesout, voronoi, neighbors, geomview;
  int nobound, nopolywritten, nonodewritten, noelewritten, noiterationnum;
  int noholes, noexact, conformdel;
  int incremental, sweepline, dwyer;
  int splitseg;
  int docheck;
  int quiet, verbose;
  int usesegments;
  int order;
  int nobisect;
  int steiner;
  REAL minangle, goodangle, offconstant;
  REAL maxarea;

// Variables for file names.

#ifndef TRILIBRARY
  char innodefilename[FILENAMESIZE];
  char inelefilename[FILENAMESIZE];
  char inpolyfilename[FILENAMESIZE];
  char areafilename[FILENAMESIZE];
  char outnodefilename[FILENAMESIZE];
  char outelefilename[FILENAMESIZE];
  char outpolyfilename[FILENAMESIZE];
  char edgefilename[FILENAMESIZE];
  char vnodefilename[FILENAMESIZE];
  char vedgefilename[FILENAMESIZE];
  char neighborfilename[FILENAMESIZE];
  char offfilename[FILENAMESIZE];
#endif // not TRILIBRARY

  void parsecommandline(int argc, char **argv);

};


// Mesh data structure. Triangle operates on only one mesh, but the mesh
// structure is used (instead of global variables) to allow reentrancy.
class Mesh {
  // Variables used to allocate memory for Triangles, subsegments, vertices,
  // viri (Triangles being eaten), encroached segments, bad (skinny or too
  // large) Triangles, and splay tree nodes.
  MemoryPool<Triangle>    triangles;
  MemoryPool<SubSeg>      subsegs;
  MemoryPool<REAL>        vertices;
  MemoryPool<Triangle*>   viri;
  MemoryPool<BadSubSeg>   badsubsegs;
  MemoryPool<BadTriangle> badtriangles;
  MemoryPool<FlipStacker> flipstackers;
  MemoryPool<SplayNode>   splaynodes;

  Behavior                &m_b; // all the options to the various algorithms

  // Variables that maintain the bad Triangle queues. The queues are
  // ordered from 4095 (highest priority) to 0 (lowest priority).
  BadTriangle *queuefront[4096];
  BadTriangle *queuetail[4096];
  int nextnonemptyq[4096];
  int firstnonemptyq;

  // Variable that maintains the stack of recently flipped triangles.
  FlipStacker *lastflip;

  // Other variables.
  REAL xmin, xmax, ymin, ymax;    // x and y bounds.
  REAL xminextreme;               // Nonexistent x value used as a flag in sweepline.
  int  invertices;                // Number of input vertices.
  int  inelements;                // Number of input triangles.
  int  insegments;                // Number of input segments.
  int  holes;                     // Number of input holes.
  int  regions;                   // Number of input regions.
  int  undeads;                   // Number of input vertices that don't appear in the mesh.
  long edges;                     // Number of output edges.
  int  mesh_dim;                  // Dimension (ought to be 2).
  int  nextras;                   // Number of attributes per Vertex.
  int  eextras;                   // Number of attributes per Triangle.
  long hullsize;                  // Number of edges in convex hull.
  int  steinerleft;               // Number of Steiner points not yet used.
  int  vertexmarkindex;           // Index to find boundary marker of a Vertex.
  int  vertex2triindex;           // Index to find a Triangle adjacent to a Vertex.
  int  highorderindex;            // Index to find extra nodes for high-order elements.
  int  elemattribindex;           // Index to find attributes of a Triangle.
  int  areaboundindex;            // Index to find area bound of a Triangle.
  int  checksegments;             // Are there segments in the triangulation yet?
  int  checkquality;              // Has quality triangulation begun yet?
  int  readnodefile;              // Has a .node file been read?
  long samples;                   // Number of random samples for point location.

  long incirclecount;             // Number of incircle tests performed.
  long counterclockcount;         // Number of counterclockwise tests performed.
  long orient3dcount;             // Number of 3D orientation tests performed.
  long hyperbolacount;            // Number of right-of-hyperbola tests performed.
  long circumcentercount;         // Number of circumcenter calculations performed.
  long circletopcount;            // Number of circle top calculations performed.

  // Triangular bounding box vertices.
  Vertex infvertex1, infvertex2, infvertex3;


  Triangle *dummytri;             // Pointer to the `Triangle' that occupies all of "outer space."
  Triangle *dummytribase;         // Keep base address so we can free() it later.

  // Pointer to the omnipresent subsegment. Referenced by any Triangle or
  // subsegment that isn't really connected to a subsegment at that
  // location.
  SubSeg *dummysub;
  SubSeg *dummysubbase;            // Keep base address so we can free() it later.

  // Pointer to a recently visited Triangle. Improves point location if
  // proximate vertices are inserted sequentially.
  otri recenttri;

  void       triangleinit();
  void       triangledeinit();

public:
  Mesh(Behavior &b) : m_b(b) {
    triangleinit();
  }
  ~Mesh() {
    triangledeinit();
  }
  void       printtriangle(        otri        *t);
  void       printsubseg(          osub        *s);
  void       dummyinit(            int          trianglebytes, int subsegbytes);
  void       initializevertexpool();
  void       initializetrisubpools();
  void       triangledealloc(      Triangle    *dyingtriangle);
  Triangle  *triangletraverse();
  void       subsegdealloc(        SubSeg      *dyingsubseg);
  SubSeg    *subsegtraverse();
  void       vertexdealloc(        Vertex       dyingvertex);
  Vertex     vertextraverse();
  void       badsubsegdealloc(     BadSubSeg   *dyingseg);
  BadSubSeg *badsubsegtraverse();
  Vertex     getvertex(            int          number);
  void       maketriangle(         otri        *newotri);
  void       makesubseg(           osub        *newsubseg);
  REAL       counterclockwise(     Vertex       pa, Vertex pb, Vertex pc);
  REAL       incircle(             Vertex       pa, Vertex pb, Vertex pc, Vertex pd);
  REAL       orient3d(             Vertex       pa
                                  ,Vertex       pb
                                  ,Vertex       pc
                                  ,Vertex       pd
                                  ,REAL         aheight
                                  ,REAL         bheight
                                  ,REAL         cheight
                                  ,REAL         dheight);
  REAL         nonregular(         Vertex       pa
                                  ,Vertex       pb
                                  ,Vertex       pc
                                  ,Vertex       pd);
  void         findcircumcenter(   Vertex       torg
                                  ,Vertex       tdest
                                  ,Vertex       tapex
                                  ,Vertex       circumcenter
                                  ,REAL        *xi
                                  ,REAL        *eta
                                  ,int          offcenter);
  void         checkmesh();
  void         checkdelaunay();
  void         enqueuebadtriang(   BadTriangle *badtri);
  void         enqueuebadtri(      otri        *enqtri
                                  ,REAL         minedge
                                  ,Vertex       enqapex
                                  ,Vertex       enqorg
                                  ,Vertex       enqdest);
  BadTriangle *dequeuebadtriang();
  int          checkseg4encroach(  osub        *testsubseg);
  void         testtriangle(       otri        *testtri);

  void         makevertexmap();
  LocateResult preciselocate(      Vertex       searchpoint, otri *searchtri, int stopatsubsegment);
  LocateResult locate(             Vertex       searchpoint, otri *searchtri);

  void insertsubseg(               otri        *tri, int subsegmark);
  void flip(                       otri        *flipedge);
  void unflip(                     otri        *flipedge);
  InserVvertexResult insertvertex( Vertex       newvertex
                                  ,otri        *searchtri
                                  ,osub        *splitseg
                                  ,int          segmentflaws
                                  ,int          triflaws);

  void triangulatepolygon(         otri        *firstedge
                                  ,otri        *lastedge
                                  ,int          edgecount
                                  ,int          doflip
                                  ,int          triflaws);
  void deletevertex(               otri        *deltri);
  void undovertex();
  void mergehulls(                 otri        *farleft
                                  ,otri        *innerleft
                                  ,otri        *innerright
                                  ,otri        *farright
                                  ,int          axis);

  void divconqrecurse(             Vertex      *sortarray
                                  ,int          vertices
                                  ,int          axis
                                  ,otri        *farleft
                                  ,otri        *farright);
  long removeghosts(               otri        *startghost);
  long divconqdelaunay();
  void boundingbox();
  long removebox();
  long incrementaldelaunay();
  void createeventheap(            Event     ***eventheap
                                  ,Event      **events
                                  ,Event      **freeevents);
  int rightofhyperbola(            otri        *fronttri
                                  ,Vertex       newsite);
  REAL circletop(                  Vertex       pa
                                  ,Vertex       pb
                                  ,Vertex       pc
                                  ,REAL         ccwabc);
  SplayNode *splay(                SplayNode   *splaytree
                                  ,Vertex       searchpoint
                                  ,otri        *searchtri);
  SplayNode *splayinsert(          SplayNode   *splayroot
                                  ,otri        *newkey
                                  ,Vertex       searchpoint);
  SplayNode *circletopinsert(      SplayNode   *splayroot
                                  ,otri        *newkey
                                  ,Vertex       pa
                                  ,Vertex       pb
                                  ,Vertex       pc
                                  ,REAL         topy);
  SplayNode *frontlocate(          SplayNode   *splayroot
                                  ,otri        *bottommost
                                  ,Vertex       searchvertex
                                  ,otri        *searchtri
                                  ,int         *farright);
  long sweeplinedelaunay();
  long delaunay();

  FindDirectionResult finddirection(otri       *searchtri, Vertex searchpoint);
  void segmentintersection(         otri       *splittri, osub *splitsubseg, Vertex endpoint2);
  int scoutsegment(                 otri       *searchtri, Vertex endpoint2, int newmark);
  void conformingedge(              Vertex      endpoint1, Vertex endpoint2, int newmark);
  void delaunayfixup(               otri       *fixuptri, int leftside);
  void constrainededge(             otri       *starttri, Vertex endpoint2, int newmark);
  void insertsegment(               Vertex      endpoint1, Vertex endpoint2, int newmark);
  void markhull();

  void infecthull();
  void plague();
  void regionplague(               REAL         attribute, REAL area);
  void carveholes(                 REAL        *holelist, int holes, REAL *regionlist, int regions);
  void tallyencs();
  void splitencsegs(               int          triflaws);
  void tallyfaces();
  void splittriangle(              BadTriangle *badtri);
  void enforcequality();
  void highorder();

#ifdef TRILIBRARY
  int reconstruct(                 int         *trianglelist
                                  ,REAL        *triangleattriblist
                                  ,REAL        *trianglearealist
                                  ,int          elements
                                  ,int          corners
                                  ,int          attribs
                                  ,int         *segmentlist
                                  ,int         *segmentmarkerlist
                                  ,int          numberofsegments);
  void formskeleton(               int         *segmentlist
                                  ,int         *segmentmarkerlist
                                  ,int          numberofsegments);
  void transfernodes(              REAL        *pointlist
                                  ,REAL        *pointattriblist
                                  ,int         *pointmarkerlist
                                  ,int          numberofpoints
                                  ,int          numberofpointattribs);
  void writenodes(                 REAL       **pointlist
                                  ,REAL       **pointattriblist
                                  ,int        **pointmarkerlist);
  void writeelements(              int        **trianglelist
                                  ,REAL       **triangleattriblist);
  void writepoly(                  int        **segmentlist
                                  ,int        **segmentmarkerlist);
  void writeedges(                 int        **edgelist
                                  ,int        **edgemarkerlist);
  void writevoronoi(               REAL       **vpointlist
                                  ,REAL       **vpointattriblist
                                  ,int        **vpointmarkerlist
                                  ,int        **vedgelist
                                  ,int        **vedgemarkerlist
                                  ,REAL       **vnormlist);
  void writeneighbors(             int        **neighborlist);
#else // not TRILIBRARY
  long reconstruct(                char        *elefilename
                                  ,char        *areafilename
                                  ,char        *polyfilename
                                  ,FILE        *polyfile);
  void formskeleton(               FILE        *polyfile
                                  ,char        *polyfilename);
  void readnodes(                  char        *nodefilename
                                  ,char        *polyfilename
                                  ,FILE       **polyfile);
  void readholes(                  FILE        *polyfile
                                  ,char        *polyfilename
                                  ,REAL       **hlist, int *holes
                                  ,REAL       **rlist, int *regions);
  void writenodes(                 char        *nodefilename
                                  ,int          argc, char **argv);
  void writeelements(              char        *elefilename
                                  ,int          argc, char **argv);
  void writepoly(                  char        *polyfilename
                                  ,REAL        *holelist, int holes
                                  ,REAL        *regionlist, int regions
                                  ,int          argc, char **argv);
  void writeedges(                 char        *edgefilename
                                  ,int          argc, char **argv);
  void writevoronoi(               char        *vnodefilename
                                  ,char        *vedgefilename
                                  ,int          argc, char **argv);
  void writeneighbors(             char        *neighborfilename
                                  ,int          argc, char **argv);
  void writeoff(                   char        *offfilename
                                  ,int          argc, char **argv);
#endif // not TRILIBRARY

  void numbernodes();
  String getQualityStatistics();
  String getStatistics();

#ifdef TRILIBRARY
  friend void triangulate(char *triswitches, TriangulateIO *in, TriangulateIO *out, TriangulateIO *vorout);
#else
  friend int triangleMain(int argc, char **argv);
#endif
};

//***************************************************************************
//
//  Mesh manipulation primitives. Each Triangle contains three pointers to
//  other triangles, with orientations. Each pointer points not to the
//  first byte of a Triangle, but to one of the first three bytes of a
//  Triangle. It is necessary to extract both the Triangle itself and the
//  orientation. To save memory, I keep both pieces of information in one
//  pointer. To make this possible, I assume that all triangles are aligned
//  to four-byte boundaries. The decode() routine below decodes a pointer,
//  extracting an orientation (in the range 0 to 2) and a pointer to the
//  beginning of a Triangle. The encode() routine compresses a pointer to a
//  Triangle and an orientation into a single pointer. My assumptions that
//  triangles are four-byte-aligned and that the `unsigned long' type is
//  long enough to hold a pointer are two of the few kludges in this program.
//
//  Subsegments are manipulated similarly. A pointer to a subsegment
//  carries both an address and an orientation in the range 0 to 1.
//
//  The other primitives take an oriented Triangle or oriented subsegment,
//  and return an oriented Triangle or oriented subsegment or Vertex; or
//  they change the connections in the data structure.
//
//  Below, triangles and subsegments are denoted by their vertices. The
//  Triangle abc has origin (org) a, destination (dest) b, and apex (apex)
//  c. These vertices occur in counterclockwise order about the Triangle.
//  The handle abc may simultaneously denote Vertex a, edge ab, and Triangle
//  abc.
//
//  Similarly, the subsegment ab has origin (sorg) a and destination (sdest)
//  b. If ab is thought to be directed upward (with b directly above a),
//  then the handle ab is thought to grasp the right side of ab, and may
//  simultaneously denote Vertex a and edge ab.
//
//  An asterisk (*) denotes a Vertex whose identity is unknown.
//
//  Given this notation, a partial list of mesh manipulation primitives
//  follows.
//
//
//  For triangles:
//
//  sym:  Find the abutting Triangle; same edge.
//  sym(abc) -> ba*
//
//  lnext:  Find the next edge (counterclockwise) of a Triangle.
//  lnext(abc) -> bca
//
//  lprev:  Find the previous edge (clockwise) of a Triangle.
//  lprev(abc) -> cab
//
//  onext:  Find the next edge counterclockwise with the same origin.
//  onext(abc) -> ac*
//
//  oprev:  Find the next edge clockwise with the same origin.
//  oprev(abc) -> a*b
//
//  dnext:  Find the next edge counterclockwise with the same destination.
//  dnext(abc) -> *ba
//
//  dprev:  Find the next edge clockwise with the same destination.
//  dprev(abc) -> cb*
//
//  rnext:  Find the next edge (counterclockwise) of the adjacent Triangle.
//  rnext(abc) -> *a*
//
//  rprev:  Find the previous edge (clockwise) of the adjacent Triangle.
//  rprev(abc) -> b**
//
//  org:  Origin          dest:  Destination          apex:  Apex
//  org(abc) -> a         dest(abc) -> b              apex(abc) -> c
//
//  bond:  Bond two triangles together at the resepective handles.
//  bond(abc, bad)
//
//
//  For subsegments:
//
//  ssym:  Reverse the orientation of a subsegment.
//  ssym(ab) -> ba
//
//  spivot:  Find adjoining subsegment with the same origin.
//  spivot(ab) -> a*
//
//  snext:  Find next subsegment in sequence.
//  snext(ab) -> b*
//
//  sorg:  Origin                      sdest:  Destination
//  sorg(ab) -> a                      sdest(ab) -> b
//
//  sbond:  Bond two subsegments together at the respective origins.
//  sbond(ab, ac)
//
//
//  For interacting tetrahedra and subfacets:
//
//  tspivot:  Find a subsegment abutting a Triangle.
//  tspivot(abc) -> ba
//
//  stpivot:  Find a Triangle abutting a subsegment.
//  stpivot(ab) -> ba*
//
//  tsbond:  Bond a Triangle to a subsegment.
//  tsbond(abc, ba)
//
//***************************************************************************

//******** Mesh manipulation primitives begin here
// Fast lookup arrays to speed some of the mesh manipulation primitives.
static const int plus1mod3[ 3] = {1, 2, 0};
static const int minus1mod3[3] = {2, 0, 1};

//******** Primitives for triangles

// decode() converts a pointer to an oriented Triangle. The orientation is
// extracted from the two least significant bits of the pointer.

#define decode(ptr, otri)                                                     \
  (otri).orient = (int) ((size_t)(ptr) & 3l);                                 \
  (otri).tri = (Triangle *)((size_t)(ptr) ^ (size_t)(otri).orient)

// encode() compresses an oriented Triangle into a single pointer. It
// relies on the assumption that all triangles are aligned to four-byte
// boundaries, so the two least significant bits of (otri).tri are zero.

#define encode(otri)                                                          \
  (Triangle) ((size_t)(otri).tri | (size_t)(otri).orient)

// The following handle manipulation primitives are all described by Guibas
// and Stolfi. However, Guibas and Stolfi use an edge-based data
// structure, whereas I use a Triangle-based data structure.

// sym() finds the abutting Triangle, on the same edge. Note that the edge
// direction is necessarily reversed, because the handle specified by an
// oriented Triangle is directed counterclockwise around the Triangle.

#define sym(otri1, otri2)                                                     \
  ptr = (otri1).tri[(otri1).orient];                                          \
  decode(ptr, otri2);

#define symself(otri)                                                         \
  ptr = (otri).tri[(otri).orient];                                            \
  decode(ptr, otri);

// lnext() finds the next edge (counterclockwise) of a Triangle.

#define lnext(otri1, otri2)                                                   \
  (otri2).tri = (otri1).tri;                                                  \
  (otri2).orient = plus1mod3[(otri1).orient]

#define lnextself(otri)                                                       \
  (otri).orient = plus1mod3[(otri).orient]

// lprev() finds the previous edge (clockwise) of a Triangle.

#define lprev(otri1, otri2)                                                   \
  (otri2).tri = (otri1).tri;                                                  \
  (otri2).orient = minus1mod3[(otri1).orient]

#define lprevself(otri)                                                       \
  (otri).orient = minus1mod3[(otri).orient]

// onext() spins counterclockwise around a Vertex; that is, it finds the
// next edge with the same origin in the counterclockwise direction. This
// edge is part of a different Triangle.

#define onext(otri1, otri2)                                                   \
  lprev(otri1, otri2);                                                        \
  symself(otri2);

#define onextself(otri)                                                       \
  lprevself(otri);                                                            \
  symself(otri);

// oprev() spins clockwise around a Vertex; that is, it finds the next edge
// with the same origin in the clockwise direction. This edge is part of
// a different Triangle.

#define oprev(otri1, otri2)                                                   \
  sym(otri1, otri2);                                                          \
  lnextself(otri2);

#define oprevself(otri)                                                       \
  symself(otri);                                                              \
  lnextself(otri);

// dnext() spins counterclockwise around a Vertex; that is, it finds the
// next edge with the same destination in the counterclockwise direction.
// This edge is part of a different Triangle.

#define dnext(otri1, otri2)                                                   \
  sym(otri1, otri2);                                                          \
  lprevself(otri2);

#define dnextself(otri)                                                       \
  symself(otri);                                                              \
  lprevself(otri);

// dprev() spins clockwise around a Vertex; that is, it finds the next edge
// with the same destination in the clockwise direction. This edge is
// part of a different Triangle.

#define dprev(otri1, otri2)                                                   \
  lnext(otri1, otri2);                                                        \
  symself(otri2);

#define dprevself(otri)                                                       \
  lnextself(otri);                                                            \
  symself(otri);

// rnext() moves one edge counterclockwise about the adjacent Triangle.
// (It's best understood by reading Guibas and Stolfi. It involves
// changing triangles twice.)

#define rnext(otri1, otri2)                                                   \
  sym(otri1, otri2);                                                          \
  lnextself(otri2);                                                           \
  symself(otri2);

#define rnextself(otri)                                                       \
  symself(otri);                                                              \
  lnextself(otri);                                                            \
  symself(otri);

// rprev() moves one edge clockwise about the adjacent Triangle.
// (It's best understood by reading Guibas and Stolfi. It involves
// changing triangles twice.)

#define rprev(otri1, otri2)                                                   \
  sym(otri1, otri2);                                                          \
  lprevself(otri2);                                                           \
  symself(otri2);

#define rprevself(otri)                                                       \
  symself(otri);                                                              \
  lprevself(otri);                                                            \
  symself(otri);

// These primitives determine or set the origin, destination, or apex of a
// Triangle.

#define org(otri, vertexptr)                                                  \
  vertexptr = (Vertex) (otri).tri[plus1mod3[(otri).orient] + 3]

#define dest(otri, vertexptr)                                                 \
  vertexptr = (Vertex) (otri).tri[minus1mod3[(otri).orient] + 3]

#define apex(otri, vertexptr)                                                 \
  vertexptr = (Vertex) (otri).tri[(otri).orient + 3]

#define setorg(otri, vertexptr)                                               \
  (otri).tri[plus1mod3[(otri).orient] + 3] = (Triangle) vertexptr

#define setdest(otri, vertexptr)                                              \
  (otri).tri[minus1mod3[(otri).orient] + 3] = (Triangle) vertexptr

#define setapex(otri, vertexptr)                                              \
  (otri).tri[(otri).orient + 3] = (Triangle) vertexptr

// Bond two triangles together.

#define bond(otri1, otri2)                                                    \
  (otri1).tri[(otri1).orient] = encode(otri2);                                \
  (otri2).tri[(otri2).orient] = encode(otri1)

// Dissolve a bond (from one side). Note that the other Triangle will still
// think it's connected to this Triangle. Usually, however, the other
// Triangle is being deleted entirely, or bonded to another Triangle, so
// it doesn't matter.

#define dissolve(otri)                                                        \
  (otri).tri[(otri).orient] = (Triangle) dummytri

// Copy an oriented Triangle.
#define otricopy(otri1, otri2)                                                \
  (otri2).tri = (otri1).tri;                                                  \
  (otri2).orient = (otri1).orient

// Test for equality of oriented triangles.
#define otriequal(otri1, otri2)                                               \
  (((otri1).tri == (otri2).tri) &&                                            \
   ((otri1).orient == (otri2).orient))

// Primitives to infect or cure a Triangle with the virus. These rely on
// the assumption that all subsegments are aligned to four-byte boundaries.
#define infect(otri)                                                          \
  (otri).tri[6] = (Triangle)((size_t)(otri).tri[6] | 2)

#define uninfect(otri)                                                        \
  (otri).tri[6] = (Triangle)((size_t) (otri).tri[6] & ~(size_t)2)

// Test a Triangle for viral infection.
#define infected(otri)                                                        \
  (((size_t)(otri).tri[6] & 2) != 0l)

// Check or set a Triangle's attributes.
#define elemattribute(otri, attnum)                                           \
  ((REAL*)(otri).tri)[elemattribindex + (attnum)]

#define setelemattribute(otri, attnum, value)                                 \
  ((REAL*)(otri).tri)[elemattribindex + (attnum)] = value

// Check or set a Triangle's maximum area bound.
#define areabound(otri)  ((REAL *) (otri).tri)[areaboundindex]

#define setareabound(otri, value)                                             \
  ((REAL*)(otri).tri)[areaboundindex] = value

// Check or set a Triangle's deallocation. Its second pointer is set to
// NULL to indicate that it is not allocated. (Its first pointer is used
// for the stack of dead items.)  Its fourth pointer (its first Vertex)
// is set to NULL in case a `BadTriangle' structure points to it.

#define deadtri(tria)  ((tria)[1] == NULL)

#define killtri(tria)                                                         \
  (tria)[1] = NULL;                                                           \
  (tria)[3] = NULL

//******** Primitives for subsegments

// sdecode() converts a pointer to an oriented subsegment. The orientation
// is extracted from the least significant bit of the pointer. The two
// least significant bits (one for orientation, one for viral infection)
// are masked out to produce the real pointer.

#define sdecode(sptr, osub)                                                   \
  (osub).ssorient = (int)((size_t)(sptr) & 1);                                \
  (osub).ss = (SubSeg*)((size_t)(sptr) & ~(size_t)3)

// sencode() compresses an oriented subsegment into a single pointer. It
// relies on the assumption that all subsegments are aligned to two-byte
// boundaries, so the least significant bit of (osub).ss is zero.

#define sencode(osub)                                                         \
  (SubSeg)((size_t)(osub).ss | (size_t) (osub).ssorient)

// ssym() toggles the orientation of a subsegment.

#define ssym(osub1, osub2)                                                    \
  (osub2).ss = (osub1).ss;                                                    \
  (osub2).ssorient = 1 - (osub1).ssorient

#define ssymself(osub)                                                        \
  (osub).ssorient = 1 - (osub).ssorient

// spivot() finds the other subsegment (from the same segment) that shares
// the same origin.

#define spivot(osub1, osub2)                                                  \
  sptr = (osub1).ss[(osub1).ssorient];                                        \
  sdecode(sptr, osub2)

#define spivotself(osub)                                                      \
  sptr = (osub).ss[(osub).ssorient];                                          \
  sdecode(sptr, osub)

// snext() finds the next subsegment (from the same segment) in sequence;
// one whose origin is the input subsegment's destination.

#define snext(osub1, osub2)                                                   \
  sptr = (osub1).ss[1 - (osub1).ssorient];                                    \
  sdecode(sptr, osub2)

#define snextself(osub)                                                       \
  sptr = (osub).ss[1 - (osub).ssorient];                                      \
  sdecode(sptr, osub)

// These primitives determine or set the origin or destination of a
// subsegment or the segment that includes it.

#define sorg(osub, vertexptr)                                                 \
  vertexptr = (Vertex) (osub).ss[2 + (osub).ssorient]

#define sdest(osub, vertexptr)                                                \
  vertexptr = (Vertex) (osub).ss[3 - (osub).ssorient]

#define setsorg(osub, vertexptr)                                              \
  (osub).ss[2 + (osub).ssorient] = (SubSeg) vertexptr

#define setsdest(osub, vertexptr)                                             \
  (osub).ss[3 - (osub).ssorient] = (SubSeg) vertexptr

#define segorg(osub, vertexptr)                                               \
  vertexptr = (Vertex) (osub).ss[4 + (osub).ssorient]

#define segdest(osub, vertexptr)                                              \
  vertexptr = (Vertex) (osub).ss[5 - (osub).ssorient]

#define setsegorg(osub, vertexptr)                                            \
  (osub).ss[4 + (osub).ssorient] = (SubSeg) vertexptr

#define setsegdest(osub, vertexptr)                                           \
  (osub).ss[5 - (osub).ssorient] = (SubSeg) vertexptr

// These primitives read or set a boundary marker. Boundary markers are
// used to hold user-defined tags for setting boundary conditions in
// finite element solvers.

#define mark(osub)  (* (int *) ((osub).ss + 8))

#define setmark(osub, value)                                                  \
  * (int *) ((osub).ss + 8) = value

// Bond two subsegments together.

#define sbond(osub1, osub2)                                                   \
  (osub1).ss[(osub1).ssorient] = sencode(osub2);                              \
  (osub2).ss[(osub2).ssorient] = sencode(osub1)

// Dissolve a subsegment bond (from one side). Note that the other
// subsegment will still think it's connected to this subsegment.

#define sdissolve(osub)                                                       \
  (osub).ss[(osub).ssorient] = (SubSeg) dummysub

// Copy a subsegment.

#define subsegcopy(osub1, osub2)                                              \
  (osub2).ss = (osub1).ss;                                                    \
  (osub2).ssorient = (osub1).ssorient

// Test for equality of subsegments.

#define subsegequal(osub1, osub2)                                             \
  (((osub1).ss == (osub2).ss) &&                                              \
   ((osub1).ssorient == (osub2).ssorient))

// Check or set a subsegment's deallocation. Its second pointer is set to
// NULL to indicate that it is not allocated. (Its first pointer is used
// for the stack of dead items.)  Its third pointer (its first Vertex)
// is set to NULL in case a `BadSubSeg' structure points to it.

#define deadsubseg(sub)  ((sub)[1] == NULL)

#define killsubseg(sub)                                                       \
  (sub)[1] = NULL;                                                            \
  (sub)[2] = NULL

//******** Primitives for interacting triangles and subsegments

// tspivot() finds a subsegment abutting a Triangle.
#define tspivot(otri, osub)                                                   \
  sptr = (SubSeg) (otri).tri[6 + (otri).orient];                              \
  sdecode(sptr, osub)

// stpivot() finds a Triangle abutting a subsegment.It requires that the
// variable `ptr' of type `Triangle' be defined.

#define stpivot(osub, otri)                                                   \
  ptr = (Triangle) (osub).ss[6 + (osub).ssorient];                            \
  decode(ptr, otri)

// Bond a Triangle to a subsegment.
#define tsbond(otri, osub)                                                    \
  (otri).tri[6 + (otri).orient] = (Triangle) sencode(osub);                   \
  (osub).ss[6 + (osub).ssorient] = (SubSeg) encode(otri)

// Dissolve a bond (from the Triangle side).
#define tsdissolve(otri)                                                      \
  (otri).tri[6 + (otri).orient] = (Triangle) dummysub

// Dissolve a bond (from the subsegment side).
#define stdissolve(osub)                                                      \
  (osub).ss[6 + (osub).ssorient] = (SubSeg) dummytri

//******** Primitives for vertices
#define vertexmark(vx)  ((int *) (vx))[vertexmarkindex]

#define setvertexmark(vx, value)                                              \
  ((int *) (vx))[vertexmarkindex] = value

#define vertextype(vx)  ((int *) (vx))[vertexmarkindex + 1]

#define setvertextype(vx, value)                                              \
  ((int *) (vx))[vertexmarkindex + 1] = value

#define vertex2tri(vx)  ((Triangle *) (vx))[vertex2triindex]

#define setvertex2tri(vx, value)                                              \
  ((Triangle *) (vx))[vertex2triindex] = value

//******** Mesh manipulation primitives end here                     ********

//******** User-defined Triangle evaluation routine begins here      ********

//
//  triunsuitable()   Determine if a Triangle is unsuitable, and thus must
//                    be further refined.
//
//  You may write your own procedure that decides whether or not a selected
//  Triangle is too big (and needs to be refined). There are two ways to do
//  this.
//
//  (1)  Modify the procedure `triunsuitable' below, then recompile
//  Triangle.
//
//  (2)  Define the symbol EXTERNAL_TEST (either by adding the definition
//  to this file, or by using the appropriate compiler switch). This way,
//  you can compile Triangle.c separately from your test. Write your own
//  `triunsuitable' procedure in a separate C file (using the same prototype
//  as below). Compile it and link the object code with Triangle.o.
//
//  This procedure returns 1 if the Triangle is too large and should be
//  refined; 0 otherwise.
//
#ifdef EXTERNAL_TEST
int triunsuitable();
#else // not EXTERNAL_TEST
int triunsuitable(Vertex triorg, Vertex tridest, Vertex triapex, REAL area) {
  REAL dxoa, dxda, dxod;
  REAL dyoa, dyda, dyod;
  REAL oalen, dalen, odlen;
  REAL maxlen;

  dxoa = triorg[0] - triapex[0];
  dyoa = triorg[1] - triapex[1];
  dxda = tridest[0] - triapex[0];
  dyda = tridest[1] - triapex[1];
  dxod = triorg[0] - tridest[0];
  dyod = triorg[1] - tridest[1];
  // Find the squares of the lengths of the Triangle's three edges.
  oalen = dxoa * dxoa + dyoa * dyoa;
  dalen = dxda * dxda + dyda * dyda;
  odlen = dxod * dxod + dyod * dyod;
  // Find the square of the length of the longest edge.
  maxlen = (dalen > oalen) ? dalen : oalen;
  maxlen = (odlen > maxlen) ? odlen : maxlen;

  if (maxlen > 0.05 * (triorg[0] * triorg[0] + triorg[1] * triorg[1]) + 0.02) {
    return 1;
  } else {
    return 0;
  }
}

#endif // not EXTERNAL_TEST

//******** User-defined Triangle evaluation routine ends here

//******** User interaction routines begin here

void triError(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throwException(msg);
}

// internalerror()   Ask the user to send me the defective product. throw Exception
void internalerror(const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const form, ...) {
  String msg = format(_T("Internal error in %s:"), method);
  va_list argptr;
  va_start(argptr, form);
  msg += vformat(form, argptr);
  va_end(argptr);
  msg += _T(".\nPlease report this bug to jrs@cs.berkeley.edu\n"
            "Include the message above, your input data set, and the exact\n"
            "command line you used to run Triangle");
  throwException(msg);
}

void internalerrorNoTriangleFound(const TCHAR *method, const Vertex &v1, const Vertex &v2) {
  internalerror(method
               ,_T("Unable to find a triangle leading from %s to %s")
               , vertexToString(v1).cstr(), vertexToString(v2).cstr());
}

void internalerrorNoPSLGVertex(const TCHAR *method, const Vertex p) {
  internalerror(method, _T("Unable to locate PSLG vertex %s in triangulation"), vertexToString(p).cstr());
}

#ifndef TRILIBRARY

static void printMsgAndExit(const String &str) {
  _tprintf(_T("%s"), getTriangleInfo().cstr());
  exit(-1);
}

static inline void printInfoMsgAndExit()   { printMsgAndExit(getTriangleInfo()  ); }
static inline void printSyntaxMsgAndExit() { printMsgAndExit(getTriangleSyntax()); }

#endif // TRILIBRARY

//  Read the command line, identify switches, and set up options and file names.
void Behavior::parsecommandline(int argc, char **argv) {
#ifdef TRILIBRARY
#define STARTINDEX 0
#else // not TRILIBRARY
#define STARTINDEX 1
  int increment;
  int meshnumber;
#endif // not TRILIBRARY
  int i, j, k;
  char workstring[FILENAMESIZE];

  poly = refine = quality = 0;
  vararea = fixedarea = usertest = 0;
  regionattrib = convex = weighted = jettison = 0;
  firstnumber = 1;
  edgesout = voronoi = neighbors = geomview = 0;
  nobound = nopolywritten = nonodewritten = noelewritten = 0;
  noiterationnum = 0;
  noholes = noexact = 0;
  incremental = sweepline = 0;
  dwyer = 1;
  splitseg = 0;
  docheck = 0;
  nobisect = 0;
  conformdel = 0;
  steiner = -1;
  order = 1;
  minangle = 0.0;
  maxarea = -1.0;
  quiet = verbose = 0;
#ifndef TRILIBRARY
  innodefilename[0] = '\0';
#endif // not TRILIBRARY

  for (i = STARTINDEX; i < argc; i++) {
#ifndef TRILIBRARY
    if (argv[i][0] == '-') {
#endif // not TRILIBRARY
      for (j = STARTINDEX; argv[i][j] != '\0'; j++) {
        if (argv[i][j] == 'p') {
          poly = 1;
        }
#ifndef CDT_ONLY
        if (argv[i][j] == 'r') {
          refine = 1;
        }
        if (argv[i][j] == 'q') {
          quality = 1;
          if (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
              (argv[i][j + 1] == '.')) {
            k = 0;
            while (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                   (argv[i][j + 1] == '.')) {
              j++;
              workstring[k] = argv[i][j];
              k++;
            }
            workstring[k] = '\0';
            minangle = (REAL) strtod(workstring, NULL);
          } else {
            minangle = 20.0;
          }
        }
        if (argv[i][j] == 'a') {
          quality = 1;
          if (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
              (argv[i][j + 1] == '.')) {
            fixedarea = 1;
            k = 0;
            while (((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) ||
                   (argv[i][j + 1] == '.')) {
              j++;
              workstring[k] = argv[i][j];
              k++;
            }
            workstring[k] = '\0';
            maxarea = (REAL) strtod(workstring, NULL);
            if (maxarea <= 0.0) {
              triError(_T("Maximum area must be greater than zero"));
	          }
	        } else {
            vararea = 1;
	        }
	      }
        if (argv[i][j] == 'u') {
          quality = 1;
          usertest = 1;
        }
#endif // not CDT_ONLY
        if (argv[i][j] == 'A') {
          regionattrib = 1;
        }
        if (argv[i][j] == 'c') {
          convex = 1;
        }
        if (argv[i][j] == 'w') {
          weighted = 1;
        }
        if (argv[i][j] == 'W') {
          weighted = 2;
        }
        if (argv[i][j] == 'j') {
          jettison = 1;
        }
        if (argv[i][j] == 'z') {
          firstnumber = 0;
        }
        if (argv[i][j] == 'e') {
          edgesout = 1;
	}
        if (argv[i][j] == 'v') {
          voronoi = 1;
	}
        if (argv[i][j] == 'n') {
          neighbors = 1;
	}
        if (argv[i][j] == 'g') {
          geomview = 1;
	}
        if (argv[i][j] == 'B') {
          nobound = 1;
	}
        if (argv[i][j] == 'P') {
          nopolywritten = 1;
	}
        if (argv[i][j] == 'N') {
          nonodewritten = 1;
	}
        if (argv[i][j] == 'E') {
          noelewritten = 1;
	}
#ifndef TRILIBRARY
        if (argv[i][j] == 'I') {
          noiterationnum = 1;
	}
#endif // not TRILIBRARY
        if (argv[i][j] == 'O') {
          noholes = 1;
	}
        if (argv[i][j] == 'X') {
          noexact = 1;
	}
        if (argv[i][j] == 'o') {
          if (argv[i][j + 1] == '2') {
            j++;
            order = 2;
          }
	}
#ifndef CDT_ONLY
        if (argv[i][j] == 'Y') {
          nobisect++;
	}
        if (argv[i][j] == 'S') {
          steiner = 0;
          while ((argv[i][j + 1] >= '0') && (argv[i][j + 1] <= '9')) {
            j++;
            steiner = steiner * 10 + (int) (argv[i][j] - '0');
          }
        }
#endif // not CDT_ONLY
#ifndef REDUCED
        if (argv[i][j] == 'i') {
          incremental = 1;
        }
        if (argv[i][j] == 'F') {
          sweepline = 1;
        }
#endif // not REDUCED
        if (argv[i][j] == 'l') {
          dwyer = 0;
        }
#ifndef REDUCED
#ifndef CDT_ONLY
        if (argv[i][j] == 's') {
          splitseg = 1;
        }
        if ((argv[i][j] == 'D') || (argv[i][j] == 'L')) {
          quality = 1;
          conformdel = 1;
        }
#endif // not CDT_ONLY
        if (argv[i][j] == 'C') {
          docheck = 1;
        }
#endif // not REDUCED
        if (argv[i][j] == 'Q') {
          quiet = 1;
        }
        if (argv[i][j] == 'V') {
          verbose++;
        }
#ifndef TRILIBRARY
        if ((argv[i][j] == 'h') || (argv[i][j] == 'H') || (argv[i][j] == '?')) {
          printInfoMsgAndExit();
	}
#endif // not TRILIBRARY
      }
#ifndef TRILIBRARY
    } else {
      strncpy(innodefilename, argv[i], FILENAMESIZE - 1);
      innodefilename[FILENAMESIZE - 1] = '\0';
    }
#endif // not TRILIBRARY
  }
#ifndef TRILIBRARY
  if (innodefilename[0] == '\0') {
    printSyntaxMsgAndExit();
  }
  if (!strcmp(&innodefilename[strlen(innodefilename) - 5], ".node")) {
    innodefilename[strlen(innodefilename) - 5] = '\0';
  }
  if (!strcmp(&innodefilename[strlen(innodefilename) - 5], ".poly")) {
    innodefilename[strlen(innodefilename) - 5] = '\0';
    poly = 1;
  }
#ifndef CDT_ONLY
  if (!strcmp(&innodefilename[strlen(innodefilename) - 4], ".ele")) {
    innodefilename[strlen(innodefilename) - 4] = '\0';
    refine = 1;
  }
  if (!strcmp(&innodefilename[strlen(innodefilename) - 5], ".area")) {
    innodefilename[strlen(innodefilename) - 5] = '\0';
    refine = 1;
    quality = 1;
    vararea = 1;
  }
#endif // not CDT_ONLY
#endif // not TRILIBRARY
  usesegments = poly || refine || quality || convex;
  goodangle = cos(minangle * PI / 180.0);
  if (goodangle == 1.0) {
    offconstant = 0.0;
  } else {
    offconstant = 0.475 * sqrt((1.0 + goodangle) / (1.0 - goodangle));
  }
  goodangle *= goodangle;
  if (refine && noiterationnum) {
    triError(_T("You cannot use the -I switch when refining a triangulation"));
  }
  // Be careful not to allocate space for element area constraints that
  // will never be assigned any value (other than the default -1.0).
  if (!refine && !poly) {
    vararea = 0;
  }
  // Be careful not to add an extra attribute to each element unless the
  // input supports it (PSLG in, but not refining a preexisting mesh).
  if (refine || !poly) {
    regionattrib = 0;
  }
  // Regular/weighted triangulations are incompatible with PSLGs and meshing.
  if (weighted && (poly || quality)) {
    weighted = 0;
    if (!quiet) {
      printf("Warning:  weighted triangulations (-w, -W) are incompatible\n");
      printf("  with PSLGs (-p) and meshing (-q, -a, -u). Weights ignored.\n");
    }
  }
  if (jettison && nonodewritten && !quiet) {
    printf("Warning:  -j and -N switches are somewhat incompatible.\n");
    printf("  If any vertices are jettisoned, you will need the output\n");
    printf("  .node file to reconstruct the new node indices.");
  }

#ifndef TRILIBRARY
  strcpy(inpolyfilename, innodefilename);
  strcpy(inelefilename, innodefilename);
  strcpy(areafilename, innodefilename);
  increment = 0;
  strcpy(workstring, innodefilename);
  j = 1;
  while (workstring[j] != '\0') {
    if ((workstring[j] == '.') && (workstring[j + 1] != '\0')) {
      increment = j + 1;
    }
    j++;
  }
  meshnumber = 0;
  if (increment > 0) {
    j = increment;
    do {
      if ((workstring[j] >= '0') && (workstring[j] <= '9')) {
        meshnumber = meshnumber * 10 + (int) (workstring[j] - '0');
      } else {
        increment = 0;
      }
      j++;
    } while (workstring[j] != '\0');
  }
  if (noiterationnum) {
    strcpy(outnodefilename, innodefilename);
    strcpy(outelefilename, innodefilename);
    strcpy(edgefilename, innodefilename);
    strcpy(vnodefilename, innodefilename);
    strcpy(vedgefilename, innodefilename);
    strcpy(neighborfilename, innodefilename);
    strcpy(offfilename, innodefilename);
    strcat(outnodefilename, ".node");
    strcat(outelefilename, ".ele");
    strcat(edgefilename, ".edge");
    strcat(vnodefilename, ".v.node");
    strcat(vedgefilename, ".v.edge");
    strcat(neighborfilename, ".neigh");
    strcat(offfilename, ".off");
  } else if (increment == 0) {
    strcpy(outnodefilename, innodefilename);
    strcpy(outpolyfilename, innodefilename);
    strcpy(outelefilename, innodefilename);
    strcpy(edgefilename, innodefilename);
    strcpy(vnodefilename, innodefilename);
    strcpy(vedgefilename, innodefilename);
    strcpy(neighborfilename, innodefilename);
    strcpy(offfilename, innodefilename);
    strcat(outnodefilename, ".1.node");
    strcat(outpolyfilename, ".1.poly");
    strcat(outelefilename, ".1.ele");
    strcat(edgefilename, ".1.edge");
    strcat(vnodefilename, ".1.v.node");
    strcat(vedgefilename, ".1.v.edge");
    strcat(neighborfilename, ".1.neigh");
    strcat(offfilename, ".1.off");
  } else {
    workstring[increment] = '%';
    workstring[increment + 1] = 'd';
    workstring[increment + 2] = '\0';
    sprintf(outnodefilename, workstring, meshnumber + 1);
    strcpy(outpolyfilename, outnodefilename);
    strcpy(outelefilename, outnodefilename);
    strcpy(edgefilename, outnodefilename);
    strcpy(vnodefilename, outnodefilename);
    strcpy(vedgefilename, outnodefilename);
    strcpy(neighborfilename, outnodefilename);
    strcpy(offfilename, outnodefilename);
    strcat(outnodefilename, ".node");
    strcat(outpolyfilename, ".poly");
    strcat(outelefilename, ".ele");
    strcat(edgefilename, ".edge");
    strcat(vnodefilename, ".v.node");
    strcat(vedgefilename, ".v.edge");
    strcat(neighborfilename, ".neigh");
    strcat(offfilename, ".off");
  }
  strcat(innodefilename, ".node");
  strcat(inpolyfilename, ".poly");
  strcat(inelefilename, ".ele");
  strcat(areafilename, ".area");
#endif // not TRILIBRARY
}

#ifdef HAS_DEBUG_FUNCTIONS
//******** User interaction routines begin here

//******** Debugging routines begin here

//
//  printtriangle()   Print out the details of an oriented triangle.
//
//  I originally wrote this procedure to simplify debugging; it can be
//  called directly from the debugger, and presents information about an
//  oriented triangle in digestible form. It's also used when the
//  highest level of verbosity (`-VVV') is specified.
void Mesh::printtriangle(otri *t) {
  otri printtri;
  osub printsh;
  Vertex printvertex;

  _tprintf(_T("triangle %p with orientation %d:\n"), t->tri, t->orient);
  decode(t->tri[0], printtri);
  if (printtri.tri == dummytri) {
    _tprintf(_T("    [0] = Outer space\n"));
  } else {
    _tprintf(_T("    [0] = %p  %d\n"), printtri.tri, printtri.orient);
  }
  decode(t->tri[1], printtri);
  if (printtri.tri == dummytri) {
    _tprintf(_T("    [1] = Outer space\n"));
  } else {
    _tprintf(_T("    [1] = %p  %d\n"), printtri.tri, printtri.orient);
  }
  decode(t->tri[2], printtri);
  if (printtri.tri == dummytri) {
    _tprintf(_T("    [2] = Outer space\n"));
  } else {
    _tprintf(_T("    [2] = %p  %d\n"), printtri.tri, printtri.orient);
  }

  org(*t, printvertex);
  if (printvertex == NULL)
    _tprintf(_T("    Origin[%d] = NULL\n"), (t->orient + 1) % 3 + 3);
  else
    _tprintf(_T("    Origin[%d] = %p  %s\n"), (t->orient + 1) % 3 + 3, printvertex, vertexToString(printvertex).cstr());
  dest(*t, printvertex);
  if (printvertex == NULL)
    _tprintf(_T("    Dest  [%d] = NULL\n"), (t->orient + 2) % 3 + 3);
  else
    _tprintf(_T("    Dest  [%d] = %p  %s\n"), (t->orient + 2) % 3 + 3, printvertex, vertexToString(printvertex).cstr());
  apex(*t, printvertex);
  if (printvertex == NULL)
    _tprintf(_T("    Apex  [%d] = NULL\n"), t->orient + 3);
  else
    _tprintf(_T("    Apex  [%d] = %p  %s\n"), t->orient + 3, printvertex, vertexToString(printvertex).cstr());

  if (m_b.usesegments) {
    sdecode(t->tri[6], printsh);
    if (printsh.ss != dummysub) {
      _tprintf(_T("    [6] = %p  %d\n"), printsh.ss, printsh.ssorient);
    }
    sdecode(t->tri[7], printsh);
    if (printsh.ss != dummysub) {
      _tprintf(_T("    [7] = %p  %d\n"), printsh.ss, printsh.ssorient);
    }
    sdecode(t->tri[8], printsh);
    if (printsh.ss != dummysub) {
      _tprintf(_T("    [8] = %p  %d\n"), printsh.ss, printsh.ssorient);
    }
  }

  if (m_b.vararea) {
    _tprintf(_T("    Area constraint:  %.4g\n"), areabound(*t));
  }
}

//  printsubseg()   Print out the details of an oriented subsegment.
//
//  I originally wrote this procedure to simplify debugging; it can be
//  called directly from the debugger, and presents information about an
//  oriented subsegment in digestible form. It's also used when the highest
//  level of verbosity (`-VVV') is specified.
//
void Mesh::printsubseg(osub *s) {
  osub printsh;
  otri printtri;
  Vertex printvertex;

  _tprintf(_T("subsegment %p with orientation %d and mark %d:\n"), s->ss, s->ssorient, mark(*s));
  sdecode(s->ss[0], printsh);
  if (printsh.ss == dummysub) {
    _tprintf(_T("    [0] = No subsegment\n"));
  } else {
    _tprintf(_T("    [0] = %p  %d\n"), printsh.ss, printsh.ssorient);
  }
  sdecode(s->ss[1], printsh);
  if (printsh.ss == dummysub) {
    _tprintf(_T("    [1] = No subsegment\n"));
  } else {
    _tprintf(_T("    [1] = %p  %d\n"), printsh.ss, printsh.ssorient);
  }

  sorg(*s, printvertex);
  if (printvertex == NULL)
    _tprintf(_T("    Origin[%d] = NULL\n"), 2+s->ssorient);
  else
    _tprintf(_T("    Origin[%d] = %p  %s\n"), 2+s->ssorient, printvertex, vertexToString(printvertex).cstr());
  sdest(*s, printvertex);
  if (printvertex == NULL)
    _tprintf(_T("    Dest  [%d] = NULL\n"), 3-s->ssorient);
  else
    _tprintf(_T("    Dest  [%d] = %p  %s\n"), 3-s->ssorient, printvertex, vertexToString(printvertex).cstr());

  decode(s->ss[6], printtri);
  if (printtri.tri == dummytri) {
    _tprintf(_T("    [6] = Outer space\n"));
  } else {
    _tprintf(_T("    [6] = %p  %d\n"), printtri.tri, printtri.orient);
  }
  decode(s->ss[7], printtri);
  if (printtri.tri == dummytri) {
    _tprintf(_T("    [7] = Outer space\n"));
  } else {
    _tprintf(_T("    [7] = %p  %d\n"), printtri.tri, printtri.orient);
  }

  segorg(*s, printvertex);
  if (printvertex == NULL)
    _tprintf(_T("    Segment origin[%d] = NULL\n"), 4+s->ssorient);
  else
    _tprintf(_T("    Segment origin[%d] = %p  %s\n"), 4+s->ssorient, printvertex, vertexToString(printvertex).cstr());
  segdest(*s, printvertex);
  if (printvertex == NULL)
    _tprintf(_T("    Segment dest  [%d] = NULL\n"), 5-s->ssorient);
  else
    _tprintf(_T("    Segment dest  [%d] = %p  %s\n"), 5-s->ssorient, printvertex, vertexToString(printvertex).cstr());
}

//******** Debugging routines end here

#endif HAS_DEBUG_FUNCTIONS

//  dummyinit()   Initialize the triangle that fills "outer space" and the
//                omnipresent subsegment.
//
//  The triangle that fills "outer space," called `dummytri', is pointed to
//  by every triangle and subsegment on a boundary (be it outer or inner) of
//  the triangulation. Also, `dummytri' points to one of the triangles on
//  the convex hull (until the holes and concavities are carved), making it
//  possible to find a starting triangle for point location.
//
//  The omnipresent subsegment, `dummysub', is pointed to by every triangle
//  or subsegment that doesn't have a full complement of real subsegments
//  to point to.
//
//  `dummytri' and `dummysub' are generally required to fulfill only a few
//  invariants:  their vertices must remain NULL and `dummytri' must always
//  be bonded (at offset zero) to some triangle on the convex hull of the
//  mesh, via a boundary edge. Otherwise, the connections of `dummytri' and
//  `dummysub' may change willy-nilly. This makes it possible to avoid
//  writing a good deal of special-case code (in the edge flip, for example)
//  for dealing with the boundary of the mesh, places where no subsegment is
//  present, and so forth. Other entities are frequently bonded to
//  `dummytri' and `dummysub' as if they were real mesh entities, with no
//  harm done.
//
void Mesh::dummyinit(int trianglebytes, int subsegbytes) {
  size_t alignptr;

  // Set up `dummytri', the `Triangle' that occupies "outer space."
  dummytribase = (Triangle *)trimalloc(trianglebytes + triangles.getAlignBytes());
  // Align `dummytri' on a `triangles.alignbytes'-byte boundary.
  alignptr = (size_t) dummytribase;
  dummytri = (Triangle*)(alignptr + (size_t) triangles.getAlignBytes()
                      - (alignptr % (size_t) triangles.getAlignBytes()));
  // Initialize the three adjoining triangles to be "outer space."  These
  // will eventually be changed by various bonding operations, but their
  // values don't really matter, as long as they can legally be
  // dereferenced.
  dummytri[0] = (Triangle)dummytri;
  dummytri[1] = (Triangle)dummytri;
  dummytri[2] = (Triangle)dummytri;
  // Three NULL vertices.
  dummytri[3] = NULL;
  dummytri[4] = NULL;
  dummytri[5] = NULL;

  if (m_b.usesegments) {
    // Set up `dummysub', the omnipresent subsegment pointed to by any
    // Triangle side or subsegment end that isn't attached to a real
    // subsegment.
    dummysubbase = (SubSeg*)trimalloc(subsegbytes + subsegs.getAlignBytes());
    // Align `dummysub' on a `subsegs.alignbytes'-byte boundary.
    alignptr = (size_t)dummysubbase;
    dummysub = (SubSeg*)(alignptr + (size_t)subsegs.getAlignBytes()
                      - (alignptr % (size_t)subsegs.getAlignBytes()));
    // Initialize the two adjoining subsegments to be the omnipresent
    // subsegment. These will eventually be changed by various bonding
    // operations, but their values don't really matter, as long as they
    // can legally be dereferenced.
    dummysub[0] = (SubSeg)dummysub;
    dummysub[1] = (SubSeg)dummysub;
    // Four NULL vertices.
    dummysub[2] = NULL;
    dummysub[3] = NULL;
    dummysub[4] = NULL;
    dummysub[5] = NULL;
    // Initialize the two adjoining triangles to be "outer space."
    dummysub[6] = (SubSeg)dummytri;
    dummysub[7] = (SubSeg)dummytri;
    // Set the boundary marker to zero.
    * (int *) (dummysub + 8) = 0;

    // Initialize the three adjoining subsegments of `dummytri' to be
    // the omnipresent subsegment.
    dummytri[6] = (Triangle)dummysub;
    dummytri[7] = (Triangle)dummysub;
    dummytri[8] = (Triangle)dummysub;
  }
}

//  initializevertexpool()   Calculate the size of the Vertex data structure
//                           and initialize its memory pool.
//
//  This routine also computes the `vertexmarkindex' and `vertex2triindex'
//  indices used to find values within each Vertex.
void Mesh::initializevertexpool() {
  int vertexsize;

  // The index within each Vertex at which the boundary marker is found,
  // followed by the Vertex type. Ensure the Vertex marker is aligned to
  // a sizeof(int)-byte address.
  vertexmarkindex = ((mesh_dim + nextras) * sizeof(REAL) +
                        sizeof(int) - 1) /
                       sizeof(int);
  vertexsize = (vertexmarkindex + 2) * sizeof(int);
  if (m_b.poly) {
    // The index within each Vertex at which a Triangle pointer is found.
    // Ensure the pointer is aligned to a sizeof(Triangle)-byte address.
    vertex2triindex = (vertexsize + sizeof(Triangle) - 1) /
                         sizeof(Triangle);
    vertexsize = (vertex2triindex + 1) * sizeof(Triangle);
  }

  // Initialize the pool of vertices.
  vertices.poolinit(vertexsize, VERTEXPERBLOCK,
           invertices > VERTEXPERBLOCK ? invertices : VERTEXPERBLOCK,
           sizeof(REAL));
}

//  initializetrisubpools()   Calculate the sizes of the Triangle and
//                            subsegment data structures and initialize
//                            their memory pools.
//
//  This routine also computes the `highorderindex', `elemattribindex', and
//  `areaboundindex' indices used to find values within each Triangle.
void Mesh::initializetrisubpools() {
  int trisize;

  // The index within each Triangle at which the extra nodes (above three)
  // associated with high order elements are found. There are three
  // pointers to other triangles, three pointers to corners, and possibly
  // three pointers to subsegments before the extra nodes.
  highorderindex = 6 + (m_b.usesegments * 3);
  // The number of bytes occupied by a Triangle.
  trisize = ((m_b.order + 1) * (m_b.order + 2) / 2 + (highorderindex - 3)) * sizeof(Triangle);
  // The index within each Triangle at which its attributes are found,
  // where the index is measured in REALs.
  elemattribindex = (trisize + sizeof(REAL) - 1) / sizeof(REAL);
  // The index within each Triangle at which the maximum area constraint
  // is found, where the index is measured in REALs. Note that if the
  // `regionattrib' flag is set, an additional attribute will be added.
  areaboundindex = elemattribindex + eextras + m_b.regionattrib;
  // If Triangle attributes or an area bound are needed, increase the number
  // of bytes occupied by a Triangle.
  if (m_b.vararea) {
    trisize = (areaboundindex + 1) * sizeof(REAL);
  } else if (eextras + m_b.regionattrib > 0) {
    trisize = areaboundindex * sizeof(REAL);
  }
  // If a Voronoi diagram or Triangle neighbor graph is requested, make
  // sure there's room to store an integer index in each Triangle. This
  // integer index can occupy the same space as the subsegment pointers
  // or attributes or area constraint or extra nodes.
  if ((m_b.voronoi || m_b.neighbors) && (trisize < 6 * sizeof(Triangle) + sizeof(int))) {
    trisize = 6 * sizeof(Triangle) + sizeof(int);
  }

  // Having determined the memory size of a Triangle, initialize the pool.
  triangles.poolinit(trisize, TRIPERBLOCK,
           (2 * invertices - 2) > TRIPERBLOCK ? (2 * invertices - 2) :
           TRIPERBLOCK, 4);

  if (m_b.usesegments) {
    // Initialize the pool of subsegments. Take into account all eight
    // pointers and one boundary marker.
    subsegs.poolinit(8 * sizeof(Triangle) + sizeof(int), SUBSEGPERBLOCK, SUBSEGPERBLOCK, 4);

    // Initialize the "outer space" Triangle and omnipresent subsegment.
    dummyinit(triangles.getItemBytes(), subsegs.getItemBytes());
  } else {
    // Initialize the "outer space" Triangle.
    dummyinit(triangles.getItemBytes(), 0);
  }
}

//  Deallocate space for a Triangle, marking it dead.
void Mesh::triangledealloc(Triangle *dyingtriangle) {
  // Mark the Triangle as dead. This makes it possible to detect dead
  //   triangles when traversing the list of all triangles.
  killtri(dyingtriangle);
  triangles.dealloc(dyingtriangle);
}

//  Traverse the triangles, skipping dead ones.
Triangle *Mesh::triangletraverse() {
  Triangle *newtriangle;

  do {
    newtriangle = triangles.traverse();
    if (newtriangle == NULL) {
      return NULL;
    }
  } while (deadtri(newtriangle)); // Skip dead ones.
  return newtriangle;
}

//  Deallocate space for a subsegment, marking it dead.
void Mesh::subsegdealloc(SubSeg *dyingsubseg) {
  // Mark the subsegment as dead. This makes it possible to detect dead
  // subsegments when traversing the list of all subsegments.
  killsubseg(dyingsubseg);
  subsegs.dealloc(dyingsubseg);
}

//  Traverse the subsegments, skipping dead ones.
SubSeg *Mesh::subsegtraverse() {
  SubSeg *newsubseg;

  do {
    newsubseg = subsegs.traverse();
    if (newsubseg == NULL) {
      return NULL;
    }
  } while (deadsubseg(newsubseg));
  return newsubseg;
}

//  Deallocate space for a Vertex, marking it dead.
void Mesh::vertexdealloc(Vertex dyingvertex) {
  // Mark the Vertex as dead. This makes it possible to detect dead
  // vertices when traversing the list of all vertices.
  setvertextype(dyingvertex, DEADVERTEX);
  vertices.dealloc(dyingvertex);
}

//  Traverse the vertices, skipping dead ones.
Vertex Mesh::vertextraverse() {
  Vertex newvertex;
  do {
    newvertex = vertices.traverse();
    if (newvertex == NULL) {
      return NULL;
    }
  } while (vertextype(newvertex) == DEADVERTEX); // Skip dead ones.
  return newvertex;
}

#ifndef CDT_ONLY

//  Deallocate space for a bad subsegment, marking it dead.
void Mesh::badsubsegdealloc(BadSubSeg *dyingseg) {
  // Set subsegment's origin to NULL. This makes it possible to detect dead
  // badsubsegs when traversing the list of all badsubsegs             .
  dyingseg->subsegorg = NULL;
  badsubsegs.dealloc(dyingseg);
}

//  Traverse the bad subsegments, skipping dead ones.
BadSubSeg *Mesh::badsubsegtraverse() {
  BadSubSeg *newseg;

  do {
    newseg = badsubsegs.traverse();
    if (newseg == NULL) {
      return NULL;
    }
  } while (newseg->subsegorg == NULL); // Skip dead ones.
  return newseg;
}

#endif // not CDT_ONLY

//  getvertex()   Get a specific Vertex, by number, from the list.
//
//  The first Vertex is number 'firstnumber'.
//
//  Note that this takes O(n) time (with a small constant, if VERTEXPERBLOCK
//  is large). I don't care to take the trouble to make it work in constant
//  time.
Vertex Mesh::getvertex(int number) {
  VOID **getblock;
  char *foundvertex;
  size_t alignptr;
  int current;

  getblock = vertices.getFirstBlock();
  current = m_b.firstnumber;

  // Find the right block.
  if (current + vertices.getItemsFirstBlock() <= number) {
    getblock = (VOID **) *getblock;
    current += vertices.getItemsFirstBlock();
    while (current + vertices.getItemsPerBlock() <= number) {
      getblock = (VOID **) *getblock;
      current += vertices.getItemsPerBlock();
    }
  }

  // Now find the right Vertex.
  alignptr = (size_t)(getblock + 1);
  foundvertex = (char *) (alignptr + (size_t)vertices.getAlignBytes()
                       - (alignptr % (size_t)vertices.getAlignBytes()));
  return (Vertex)(foundvertex + vertices.getItemBytes() * (number - current));
}

//  Free all remaining allocated memory.
void Mesh::triangledeinit() {
  triangles.pooldeinit();
  trifree((VOID *) dummytribase);
  if (m_b.usesegments) {
    subsegs.pooldeinit();
    trifree((VOID *) dummysubbase);
  }
  vertices.pooldeinit();
#ifndef CDT_ONLY
  if (m_b.quality) {
    badsubsegs.pooldeinit();
    if ((m_b.minangle > 0.0) || m_b.vararea || m_b.fixedarea || m_b.usertest) {
      badtriangles.pooldeinit();
      flipstackers.pooldeinit();
    }
  }
#endif // not CDT_ONLY
}

//******** Memory management routines end here

//******** Constructors begin here

//  Create a new Triangle with orientation zero.
void Mesh::maketriangle(otri *newotri) {
  int i;

  newotri->tri = triangles.alloc();
  // Initialize the three adjoining triangles to be "outer space".
  newotri->tri[0] = (Triangle) dummytri;
  newotri->tri[1] = (Triangle) dummytri;
  newotri->tri[2] = (Triangle) dummytri;
  // Three NULL vertices.
  newotri->tri[3] = NULL;
  newotri->tri[4] = NULL;
  newotri->tri[5] = NULL;
  if (m_b.usesegments) {
    // Initialize the three adjoining subsegments to be the omnipresent subsegment.
    newotri->tri[6] = (Triangle) dummysub;
    newotri->tri[7] = (Triangle) dummysub;
    newotri->tri[8] = (Triangle) dummysub;
  }
  for (i = 0; i < eextras; i++) {
    setelemattribute(*newotri, i, 0.0);
  }
  if (m_b.vararea) {
    setareabound(*newotri, -1.0);
  }

  newotri->orient = 0;
}

// Create a new subsegment with orientation zero.
void Mesh::makesubseg(osub *newsubseg) {
  newsubseg->ss = subsegs.alloc();
  // Initialize the two adjoining subsegments to be the omnipresent subsegment.
  newsubseg->ss[0] = (SubSeg) dummysub;
  newsubseg->ss[1] = (SubSeg) dummysub;
  // Four NULL vertices.
  newsubseg->ss[2] = NULL;
  newsubseg->ss[3] = NULL;
  newsubseg->ss[4] = NULL;
  newsubseg->ss[5] = NULL;
  // Initialize the two adjoining triangles to be "outer space."
  newsubseg->ss[6] = (SubSeg) dummytri;
  newsubseg->ss[7] = (SubSeg) dummytri;
  // Set the boundary marker to zero.
  setmark(*newsubseg, 0);

  newsubseg->ssorient = 0;
}

//******** Constructors end here

//******** Geometric primitives begin here

// The adaptive exact arithmetic geometric predicates implemented herein are
// described in detail in my paper, "Adaptive Precision Floating-Point
// Arithmetic and Fast Robust Geometric Predicates."  See the header for a
// full citation.

// Which of the following two methods of finding the absolute values is
// fastest is compiler-dependent. A few compilers can inline and optimize
// the fabs() call; but most will incur the overhead of a function call,
// which is disastrously slow. A faster way on IEEE machines might be to
// mask the appropriate bit, but that's difficult to do in C without
// forcing the value to be stored to memory (rather than be kept in the
// register to which the optimizer assigned it).

#define Absolute(a)  ((a) >= 0.0 ? (a) : -(a))
// #define Absolute(a)  fabs(a)

// Many of the operations are broken up into two pieces, a main part that
// performs an approximate operation, and a "tail" that computes the
// roundoff error of that operation.
//
// The operations Fast_Two_Sum(), Fast_Two_Diff(), Two_Sum(), Two_Diff(),
// Split(), and Two_Product() are all implemented as described in the
// reference. Each of these macros requires certain variables to be
// defined in the calling routine. The variables `bvirt', `c', `abig',
// `_i', `_j', `_k', `_l', `_m', and `_n' are declared `INEXACT' because
// they store the result of an operation that may incur roundoff error.
// The input parameter `x' (or the highest numbered `x_' parameter) must
// also be declared `INEXACT'.

#define Fast_Two_Sum_Tail(a, b, x, y) \
  bvirt = x - a;                      \
  y = b - bvirt

#define Fast_Two_Sum(a, b, x, y)       \
  x = (REAL) (a + b);                  \
  Fast_Two_Sum_Tail(a, b, x, y)

#define Two_Sum_Tail(a, b, x, y)       \
  bvirt = (REAL) (x - a);              \
  avirt = x - bvirt;                   \
  bround = b - bvirt;                  \
  around = a - avirt;                  \
  y = around + bround

#define Two_Sum(a, b, x, y)            \
  x = (REAL) (a + b);                  \
  Two_Sum_Tail(a, b, x, y)

#define Two_Diff_Tail(a, b, x, y)      \
  bvirt = (REAL) (a - x);              \
  avirt = x + bvirt;                   \
  bround = bvirt - b;                  \
  around = a - avirt;                  \
  y = around + bround

#define Two_Diff(a, b, x, y)           \
  x = (REAL) (a - b);                  \
  Two_Diff_Tail(a, b, x, y)

#define Split(a, ahi, alo)             \
  c = (REAL) (splitter * a);           \
  abig = (REAL) (c - a);               \
  ahi = c - abig;                      \
  alo = a - ahi

#define Two_Product_Tail(a, b, x, y)   \
  Split(a, ahi, alo);                  \
  Split(b, bhi, blo);                  \
  err1 = x - (ahi * bhi);              \
  err2 = err1 - (alo * bhi);           \
  err3 = err2 - (ahi * blo);           \
  y = (alo * blo) - err3

#define Two_Product(a, b, x, y)        \
  x = (REAL) (a * b);                  \
  Two_Product_Tail(a, b, x, y)

// Two_Product_Presplit() is Two_Product() where one of the inputs has
// already been split. Avoids redundant splitting.

#define Two_Product_Presplit(a, b, bhi, blo, x, y)   \
  x = (REAL) (a * b);                                \
  Split(a, ahi, alo);                                \
  err1 = x - (ahi * bhi);                            \
  err2 = err1 - (alo * bhi);                         \
  err3 = err2 - (ahi * blo);                         \
  y = (alo * blo) - err3

// Square() can be done more quickly than Two_Product().

#define Square_Tail(a, x, y)                         \
  Split(a, ahi, alo);                                \
  err1 = x - (ahi * ahi);                            \
  err3 = err1 - ((ahi + ahi) * alo);                 \
  y = (alo * alo) - err3

#define Square(a, x, y)                              \
  x = (REAL) (a * a);                                \
  Square_Tail(a, x, y)

// Macros for summing expansions of various fixed lengths. These are all
// unrolled versions of Expansion_Sum().

#define Two_One_Sum(a1, a0, b, x2, x1, x0)           \
  Two_Sum(a0, b , _i, x0);                           \
  Two_Sum(a1, _i, x2, x1)

#define Two_One_Diff(a1, a0, b, x2, x1, x0)          \
  Two_Diff(a0, b , _i, x0);                          \
  Two_Sum( a1, _i, x2, x1)

#define Two_Two_Sum(a1, a0, b1, b0, x3, x2, x1, x0)  \
  Two_One_Sum(a1, a0, b0, _j, _0, x0);               \
  Two_One_Sum(_j, _0, b1, x3, x2, x1)

#define Two_Two_Diff(a1, a0, b1, b0, x3, x2, x1, x0) \
  Two_One_Diff(a1, a0, b0, _j, _0, x0);              \
  Two_One_Diff(_j, _0, b1, x3, x2, x1)

// Macro for multiplying a two-component expansion by a single component.

#define Two_One_Product(a1, a0, b, x3, x2, x1, x0)   \
  Split(b, bhi, blo);                                \
  Two_Product_Presplit(a0, b, bhi, blo, _i, x0);     \
  Two_Product_Presplit(a1, b, bhi, blo, _j, _0);     \
  Two_Sum(_i, _0, _k, x1);                           \
  Fast_Two_Sum(_j, _k, x3, x2)

//  exactinit()   Initialize the variables used for exact arithmetic.
//  `epsilon' is the largest power of two such that 1.0 + epsilon = 1.0 in
//  floating-point arithmetic. `epsilon' bounds the relative roundoff
//  error. It is used for floating-point error analysis.
//
//  `splitter' is used to split floating-point numbers into two half-
//  length significands for exact multiplication.
//
//  I imagine that a highly optimizing compiler might be too smart for its
//  own good, and somehow cause this routine to fail, if it pretends that
//  floating-point arithmetic is too much like real arithmetic.
//
//  Don't change this routine unless you fully understand it.
void exactinit() {
  REAL half;
  REAL check, lastcheck;
  int every_other;
#ifdef LINUX
  int cword;
#endif // LINUX

#ifdef CPU86
#ifdef SINGLE
  _control87(_PC_24, _MCW_PC); // Set FPU control word for single precision.
#else // not SINGLE
  _control87(_PC_53, _MCW_PC); // Set FPU control word for double precision.
#endif // not SINGLE
#endif // CPU86
#ifdef LINUX
#ifdef SINGLE
  //  cword = 4223;
  cword = 4210;                 // set FPU control word for single precision
#else // not SINGLE
  //  cword = 4735;
  cword = 4722;                 // set FPU control word for double precision
#endif // not SINGLE
  _FPU_SETCW(cword);
#endif // LINUX

  every_other = 1;
  half = 0.5;
  epsilon = 1.0;
  splitter = 1.0;
  check = 1.0;
  // Repeatedly divide `epsilon' by two until it is too small to add to
  // one without causing roundoff. (Also check if the sum is equal to
  // the previous sum, for machines that round up instead of using exact
  // rounding. Not that these routines will work on such machines.)
  do {
    lastcheck = check;
    epsilon *= half;
    if (every_other) {
      splitter *= 2.0;
    }
    every_other = !every_other;
    check = 1.0 + epsilon;
  } while ((check != 1.0) && (check != lastcheck));
  splitter += 1.0;
  // Error bounds for orientation and incircle tests.
  resulterrbound = (3.0 + 8.0 * epsilon) * epsilon;
  ccwerrboundA = (3.0 + 16.0 * epsilon) * epsilon;
  ccwerrboundB = (2.0 + 12.0 * epsilon) * epsilon;
  ccwerrboundC = (9.0 + 64.0 * epsilon) * epsilon * epsilon;
  iccerrboundA = (10.0 + 96.0 * epsilon) * epsilon;
  iccerrboundB = (4.0 + 48.0 * epsilon) * epsilon;
  iccerrboundC = (44.0 + 576.0 * epsilon) * epsilon * epsilon;
  o3derrboundA = (7.0 + 56.0 * epsilon) * epsilon;
  o3derrboundB = (3.0 + 28.0 * epsilon) * epsilon;
  o3derrboundC = (26.0 + 288.0 * epsilon) * epsilon * epsilon;
}

//***************************************************************************
//  fast_expansion_sum_zeroelim()   Sum two expansions, eliminating zero
//                                  components from the output expansion.
//
//  Sets h = e + f. See my Robust Predicates paper for details.
//
//  If round-to-even is used (as with IEEE 754), maintains the strongly
//  nonoverlapping property. (That is, if e is strongly nonoverlapping, h
//  will be also.)  Does NOT maintain the nonoverlapping or nonadjacent
//  properties.
int fast_expansion_sum_zeroelim(int elen, REAL *e, int flen, REAL *f, REAL *h) {
  REAL Q;
  INEXACT REAL Qnew;
  INEXACT REAL hh;
  INEXACT REAL bvirt;
  REAL avirt, bround, around;
  int eindex, findex, hindex;
  REAL enow, fnow;

  enow = e[0];
  fnow = f[0];
  eindex = findex = 0;
  if ((fnow > enow) == (fnow > -enow)) {
    Q = enow;
    enow = e[++eindex];
  } else {
    Q = fnow;
    fnow = f[++findex];
  }
  hindex = 0;
  if ((eindex < elen) && (findex < flen)) {
    if ((fnow > enow) == (fnow > -enow)) {
      Fast_Two_Sum(enow, Q, Qnew, hh);
      enow = e[++eindex];
    } else {
      Fast_Two_Sum(fnow, Q, Qnew, hh);
      fnow = f[++findex];
    }
    Q = Qnew;
    if (hh != 0.0) {
      h[hindex++] = hh;
    }
    while ((eindex < elen) && (findex < flen)) {
      if ((fnow > enow) == (fnow > -enow)) {
        Two_Sum(Q, enow, Qnew, hh);
        enow = e[++eindex];
      } else {
        Two_Sum(Q, fnow, Qnew, hh);
        fnow = f[++findex];
      }
      Q = Qnew;
      if (hh != 0.0) {
        h[hindex++] = hh;
      }
    }
  }
  while (eindex < elen) {
    Two_Sum(Q, enow, Qnew, hh);
    enow = e[++eindex];
    Q = Qnew;
    if (hh != 0.0) {
      h[hindex++] = hh;
    }
  }
  while (findex < flen) {
    Two_Sum(Q, fnow, Qnew, hh);
    fnow = f[++findex];
    Q = Qnew;
    if (hh != 0.0) {
      h[hindex++] = hh;
    }
  }
  if ((Q != 0.0) || (hindex == 0)) {
    h[hindex++] = Q;
  }
  return hindex;
}

//***************************************************************************
//  scale_expansion_zeroelim()   Multiply an expansion by a scalar,
//                               eliminating zero components from the
//                               output expansion.
//
//  Sets h = be. See my Robust Predicates paper for details.
//
//  Maintains the nonoverlapping property. If round-to-even is used (as
//  with IEEE 754), maintains the strongly nonoverlapping and nonadjacent
//  properties as well. (That is, if e has one of these properties, so
//  will h.)
int scale_expansion_zeroelim(int elen, REAL *e, REAL b, REAL *h) {
  INEXACT REAL Q, sum;
  REAL hh;
  INEXACT REAL product1;
  REAL product0;
  int eindex, hindex;
  REAL enow;
  INEXACT REAL bvirt;
  REAL avirt, bround, around;
  INEXACT REAL c;
  INEXACT REAL abig;
  REAL ahi, alo, bhi, blo;
  REAL err1, err2, err3;

  Split(b, bhi, blo);
  Two_Product_Presplit(e[0], b, bhi, blo, Q, hh);
  hindex = 0;
  if (hh != 0) {
    h[hindex++] = hh;
  }
  for (eindex = 1; eindex < elen; eindex++) {
    enow = e[eindex];
    Two_Product_Presplit(enow, b, bhi, blo, product1, product0);
    Two_Sum(Q, product0, sum, hh);
    if (hh != 0) {
      h[hindex++] = hh;
    }
    Fast_Two_Sum(product1, sum, Q, hh);
    if (hh != 0) {
      h[hindex++] = hh;
    }
  }
  if ((Q != 0.0) || (hindex == 0)) {
    h[hindex++] = Q;
  }
  return hindex;
}

//  estimate()   Produce a one-word estimate of an expansion's value.
//  See my Robust Predicates paper for details.
REAL estimate(int elen, REAL *e) {
  REAL Q;
  int eindex;

  Q = e[0];
  for (eindex = 1; eindex < elen; eindex++) {
    Q += e[eindex];
  }
  return Q;
}

//***************************************************************************
//
//  counterclockwise()   Return a positive value if the points pa, pb, and
//                       pc occur in counterclockwise order; a negative
//                       value if they occur in clockwise order; and zero
//                       if they are collinear. The result is also a rough
//                       approximation of twice the signed area of the
//                       Triangle defined by the three points.
//
//  Uses exact arithmetic if necessary to ensure a correct answer. The
//  result returned is the determinant of a matrix. This determinant is
//  computed adaptively, in the sense that exact arithmetic is used only to
//  the degree it is needed to ensure that the returned value has the
//  correct sign. Hence, this function is usually quite fast, but will run
//  more slowly when the input points are collinear or nearly so.
//
//  See my Robust Predicates paper for details.
//
REAL counterclockwiseadapt(Vertex pa, Vertex pb, Vertex pc, REAL detsum) {
  INEXACT REAL acx, acy, bcx, bcy;
  REAL acxtail, acytail, bcxtail, bcytail;
  INEXACT REAL detleft, detright;
  REAL detlefttail, detrighttail;
  REAL det, errbound;
  REAL B[4], C1[8], C2[12], D[16];
  INEXACT REAL B3;
  int C1length, C2length, Dlength;
  REAL u[4];
  INEXACT REAL u3;
  INEXACT REAL s1, t1;
  REAL s0, t0;

  INEXACT REAL bvirt;
  REAL avirt, bround, around;
  INEXACT REAL c;
  INEXACT REAL abig;
  REAL ahi, alo, bhi, blo;
  REAL err1, err2, err3;
  INEXACT REAL _i, _j;
  REAL _0;

  acx = (REAL) (pa[0] - pc[0]);
  bcx = (REAL) (pb[0] - pc[0]);
  acy = (REAL) (pa[1] - pc[1]);
  bcy = (REAL) (pb[1] - pc[1]);

  Two_Product(acx, bcy, detleft, detlefttail);
  Two_Product(acy, bcx, detright, detrighttail);

  Two_Two_Diff(detleft, detlefttail, detright, detrighttail,
               B3, B[2], B[1], B[0]);
  B[3] = B3;

  det = estimate(4, B);
  errbound = ccwerrboundB * detsum;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Diff_Tail(pa[0], pc[0], acx, acxtail);
  Two_Diff_Tail(pb[0], pc[0], bcx, bcxtail);
  Two_Diff_Tail(pa[1], pc[1], acy, acytail);
  Two_Diff_Tail(pb[1], pc[1], bcy, bcytail);

  if ((acxtail == 0.0) && (acytail == 0.0)
      && (bcxtail == 0.0) && (bcytail == 0.0)) {
    return det;
  }

  errbound = ccwerrboundC * detsum + resulterrbound * Absolute(det);
  det += (acx * bcytail + bcy * acxtail)
       - (acy * bcxtail + bcx * acytail);
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Product(acxtail, bcy, s1, s0);
  Two_Product(acytail, bcx, t1, t0);
  Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
  u[3] = u3;
  C1length = fast_expansion_sum_zeroelim(4, B, 4, u, C1);

  Two_Product(acx, bcytail, s1, s0);
  Two_Product(acy, bcxtail, t1, t0);
  Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
  u[3] = u3;
  C2length = fast_expansion_sum_zeroelim(C1length, C1, 4, u, C2);

  Two_Product(acxtail, bcytail, s1, s0);
  Two_Product(acytail, bcxtail, t1, t0);
  Two_Two_Diff(s1, s0, t1, t0, u3, u[2], u[1], u[0]);
  u[3] = u3;
  Dlength = fast_expansion_sum_zeroelim(C2length, C2, 4, u, D);

  return(D[Dlength - 1]);
}

REAL Mesh::counterclockwise(Vertex pa, Vertex pb, Vertex pc) {
  REAL detleft, detright, det;
  REAL detsum, errbound;

  counterclockcount++;

  detleft = (pa[0] - pc[0]) * (pb[1] - pc[1]);
  detright = (pa[1] - pc[1]) * (pb[0] - pc[0]);
  det = detleft - detright;

  if (m_b.noexact) {
    return det;
  }

  if (detleft > 0.0) {
    if (detright <= 0.0) {
      return det;
    } else {
      detsum = detleft + detright;
    }
  } else if (detleft < 0.0) {
    if (detright >= 0.0) {
      return det;
    } else {
      detsum = -detleft - detright;
    }
  } else {
    return det;
  }

  errbound = ccwerrboundA * detsum;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  return counterclockwiseadapt(pa, pb, pc, detsum);
}

//***************************************************************************
//
//  incircle()   Return a positive value if the point pd lies inside the
//               circle passing through pa, pb, and pc; a negative value if
//               it lies outside; and zero if the four points are cocircular.
//               The points pa, pb, and pc must be in counterclockwise
//               order, or the sign of the result will be reversed.
//
//  Uses exact arithmetic if necessary to ensure a correct answer. The
//  result returned is the determinant of a matrix. This determinant is
//  computed adaptively, in the sense that exact arithmetic is used only to
//  the degree it is needed to ensure that the returned value has the
//  correct sign. Hence, this function is usually quite fast, but will run
//  more slowly when the input points are cocircular or nearly so.
//
//  See my Robust Predicates paper for details.
//
REAL incircleadapt(Vertex pa, Vertex pb, Vertex pc, Vertex pd, REAL permanent) {
  INEXACT REAL adx, bdx, cdx, ady, bdy, cdy;
  REAL det, errbound;

  INEXACT REAL bdxcdy1, cdxbdy1, cdxady1, adxcdy1, adxbdy1, bdxady1;
  REAL bdxcdy0, cdxbdy0, cdxady0, adxcdy0, adxbdy0, bdxady0;
  REAL bc[4], ca[4], ab[4];
  INEXACT REAL bc3, ca3, ab3;
  REAL axbc[8], axxbc[16], aybc[8], ayybc[16], adet[32];
  int axbclen, axxbclen, aybclen, ayybclen, alen;
  REAL bxca[8], bxxca[16], byca[8], byyca[16], bdet[32];
  int bxcalen, bxxcalen, bycalen, byycalen, blen;
  REAL cxab[8], cxxab[16], cyab[8], cyyab[16], cdet[32];
  int cxablen, cxxablen, cyablen, cyyablen, clen;
  REAL abdet[64];
  int ablen;
  REAL fin1[1152], fin2[1152];
  REAL *finnow, *finother, *finswap;
  int finlength;

  REAL adxtail, bdxtail, cdxtail, adytail, bdytail, cdytail;
  INEXACT REAL adxadx1, adyady1, bdxbdx1, bdybdy1, cdxcdx1, cdycdy1;
  REAL adxadx0, adyady0, bdxbdx0, bdybdy0, cdxcdx0, cdycdy0;
  REAL aa[4], bb[4], cc[4];
  INEXACT REAL aa3, bb3, cc3;
  INEXACT REAL ti1, tj1;
  REAL ti0, tj0;
  REAL u[4], v[4];
  INEXACT REAL u3, v3;
  REAL temp8[8], temp16a[16], temp16b[16], temp16c[16];
  REAL temp32a[32], temp32b[32], temp48[48], temp64[64];
  int temp8len, temp16alen, temp16blen, temp16clen;
  int temp32alen, temp32blen, temp48len, temp64len;
  REAL axtbb[8], axtcc[8], aytbb[8], aytcc[8];
  int axtbblen, axtcclen, aytbblen, aytcclen;
  REAL bxtaa[8], bxtcc[8], bytaa[8], bytcc[8];
  int bxtaalen, bxtcclen, bytaalen, bytcclen;
  REAL cxtaa[8], cxtbb[8], cytaa[8], cytbb[8];
  int cxtaalen, cxtbblen, cytaalen, cytbblen;
  REAL axtbc[8], aytbc[8], bxtca[8], bytca[8], cxtab[8], cytab[8];
  int axtbclen, aytbclen, bxtcalen, bytcalen, cxtablen, cytablen;
  REAL axtbct[16], aytbct[16], bxtcat[16], bytcat[16], cxtabt[16], cytabt[16];
  int axtbctlen, aytbctlen, bxtcatlen, bytcatlen, cxtabtlen, cytabtlen;
  REAL axtbctt[8], aytbctt[8], bxtcatt[8];
  REAL bytcatt[8], cxtabtt[8], cytabtt[8];
  int axtbcttlen, aytbcttlen, bxtcattlen, bytcattlen, cxtabttlen, cytabttlen;
  REAL abt[8], bct[8], cat[8];
  int abtlen, bctlen, catlen;
  REAL abtt[4], bctt[4], catt[4];
  int abttlen, bcttlen, cattlen;
  INEXACT REAL abtt3, bctt3, catt3;
  REAL negate;

  INEXACT REAL bvirt;
  REAL avirt, bround, around;
  INEXACT REAL c;
  INEXACT REAL abig;
  REAL ahi, alo, bhi, blo;
  REAL err1, err2, err3;
  INEXACT REAL _i, _j;
  REAL _0;

  adx = (REAL) (pa[0] - pd[0]);
  bdx = (REAL) (pb[0] - pd[0]);
  cdx = (REAL) (pc[0] - pd[0]);
  ady = (REAL) (pa[1] - pd[1]);
  bdy = (REAL) (pb[1] - pd[1]);
  cdy = (REAL) (pc[1] - pd[1]);

  Two_Product(bdx, cdy, bdxcdy1, bdxcdy0);
  Two_Product(cdx, bdy, cdxbdy1, cdxbdy0);
  Two_Two_Diff(bdxcdy1, bdxcdy0, cdxbdy1, cdxbdy0, bc3, bc[2], bc[1], bc[0]);
  bc[3] = bc3;
  axbclen = scale_expansion_zeroelim(4, bc, adx, axbc);
  axxbclen = scale_expansion_zeroelim(axbclen, axbc, adx, axxbc);
  aybclen = scale_expansion_zeroelim(4, bc, ady, aybc);
  ayybclen = scale_expansion_zeroelim(aybclen, aybc, ady, ayybc);
  alen = fast_expansion_sum_zeroelim(axxbclen, axxbc, ayybclen, ayybc, adet);

  Two_Product(cdx, ady, cdxady1, cdxady0);
  Two_Product(adx, cdy, adxcdy1, adxcdy0);
  Two_Two_Diff(cdxady1, cdxady0, adxcdy1, adxcdy0, ca3, ca[2], ca[1], ca[0]);
  ca[3] = ca3;
  bxcalen = scale_expansion_zeroelim(4, ca, bdx, bxca);
  bxxcalen = scale_expansion_zeroelim(bxcalen, bxca, bdx, bxxca);
  bycalen = scale_expansion_zeroelim(4, ca, bdy, byca);
  byycalen = scale_expansion_zeroelim(bycalen, byca, bdy, byyca);
  blen = fast_expansion_sum_zeroelim(bxxcalen, bxxca, byycalen, byyca, bdet);

  Two_Product(adx, bdy, adxbdy1, adxbdy0);
  Two_Product(bdx, ady, bdxady1, bdxady0);
  Two_Two_Diff(adxbdy1, adxbdy0, bdxady1, bdxady0, ab3, ab[2], ab[1], ab[0]);
  ab[3] = ab3;
  cxablen = scale_expansion_zeroelim(4, ab, cdx, cxab);
  cxxablen = scale_expansion_zeroelim(cxablen, cxab, cdx, cxxab);
  cyablen = scale_expansion_zeroelim(4, ab, cdy, cyab);
  cyyablen = scale_expansion_zeroelim(cyablen, cyab, cdy, cyyab);
  clen = fast_expansion_sum_zeroelim(cxxablen, cxxab, cyyablen, cyyab, cdet);

  ablen = fast_expansion_sum_zeroelim(alen, adet, blen, bdet, abdet);
  finlength = fast_expansion_sum_zeroelim(ablen, abdet, clen, cdet, fin1);

  det = estimate(finlength, fin1);
  errbound = iccerrboundB * permanent;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Diff_Tail(pa[0], pd[0], adx, adxtail);
  Two_Diff_Tail(pa[1], pd[1], ady, adytail);
  Two_Diff_Tail(pb[0], pd[0], bdx, bdxtail);
  Two_Diff_Tail(pb[1], pd[1], bdy, bdytail);
  Two_Diff_Tail(pc[0], pd[0], cdx, cdxtail);
  Two_Diff_Tail(pc[1], pd[1], cdy, cdytail);
  if ((adxtail == 0.0) && (bdxtail == 0.0) && (cdxtail == 0.0)
      && (adytail == 0.0) && (bdytail == 0.0) && (cdytail == 0.0)) {
    return det;
  }

  errbound = iccerrboundC * permanent + resulterrbound * Absolute(det);
  det += ((adx * adx + ady * ady) * ((bdx * cdytail + cdy * bdxtail)
                                     - (bdy * cdxtail + cdx * bdytail))
          + 2.0 * (adx * adxtail + ady * adytail) * (bdx * cdy - bdy * cdx))
       + ((bdx * bdx + bdy * bdy) * ((cdx * adytail + ady * cdxtail)
                                     - (cdy * adxtail + adx * cdytail))
          + 2.0 * (bdx * bdxtail + bdy * bdytail) * (cdx * ady - cdy * adx))
       + ((cdx * cdx + cdy * cdy) * ((adx * bdytail + bdy * adxtail)
                                     - (ady * bdxtail + bdx * adytail))
          + 2.0 * (cdx * cdxtail + cdy * cdytail) * (adx * bdy - ady * bdx));
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  finnow = fin1;
  finother = fin2;

  if ((bdxtail != 0.0) || (bdytail != 0.0)
      || (cdxtail != 0.0) || (cdytail != 0.0)) {
    Square(adx, adxadx1, adxadx0);
    Square(ady, adyady1, adyady0);
    Two_Two_Sum(adxadx1, adxadx0, adyady1, adyady0, aa3, aa[2], aa[1], aa[0]);
    aa[3] = aa3;
  }
  if ((cdxtail != 0.0) || (cdytail != 0.0)
      || (adxtail != 0.0) || (adytail != 0.0)) {
    Square(bdx, bdxbdx1, bdxbdx0);
    Square(bdy, bdybdy1, bdybdy0);
    Two_Two_Sum(bdxbdx1, bdxbdx0, bdybdy1, bdybdy0, bb3, bb[2], bb[1], bb[0]);
    bb[3] = bb3;
  }
  if ((adxtail != 0.0) || (adytail != 0.0)
      || (bdxtail != 0.0) || (bdytail != 0.0)) {
    Square(cdx, cdxcdx1, cdxcdx0);
    Square(cdy, cdycdy1, cdycdy0);
    Two_Two_Sum(cdxcdx1, cdxcdx0, cdycdy1, cdycdy0, cc3, cc[2], cc[1], cc[0]);
    cc[3] = cc3;
  }

  if (adxtail != 0.0) {
    axtbclen = scale_expansion_zeroelim(4, bc, adxtail, axtbc);
    temp16alen = scale_expansion_zeroelim(axtbclen, axtbc, 2.0 * adx,
                                          temp16a);

    axtcclen = scale_expansion_zeroelim(4, cc, adxtail, axtcc);
    temp16blen = scale_expansion_zeroelim(axtcclen, axtcc, bdy, temp16b);

    axtbblen = scale_expansion_zeroelim(4, bb, adxtail, axtbb);
    temp16clen = scale_expansion_zeroelim(axtbblen, axtbb, -cdy, temp16c);

    temp32alen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                            temp16blen, temp16b, temp32a);
    temp48len = fast_expansion_sum_zeroelim(temp16clen, temp16c,
                                            temp32alen, temp32a, temp48);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                            temp48, finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (adytail != 0.0) {
    aytbclen = scale_expansion_zeroelim(4, bc, adytail, aytbc);
    temp16alen = scale_expansion_zeroelim(aytbclen, aytbc, 2.0 * ady,
                                          temp16a);

    aytbblen = scale_expansion_zeroelim(4, bb, adytail, aytbb);
    temp16blen = scale_expansion_zeroelim(aytbblen, aytbb, cdx, temp16b);

    aytcclen = scale_expansion_zeroelim(4, cc, adytail, aytcc);
    temp16clen = scale_expansion_zeroelim(aytcclen, aytcc, -bdx, temp16c);

    temp32alen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                            temp16blen, temp16b, temp32a);
    temp48len = fast_expansion_sum_zeroelim(temp16clen, temp16c,
                                            temp32alen, temp32a, temp48);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                            temp48, finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (bdxtail != 0.0) {
    bxtcalen = scale_expansion_zeroelim(4, ca, bdxtail, bxtca);
    temp16alen = scale_expansion_zeroelim(bxtcalen, bxtca, 2.0 * bdx,
                                          temp16a);

    bxtaalen = scale_expansion_zeroelim(4, aa, bdxtail, bxtaa);
    temp16blen = scale_expansion_zeroelim(bxtaalen, bxtaa, cdy, temp16b);

    bxtcclen = scale_expansion_zeroelim(4, cc, bdxtail, bxtcc);
    temp16clen = scale_expansion_zeroelim(bxtcclen, bxtcc, -ady, temp16c);

    temp32alen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                            temp16blen, temp16b, temp32a);
    temp48len = fast_expansion_sum_zeroelim(temp16clen, temp16c,
                                            temp32alen, temp32a, temp48);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                            temp48, finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (bdytail != 0.0) {
    bytcalen = scale_expansion_zeroelim(4, ca, bdytail, bytca);
    temp16alen = scale_expansion_zeroelim(bytcalen, bytca, 2.0 * bdy,
                                          temp16a);

    bytcclen = scale_expansion_zeroelim(4, cc, bdytail, bytcc);
    temp16blen = scale_expansion_zeroelim(bytcclen, bytcc, adx, temp16b);

    bytaalen = scale_expansion_zeroelim(4, aa, bdytail, bytaa);
    temp16clen = scale_expansion_zeroelim(bytaalen, bytaa, -cdx, temp16c);

    temp32alen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                            temp16blen, temp16b, temp32a);
    temp48len = fast_expansion_sum_zeroelim(temp16clen, temp16c,
                                            temp32alen, temp32a, temp48);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                            temp48, finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (cdxtail != 0.0) {
    cxtablen = scale_expansion_zeroelim(4, ab, cdxtail, cxtab);
    temp16alen = scale_expansion_zeroelim(cxtablen, cxtab, 2.0 * cdx,
                                          temp16a);

    cxtbblen = scale_expansion_zeroelim(4, bb, cdxtail, cxtbb);
    temp16blen = scale_expansion_zeroelim(cxtbblen, cxtbb, ady, temp16b);

    cxtaalen = scale_expansion_zeroelim(4, aa, cdxtail, cxtaa);
    temp16clen = scale_expansion_zeroelim(cxtaalen, cxtaa, -bdy, temp16c);

    temp32alen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                            temp16blen, temp16b, temp32a);
    temp48len = fast_expansion_sum_zeroelim(temp16clen, temp16c,
                                            temp32alen, temp32a, temp48);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                            temp48, finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (cdytail != 0.0) {
    cytablen = scale_expansion_zeroelim(4, ab, cdytail, cytab);
    temp16alen = scale_expansion_zeroelim(cytablen, cytab, 2.0 * cdy,
                                          temp16a);

    cytaalen = scale_expansion_zeroelim(4, aa, cdytail, cytaa);
    temp16blen = scale_expansion_zeroelim(cytaalen, cytaa, bdx, temp16b);

    cytbblen = scale_expansion_zeroelim(4, bb, cdytail, cytbb);
    temp16clen = scale_expansion_zeroelim(cytbblen, cytbb, -adx, temp16c);

    temp32alen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                            temp16blen, temp16b, temp32a);
    temp48len = fast_expansion_sum_zeroelim(temp16clen, temp16c,
                                            temp32alen, temp32a, temp48);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                            temp48, finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }

  if ((adxtail != 0.0) || (adytail != 0.0)) {
    if ((bdxtail != 0.0) || (bdytail != 0.0)
        || (cdxtail != 0.0) || (cdytail != 0.0)) {
      Two_Product(bdxtail, cdy, ti1, ti0);
      Two_Product(bdx, cdytail, tj1, tj0);
      Two_Two_Sum(ti1, ti0, tj1, tj0, u3, u[2], u[1], u[0]);
      u[3] = u3;
      negate = -bdy;
      Two_Product(cdxtail, negate, ti1, ti0);
      negate = -bdytail;
      Two_Product(cdx, negate, tj1, tj0);
      Two_Two_Sum(ti1, ti0, tj1, tj0, v3, v[2], v[1], v[0]);
      v[3] = v3;
      bctlen = fast_expansion_sum_zeroelim(4, u, 4, v, bct);

      Two_Product(bdxtail, cdytail, ti1, ti0);
      Two_Product(cdxtail, bdytail, tj1, tj0);
      Two_Two_Diff(ti1, ti0, tj1, tj0, bctt3, bctt[2], bctt[1], bctt[0]);
      bctt[3] = bctt3;
      bcttlen = 4;
    } else {
      bct[0] = 0.0;
      bctlen = 1;
      bctt[0] = 0.0;
      bcttlen = 1;
    }

    if (adxtail != 0.0) {
      temp16alen = scale_expansion_zeroelim(axtbclen, axtbc, adxtail, temp16a);
      axtbctlen = scale_expansion_zeroelim(bctlen, bct, adxtail, axtbct);
      temp32alen = scale_expansion_zeroelim(axtbctlen, axtbct, 2.0 * adx,
                                            temp32a);
      temp48len = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp32alen, temp32a, temp48);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                              temp48, finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (bdytail != 0.0) {
        temp8len = scale_expansion_zeroelim(4, cc, adxtail, temp8);
        temp16alen = scale_expansion_zeroelim(temp8len, temp8, bdytail,
                                              temp16a);
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp16alen,
                                                temp16a, finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
      if (cdytail != 0.0) {
        temp8len = scale_expansion_zeroelim(4, bb, -adxtail, temp8);
        temp16alen = scale_expansion_zeroelim(temp8len, temp8, cdytail,
                                              temp16a);
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp16alen,
                                                temp16a, finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }

      temp32alen = scale_expansion_zeroelim(axtbctlen, axtbct, adxtail,
                                            temp32a);
      axtbcttlen = scale_expansion_zeroelim(bcttlen, bctt, adxtail, axtbctt);
      temp16alen = scale_expansion_zeroelim(axtbcttlen, axtbctt, 2.0 * adx,
                                            temp16a);
      temp16blen = scale_expansion_zeroelim(axtbcttlen, axtbctt, adxtail,
                                            temp16b);
      temp32blen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp16blen, temp16b, temp32b);
      temp64len = fast_expansion_sum_zeroelim(temp32alen, temp32a,
                                              temp32blen, temp32b, temp64);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp64len,
                                              temp64, finother);
      finswap = finnow; finnow = finother; finother = finswap;
    }
    if (adytail != 0.0) {
      temp16alen = scale_expansion_zeroelim(aytbclen, aytbc, adytail, temp16a);
      aytbctlen = scale_expansion_zeroelim(bctlen, bct, adytail, aytbct);
      temp32alen = scale_expansion_zeroelim(aytbctlen, aytbct, 2.0 * ady,
                                            temp32a);
      temp48len = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp32alen, temp32a, temp48);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                              temp48, finother);
      finswap = finnow; finnow = finother; finother = finswap;


      temp32alen = scale_expansion_zeroelim(aytbctlen, aytbct, adytail,
                                            temp32a);
      aytbcttlen = scale_expansion_zeroelim(bcttlen, bctt, adytail, aytbctt);
      temp16alen = scale_expansion_zeroelim(aytbcttlen, aytbctt, 2.0 * ady,
                                            temp16a);
      temp16blen = scale_expansion_zeroelim(aytbcttlen, aytbctt, adytail,
                                            temp16b);
      temp32blen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp16blen, temp16b, temp32b);
      temp64len = fast_expansion_sum_zeroelim(temp32alen, temp32a,
                                              temp32blen, temp32b, temp64);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp64len,
                                              temp64, finother);
      finswap = finnow; finnow = finother; finother = finswap;
    }
  }
  if ((bdxtail != 0.0) || (bdytail != 0.0)) {
    if ((cdxtail != 0.0) || (cdytail != 0.0)
        || (adxtail != 0.0) || (adytail != 0.0)) {
      Two_Product(cdxtail, ady, ti1, ti0);
      Two_Product(cdx, adytail, tj1, tj0);
      Two_Two_Sum(ti1, ti0, tj1, tj0, u3, u[2], u[1], u[0]);
      u[3] = u3;
      negate = -cdy;
      Two_Product(adxtail, negate, ti1, ti0);
      negate = -cdytail;
      Two_Product(adx, negate, tj1, tj0);
      Two_Two_Sum(ti1, ti0, tj1, tj0, v3, v[2], v[1], v[0]);
      v[3] = v3;
      catlen = fast_expansion_sum_zeroelim(4, u, 4, v, cat);

      Two_Product(cdxtail, adytail, ti1, ti0);
      Two_Product(adxtail, cdytail, tj1, tj0);
      Two_Two_Diff(ti1, ti0, tj1, tj0, catt3, catt[2], catt[1], catt[0]);
      catt[3] = catt3;
      cattlen = 4;
    } else {
      cat[0] = 0.0;
      catlen = 1;
      catt[0] = 0.0;
      cattlen = 1;
    }

    if (bdxtail != 0.0) {
      temp16alen = scale_expansion_zeroelim(bxtcalen, bxtca, bdxtail, temp16a);
      bxtcatlen = scale_expansion_zeroelim(catlen, cat, bdxtail, bxtcat);
      temp32alen = scale_expansion_zeroelim(bxtcatlen, bxtcat, 2.0 * bdx,
                                            temp32a);
      temp48len = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp32alen, temp32a, temp48);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                              temp48, finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (cdytail != 0.0) {
        temp8len = scale_expansion_zeroelim(4, aa, bdxtail, temp8);
        temp16alen = scale_expansion_zeroelim(temp8len, temp8, cdytail,
                                              temp16a);
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp16alen,
                                                temp16a, finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
      if (adytail != 0.0) {
        temp8len = scale_expansion_zeroelim(4, cc, -bdxtail, temp8);
        temp16alen = scale_expansion_zeroelim(temp8len, temp8, adytail,
                                              temp16a);
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp16alen,
                                                temp16a, finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }

      temp32alen = scale_expansion_zeroelim(bxtcatlen, bxtcat, bdxtail,
                                            temp32a);
      bxtcattlen = scale_expansion_zeroelim(cattlen, catt, bdxtail, bxtcatt);
      temp16alen = scale_expansion_zeroelim(bxtcattlen, bxtcatt, 2.0 * bdx,
                                            temp16a);
      temp16blen = scale_expansion_zeroelim(bxtcattlen, bxtcatt, bdxtail,
                                            temp16b);
      temp32blen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp16blen, temp16b, temp32b);
      temp64len = fast_expansion_sum_zeroelim(temp32alen, temp32a,
                                              temp32blen, temp32b, temp64);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp64len,
                                              temp64, finother);
      finswap = finnow; finnow = finother; finother = finswap;
    }
    if (bdytail != 0.0) {
      temp16alen = scale_expansion_zeroelim(bytcalen, bytca, bdytail, temp16a);
      bytcatlen = scale_expansion_zeroelim(catlen, cat, bdytail, bytcat);
      temp32alen = scale_expansion_zeroelim(bytcatlen, bytcat, 2.0 * bdy,
                                            temp32a);
      temp48len = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp32alen, temp32a, temp48);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                              temp48, finother);
      finswap = finnow; finnow = finother; finother = finswap;


      temp32alen = scale_expansion_zeroelim(bytcatlen, bytcat, bdytail,
                                            temp32a);
      bytcattlen = scale_expansion_zeroelim(cattlen, catt, bdytail, bytcatt);
      temp16alen = scale_expansion_zeroelim(bytcattlen, bytcatt, 2.0 * bdy,
                                            temp16a);
      temp16blen = scale_expansion_zeroelim(bytcattlen, bytcatt, bdytail,
                                            temp16b);
      temp32blen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp16blen, temp16b, temp32b);
      temp64len = fast_expansion_sum_zeroelim(temp32alen, temp32a,
                                              temp32blen, temp32b, temp64);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp64len,
                                              temp64, finother);
      finswap = finnow; finnow = finother; finother = finswap;
    }
  }
  if ((cdxtail != 0.0) || (cdytail != 0.0)) {
    if ((adxtail != 0.0) || (adytail != 0.0)
        || (bdxtail != 0.0) || (bdytail != 0.0)) {
      Two_Product(adxtail, bdy, ti1, ti0);
      Two_Product(adx, bdytail, tj1, tj0);
      Two_Two_Sum(ti1, ti0, tj1, tj0, u3, u[2], u[1], u[0]);
      u[3] = u3;
      negate = -ady;
      Two_Product(bdxtail, negate, ti1, ti0);
      negate = -adytail;
      Two_Product(bdx, negate, tj1, tj0);
      Two_Two_Sum(ti1, ti0, tj1, tj0, v3, v[2], v[1], v[0]);
      v[3] = v3;
      abtlen = fast_expansion_sum_zeroelim(4, u, 4, v, abt);

      Two_Product(adxtail, bdytail, ti1, ti0);
      Two_Product(bdxtail, adytail, tj1, tj0);
      Two_Two_Diff(ti1, ti0, tj1, tj0, abtt3, abtt[2], abtt[1], abtt[0]);
      abtt[3] = abtt3;
      abttlen = 4;
    } else {
      abt[0] = 0.0;
      abtlen = 1;
      abtt[0] = 0.0;
      abttlen = 1;
    }

    if (cdxtail != 0.0) {
      temp16alen = scale_expansion_zeroelim(cxtablen, cxtab, cdxtail, temp16a);
      cxtabtlen = scale_expansion_zeroelim(abtlen, abt, cdxtail, cxtabt);
      temp32alen = scale_expansion_zeroelim(cxtabtlen, cxtabt, 2.0 * cdx,
                                            temp32a);
      temp48len = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp32alen, temp32a, temp48);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                              temp48, finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (adytail != 0.0) {
        temp8len = scale_expansion_zeroelim(4, bb, cdxtail, temp8);
        temp16alen = scale_expansion_zeroelim(temp8len, temp8, adytail,
                                              temp16a);
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp16alen,
                                                temp16a, finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
      if (bdytail != 0.0) {
        temp8len = scale_expansion_zeroelim(4, aa, -cdxtail, temp8);
        temp16alen = scale_expansion_zeroelim(temp8len, temp8, bdytail,
                                              temp16a);
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp16alen,
                                                temp16a, finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }

      temp32alen = scale_expansion_zeroelim(cxtabtlen, cxtabt, cdxtail,
                                            temp32a);
      cxtabttlen = scale_expansion_zeroelim(abttlen, abtt, cdxtail, cxtabtt);
      temp16alen = scale_expansion_zeroelim(cxtabttlen, cxtabtt, 2.0 * cdx,
                                            temp16a);
      temp16blen = scale_expansion_zeroelim(cxtabttlen, cxtabtt, cdxtail,
                                            temp16b);
      temp32blen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp16blen, temp16b, temp32b);
      temp64len = fast_expansion_sum_zeroelim(temp32alen, temp32a,
                                              temp32blen, temp32b, temp64);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp64len,
                                              temp64, finother);
      finswap = finnow; finnow = finother; finother = finswap;
    }
    if (cdytail != 0.0) {
      temp16alen = scale_expansion_zeroelim(cytablen, cytab, cdytail, temp16a);
      cytabtlen = scale_expansion_zeroelim(abtlen, abt, cdytail, cytabt);
      temp32alen = scale_expansion_zeroelim(cytabtlen, cytabt, 2.0 * cdy,
                                            temp32a);
      temp48len = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp32alen, temp32a, temp48);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp48len,
                                              temp48, finother);
      finswap = finnow; finnow = finother; finother = finswap;


      temp32alen = scale_expansion_zeroelim(cytabtlen, cytabt, cdytail,
                                            temp32a);
      cytabttlen = scale_expansion_zeroelim(abttlen, abtt, cdytail, cytabtt);
      temp16alen = scale_expansion_zeroelim(cytabttlen, cytabtt, 2.0 * cdy,
                                            temp16a);
      temp16blen = scale_expansion_zeroelim(cytabttlen, cytabtt, cdytail,
                                            temp16b);
      temp32blen = fast_expansion_sum_zeroelim(temp16alen, temp16a,
                                              temp16blen, temp16b, temp32b);
      temp64len = fast_expansion_sum_zeroelim(temp32alen, temp32a,
                                              temp32blen, temp32b, temp64);
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, temp64len,
                                              temp64, finother);
      finswap = finnow; finnow = finother; finother = finswap;
    }
  }

  return finnow[finlength - 1];
}

REAL Mesh::incircle(Vertex pa, Vertex pb, Vertex pc, Vertex pd) {
  REAL adx, bdx, cdx, ady, bdy, cdy;
  REAL bdxcdy, cdxbdy, cdxady, adxcdy, adxbdy, bdxady;
  REAL alift, blift, clift;
  REAL det;
  REAL permanent, errbound;

  incirclecount++;

  adx = pa[0] - pd[0];
  bdx = pb[0] - pd[0];
  cdx = pc[0] - pd[0];
  ady = pa[1] - pd[1];
  bdy = pb[1] - pd[1];
  cdy = pc[1] - pd[1];

  bdxcdy = bdx * cdy;
  cdxbdy = cdx * bdy;
  alift = adx * adx + ady * ady;

  cdxady = cdx * ady;
  adxcdy = adx * cdy;
  blift = bdx * bdx + bdy * bdy;

  adxbdy = adx * bdy;
  bdxady = bdx * ady;
  clift = cdx * cdx + cdy * cdy;

  det = alift * (bdxcdy - cdxbdy)
      + blift * (cdxady - adxcdy)
      + clift * (adxbdy - bdxady);

  if (m_b.noexact) {
    return det;
  }

  permanent = (Absolute(bdxcdy) + Absolute(cdxbdy)) * alift
            + (Absolute(cdxady) + Absolute(adxcdy)) * blift
            + (Absolute(adxbdy) + Absolute(bdxady)) * clift;
  errbound = iccerrboundA * permanent;
  if ((det > errbound) || (-det > errbound)) {
    return det;
  }

  return incircleadapt(pa, pb, pc, pd, permanent);
}

//***************************************************************************
//
//  orient3d()   Return a positive value if the point pd lies below the
//               plane passing through pa, pb, and pc; "below" is defined so
//               that pa, pb, and pc appear in counterclockwise order when
//               viewed from above the plane. Returns a negative value if
//               pd lies above the plane. Returns zero if the points are
//               coplanar. The result is also a rough approximation of six
//               times the signed volume of the tetrahedron defined by the
//               four points.
//
//  Uses exact arithmetic if necessary to ensure a correct answer. The
//  result returned is the determinant of a matrix. This determinant is
//  computed adaptively, in the sense that exact arithmetic is used only to
//  the degree it is needed to ensure that the returned value has the
//  correct sign. Hence, this function is usually quite fast, but will run
//  more slowly when the input points are coplanar or nearly so.
//
//  See my Robust Predicates paper for details.
//
REAL orient3dadapt(Vertex pa, Vertex pb, Vertex pc, Vertex pd,
                   REAL aheight, REAL bheight, REAL cheight, REAL dheight,
                   REAL permanent)
{
  INEXACT REAL adx, bdx, cdx, ady, bdy, cdy, adheight, bdheight, cdheight;
  REAL det, errbound;

  INEXACT REAL bdxcdy1, cdxbdy1, cdxady1, adxcdy1, adxbdy1, bdxady1;
  REAL bdxcdy0, cdxbdy0, cdxady0, adxcdy0, adxbdy0, bdxady0;
  REAL bc[4], ca[4], ab[4];
  INEXACT REAL bc3, ca3, ab3;
  REAL adet[8], bdet[8], cdet[8];
  int alen, blen, clen;
  REAL abdet[16];
  int ablen;
  REAL *finnow, *finother, *finswap;
  REAL fin1[192], fin2[192];
  int finlength;

  REAL adxtail, bdxtail, cdxtail;
  REAL adytail, bdytail, cdytail;
  REAL adheighttail, bdheighttail, cdheighttail;
  INEXACT REAL at_blarge, at_clarge;
  INEXACT REAL bt_clarge, bt_alarge;
  INEXACT REAL ct_alarge, ct_blarge;
  REAL at_b[4], at_c[4], bt_c[4], bt_a[4], ct_a[4], ct_b[4];
  int at_blen, at_clen, bt_clen, bt_alen, ct_alen, ct_blen;
  INEXACT REAL bdxt_cdy1, cdxt_bdy1, cdxt_ady1;
  INEXACT REAL adxt_cdy1, adxt_bdy1, bdxt_ady1;
  REAL bdxt_cdy0, cdxt_bdy0, cdxt_ady0;
  REAL adxt_cdy0, adxt_bdy0, bdxt_ady0;
  INEXACT REAL bdyt_cdx1, cdyt_bdx1, cdyt_adx1;
  INEXACT REAL adyt_cdx1, adyt_bdx1, bdyt_adx1;
  REAL bdyt_cdx0, cdyt_bdx0, cdyt_adx0;
  REAL adyt_cdx0, adyt_bdx0, bdyt_adx0;
  REAL bct[8], cat[8], abt[8];
  int bctlen, catlen, abtlen;
  INEXACT REAL bdxt_cdyt1, cdxt_bdyt1, cdxt_adyt1;
  INEXACT REAL adxt_cdyt1, adxt_bdyt1, bdxt_adyt1;
  REAL bdxt_cdyt0, cdxt_bdyt0, cdxt_adyt0;
  REAL adxt_cdyt0, adxt_bdyt0, bdxt_adyt0;
  REAL u[4], v[12], w[16];
  INEXACT REAL u3;
  int vlength, wlength;
  REAL negate;

  INEXACT REAL bvirt;
  REAL avirt, bround, around;
  INEXACT REAL c;
  INEXACT REAL abig;
  REAL ahi, alo, bhi, blo;
  REAL err1, err2, err3;
  INEXACT REAL _i, _j, _k;
  REAL _0;

  adx = (REAL) (pa[0] - pd[0]);
  bdx = (REAL) (pb[0] - pd[0]);
  cdx = (REAL) (pc[0] - pd[0]);
  ady = (REAL) (pa[1] - pd[1]);
  bdy = (REAL) (pb[1] - pd[1]);
  cdy = (REAL) (pc[1] - pd[1]);
  adheight = (REAL) (aheight - dheight);
  bdheight = (REAL) (bheight - dheight);
  cdheight = (REAL) (cheight - dheight);

  Two_Product(bdx, cdy, bdxcdy1, bdxcdy0);
  Two_Product(cdx, bdy, cdxbdy1, cdxbdy0);
  Two_Two_Diff(bdxcdy1, bdxcdy0, cdxbdy1, cdxbdy0, bc3, bc[2], bc[1], bc[0]);
  bc[3] = bc3;
  alen = scale_expansion_zeroelim(4, bc, adheight, adet);

  Two_Product(cdx, ady, cdxady1, cdxady0);
  Two_Product(adx, cdy, adxcdy1, adxcdy0);
  Two_Two_Diff(cdxady1, cdxady0, adxcdy1, adxcdy0, ca3, ca[2], ca[1], ca[0]);
  ca[3] = ca3;
  blen = scale_expansion_zeroelim(4, ca, bdheight, bdet);

  Two_Product(adx, bdy, adxbdy1, adxbdy0);
  Two_Product(bdx, ady, bdxady1, bdxady0);
  Two_Two_Diff(adxbdy1, adxbdy0, bdxady1, bdxady0, ab3, ab[2], ab[1], ab[0]);
  ab[3] = ab3;
  clen = scale_expansion_zeroelim(4, ab, cdheight, cdet);

  ablen = fast_expansion_sum_zeroelim(alen, adet, blen, bdet, abdet);
  finlength = fast_expansion_sum_zeroelim(ablen, abdet, clen, cdet, fin1);

  det = estimate(finlength, fin1);
  errbound = o3derrboundB * permanent;
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  Two_Diff_Tail(pa[0], pd[0], adx, adxtail);
  Two_Diff_Tail(pb[0], pd[0], bdx, bdxtail);
  Two_Diff_Tail(pc[0], pd[0], cdx, cdxtail);
  Two_Diff_Tail(pa[1], pd[1], ady, adytail);
  Two_Diff_Tail(pb[1], pd[1], bdy, bdytail);
  Two_Diff_Tail(pc[1], pd[1], cdy, cdytail);
  Two_Diff_Tail(aheight, dheight, adheight, adheighttail);
  Two_Diff_Tail(bheight, dheight, bdheight, bdheighttail);
  Two_Diff_Tail(cheight, dheight, cdheight, cdheighttail);

  if ((adxtail == 0.0) && (bdxtail == 0.0) && (cdxtail == 0.0) &&
      (adytail == 0.0) && (bdytail == 0.0) && (cdytail == 0.0) &&
      (adheighttail == 0.0) &&
      (bdheighttail == 0.0) &&
      (cdheighttail == 0.0)) {
    return det;
  }

  errbound = o3derrboundC * permanent + resulterrbound * Absolute(det);
  det += (adheight * ((bdx * cdytail + cdy * bdxtail) -
                      (bdy * cdxtail + cdx * bdytail)) +
          adheighttail * (bdx * cdy - bdy * cdx)) +
         (bdheight * ((cdx * adytail + ady * cdxtail) -
                      (cdy * adxtail + adx * cdytail)) +
          bdheighttail * (cdx * ady - cdy * adx)) +
         (cdheight * ((adx * bdytail + bdy * adxtail) -
                      (ady * bdxtail + bdx * adytail)) +
          cdheighttail * (adx * bdy - ady * bdx));
  if ((det >= errbound) || (-det >= errbound)) {
    return det;
  }

  finnow = fin1;
  finother = fin2;

  if (adxtail == 0.0) {
    if (adytail == 0.0) {
      at_b[0] = 0.0;
      at_blen = 1;
      at_c[0] = 0.0;
      at_clen = 1;
    } else {
      negate = -adytail;
      Two_Product(negate, bdx, at_blarge, at_b[0]);
      at_b[1] = at_blarge;
      at_blen = 2;
      Two_Product(adytail, cdx, at_clarge, at_c[0]);
      at_c[1] = at_clarge;
      at_clen = 2;
    }
  } else {
    if (adytail == 0.0) {
      Two_Product(adxtail, bdy, at_blarge, at_b[0]);
      at_b[1] = at_blarge;
      at_blen = 2;
      negate = -adxtail;
      Two_Product(negate, cdy, at_clarge, at_c[0]);
      at_c[1] = at_clarge;
      at_clen = 2;
    } else {
      Two_Product(adxtail, bdy, adxt_bdy1, adxt_bdy0);
      Two_Product(adytail, bdx, adyt_bdx1, adyt_bdx0);
      Two_Two_Diff(adxt_bdy1, adxt_bdy0, adyt_bdx1, adyt_bdx0,
                   at_blarge, at_b[2], at_b[1], at_b[0]);
      at_b[3] = at_blarge;
      at_blen = 4;
      Two_Product(adytail, cdx, adyt_cdx1, adyt_cdx0);
      Two_Product(adxtail, cdy, adxt_cdy1, adxt_cdy0);
      Two_Two_Diff(adyt_cdx1, adyt_cdx0, adxt_cdy1, adxt_cdy0,
                   at_clarge, at_c[2], at_c[1], at_c[0]);
      at_c[3] = at_clarge;
      at_clen = 4;
    }
  }
  if (bdxtail == 0.0) {
    if (bdytail == 0.0) {
      bt_c[0] = 0.0;
      bt_clen = 1;
      bt_a[0] = 0.0;
      bt_alen = 1;
    } else {
      negate = -bdytail;
      Two_Product(negate, cdx, bt_clarge, bt_c[0]);
      bt_c[1] = bt_clarge;
      bt_clen = 2;
      Two_Product(bdytail, adx, bt_alarge, bt_a[0]);
      bt_a[1] = bt_alarge;
      bt_alen = 2;
    }
  } else {
    if (bdytail == 0.0) {
      Two_Product(bdxtail, cdy, bt_clarge, bt_c[0]);
      bt_c[1] = bt_clarge;
      bt_clen = 2;
      negate = -bdxtail;
      Two_Product(negate, ady, bt_alarge, bt_a[0]);
      bt_a[1] = bt_alarge;
      bt_alen = 2;
    } else {
      Two_Product(bdxtail, cdy, bdxt_cdy1, bdxt_cdy0);
      Two_Product(bdytail, cdx, bdyt_cdx1, bdyt_cdx0);
      Two_Two_Diff(bdxt_cdy1, bdxt_cdy0, bdyt_cdx1, bdyt_cdx0,
                   bt_clarge, bt_c[2], bt_c[1], bt_c[0]);
      bt_c[3] = bt_clarge;
      bt_clen = 4;
      Two_Product(bdytail, adx, bdyt_adx1, bdyt_adx0);
      Two_Product(bdxtail, ady, bdxt_ady1, bdxt_ady0);
      Two_Two_Diff(bdyt_adx1, bdyt_adx0, bdxt_ady1, bdxt_ady0,
                  bt_alarge, bt_a[2], bt_a[1], bt_a[0]);
      bt_a[3] = bt_alarge;
      bt_alen = 4;
    }
  }
  if (cdxtail == 0.0) {
    if (cdytail == 0.0) {
      ct_a[0] = 0.0;
      ct_alen = 1;
      ct_b[0] = 0.0;
      ct_blen = 1;
    } else {
      negate = -cdytail;
      Two_Product(negate, adx, ct_alarge, ct_a[0]);
      ct_a[1] = ct_alarge;
      ct_alen = 2;
      Two_Product(cdytail, bdx, ct_blarge, ct_b[0]);
      ct_b[1] = ct_blarge;
      ct_blen = 2;
    }
  } else {
    if (cdytail == 0.0) {
      Two_Product(cdxtail, ady, ct_alarge, ct_a[0]);
      ct_a[1] = ct_alarge;
      ct_alen = 2;
      negate = -cdxtail;
      Two_Product(negate, bdy, ct_blarge, ct_b[0]);
      ct_b[1] = ct_blarge;
      ct_blen = 2;
    } else {
      Two_Product(cdxtail, ady, cdxt_ady1, cdxt_ady0);
      Two_Product(cdytail, adx, cdyt_adx1, cdyt_adx0);
      Two_Two_Diff(cdxt_ady1, cdxt_ady0, cdyt_adx1, cdyt_adx0,
                   ct_alarge, ct_a[2], ct_a[1], ct_a[0]);
      ct_a[3] = ct_alarge;
      ct_alen = 4;
      Two_Product(cdytail, bdx, cdyt_bdx1, cdyt_bdx0);
      Two_Product(cdxtail, bdy, cdxt_bdy1, cdxt_bdy0);
      Two_Two_Diff(cdyt_bdx1, cdyt_bdx0, cdxt_bdy1, cdxt_bdy0,
                   ct_blarge, ct_b[2], ct_b[1], ct_b[0]);
      ct_b[3] = ct_blarge;
      ct_blen = 4;
    }
  }

  bctlen = fast_expansion_sum_zeroelim(bt_clen, bt_c, ct_blen, ct_b, bct);
  wlength = scale_expansion_zeroelim(bctlen, bct, adheight, w);
  finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                          finother);
  finswap = finnow; finnow = finother; finother = finswap;

  catlen = fast_expansion_sum_zeroelim(ct_alen, ct_a, at_clen, at_c, cat);
  wlength = scale_expansion_zeroelim(catlen, cat, bdheight, w);
  finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                          finother);
  finswap = finnow; finnow = finother; finother = finswap;

  abtlen = fast_expansion_sum_zeroelim(at_blen, at_b, bt_alen, bt_a, abt);
  wlength = scale_expansion_zeroelim(abtlen, abt, cdheight, w);
  finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                          finother);
  finswap = finnow; finnow = finother; finother = finswap;

  if (adheighttail != 0.0) {
    vlength = scale_expansion_zeroelim(4, bc, adheighttail, v);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, vlength, v,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (bdheighttail != 0.0) {
    vlength = scale_expansion_zeroelim(4, ca, bdheighttail, v);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, vlength, v,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (cdheighttail != 0.0) {
    vlength = scale_expansion_zeroelim(4, ab, cdheighttail, v);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, vlength, v,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }

  if (adxtail != 0.0) {
    if (bdytail != 0.0) {
      Two_Product(adxtail, bdytail, adxt_bdyt1, adxt_bdyt0);
      Two_One_Product(adxt_bdyt1, adxt_bdyt0, cdheight, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (cdheighttail != 0.0) {
        Two_One_Product(adxt_bdyt1, adxt_bdyt0, cdheighttail,
                        u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
    if (cdytail != 0.0) {
      negate = -adxtail;
      Two_Product(negate, cdytail, adxt_cdyt1, adxt_cdyt0);
      Two_One_Product(adxt_cdyt1, adxt_cdyt0, bdheight, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (bdheighttail != 0.0) {
        Two_One_Product(adxt_cdyt1, adxt_cdyt0, bdheighttail,
                        u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
  }
  if (bdxtail != 0.0) {
    if (cdytail != 0.0) {
      Two_Product(bdxtail, cdytail, bdxt_cdyt1, bdxt_cdyt0);
      Two_One_Product(bdxt_cdyt1, bdxt_cdyt0, adheight, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (adheighttail != 0.0) {
        Two_One_Product(bdxt_cdyt1, bdxt_cdyt0, adheighttail,
                        u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
    if (adytail != 0.0) {
      negate = -bdxtail;
      Two_Product(negate, adytail, bdxt_adyt1, bdxt_adyt0);
      Two_One_Product(bdxt_adyt1, bdxt_adyt0, cdheight, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (cdheighttail != 0.0) {
        Two_One_Product(bdxt_adyt1, bdxt_adyt0, cdheighttail,
                        u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
  }
  if (cdxtail != 0.0) {
    if (adytail != 0.0) {
      Two_Product(cdxtail, adytail, cdxt_adyt1, cdxt_adyt0);
      Two_One_Product(cdxt_adyt1, cdxt_adyt0, bdheight, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (bdheighttail != 0.0) {
        Two_One_Product(cdxt_adyt1, cdxt_adyt0, bdheighttail,
                        u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
    if (bdytail != 0.0) {
      negate = -cdxtail;
      Two_Product(negate, bdytail, cdxt_bdyt1, cdxt_bdyt0);
      Two_One_Product(cdxt_bdyt1, cdxt_bdyt0, adheight, u3, u[2], u[1], u[0]);
      u[3] = u3;
      finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                              finother);
      finswap = finnow; finnow = finother; finother = finswap;
      if (adheighttail != 0.0) {
        Two_One_Product(cdxt_bdyt1, cdxt_bdyt0, adheighttail,
                        u3, u[2], u[1], u[0]);
        u[3] = u3;
        finlength = fast_expansion_sum_zeroelim(finlength, finnow, 4, u,
                                                finother);
        finswap = finnow; finnow = finother; finother = finswap;
      }
    }
  }

  if (adheighttail != 0.0) {
    wlength = scale_expansion_zeroelim(bctlen, bct, adheighttail, w);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (bdheighttail != 0.0) {
    wlength = scale_expansion_zeroelim(catlen, cat, bdheighttail, w);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }
  if (cdheighttail != 0.0) {
    wlength = scale_expansion_zeroelim(abtlen, abt, cdheighttail, w);
    finlength = fast_expansion_sum_zeroelim(finlength, finnow, wlength, w,
                                            finother);
    finswap = finnow; finnow = finother; finother = finswap;
  }

  return finnow[finlength - 1];
}

REAL Mesh::orient3d(Vertex pa, Vertex pb, Vertex pc, Vertex pd,
                    REAL aheight, REAL bheight, REAL cheight, REAL dheight)
{
  REAL adx, bdx, cdx, ady, bdy, cdy, adheight, bdheight, cdheight;
  REAL bdxcdy, cdxbdy, cdxady, adxcdy, adxbdy, bdxady;
  REAL det;
  REAL permanent, errbound;

  orient3dcount++;

  adx = pa[0] - pd[0];
  bdx = pb[0] - pd[0];
  cdx = pc[0] - pd[0];
  ady = pa[1] - pd[1];
  bdy = pb[1] - pd[1];
  cdy = pc[1] - pd[1];
  adheight = aheight - dheight;
  bdheight = bheight - dheight;
  cdheight = cheight - dheight;

  bdxcdy = bdx * cdy;
  cdxbdy = cdx * bdy;

  cdxady = cdx * ady;
  adxcdy = adx * cdy;

  adxbdy = adx * bdy;
  bdxady = bdx * ady;

  det = adheight * (bdxcdy - cdxbdy)
      + bdheight * (cdxady - adxcdy)
      + cdheight * (adxbdy - bdxady);

  if (m_b.noexact) {
    return det;
  }

  permanent = (Absolute(bdxcdy) + Absolute(cdxbdy)) * Absolute(adheight)
            + (Absolute(cdxady) + Absolute(adxcdy)) * Absolute(bdheight)
            + (Absolute(adxbdy) + Absolute(bdxady)) * Absolute(cdheight);
  errbound = o3derrboundA * permanent;
  if ((det > errbound) || (-det > errbound)) {
    return det;
  }

  return orient3dadapt(pa, pb, pc, pd, aheight, bheight, cheight, dheight,
                       permanent);
}

//***************************************************************************
//  nonregular()   Return a positive value if the point pd is incompatible
//                 with the circle or plane passing through pa, pb, and pc
//                 (meaning that pd is inside the circle or below the
//                 plane); a negative value if it is compatible; and zero if
//                 the four points are cocircular/coplanar. The points pa,
//                 pb, and pc must be in counterclockwise order, or the sign
//                 of the result will be reversed.
//
//  If the -w switch is used, the points are lifted onto the parabolic
//  lifting map, then they are dropped according to their weights, then the
//  3D orientation test is applied. If the -W switch is used, the points'
//  heights are already provided, so the 3D orientation test is applied
//  directly. If neither switch is used, the incircle test is applied.
//***************************************************************************
REAL Mesh::nonregular(Vertex pa, Vertex pb, Vertex pc, Vertex pd) {
  if (m_b.weighted == 0) {
    return incircle(pa, pb, pc, pd);
  } else if (m_b.weighted == 1) {
    return orient3d(pa, pb, pc, pd,
                    pa[0] * pa[0] + pa[1] * pa[1] - pa[2],
                    pb[0] * pb[0] + pb[1] * pb[1] - pb[2],
                    pc[0] * pc[0] + pc[1] * pc[1] - pc[2],
                    pd[0] * pd[0] + pd[1] * pd[1] - pd[2]);
  } else {
    return orient3d(pa, pb, pc, pd, pa[2], pb[2], pc[2], pd[2]);
  }
}

//***************************************************************************
//  findcircumcenter()   Find the circumcenter of a Triangle.
//
//  The result is returned both in terms of x-y coordinates and xi-eta
//  (barycentric) coordinates. The xi-eta coordinate system is defined in
//  terms of the Triangle:  the origin of the Triangle is the origin of the
//  coordinate system; the destination of the Triangle is one unit along the
//  xi axis; and the apex of the Triangle is one unit along the eta axis.
//  This procedure also returns the square of the length of the Triangle's
//  shortest edge.
//***************************************************************************
void Mesh::findcircumcenter(Vertex torg, Vertex tdest, Vertex tapex,
                            Vertex circumcenter, REAL *xi, REAL *eta, int offcenter)
{
  REAL xdo, ydo, xao, yao;
  REAL dodist, aodist, dadist;
  REAL denominator;
  REAL dx, dy, dxoff, dyoff;

  circumcentercount++;

  // Compute the circumcenter of the Triangle.
  xdo = tdest[0] - torg[0];
  ydo = tdest[1] - torg[1];
  xao = tapex[0] - torg[0];
  yao = tapex[1] - torg[1];
  dodist = xdo * xdo + ydo * ydo;
  aodist = xao * xao + yao * yao;
  dadist = (tdest[0] - tapex[0]) * (tdest[0] - tapex[0]) +
           (tdest[1] - tapex[1]) * (tdest[1] - tapex[1]);
  if (m_b.noexact) {
    denominator = 0.5 / (xdo * yao - xao * ydo);
  } else {
    // Use the counterclockwise() routine to ensure a positive (and
    // reasonably accurate) result, avoiding any possibility of
    // division by zero.
    denominator = 0.5 / counterclockwise(tdest, tapex, torg);
    // Don't count the above as an orientation test.
    counterclockcount--;
  }
  dx = (yao * dodist - ydo * aodist) * denominator;
  dy = (xdo * aodist - xao * dodist) * denominator;

  // Find the (squared) length of the Triangle's shortest edge. This
  // serves as a conservative estimate of the insertion radius of the
  // circumcenter's parent. The estimate is used to ensure that
  // the algorithm terminates even if very small angles appear in
  // the input PSLG.
  if ((dodist < aodist) && (dodist < dadist)) {
    if (offcenter && (m_b.offconstant > 0.0)) {
      // Find the position of the off-center, as described by Alper Ungor.
      dxoff = 0.5 * xdo - m_b.offconstant * ydo;
      dyoff = 0.5 * ydo + m_b.offconstant * xdo;
      // If the off-center is closer to the origin than the
      // circumcenter, use the off-center instead.
      if (dxoff * dxoff + dyoff * dyoff < dx * dx + dy * dy) {
        dx = dxoff;
        dy = dyoff;
      }
    }
  } else if (aodist < dadist) {
    if (offcenter && (m_b.offconstant > 0.0)) {
      dxoff = 0.5 * xao + m_b.offconstant * yao;
      dyoff = 0.5 * yao - m_b.offconstant * xao;
      // If the off-center is closer to the origin than the
      // circumcenter, use the off-center instead.
      if (dxoff * dxoff + dyoff * dyoff < dx * dx + dy * dy) {
        dx = dxoff;
        dy = dyoff;
      }
    }
  } else {
    if (offcenter && (m_b.offconstant > 0.0)) {
      dxoff = 0.5 * (tapex[0] - tdest[0]) -
              m_b.offconstant * (tapex[1] - tdest[1]);
      dyoff = 0.5 * (tapex[1] - tdest[1]) +
              m_b.offconstant * (tapex[0] - tdest[0]);
      // If the off-center is closer to the destination than the
      // circumcenter, use the off-center instead.
      if (dxoff * dxoff + dyoff * dyoff <
          (dx - xdo) * (dx - xdo) + (dy - ydo) * (dy - ydo)) {
        dx = xdo + dxoff;
        dy = ydo + dyoff;
      }
    }
  }

  circumcenter[0] = torg[0] + dx;
  circumcenter[1] = torg[1] + dy;

  // To interpolate Vertex attributes for the new Vertex inserted at
  // the circumcenter, define a coordinate system with a xi-axis,
  // directed from the Triangle's origin to its destination, and
  // an eta-axis, directed from its origin to its apex.
  // Calculate the xi and eta coordinates of the circumcenter.
  *xi  = (yao * dx - xao * dy) * (2.0 * denominator);
  *eta = (xdo * dy - ydo * dx) * (2.0 * denominator);
}

//******** Geometric primitives end here

//  Initialize some variables.
void Mesh::triangleinit() {
  recenttri.tri = NULL;   // No Triangle has been visited yet.
  undeads = 0;            // No eliminated input vertices yet.
  samples = 1;            // Point location should take at least one sample.
  checksegments = 0;      // There are no segments in the triangulation yet.
  checkquality = 0;       // The quality triangulation stage has not begun.
  incirclecount = counterclockcount = orient3dcount = 0;
  hyperbolacount = circletopcount = circumcentercount = 0;

  exactinit();                     // Initialize exact arithmetic constants.
}

//******** Mesh quality testing routines begin here

#ifndef REDUCED
//  Test the mesh for topological consistency.
void Mesh::checkmesh() {
  otri triangleloop;
  otri oppotri, oppooppotri;
  Vertex triorg, tridest, triapex;
  Vertex oppoorg, oppodest;
  int horrors;
  int saveexact;
  Triangle ptr;                         // Temporary variable used by sym().

  // Temporarily turn on exact arithmetic if it's off.
  saveexact = m_b.noexact;
  m_b.noexact = 0;
  if (!m_b.quiet) {
    _tprintf(_T("  Checking consistency of mesh...\n"));
  }
  horrors = 0;
  // Run through the list of triangles, checking each one.
  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  while (triangleloop.tri != NULL) {
    // Check all three edges of the Triangle.
    for (triangleloop.orient = 0; triangleloop.orient < 3;triangleloop.orient++) {
      org(triangleloop, triorg);
      dest(triangleloop, tridest);
      if (triangleloop.orient == 0) {       // Only test for inversion once.
        // Test if the Triangle is flat or inverted.
        apex(triangleloop, triapex);
        if (counterclockwise(triorg, tridest, triapex) <= 0.0) {
          printf("  !! !! Inverted ");
          printtriangle(&triangleloop);
          horrors++;
        }
      }
      // Find the neighboring Triangle on this edge.
      sym(triangleloop, oppotri);
      if (oppotri.tri != dummytri) {
        // Check that the Triangle's neighbor knows it's a neighbor.
        sym(oppotri, oppooppotri);
        if ((triangleloop.tri != oppooppotri.tri)
            || (triangleloop.orient != oppooppotri.orient)) {
          printf("  !! !! Asymmetric Triangle-Triangle bond:\n");
          if (triangleloop.tri == oppooppotri.tri) {
            printf("   (Right Triangle, wrong orientation)\n");
          }
          printf("    First ");
          printtriangle(&triangleloop);
          printf("    Second (nonreciprocating) ");
          printtriangle(&oppotri);
          horrors++;
        }
        // Check that both triangles agree on the identities
        // of their shared vertices.
        org(oppotri, oppoorg);
        dest(oppotri, oppodest);
        if ((triorg != oppodest) || (tridest != oppoorg)) {
          printf("  !! !! Mismatched edge coordinates between two triangles:\n");
          printf("    First mismatched ");
          printtriangle(&triangleloop);
          printf("    Second mismatched ");
          printtriangle(&oppotri);
          horrors++;
        }
      }
    }
    triangleloop.tri = triangletraverse();
  }
  if (horrors == 0) {
    if (!m_b.quiet) {
      printf("  In my studied opinion, the mesh appears to be consistent.\n");
    }
  } else if (horrors == 1) {
    printf("  !! !! !! !! Precisely one festering wound discovered.\n");
  } else {
    printf("  !! !! !! !! %d abominations witnessed.\n", horrors);
  }
  // Restore the status of exact arithmetic.
  m_b.noexact = saveexact;
}

#endif // not REDUCED

#ifndef REDUCED

//  Ensure that the mesh is (constrained) Delaunay.
void Mesh::checkdelaunay() {
  otri triangleloop;
  otri oppotri;
  osub opposubseg;
  Vertex triorg, tridest, triapex;
  Vertex oppoapex;
  int shouldbedelaunay;
  int horrors;
  int saveexact;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  // Temporarily turn on exact arithmetic if it's off.
  saveexact = m_b.noexact;
  m_b.noexact = 0;
  if (!m_b.quiet) {
    printf("  Checking Delaunay property of mesh...\n");
  }
  horrors = 0;
  // Run through the list of triangles, checking each one.
  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  while (triangleloop.tri != NULL) {
    // Check all three edges of the Triangle.
    for (triangleloop.orient = 0; triangleloop.orient < 3; triangleloop.orient++) {
      org(triangleloop, triorg);
      dest(triangleloop, tridest);
      apex(triangleloop, triapex);
      sym(triangleloop, oppotri);
      apex(oppotri, oppoapex);
      // Only test that the edge is locally Delaunay if there is an
      // adjoining Triangle whose pointer is larger (to ensure that
      // each pair isn't tested twice).
      shouldbedelaunay = (oppotri.tri != dummytri) &&
            !deadtri(oppotri.tri) && (triangleloop.tri < oppotri.tri) &&
            (triorg != infvertex1) && (triorg != infvertex2) &&
            (triorg != infvertex3) &&
            (tridest != infvertex1) && (tridest != infvertex2) &&
            (tridest != infvertex3) &&
            (triapex != infvertex1) && (triapex != infvertex2) &&
            (triapex != infvertex3) &&
            (oppoapex != infvertex1) && (oppoapex != infvertex2) &&
            (oppoapex != infvertex3);
      if (checksegments && shouldbedelaunay) {
        // If a subsegment separates the triangles, then the edge is
        // constrained, so no local Delaunay test should be done.
        tspivot(triangleloop, opposubseg);
        if (opposubseg.ss != dummysub){
          shouldbedelaunay = 0;
        }
      }
      if (shouldbedelaunay) {
        if (nonregular(triorg, tridest, triapex, oppoapex) > 0.0) {
          if (!m_b.weighted) {
            printf("  !! !! Non-Delaunay pair of triangles:\n");
            printf("    First non-Delaunay ");
            printtriangle(&triangleloop);
            printf("    Second non-Delaunay ");
          } else {
            printf("  !! !! Non-regular pair of triangles:\n");
            printf("    First non-regular ");
            printtriangle(&triangleloop);
            printf("    Second non-regular ");
          }
          printtriangle(&oppotri);
          horrors++;
        }
      }
    }
    triangleloop.tri = triangletraverse();
  }
  if (horrors == 0) {
    if (!m_b.quiet) {
      printf("  By virtue of my perceptive intelligence, I declare the mesh Delaunay.\n");
    }
  } else if (horrors == 1) {
    printf("  !! !! !! !! Precisely one terrifying transgression identified.\n");
  } else {
    printf("  !! !! !! !! %d obscenities viewed with horror.\n", horrors);
  }
  // Restore the status of exact arithmetic.
  m_b.noexact = saveexact;
}

#endif // not REDUCED

#ifndef CDT_ONLY
//***************************************************************************
//  enqueuebadtriang()   Add a bad Triangle data structure to the end of a
//                       queue.
//
//  The queue is actually a set of 4096 queues. I use multiple queues to
//  give priority to smaller angles. I originally implemented a heap, but
//  the queues are faster by a larger margin than I'd suspected.
void Mesh::enqueuebadtriang(BadTriangle *badtri) {
  REAL length, multiplier;
  int exponent, expincrement;
  int queuenumber;
  int posexponent;
  int i;

  if (m_b.verbose > 2) {
    _tprintf(_T("  Queueing bad Triangle: %s\n")
            ,triangleToString(badtri->triangorg, badtri->triangdest, badtri->triangapex).cstr());
  }

  // Determine the appropriate queue to put the bad Triangle into.
  // Recall that the key is the square of its shortest edge length.
  if (badtri->key >= 1.0) {
    length = badtri->key;
    posexponent = 1;
  } else {
    // badtri->key' is 2.0 to a negative exponent, so we'll record that
    // fact and use the reciprocal of `badtri->key', which is > 1.0.
    length = 1.0 / badtri->key;
    posexponent = 0;
  }
  // length' is approximately 2.0 to what exponent?  The following code
  // determines the answer in time logarithmic in the exponent.
  exponent = 0;
  while (length > 2.0) {
    // Find an approximation by repeated squaring of two.
    expincrement = 1;
    multiplier = 0.5;
    while (length * multiplier * multiplier > 1.0) {
      expincrement *= 2;
      multiplier *= multiplier;
    }
    // Reduce the value of `length', then iterate if necessary.
    exponent += expincrement;
    length *= multiplier;
  }
  // length' is approximately squareroot(2.0) to what exponent?
  exponent = (int)(2.0 * exponent + (length > SQUAREROOTTWO));
  // `exponent' is now in the range 0...2047 for IEEE double precision.
  //  Choose a queue in the range 0...4095. The shortest edges have the
  //  highest priority (queue 4095).
  if (posexponent) {
    queuenumber = 2047 - exponent;
  } else {
    queuenumber = 2048 + exponent;
  }

  // Are we inserting into an empty queue?
  if (queuefront[queuenumber] == NULL) {
    // Yes, we are inserting into an empty queue.
    // Will this become the highest-priority queue?
    if (queuenumber > firstnonemptyq) {
      // Yes, this is the highest-priority queue.
      nextnonemptyq[queuenumber] = firstnonemptyq;
      firstnonemptyq = queuenumber;
    } else {
      // No, this is not the highest-priority queue.
      // Find the queue with next higher priority.
      i = queuenumber + 1;
      while (queuefront[i] == NULL) {
        i++;
      }
      // Mark the newly nonempty queue as following a higher-priority queue.
      nextnonemptyq[queuenumber] = nextnonemptyq[i];
      nextnonemptyq[i] = queuenumber;
    }
    // Put the bad Triangle at the beginning of the (empty) queue.
    queuefront[queuenumber] = badtri;
  } else {
    // Add the bad Triangle to the end of an already nonempty queue.
    queuetail[queuenumber]->nexttriang = badtri;
  }
  // Maintain a pointer to the last Triangle of the queue.
  queuetail[queuenumber] = badtri;
  // Newly enqueued bad Triangle has no successor in the queue.
  badtri->nexttriang = NULL;
}

//***************************************************************************
//  enqueuebadtri()   Add a bad Triangle to the end of a queue.
//
//  Allocates a BadTriangle data structure for the Triangle, then passes it to
//  enqueuebadtriang().
void Mesh::enqueuebadtri(otri *enqtri, REAL minedge, Vertex enqapex, Vertex enqorg, Vertex enqdest) {
  BadTriangle *newbad;

  // Allocate space for the bad Triangle.
  newbad = badtriangles.alloc();
  newbad->poortri = encode(*enqtri);
  newbad->key = minedge;
  newbad->triangapex = enqapex;
  newbad->triangorg = enqorg;
  newbad->triangdest = enqdest;
  enqueuebadtriang(newbad);
}

//  Remove a Triangle from the front of the queue.
BadTriangle *Mesh::dequeuebadtriang() {
  BadTriangle *result;

  // If no queues are nonempty, return NULL.
  if (firstnonemptyq < 0) {
    return NULL;
  }
  // Find the first Triangle of the highest-priority queue.
  result = queuefront[firstnonemptyq];
  // Remove the Triangle from the queue.
  queuefront[firstnonemptyq] = result->nexttriang;
  // If this queue is now empty, note the new highest-priority
  //   nonempty queue.
  if (result == queuetail[firstnonemptyq]) {
    firstnonemptyq = nextnonemptyq[firstnonemptyq];
  }
  return result;
}

//***************************************************************************
//
//  checkseg4encroach()   Check a subsegment to see if it is encroached; add
//                        it to the list if it is.
//
//  A subsegment is encroached if there is a Vertex in its diametral lens.
//  For Ruppert's algorithm (-D switch), the "diametral lens" is the
//  diametral circle. For Chew's algorithm (default), the diametral lens is
//  just big enough to enclose two isosceles triangles whose bases are the
//  subsegment. Each of the two isosceles triangles has two angles equal
//  to `b->minangle'.
//
//  Chew's algorithm does not require diametral lenses at all--but they save
//  time. Any Vertex inside a subsegment's diametral lens implies that the
//  Triangle adjoining the subsegment will be too skinny, so it's only a
//  matter of time before the encroaching Vertex is deleted by Chew's
//  algorithm. It's faster to simply not insert the doomed Vertex in the
//  first place, which is why I use diametral lenses with Chew's algorithm.
//
//  Returns a nonzero value if the subsegment is encroached.
int Mesh::checkseg4encroach(osub *testsubseg) {
  otri neighbortri;
  osub testsym;
  BadSubSeg *encroachedseg;
  REAL dotproduct;
  int encroached;
  int sides;
  Vertex eorg, edest, eapex;
  Triangle ptr;                     // Temporary variable used by stpivot().

  encroached = 0;
  sides = 0;

  sorg(*testsubseg, eorg);
  sdest(*testsubseg, edest);
  // Check one neighbor of the subsegment.
  stpivot(*testsubseg, neighbortri);
  // Does the neighbor exist, or is this a boundary edge?
  if (neighbortri.tri != dummytri) {
    sides++;
    // Find a Vertex opposite this subsegment.
    apex(neighbortri, eapex);
    // Check whether the apex is in the diametral lens of the subsegment
    // (the diametral circle if `conformdel' is set). A dot product
    // of two sides of the Triangle is used to check whether the angle
    // at the apex is greater than (180 - 2 `minangle') degrees (for
    // lenses; 90 degrees for diametral circles).
    dotproduct = (eorg[0] - eapex[0]) * (edest[0] - eapex[0])
               + (eorg[1] - eapex[1]) * (edest[1] - eapex[1]);
    if (dotproduct < 0.0) {
      if (m_b.conformdel ||
          (dotproduct * dotproduct >=
           (2.0 * m_b.goodangle - 1.0) * (2.0 * m_b.goodangle - 1.0) *
           ((eorg[0] - eapex[0]) * (eorg[0] - eapex[0]) +
            (eorg[1] - eapex[1]) * (eorg[1] - eapex[1])) *
           ((edest[0] - eapex[0]) * (edest[0] - eapex[0]) +
            (edest[1] - eapex[1]) * (edest[1] - eapex[1])))) {
        encroached = 1;
      }
    }
  }
  // Check the other neighbor of the subsegment.
  ssym(*testsubseg, testsym);
  stpivot(testsym, neighbortri);
  // Does the neighbor exist, or is this a boundary edge?
  if (neighbortri.tri != dummytri) {
    sides++;
    // Find the other Vertex opposite this subsegment.
    apex(neighbortri, eapex);
    // Check whether the apex is in the diametral lens of the subsegment
    // (or the diametral circle, if `conformdel' is set).
    dotproduct = (eorg[0] - eapex[0]) * (edest[0] - eapex[0]) +
                 (eorg[1] - eapex[1]) * (edest[1] - eapex[1]);
    if (dotproduct < 0.0) {
      if (m_b.conformdel ||
          (dotproduct * dotproduct >=
           (2.0 * m_b.goodangle - 1.0) * (2.0 * m_b.goodangle - 1.0) *
           ((eorg[0] - eapex[0]) * (eorg[0] - eapex[0]) +
            (eorg[1] - eapex[1]) * (eorg[1] - eapex[1])) *
           ((edest[0] - eapex[0]) * (edest[0] - eapex[0]) +
            (edest[1] - eapex[1]) * (edest[1] - eapex[1])))) {
        encroached += 2;
      }
    }
  }

  if (encroached && (!m_b.nobisect || ((m_b.nobisect == 1) && (sides == 2)))) {
    if (m_b.verbose > 2) {
      _tprintf(_T("  Queueing encroached subsegment %s\n"), edgeToString(eorg, edest).cstr());
    }
    // Add the subsegment to the list of encroached subsegments.
    // Be sure to get the orientation right.
    encroachedseg = badsubsegs.alloc();
    if (encroached == 1) {
      encroachedseg->encsubseg = sencode(*testsubseg);
      encroachedseg->subsegorg = eorg;
      encroachedseg->subsegdest = edest;
    } else {
      encroachedseg->encsubseg = sencode(testsym);
      encroachedseg->subsegorg = edest;
      encroachedseg->subsegdest = eorg;
    }
  }

  return encroached;
}

//***************************************************************************
//
//  testtriangle()   Test a Triangle for quality and size.
//
//  Tests a Triangle to see if it satisfies the minimum angle condition and
//  the maximum area condition. Triangles that aren't up to spec are added
//  to the bad Triangle queue.
void Mesh::testtriangle(otri *testtri) {
  otri tri1, tri2;
  osub testsub;
  Vertex torg, tdest, tapex;
  Vertex base1, base2;
  Vertex org1, dest1, org2, dest2;
  Vertex joinvertex;
  REAL dxod, dyod, dxda, dyda, dxao, dyao;
  REAL dxod2, dyod2, dxda2, dyda2, dxao2, dyao2;
  REAL apexlen, orglen, destlen, minedge;
  REAL angle;
  REAL area;
  REAL dist1, dist2;
  SubSeg sptr;            // Temporary variable used by tspivot().
  Triangle ptr;           // Temporary variable used by oprev() and dnext().

  org(*testtri, torg);
  dest(*testtri, tdest);
  apex(*testtri, tapex);
  dxod = torg[0] - tdest[0];
  dyod = torg[1] - tdest[1];
  dxda = tdest[0] - tapex[0];
  dyda = tdest[1] - tapex[1];
  dxao = tapex[0] - torg[0];
  dyao = tapex[1] - torg[1];
  dxod2 = dxod * dxod;
  dyod2 = dyod * dyod;
  dxda2 = dxda * dxda;
  dyda2 = dyda * dyda;
  dxao2 = dxao * dxao;
  dyao2 = dyao * dyao;
  // Find the lengths of the Triangle's three edges.
  apexlen = dxod2 + dyod2;
  orglen = dxda2 + dyda2;
  destlen = dxao2 + dyao2;

  if ((apexlen < orglen) && (apexlen < destlen)) {
    // The edge opposite the apex is shortest.
    minedge = apexlen;
    // Find the square of the cosine of the angle at the apex.
    angle = dxda * dxao + dyda * dyao;
    angle = angle * angle / (orglen * destlen);
    base1 = torg;
    base2 = tdest;
    otricopy(*testtri, tri1);
  } else if (orglen < destlen) {
    // The edge opposite the origin is shortest.
    minedge = orglen;
    // Find the square of the cosine of the angle at the origin.
    angle = dxod * dxao + dyod * dyao;
    angle = angle * angle / (apexlen * destlen);
    base1 = tdest;
    base2 = tapex;
    lnext(*testtri, tri1);
  } else {
    // The edge opposite the destination is shortest.
    minedge = destlen;
    // Find the square of the cosine of the angle at the destination.
    angle = dxod * dxda + dyod * dyda;
    angle = angle * angle / (apexlen * orglen);
    base1 = tapex;
    base2 = torg;
    lprev(*testtri, tri1);
  }

  if (m_b.vararea || m_b.fixedarea || m_b.usertest) {
    // Check whether the area is larger than permitted.
    area = 0.5 * (dxod * dyda - dyod * dxda);
    if (m_b.fixedarea && (area > m_b.maxarea)) {
      // Add this Triangle to the list of bad triangles.
      enqueuebadtri(testtri, minedge, tapex, torg, tdest);
      return;
    }

    // Nonpositive area constraints are treated as unconstrained.
    if ((m_b.vararea) && (area > areabound(*testtri)) &&
        (areabound(*testtri) > 0.0)) {
      // Add this Triangle to the list of bad triangles.
      enqueuebadtri(testtri, minedge, tapex, torg, tdest);
      return;
    }

    if (m_b.usertest) {
      // Check whether the user thinks this Triangle is too large.
      if (triunsuitable(torg, tdest, tapex, area)) {
        enqueuebadtri(testtri, minedge, tapex, torg, tdest);
        return;
      }
    }
  }

  // Check whether the angle is smaller than permitted.
  if (angle > m_b.goodangle) {
    // Use the rules of Miller, Pav, and Walkington to decide that certain
    // triangles should not be split, even if they have bad angles.
    // A skinny Triangle is not split if its shortest edge subtends a
    // small input angle, and both endpoints of the edge lie on a
    // concentric circular shell. For convenience, I make a small
    // adjustment to that rule:  I check if the endpoints of the edge
    // both lie in segment interiors, equidistant from the apex where
    // the two segments meet.
    // First, check if both points lie in segment interiors.
    if ((vertextype(base1) == SEGMENTVERTEX) && (vertextype(base2) == SEGMENTVERTEX)) {
      // Check if both points lie in a common segment. If they do, the
      // skinny Triangle is enqueued to be split as usual.
      tspivot(tri1, testsub);
      if (testsub.ss == dummysub) {
        // No common segment. Find a subsegment that contains `torg'.
        otricopy(tri1, tri2);
        do {
          oprevself(tri1);
          tspivot(tri1, testsub);
        } while (testsub.ss == dummysub);
        // Find the endpoints of the containing segment.
        segorg(testsub, org1);
        segdest(testsub, dest1);
        // Find a subsegment that contains `tdest'.
        do {
          dnextself(tri2);
          tspivot(tri2, testsub);
        } while (testsub.ss == dummysub);
        // Find the endpoints of the containing segment.
        segorg(testsub, org2);
        segdest(testsub, dest2);
        // Check if the two containing segments have an endpoint in common.
        joinvertex = NULL;
        if ((dest1[0] == org2[0]) && (dest1[1] == org2[1])) {
          joinvertex = dest1;
        } else if ((org1[0] == dest2[0]) && (org1[1] == dest2[1])) {
          joinvertex = org1;
        }
        if (joinvertex != NULL) {
          // Compute the distance from the common endpoint (of the two
          // segments) to each of the endpoints of the shortest edge.
          dist1 = ((base1[0] - joinvertex[0]) * (base1[0] - joinvertex[0])
                +  (base1[1] - joinvertex[1]) * (base1[1] - joinvertex[1]));
          dist2 = ((base2[0] - joinvertex[0]) * (base2[0] - joinvertex[0])
                +  (base2[1] - joinvertex[1]) * (base2[1] - joinvertex[1]));
          // If the two distances are equal, don't split the Triangle.
          if ((dist1 < 1.001 * dist2) && (dist1 > 0.999 * dist2)) {
            // Return now to avoid enqueueing the bad Triangle.
            return;
          }
        }
      }
    }

    // Add this Triangle to the list of bad triangles.
    enqueuebadtri(testtri, minedge, tapex, torg, tdest);
  }
}

#endif // not CDT_ONLY

//******** Mesh quality testing routines end here

//******** Point location routines begin here

//***************************************************************************
//
//  makevertexmap()   Construct a mapping from vertices to triangles to
//                    improve the speed of point location for segment
//                    insertion.
//
//  Traverses all the triangles, and provides each corner of each Triangle
//  with a pointer to that Triangle. Of course, pointers will be
//  overwritten by other pointers because (almost) each Vertex is a corner
//  of several triangles, but in the end every Vertex will point to some
//  Triangle that contains it.
void Mesh::makevertexmap() {
  otri triangleloop;
  Vertex triorg;

  if (m_b.verbose) {
    printf("    Constructing mapping from vertices to triangles.\n");
  }
  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  while (triangleloop.tri != NULL) {
    // Check all three vertices of the Triangle.
    for (triangleloop.orient = 0; triangleloop.orient < 3; triangleloop.orient++) {
      org(triangleloop, triorg);
      setvertex2tri(triorg, encode(triangleloop));
    }
    triangleloop.tri = triangletraverse();
  }
}

//***************************************************************************
//
//  preciselocate()   Find a Triangle or edge containing a given point.
//
//  Begins its search from `searchtri'. It is important that `searchtri'
//  be a handle with the property that `searchpoint' is strictly to the left
//  of the edge denoted by `searchtri', or is collinear with that edge and
//  does not intersect that edge. (In particular, `searchpoint' should not
//  be the origin or destination of that edge.)
//
//  These conditions are imposed because preciselocate() is normally used in
//  one of two situations:
//
//  (1)  To try to find the location to insert a new point. Normally, we
//       know an edge that the point is strictly to the left of. In the
//       incremental Delaunay algorithm, that edge is a bounding box edge.
//       In Ruppert's Delaunay refinement algorithm for quality meshing,
//       that edge is the shortest edge of the Triangle whose circumcenter
//       is being inserted.
//
//  (2)  To try to find an existing point. In this case, any edge on the
//       convex hull is a good starting edge. You must screen out the
//       possibility that the Vertex sought is an endpoint of the starting
//       edge before you call preciselocate().
//
//  On completion, `searchtri' is a Triangle that contains `searchpoint'.
//
//  This implementation differs from that given by Guibas and Stolfi. It
//  walks from Triangle to Triangle, crossing an edge only if `searchpoint'
//  is on the other side of the line containing that edge. After entering
//  a Triangle, there are two edges by which one can leave that Triangle.
//  If both edges are valid (`searchpoint' is on the other side of both
//  edges), one of the two is chosen by drawing a line perpendicular to
//  the entry edge (whose endpoints are `forg' and `fdest') passing through
//  `fapex'. Depending on which side of this perpendicular `searchpoint'
//  falls on, an exit edge is chosen.
//
//  This implementation is empirically faster than the Guibas and Stolfi
//  point location routine (which I originally used), which tends to spiral
//  in toward its target.
//
//  Returns ONVERTEX if the point lies on an existing Vertex. `searchtri'
//  is a handle whose origin is the existing Vertex.
//
//  Returns ONEDGE if the point lies on a mesh edge. `searchtri' is a
//  handle whose primary edge is the edge on which the point lies.
//
//  Returns INTRIANGLE if the point lies strictly within a Triangle.
//  `searchtri' is a handle on the Triangle that contains the point.
//
//  Returns OUTSIDE if the point lies outside the mesh. `searchtri' is a
//  handle whose primary edge the point is to the right of. This might
//  occur when the circumcenter of a Triangle falls just slightly outside
//  the mesh due to floating-point roundoff error. It also occurs when
//  seeking a hole or region point that a foolish user has placed outside
//  the mesh.
//
//  If `stopatsubsegment' is nonzero, the search will stop if it tries to
//  walk through a subsegment, and will return OUTSIDE.
//
//  WARNING:  This routine is designed for convex triangulations, and will
//  not generally work after the holes and concavities have been carved.
//  However, it can still be used to find the circumcenter of a Triangle, as
//  long as the search is begun from the Triangle in question.
LocateResult Mesh::preciselocate(Vertex searchpoint, otri *searchtri, int stopatsubsegment) {
  otri backtracktri;
  osub checkedge;
  Vertex forg, fdest, fapex;
  REAL orgorient, destorient;
  int moveleft;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  if (m_b.verbose > 2) {
    _tprintf(_T("  Searching for point %s\n"), vertexToString(searchpoint).cstr());
  }
  // Where are we?
  org(*searchtri, forg);
  dest(*searchtri, fdest);
  apex(*searchtri, fapex);
  while (1) {
    if (m_b.verbose > 2) {
      _tprintf(_T("    At %s\n"), triangleToString(forg,fdest,fapex).cstr());
    }
    // Check whether the apex is the point we seek.
    if ((fapex[0] == searchpoint[0]) && (fapex[1] == searchpoint[1])) {
      lprevself(*searchtri);
      return ONVERTEX;
    }
    // Does the point lie on the other side of the line defined by the
    // Triangle edge opposite the Triangle's destination?
    destorient = counterclockwise(forg, fapex, searchpoint);
    // Does the point lie on the other side of the line defined by the
    // Triangle edge opposite the Triangle's origin?
    orgorient = counterclockwise(fapex, fdest, searchpoint);
    if (destorient > 0.0) {
      if (orgorient > 0.0) {
        // Move left if the inner product of (fapex - searchpoint) and
        // (fdest - forg) is positive. This is equivalent to drawing
        // a line perpendicular to the line (forg, fdest) and passing
        // through `fapex', and determining which side of this line
        // `searchpoint' falls on.
        moveleft = (fapex[0] - searchpoint[0]) * (fdest[0] - forg[0])
                 + (fapex[1] - searchpoint[1]) * (fdest[1] - forg[1]) > 0.0;
      } else {
        moveleft = 1;
      }
    } else {
      if (orgorient > 0.0) {
        moveleft = 0;
      } else {
        // The point we seek must be on the boundary of or inside this Triangle.
        if (destorient == 0.0) {
          lprevself(*searchtri);
          return ONEDGE;
        }
        if (orgorient == 0.0) {
          lnextself(*searchtri);
          return ONEDGE;
        }
        return INTRIANGLE;
      }
    }

    // Move to another Triangle. Leave a trace `backtracktri' in case
    // floating-point roundoff or some such bogey causes us to walk
    // off a boundary of the triangulation.
    if (moveleft) {
      lprev(*searchtri, backtracktri);
      fdest = fapex;
    } else {
      lnext(*searchtri, backtracktri);
      forg = fapex;
    }
    sym(backtracktri, *searchtri);

    if (checksegments && stopatsubsegment) {
      // Check for walking through a subsegment.
      tspivot(backtracktri, checkedge);
      if (checkedge.ss != dummysub) {
        // Go back to the last Triangle.
        otricopy(backtracktri, *searchtri);
        return OUTSIDE;
      }
    }
    // Check for walking right out of the triangulation.
    if (searchtri->tri == dummytri) {
      // Go back to the last Triangle.
      otricopy(backtracktri, *searchtri);
      return OUTSIDE;
    }

    apex(*searchtri, fapex);
  }
}

//***************************************************************************
//
//  locate()   Find a Triangle or edge containing a given point.
//
//  Searching begins from one of:  the input `searchtri', a recently
//  encountered Triangle `recenttri', or from a Triangle chosen from a
//  random sample. The choice is made by determining which Triangle's
//  origin is closest to the point we are searching for. Normally,
//  `searchtri' should be a handle on the convex hull of the triangulation.
//
//  Details on the random sampling method can be found in the Mucke, Saias,
//  and Zhu paper cited in the header of this code.
//
//  On completion, `searchtri' is a Triangle that contains `searchpoint'.
//
//  Returns ONVERTEX if the point lies on an existing Vertex. `searchtri'
//  is a handle whose origin is the existing Vertex.
//
//  Returns ONEDGE if the point lies on a mesh edge. `searchtri' is a
//  handle whose primary edge is the edge on which the point lies.
//
//  Returns INTRIANGLE if the point lies strictly within a Triangle.
//  searchtri' is a handle on the Triangle that contains the point.
//
//  Returns OUTSIDE if the point lies outside the mesh. `searchtri' is a
//  handle whose primary edge the point is to the right of. This might
//  occur when the circumcenter of a Triangle falls just slightly outside
//  the mesh due to floating-point roundoff error. It also occurs when
//  seeking a hole or region point that a foolish user has placed outside
//  the mesh.
//
//  WARNING:  This routine is designed for convex triangulations, and will
//  not generally work after the holes and concavities have been carved.
LocateResult Mesh::locate(Vertex searchpoint, otri *searchtri) {
  VOID **sampleblock;
  char *firsttri;
  otri sampletri;
  Vertex torg, tdest;
  size_t alignptr;
  REAL searchdist, dist;
  REAL ahead;
  long samplesperblock, totalsamplesleft, samplesleft;
  long population, totalpopulation;
  Triangle ptr;                         // Temporary variable used by sym().

  if (m_b.verbose > 2) {
    _tprintf(_T("  Randomly sampling for a Triangle near point %s\n"), vertexToString(searchpoint).cstr());
  }
  // Record the distance from the suggested starting Triangle to the point we seek.
  org(*searchtri, torg);
  searchdist = (searchpoint[0] - torg[0]) * (searchpoint[0] - torg[0])
             + (searchpoint[1] - torg[1]) * (searchpoint[1] - torg[1]);
  if (m_b.verbose > 2) {
    _tprintf(_T("    Boundary Triangle has origin %s\n"), vertexToString(torg).cstr());
  }

  // If a recently encountered Triangle has been recorded and has not been
  // deallocated, test it as a good starting point.
  if (recenttri.tri != NULL) {
    if (!deadtri(recenttri.tri)) {
      org(recenttri, torg);
      if ((torg[0] == searchpoint[0]) && (torg[1] == searchpoint[1])) {
        otricopy(recenttri, *searchtri);
        return ONVERTEX;
      }
      dist = (searchpoint[0] - torg[0]) * (searchpoint[0] - torg[0])
           + (searchpoint[1] - torg[1]) * (searchpoint[1] - torg[1]);
      if (dist < searchdist) {
        otricopy(recenttri, *searchtri);
        searchdist = dist;
        if (m_b.verbose > 2) {
          _tprintf(_T("    Choosing recent Triangle with origin %s\n"), vertexToString(torg).cstr());
        }
      }
    }
  }

  // The number of random samples taken is proportional to the cube root of
  // the number of triangles in the mesh. The next bit of code assumes
  // that the number of triangles increases monotonically (or at least
  // doesn't decrease enough to matter).
  while (SAMPLEFACTOR * samples * samples * samples < triangles.getItems()) {
    samples++;
  }

  // We'll draw ceiling(samples * TRIPERBLOCK / maxitems) random samples
  // from each block of triangles (except the first)--until we meet the
  // sample quota. The ceiling means that blocks at the end might be
  // neglected, but I don't care.
  samplesperblock  = (samples * TRIPERBLOCK - 1) / triangles.getMaxItems() + 1;
  // We'll draw ceiling(samples * itemsfirstblock / maxitems) random samples
  // from the first block of triangles.
  samplesleft      = (samples * triangles.getItemsFirstBlock() - 1) / triangles.getMaxItems() + 1;
  totalsamplesleft = samples;
  population       = triangles.getItemsFirstBlock();
  totalpopulation  = triangles.getMaxItems();
  sampleblock      = triangles.getFirstBlock();
  sampletri.orient = 0;
  while (totalsamplesleft > 0) {
    // If we're in the last block, `population' needs to be corrected.
    if (population > totalpopulation) {
      population = totalpopulation;
    }
    // Find a pointer to the first Triangle in the block.
    alignptr = (size_t)(sampleblock + 1);
    firsttri = (char *)(alignptr + (size_t)triangles.getAlignBytes()
                     - (alignptr % (size_t)triangles.getAlignBytes()));

    // Choose `samplesleft' randomly sampled triangles in this block.
    do {
      sampletri.tri = (Triangle*)(firsttri + (randInt((UINT) population) * triangles.getItemBytes()));
      if (!deadtri(sampletri.tri)) {
        org(sampletri, torg);
        dist = (searchpoint[0] - torg[0]) * (searchpoint[0] - torg[0])
             + (searchpoint[1] - torg[1]) * (searchpoint[1] - torg[1]);
        if (dist < searchdist) {
          otricopy(sampletri, *searchtri);
          searchdist = dist;
          if (m_b.verbose > 2) {
            _tprintf(_T("    Choosing Triangle with origin %s\n"), vertexToString(torg).cstr());
          }
        }
      }

      samplesleft--;
      totalsamplesleft--;
    } while ((samplesleft > 0) && (totalsamplesleft > 0));

    if (totalsamplesleft > 0) {
      sampleblock = (VOID **)*sampleblock;
      samplesleft = samplesperblock;
      totalpopulation -= population;
      population = TRIPERBLOCK;
    }
  }

  // Where are we?
  org(*searchtri, torg);
  dest(*searchtri, tdest);
  // Check the starting Triangle's vertices.
  if ((torg[0] == searchpoint[0]) && (torg[1] == searchpoint[1])) {
    return ONVERTEX;
  }
  if ((tdest[0] == searchpoint[0]) && (tdest[1] == searchpoint[1])) {
    lnextself(*searchtri);
    return ONVERTEX;
  }
  // Orient `searchtri' to fit the preconditions of calling preciselocate().
  ahead = counterclockwise(torg, tdest, searchpoint);
  if (ahead < 0.0) {
    // Turn around so that `searchpoint' is to the left of the edge specified by `searchtri'.
    symself(*searchtri);
  } else if (ahead == 0.0) {
    // Check if `searchpoint' is between `torg' and `tdest'.
    if (((torg[0] < searchpoint[0]) == (searchpoint[0] < tdest[0]))
     && ((torg[1] < searchpoint[1]) == (searchpoint[1] < tdest[1]))) {
      return ONEDGE;
    }
  }
  return preciselocate(searchpoint, searchtri, 0);
}

//******** Point location routines end here

//******** Mesh transformation routines begin here

//  insertsubseg()   Create a new subsegment and insert it between two
//                   triangles.
//
//  The new subsegment is inserted at the edge described by the handle
//  `tri'. Its vertices are properly initialized. The marker `subsegmark'
//  is applied to the subsegment and, if appropriate, its vertices.
void Mesh::insertsubseg(otri *tri, int subsegmark) {
  otri oppotri;
  osub newsubseg;
  Vertex triorg, tridest;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  org(*tri, triorg);
  dest(*tri, tridest);
  // Mark vertices if possible.
  if (vertexmark(triorg) == 0) {
    setvertexmark(triorg, subsegmark);
  }
  if (vertexmark(tridest) == 0) {
    setvertexmark(tridest, subsegmark);
  }
  // Check if there's already a subsegment here.
  tspivot(*tri, newsubseg);
  if (newsubseg.ss == dummysub) {
    // Make new subsegment and initialize its vertices.
    makesubseg(&newsubseg);
    setsorg(newsubseg, tridest);
    setsdest(newsubseg, triorg);
    setsegorg(newsubseg, tridest);
    setsegdest(newsubseg, triorg);
    // Bond new subsegment to the two triangles it is sandwiched between.
    // Note that the facing Triangle `oppotri' might be equal to
    // dummytri' (outer space), but the new subsegment is bonded to it
    // all the same.
    tsbond(*tri, newsubseg);
    sym(*tri, oppotri);
    ssymself(newsubseg);
    tsbond(oppotri, newsubseg);
    setmark(newsubseg, subsegmark);
    if (m_b.verbose > 2) {
      printf("  Inserting new ");
      printsubseg(&newsubseg);
    }
  } else {
    if (mark(newsubseg) == 0) {
      setmark(newsubseg, subsegmark);
    }
  }
}

//***************************************************************************
//
//  Terminology
//
//  A "local transformation" replaces a small set of triangles with another
//  set of triangles. This may or may not involve inserting or deleting a
//  Vertex.
//
//  The term "casing" is used to describe the set of triangles that are
//  attached to the triangles being transformed, but are not transformed
//  themselves. Think of the casing as a fixed hollow structure inside
//  which all the action happens. A "casing" is only defined relative to
//  a single transformation; each occurrence of a transformation will
//  involve a different casing.
//
//***************************************************************************

//***************************************************************************
//
//  flip()   Transform two triangles to two different triangles by flipping
//           an edge counterclockwise within a quadrilateral.
//
//  Imagine the original triangles, abc and bad, oriented so that the
//  shared edge ab lies in a horizontal plane, with the Vertex b on the left
//  and the Vertex a on the right. The Vertex c lies below the edge, and
//  the Vertex d lies above the edge. The `flipedge' handle holds the edge
//  ab of Triangle abc, and is directed left, from Vertex a to Vertex b.
//
//  The triangles abc and bad are deleted and replaced by the triangles cdb
//  and dca. The triangles that represent abc and bad are NOT deallocated;
//  they are reused for dca and cdb, respectively. Hence, any handles that
//  may have held the original triangles are still valid, although not
//  directed as they were before.
//
//  Upon completion of this routine, the `flipedge' handle holds the edge
//  dc of Triangle dca, and is directed down, from Vertex d to Vertex c.
//  (Hence, the two triangles have rotated counterclockwise.)
//
//  WARNING:  This transformation is geometrically valid only if the
//  quadrilateral adbc is convex. Furthermore, this transformation is
//  valid only if there is not a subsegment between the triangles abc and
//  bad. This routine does not check either of these preconditions, and
//  it is the responsibility of the calling routine to ensure that they are
//  met. If they are not, the streets shall be filled with wailing and
//  gnashing of teeth.
//
//***************************************************************************

void Mesh::flip(otri *flipedge) {
  otri botleft, botright;
  otri topleft, topright;
  otri top;
  otri botlcasing, botrcasing;
  otri toplcasing, toprcasing;
  osub botlsubseg, botrsubseg;
  osub toplsubseg, toprsubseg;
  Vertex leftvertex, rightvertex, botvertex;
  Vertex farvertex;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  // Identify the vertices of the quadrilateral.
  org(*flipedge, rightvertex);
  dest(*flipedge, leftvertex);
  apex(*flipedge, botvertex);
  sym(*flipedge, top);
#ifdef SELF_CHECK
  if (top.tri == dummytri) {
    printf("Internal error in flip():  Attempt to flip on boundary.\n");
    lnextself(*flipedge);
    return;
  }
  if (checksegments) {
    tspivot(*flipedge, toplsubseg);
    if (toplsubseg.ss != dummysub) {
      printf("Internal error in flip():  Attempt to flip a segment.\n");
      lnextself(*flipedge);
      return;
    }
  }
#endif // SELF_CHECK
  apex(top, farvertex);

  // Identify the casing of the quadrilateral.
  lprev(top, topleft);
  sym(topleft, toplcasing);
  lnext(top, topright);
  sym(topright, toprcasing);
  lnext(*flipedge, botleft);
  sym(botleft, botlcasing);
  lprev(*flipedge, botright);
  sym(botright, botrcasing);
  // Rotate the quadrilateral one-quarter turn counterclockwise.
  bond(topleft, botlcasing);
  bond(botleft, botrcasing);
  bond(botright, toprcasing);
  bond(topright, toplcasing);

  if (checksegments) {
    // Check for subsegments and rebond them to the quadrilateral.
    tspivot(topleft, toplsubseg);
    tspivot(botleft, botlsubseg);
    tspivot(botright, botrsubseg);
    tspivot(topright, toprsubseg);
    if (toplsubseg.ss == dummysub) {
      tsdissolve(topright);
    } else {
      tsbond(topright, toplsubseg);
    }
    if (botlsubseg.ss == dummysub) {
      tsdissolve(topleft);
    } else {
      tsbond(topleft, botlsubseg);
    }
    if (botrsubseg.ss == dummysub) {
      tsdissolve(botleft);
    } else {
      tsbond(botleft, botrsubseg);
    }
    if (toprsubseg.ss == dummysub) {
      tsdissolve(botright);
    } else {
      tsbond(botright, toprsubseg);
    }
  }

  // New Vertex assignments for the rotated quadrilateral.
  setorg(*flipedge, farvertex);
  setdest(*flipedge, botvertex);
  setapex(*flipedge, rightvertex);
  setorg(top, botvertex);
  setdest(top, farvertex);
  setapex(top, leftvertex);
  if (m_b.verbose > 2) {
    printf("  Edge flip results in left ");
    printtriangle(&top);
    printf("  and right ");
    printtriangle(flipedge);
  }
}

//***************************************************************************
//
//  unflip()   Transform two triangles to two different triangles by
//             flipping an edge clockwise within a quadrilateral. Reverses
//             the flip() operation so that the data structures representing
//             the triangles are back where they were before the flip().
//
//  Imagine the original triangles, abc and bad, oriented so that the
//  shared edge ab lies in a horizontal plane, with the Vertex b on the left
//  and the Vertex a on the right. The Vertex c lies below the edge, and
//  the Vertex d lies above the edge. The `flipedge' handle holds the edge
//  ab of Triangle abc, and is directed left, from Vertex a to Vertex b.
//
//  The triangles abc and bad are deleted and replaced by the triangles cdb
//  and dca. The triangles that represent abc and bad are NOT deallocated;
//  they are reused for cdb and dca, respectively. Hence, any handles that
//  may have held the original triangles are still valid, although not
//  directed as they were before.
//
//  Upon completion of this routine, the `flipedge' handle holds the edge
//  cd of Triangle cdb, and is directed up, from Vertex c to Vertex d.
//  (Hence, the two triangles have rotated clockwise.)
//
//  WARNING:  This transformation is geometrically valid only if the
//  quadrilateral adbc is convex. Furthermore, this transformation is
//  valid only if there is not a subsegment between the triangles abc and
//  bad. This routine does not check either of these preconditions, and
//  it is the responsibility of the calling routine to ensure that they are
//  met. If they are not, the streets shall be filled with wailing and
//  gnashing of teeth.
//
//***************************************************************************
void Mesh::unflip(otri *flipedge) {
  otri botleft, botright;
  otri topleft, topright;
  otri top;
  otri botlcasing, botrcasing;
  otri toplcasing, toprcasing;
  osub botlsubseg, botrsubseg;
  osub toplsubseg, toprsubseg;
  Vertex leftvertex, rightvertex, botvertex;
  Vertex farvertex;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  // Identify the vertices of the quadrilateral.
  org(*flipedge, rightvertex);
  dest(*flipedge, leftvertex);
  apex(*flipedge, botvertex);
  sym(*flipedge, top);
#ifdef SELF_CHECK
  if (top.tri == dummytri) {
    printf("Internal error in unflip():  Attempt to flip on boundary.\n");
    lnextself(*flipedge);
    return;
  }
  if (checksegments) {
    tspivot(*flipedge, toplsubseg);
    if (toplsubseg.ss != dummysub) {
      printf("Internal error in unflip():  Attempt to flip a subsegment.\n");
      lnextself(*flipedge);
      return;
    }
  }
#endif // SELF_CHECK
  apex(top, farvertex);

  // Identify the casing of the quadrilateral.
  lprev(top, topleft);
  sym(topleft, toplcasing);
  lnext(top, topright);
  sym(topright, toprcasing);
  lnext(*flipedge, botleft);
  sym(botleft, botlcasing);
  lprev(*flipedge, botright);
  sym(botright, botrcasing);
  // Rotate the quadrilateral one-quarter turn clockwise.
  bond(topleft, toprcasing);
  bond(botleft, toplcasing);
  bond(botright, botlcasing);
  bond(topright, botrcasing);

  if (checksegments) {
    // Check for subsegments and rebond them to the quadrilateral.
    tspivot(topleft, toplsubseg);
    tspivot(botleft, botlsubseg);
    tspivot(botright, botrsubseg);
    tspivot(topright, toprsubseg);
    if (toplsubseg.ss == dummysub) {
      tsdissolve(botleft);
    } else {
      tsbond(botleft, toplsubseg);
    }
    if (botlsubseg.ss == dummysub) {
      tsdissolve(botright);
    } else {
      tsbond(botright, botlsubseg);
    }
    if (botrsubseg.ss == dummysub) {
      tsdissolve(topright);
    } else {
      tsbond(topright, botrsubseg);
    }
    if (toprsubseg.ss == dummysub) {
      tsdissolve(topleft);
    } else {
      tsbond(topleft, toprsubseg);
    }
  }

  // New Vertex assignments for the rotated quadrilateral.
  setorg(*flipedge, botvertex);
  setdest(*flipedge, farvertex);
  setapex(*flipedge, leftvertex);
  setorg(top, farvertex);
  setdest(top, botvertex);
  setapex(top, rightvertex);
  if (m_b.verbose > 2) {
    printf("  Edge unflip results in left ");
    printtriangle(flipedge);
    printf("  and right ");
    printtriangle(&top);
  }
}

//***************************************************************************
//
//  insertvertex()   Insert a Vertex into a Delaunay triangulation,
//                   performing flips as necessary to maintain the Delaunay
//                   property.
//
//  The point `insertvertex' is located. If `searchtri.tri' is not NULL,
//  the search for the containing Triangle begins from `searchtri'. If
//  `searchtri.tri' is NULL, a full point location procedure is called.
//  If `insertvertex' is found inside a Triangle, the Triangle is split into
//  three; if `insertvertex' lies on an edge, the edge is split in two,
//  thereby splitting the two adjacent triangles into four. Edge flips are
//  used to restore the Delaunay property. If `insertvertex' lies on an
//  existing Vertex, no action is taken, and the value DUPLICATEVERTEX is
//  returned. On return, `searchtri' is set to a handle whose origin is the
//  existing Vertex.
//
//  Normally, the parameter `splitseg' is set to NULL, implying that no
//  subsegment should be split. In this case, if `insertvertex' is found to
//  lie on a segment, no action is taken, and the value VIOLATINGVERTEX is
//  returned. On return, `searchtri' is set to a handle whose primary edge
//  is the violated subsegment.
//
//  If the calling routine wishes to split a subsegment by inserting a
//  Vertex in it, the parameter `splitseg' should be that subsegment. In
//  this case, `searchtri' MUST be the Triangle handle reached by pivoting
//  from that subsegment; no point location is done.
//
//  `segmentflaws' and `triflaws' are flags that indicate whether or not
//  there should be checks for the creation of encroached subsegments or bad
//  quality triangles. If a newly inserted Vertex encroaches upon
//  subsegments, these subsegments are added to the list of subsegments to
//  be split if `segmentflaws' is set. If bad triangles are created, these
//  are added to the queue if `triflaws' is set.
//
//  If a duplicate Vertex or violated segment does not prevent the Vertex
//  from being inserted, the return value will be ENCROACHINGVERTEX if the
//  Vertex encroaches upon a subsegment (and checking is enabled), or
//  SUCCESSFULVERTEX otherwise. In either case, `searchtri' is set to a
//  handle whose origin is the newly inserted Vertex.
//
//  insertvertex() does not use flip() for reasons of speed; some
//  information can be reused from edge flip to edge flip, like the
//  locations of subsegments.
//
//***************************************************************************
InserVvertexResult Mesh::insertvertex(Vertex    newvertex
                                     ,otri     *searchtri
                                     ,osub     *splitseg
                                     ,int       segmentflaws
                                     ,int       triflaws)
{
  otri horiz;
  otri top;
  otri botleft, botright;
  otri topleft, topright;
  otri newbotleft, newbotright;
  otri newtopright;
  otri botlcasing, botrcasing;
  otri toplcasing, toprcasing;
  otri testtri;
  osub botlsubseg, botrsubseg;
  osub toplsubseg, toprsubseg;
  osub brokensubseg;
  osub checksubseg;
  osub rightsubseg;
  osub newsubseg;
  BadSubSeg *encroached;
  FlipStacker *newflip;
  Vertex first;
  Vertex leftvertex, rightvertex, botvertex, topvertex, farvertex;
  Vertex segmentorg, segmentdest;
  REAL attrib;
  REAL area;
  InserVvertexResult success;
  LocateResult       intersect;
  int doflip;
  int mirrorflag;
  int enq;
  int i;
  Triangle ptr;        // Temporary variable used by sym().
  SubSeg sptr;         // Temporary variable used by spivot() and tspivot().

  if (m_b.verbose > 1) {
    _tprintf(_T("  Inserting %s\n"), vertexToString(newvertex).cstr());
  }

  if (splitseg == NULL) {
    // Find the location of the Vertex to be inserted. Check if a good
    // starting Triangle has already been provided by the caller.
    if (searchtri->tri == dummytri) {
      // Find a boundary Triangle.
      horiz.tri = dummytri;
      horiz.orient = 0;
      symself(horiz);
      // Search for a Triangle containing `newvertex'.
      intersect = locate(newvertex, &horiz);
    } else {
      // Start searching from the Triangle provided by the caller.
      otricopy(*searchtri, horiz);
      intersect = preciselocate(newvertex, &horiz, 1);
    }
  } else {
    // The calling routine provides the subsegment in which the Vertex is inserted.
    otricopy(*searchtri, horiz);
    intersect = ONEDGE;
  }

  if (intersect == ONVERTEX) {
    // There's already a Vertex there. Return in `searchtri' a Triangle
    // whose origin is the existing Vertex.
    otricopy(horiz, *searchtri);
    otricopy(horiz, recenttri);
    return DUPLICATEVERTEX;
  }
  if ((intersect == ONEDGE) || (intersect == OUTSIDE)) {
    // The Vertex falls on an edge or boundary.
    if (checksegments && (splitseg == NULL)) {
      // Check whether the Vertex falls on a subsegment.
      tspivot(horiz, brokensubseg);
      if (brokensubseg.ss != dummysub) {
        // The Vertex falls on a subsegment, and hence will not be inserted.
        if (segmentflaws) {
          enq = m_b.nobisect != 2;
          if (enq && (m_b.nobisect == 1)) {
            // This subsegment may be split only if it is an internal boundary.
            sym(horiz, testtri);
            enq = testtri.tri != dummytri;
          }
          if (enq) {
            // Add the subsegment to the list of encroached subsegments.
            encroached = badsubsegs.alloc();
            encroached->encsubseg = sencode(brokensubseg);
            sorg(brokensubseg, encroached->subsegorg);
            sdest(brokensubseg, encroached->subsegdest);
            if (m_b.verbose > 2) {
              _tprintf(_T("  Queueing encroached subsegment %s\n")
                      ,edgeToString(encroached->subsegorg, encroached->subsegdest).cstr());
            }
          }
        }
        // Return a handle whose primary edge contains the Vertex,which has not been inserted.
        otricopy(horiz, *searchtri);
        otricopy(horiz, recenttri);
        return VIOLATINGVERTEX;
      }
    }

    // Insert the Vertex on an edge, dividing one Triangle into two (if
    // the edge lies on a boundary) or two triangles into four.
    lprev(horiz, botright);
    sym(botright, botrcasing);
    sym(horiz, topright);
    // Is there a second Triangle?  (Or does this edge lie on a boundary?)
    mirrorflag = topright.tri != dummytri;
    if (mirrorflag) {
      lnextself(topright);
      sym(topright, toprcasing);
      maketriangle(&newtopright);
    } else {
      // Splitting a boundary edge increases the number of boundary edges.
      hullsize++;
    }
    maketriangle(&newbotright);

    // Set the vertices of changed and new triangles.
    org(horiz, rightvertex);
    dest(horiz, leftvertex);
    apex(horiz, botvertex);
    setorg(newbotright, botvertex);
    setdest(newbotright, rightvertex);
    setapex(newbotright, newvertex);
    setorg(horiz, newvertex);
    for (i = 0; i < eextras; i++) {
      // Set the element attributes of a new Triangle.
      setelemattribute(newbotright, i, elemattribute(botright, i));
    }
    if (m_b.vararea) {
      // Set the area constraint of a new Triangle.
      setareabound(newbotright, areabound(botright));
    }
    if (mirrorflag) {
      dest(topright, topvertex);
      setorg(newtopright, rightvertex);
      setdest(newtopright, topvertex);
      setapex(newtopright, newvertex);
      setorg(topright, newvertex);
      for (i = 0; i < eextras; i++) {
        // Set the element attributes of another new Triangle.
        setelemattribute(newtopright, i, elemattribute(topright, i));
      }
      if (m_b.vararea) {
        // Set the area constraint of another new Triangle.
        setareabound(newtopright, areabound(topright));
      }
    }

    // There may be subsegments that need to be bonded to the new Triangle(s).
    if (checksegments) {
      tspivot(botright, botrsubseg);
      if (botrsubseg.ss != dummysub) {
        tsdissolve(botright);
        tsbond(newbotright, botrsubseg);
      }
      if (mirrorflag) {
        tspivot(topright, toprsubseg);
        if (toprsubseg.ss != dummysub) {
          tsdissolve(topright);
          tsbond(newtopright, toprsubseg);
        }
      }
    }

    // Bond the new Triangle(s) to the surrounding triangles.
    bond(newbotright, botrcasing);
    lprevself(newbotright);
    bond(newbotright, botright);
    lprevself(newbotright);
    if (mirrorflag) {
      bond(newtopright, toprcasing);
      lnextself(newtopright);
      bond(newtopright, topright);
      lnextself(newtopright);
      bond(newtopright, newbotright);
    }

    if (splitseg != NULL) {
      // Split the subsegment into two.
      setsdest(*splitseg, newvertex);
      segorg(*splitseg, segmentorg);
      segdest(*splitseg, segmentdest);
      ssymself(*splitseg);
      spivot(*splitseg, rightsubseg);
      insertsubseg(&newbotright, mark(*splitseg));
      tspivot(newbotright, newsubseg);
      setsegorg(newsubseg, segmentorg);
      setsegdest(newsubseg, segmentdest);
      sbond(*splitseg, newsubseg);
      ssymself(newsubseg);
      sbond(newsubseg, rightsubseg);
      ssymself(*splitseg);
      // Transfer the subsegment's boundary marker to the Vertex if required.
      if (vertexmark(newvertex) == 0) {
        setvertexmark(newvertex, mark(*splitseg));
      }
    }

    if (checkquality) {
      flipstackers.restart();
      lastflip = flipstackers.alloc();
      lastflip->flippedtri = encode(horiz);
      lastflip->prevflip = FLIPSTACKER_MERGE4;
    }

#ifdef SELF_CHECK
    if (counterclockwise(rightvertex, leftvertex, botvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise Triangle prior to edge Vertex insertion (bottom).\n");
    }
    if (mirrorflag) {
      if (counterclockwise(leftvertex, rightvertex, topvertex) < 0.0) {
        printf("Internal error in insertvertex():\n");
        printf("  Clockwise Triangle prior to edge Vertex insertion (top).\n");
      }
      if (counterclockwise(rightvertex, topvertex, newvertex) < 0.0) {
        printf("Internal error in insertvertex():\n");
        printf("  Clockwise Triangle after edge Vertex insertion (top right).\n");
      }
      if (counterclockwise(topvertex, leftvertex, newvertex) < 0.0) {
        printf("Internal error in insertvertex():\n");
        printf("  Clockwise Triangle after edge Vertex insertion (top left).\n");
      }
    }
    if (counterclockwise(leftvertex, botvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise Triangle after edge Vertex insertion (bottom left).\n");
    }
    if (counterclockwise(botvertex, rightvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise Triangle after edge Vertex insertion (bottom right).\n");
    }
#endif // SELF_CHECK
    if (m_b.verbose > 2) {
      printf("  Updating bottom left ");
      printtriangle(&botright);
      if (mirrorflag) {
        printf("  Updating top left ");
        printtriangle(&topright);
        printf("  Creating top right ");
        printtriangle(&newtopright);
      }
      printf("  Creating bottom right ");
      printtriangle(&newbotright);
    }

    // Position `horiz' on the first edge to check for the Delaunay property.
    lnextself(horiz);
  } else {
    // Insert the Vertex in a Triangle, splitting it into three.
    lnext(horiz, botleft);
    lprev(horiz, botright);
    sym(botleft, botlcasing);
    sym(botright, botrcasing);
    maketriangle(&newbotleft);
    maketriangle(&newbotright);

    // Set the vertices of changed and new triangles.
    org(horiz, rightvertex);
    dest(horiz, leftvertex);
    apex(horiz, botvertex);
    setorg(newbotleft, leftvertex);
    setdest(newbotleft, botvertex);
    setapex(newbotleft, newvertex);
    setorg(newbotright, botvertex);
    setdest(newbotright, rightvertex);
    setapex(newbotright, newvertex);
    setapex(horiz, newvertex);
    for (i = 0; i < eextras; i++) {
      // Set the element attributes of the new triangles.
      attrib = elemattribute(horiz, i);
      setelemattribute(newbotleft, i, attrib);
      setelemattribute(newbotright, i, attrib);
    }
    if (m_b.vararea) {
      // Set the area constraint of the new triangles.
      area = areabound(horiz);
      setareabound(newbotleft, area);
      setareabound(newbotright, area);
    }

    // There may be subsegments that need to be bonded to the new triangles.
    if (checksegments) {
      tspivot(botleft, botlsubseg);
      if (botlsubseg.ss != dummysub) {
        tsdissolve(botleft);
        tsbond(newbotleft, botlsubseg);
      }
      tspivot(botright, botrsubseg);
      if (botrsubseg.ss != dummysub) {
        tsdissolve(botright);
        tsbond(newbotright, botrsubseg);
      }
    }

    // Bond the new triangles to the surrounding triangles.
    bond(newbotleft, botlcasing);
    bond(newbotright, botrcasing);
    lnextself(newbotleft);
    lprevself(newbotright);
    bond(newbotleft, newbotright);
    lnextself(newbotleft);
    bond(botleft, newbotleft);
    lprevself(newbotright);
    bond(botright, newbotright);

    if (checkquality) {
      flipstackers.restart();
      lastflip = flipstackers.alloc();
      lastflip->flippedtri = encode(horiz);
      lastflip->prevflip = NULL;
    }

#ifdef SELF_CHECK
    if (counterclockwise(rightvertex, leftvertex, botvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise Triangle prior to Vertex insertion.\n");
    }
    if (counterclockwise(rightvertex, leftvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise Triangle after Vertex insertion (top).\n");
    }
    if (counterclockwise(leftvertex, botvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise Triangle after Vertex insertion (left).\n");
    }
    if (counterclockwise(botvertex, rightvertex, newvertex) < 0.0) {
      printf("Internal error in insertvertex():\n");
      printf("  Clockwise Triangle after Vertex insertion (right).\n");
    }
#endif // SELF_CHECK
    if (m_b.verbose > 2) {
      printf("  Updating top ");
      printtriangle(&horiz);
      printf("  Creating left ");
      printtriangle(&newbotleft);
      printf("  Creating right ");
      printtriangle(&newbotright);
    }
  }

  // The insertion is successful by default, unless an encroached subsegment is found.
  success = SUCCESSFULVERTEX;
  // Circle around the newly inserted Vertex, checking each edge opposite
  // it for the Delaunay property. Non-Delaunay edges are flipped.
  // horiz' is always the edge being checked. `first' marks where to
  // stop circling.
  org(horiz, first);
  rightvertex = first;
  dest(horiz, leftvertex);
  // Circle until finished.
  while (1) {
    // By default, the edge will be flipped.
    doflip = 1;

    if (checksegments) {
      // Check for a subsegment, which cannot be flipped.
      tspivot(horiz, checksubseg);
      if (checksubseg.ss != dummysub) {
        // The edge is a subsegment and cannot be flipped.
        doflip = 0;
#ifndef CDT_ONLY
        if (segmentflaws) {
          // Does the new Vertex encroach upon this subsegment?
          if (checkseg4encroach(&checksubseg)) {
            success = ENCROACHINGVERTEX;
          }
        }
#endif // not CDT_ONLY
      }
    }

    if (doflip) {
      // Check if the edge is a boundary edge.
      sym(horiz, top);
      if (top.tri == dummytri) {
        // The edge is a boundary edge and cannot be flipped.
        doflip = 0;
      } else {
        // Find the Vertex on the other side of the edge.
        apex(top, farvertex);
        // In the incremental Delaunay triangulation algorithm, any of
        // `leftvertex', `rightvertex', and `farvertex' could be vertices
        // of the triangular bounding box. These vertices must be
        // treated as if they are infinitely distant, even though their
        // "coordinates" are not.
        if ((leftvertex == infvertex1) || (leftvertex == infvertex2) ||
            (leftvertex == infvertex3)) {
          // leftvertex' is infinitely distant. Check the convexity of
          // the boundary of the triangulation. 'farvertex' might be
          // infinite as well, but trust me, this same condition should
          // be applied.
          doflip = counterclockwise(newvertex, rightvertex, farvertex) > 0.0;
        } else if ((rightvertex == infvertex1) ||
                   (rightvertex == infvertex2) ||
                   (rightvertex == infvertex3)) {
          // `rightvertex' is infinitely distant. Check the convexity of
          // the boundary of the triangulation. 'farvertex' might be
          // infinite as well, but trust me, this same condition should
          // be applied.
          doflip = counterclockwise(farvertex, leftvertex, newvertex) > 0.0;
        } else if ((farvertex == infvertex1) ||
                   (farvertex == infvertex2) ||
                   (farvertex == infvertex3)) {
          // `farvertex' is infinitely distant and cannot be inside
          // the circumcircle of the Triangle `horiz'.
          doflip = 0;
        } else {
          // Test whether the edge is locally Delaunay.
          doflip = incircle(leftvertex, newvertex, rightvertex, farvertex) > 0.0;
        }
        if (doflip) {
          // We made it!  Flip the edge `horiz' by rotating its containing
          // quadrilateral (the two triangles adjacent to `horiz').
          // Identify the casing of the quadrilateral.
          lprev(top, topleft);
          sym(topleft, toplcasing);
          lnext(top, topright);
          sym(topright, toprcasing);
          lnext(horiz, botleft);
          sym(botleft, botlcasing);
          lprev(horiz, botright);
          sym(botright, botrcasing);
          // Rotate the quadrilateral one-quarter turn counterclockwise.
          bond(topleft, botlcasing);
          bond(botleft, botrcasing);
          bond(botright, toprcasing);
          bond(topright, toplcasing);
          if (checksegments) {
            // Check for subsegments and rebond them to the quadrilateral.
            tspivot(topleft, toplsubseg);
            tspivot(botleft, botlsubseg);
            tspivot(botright, botrsubseg);
            tspivot(topright, toprsubseg);
            if (toplsubseg.ss == dummysub) {
              tsdissolve(topright);
            } else {
              tsbond(topright, toplsubseg);
            }
            if (botlsubseg.ss == dummysub) {
              tsdissolve(topleft);
            } else {
              tsbond(topleft, botlsubseg);
            }
            if (botrsubseg.ss == dummysub) {
              tsdissolve(botleft);
            } else {
              tsbond(botleft, botrsubseg);
            }
            if (toprsubseg.ss == dummysub) {
              tsdissolve(botright);
            } else {
              tsbond(botright, toprsubseg);
            }
          }
          // New Vertex assignments for the rotated quadrilateral.
          setorg(horiz, farvertex);
          setdest(horiz, newvertex);
          setapex(horiz, rightvertex);
          setorg(top, newvertex);
          setdest(top, farvertex);
          setapex(top, leftvertex);
          for (i = 0; i < eextras; i++) {
            // Take the average of the two triangles' attributes.
            attrib = 0.5 * (elemattribute(top, i) + elemattribute(horiz, i));
            setelemattribute(top, i, attrib);
            setelemattribute(horiz, i, attrib);
          }
          if (m_b.vararea) {
            if ((areabound(top) <= 0.0) || (areabound(horiz) <= 0.0)) {
              area = -1.0;
            } else {
              // Take the average of the two triangles' area constraints.
              // This prevents small area constraints from migrating a
              // long, long way from their original location due to flips.
              area = 0.5 * (areabound(top) + areabound(horiz));
            }
            setareabound(top, area);
            setareabound(horiz, area);
          }

          if (checkquality) {
            newflip = flipstackers.alloc();
            newflip->flippedtri = encode(horiz);
            newflip->prevflip = lastflip;
            lastflip = newflip;
          }

#ifdef SELF_CHECK
          if (newvertex != NULL) {
            if (counterclockwise(leftvertex, newvertex, rightvertex) < 0.0) {
              printf("Internal error in insertvertex():\n");
              printf("  Clockwise Triangle prior to edge flip (bottom).\n");
            }
            // The following test has been removed because constrainededge()
            // sometimes generates inverted triangles that insertvertex()
            // removes.
//
            if (counterclockwise(rightvertex, farvertex, leftvertex) < 0.0) {
              printf("Internal error in insertvertex():\n");
              printf("  Clockwise Triangle prior to edge flip (top).\n");
            }

            if (counterclockwise(farvertex, leftvertex, newvertex) < 0.0) {
              printf("Internal error in insertvertex():\n");
              printf("  Clockwise Triangle after edge flip (left).\n");
            }
            if (counterclockwise(newvertex, rightvertex, farvertex) < 0.0) {
              printf("Internal error in insertvertex():\n");
              printf("  Clockwise Triangle after edge flip (right).\n");
            }
          }
#endif // SELF_CHECK
          if (m_b.verbose > 2) {
            printf("  Edge flip results in left ");
            lnextself(topleft);
            printtriangle(&topleft);
            printf("  and right ");
            printtriangle(&horiz);
          }
          // On the next iterations, consider the two edges that were
          // exposed (this is, are now visible to the newly inserted
          // Vertex) by the edge flip.
          lprevself(horiz);
          leftvertex = farvertex;
        }
      }
    }
    if (!doflip) {
      // The handle `horiz' is accepted as locally Delaunay.
#ifndef CDT_ONLY
      if (triflaws) {
        // Check the Triangle `horiz' for quality.
        testtriangle(&horiz);
      }
#endif // not CDT_ONLY
      // Look for the next edge around the newly inserted Vertex.
      lnextself(horiz);
      sym(horiz, testtri);
      // Check for finishing a complete revolution about the new Vertex, or
      // falling outside  of the triangulation. The latter will happen
      // when a Vertex is inserted at a boundary.
      if ((leftvertex == first) || (testtri.tri == dummytri)) {
        // We're done. Return a Triangle whose origin is the new Vertex.
        lnext(horiz, *searchtri);
        lnext(horiz, recenttri);
        return success;
      }
      // Finish finding the next edge around the newly inserted Vertex.
      lnext(testtri, horiz);
      rightvertex = leftvertex;
      dest(horiz, leftvertex);
    }
  }
}

//***************************************************************************
//
//  triangulatepolygon()   Find the Delaunay triangulation of a polygon that
//                         has a certain "nice" shape. This includes the
//                         polygons that result from deletion of a Vertex or
//                         insertion of a segment.
//
//  This is a conceptually difficult routine. The starting assumption is
//  that we have a polygon with n sides. n - 1 of these sides are currently
//  represented as edges in the mesh. One side, called the "base", need not
//  be.
//
//  Inside the polygon is a structure I call a "fan", consisting of n - 1
//  triangles that share a common origin. For each of these triangles, the
//  edge opposite the origin is one of the sides of the polygon. The
//  primary edge of each Triangle is the edge directed from the origin to
//  the destination; note that this is not the same edge that is a side of
//  the polygon. `firstedge' is the primary edge of the first Triangle.
//  From there, the triangles follow in counterclockwise order about the
//  polygon, until `lastedge', the primary edge of the last Triangle.
//  `firstedge' and `lastedge' are probably connected to other triangles
//  beyond the extremes of the fan, but their identity is not important, as
//  long as the fan remains connected to them.
//
//  Imagine the polygon oriented so that its base is at the bottom. This
//  puts `firstedge' on the far right, and `lastedge' on the far left.
//  The right Vertex of the base is the destination of `firstedge', and the
//  left Vertex of the base is the apex of `lastedge'.
//
//  The challenge now is to find the right sequence of edge flips to
//  transform the fan into a Delaunay triangulation of the polygon. Each
//  edge flip effectively removes one Triangle from the fan, committing it
//  to the polygon. The resulting polygon has one fewer edge. If `doflip'
//  is set, the final flip will be performed, resulting in a fan of one
//  (useless?) Triangle. If `doflip' is not set, the final flip is not
//  performed, resulting in a fan of two triangles, and an unfinished
//  triangular polygon that is not yet filled out with a single Triangle.
//  On completion of the routine, `lastedge' is the last remaining Triangle,
//  or the leftmost of the last two.
//
//  Although the flips are performed in the order described above, the
//  decisions about what flips to perform are made in precisely the reverse
//  order. The recursive triangulatepolygon() procedure makes a decision,
//  uses up to two recursive calls to triangulate the "subproblems"
//  (polygons with fewer edges), and then performs an edge flip.
//
//  The "decision" it makes is which Vertex of the polygon should be
//  connected to the base. This decision is made by testing every possible
//  Vertex. Once the best Vertex is found, the two edges that connect this
//  Vertex to the base become the bases for two smaller polygons. These
//  are triangulated recursively. Unfortunately, this approach can take
//  O(n^2) time not only in the worst case, but in many common cases. It's
//  rarely a big deal for Vertex deletion, where n is rarely larger than
//  ten, but it could be a big deal for segment insertion, especially if
//  there's a lot of long segments that each cut many triangles. I ought to
//  code a faster algorithm some day.
//
//  The `edgecount' parameter is the number of sides of the polygon,
//  including its base. `triflaws' is a flag that determines whether the
//  new triangles should be tested for quality, and enqueued if they are
//  bad.
//
//***************************************************************************
void Mesh::triangulatepolygon(otri     *firstedge
                             ,otri     *lastedge
                             ,int       edgecount
                             ,int       doflip
                             ,int       triflaws)
{
  otri testtri;
  otri besttri;
  otri tempedge;
  Vertex leftbasevertex, rightbasevertex;
  Vertex testvertex;
  Vertex bestvertex;
  int bestnumber;
  int i;
  Triangle ptr;   // Temporary variable used by sym(), onext(), and oprev().

  // Identify the base vertices.
  apex(*lastedge, leftbasevertex);
  dest(*firstedge, rightbasevertex);
  if (m_b.verbose > 2) {
    _tprintf(_T("  Triangulating interior polygon at edge %s\n")
            ,edgeToString(leftbasevertex, rightbasevertex).cstr());
  }
  // Find the best Vertex to connect the base to.
  onext(*firstedge, besttri);
  dest(besttri, bestvertex);
  otricopy(besttri, testtri);
  bestnumber = 1;
  for (i = 2; i <= edgecount - 2; i++) {
    onextself(testtri);
    dest(testtri, testvertex);
    // Is this a better Vertex?
    if (incircle(leftbasevertex, rightbasevertex, bestvertex,
                 testvertex) > 0.0) {
      otricopy(testtri, besttri);
      bestvertex = testvertex;
      bestnumber = i;
    }
  }
  if (m_b.verbose > 2) {
    _tprintf(_T("    Connecting edge to %s\n"), vertexToString(bestvertex).cstr());
  }
  if (bestnumber > 1) {
    // Recursively triangulate the smaller polygon on the right.
    oprev(besttri, tempedge);
    triangulatepolygon(firstedge, &tempedge, bestnumber + 1, 1, triflaws);
  }
  if (bestnumber < edgecount - 2) {
    // Recursively triangulate the smaller polygon on the left.
    sym(besttri, tempedge);
    triangulatepolygon(&besttri, lastedge, edgecount - bestnumber, 1,
                       triflaws);
    // Find `besttri' again; it may have been lost to edge flips.
    sym(tempedge, besttri);
  }
  if (doflip) {
    // Do one final edge flip.
    flip(&besttri);
#ifndef CDT_ONLY
    if (triflaws) {
      // Check the quality of the newly committed Triangle.
      sym(besttri, testtri);
      testtriangle(&testtri);
    }
#endif // not CDT_ONLY
  }
  // Return the base Triangle.
  otricopy(besttri, *lastedge);
}

#ifndef CDT_ONLY
//***************************************************************************
//
//  deletevertex()   Delete a Vertex from a Delaunay triangulation, ensuring
//                   that the triangulation remains Delaunay.
//
//  The origin of `deltri' is deleted. The union of the triangles adjacent
//  to this Vertex is a polygon, for which the Delaunay triangulation is
//  found. Two triangles are removed from the mesh.
//
//  Only interior vertices that do not lie on segments or boundaries may be
//  deleted.
//
//***************************************************************************
void Mesh::deletevertex(otri *deltri) {
  otri countingtri;
  otri firstedge, lastedge;
  otri deltriright;
  otri lefttri, righttri;
  otri leftcasing, rightcasing;
  osub leftsubseg, rightsubseg;
  Vertex delvertex;
  Vertex neworg;
  int edgecount;
  Triangle ptr;   // Temporary variable used by sym(), onext(), and oprev().
  SubSeg sptr;    // Temporary variable used by tspivot().

  org(*deltri, delvertex);
  if (m_b.verbose > 1) {
    _tprintf(_T("  Deleting %s\n"), vertexToString(delvertex).cstr());
  }
  vertexdealloc(delvertex);

  // Count the degree of the Vertex being deleted.
  onext(*deltri, countingtri);
  edgecount = 1;
  while (!otriequal(*deltri, countingtri)) {
#ifdef SELF_CHECK
    if (countingtri.tri == dummytri) {
      internalerror(__TFUNCTION__, _T("Attempt to delete boundary Vertex"));
    }
#endif // SELF_CHECK
    edgecount++;
    onextself(countingtri);
  }

#ifdef SELF_CHECK
  if (edgecount < 3) {
    internalerror(__TFUNCTION__, _T("Vertex has degree %d"), edgecount);
  }
#endif // SELF_CHECK
  if (edgecount > 3) {
    // Triangulate the polygon defined by the union of all triangles
    // adjacent to the Vertex being deleted. Check the quality of the resulting triangles.
    onext(*deltri, firstedge);
    oprev(*deltri, lastedge);
    triangulatepolygon(&firstedge, &lastedge, edgecount, 0, !m_b.nobisect);
  }
  // Splice out two triangles.
  lprev(*deltri, deltriright);
  dnext(*deltri, lefttri);
  sym(lefttri, leftcasing);
  oprev(deltriright, righttri);
  sym(righttri, rightcasing);
  bond(*deltri, leftcasing);
  bond(deltriright, rightcasing);
  tspivot(lefttri, leftsubseg);
  if (leftsubseg.ss != dummysub) {
    tsbond(*deltri, leftsubseg);
  }
  tspivot(righttri, rightsubseg);
  if (rightsubseg.ss != dummysub) {
    tsbond(deltriright, rightsubseg);
  }

  // Set the new origin of `deltri' and check its quality.
  org(lefttri, neworg);
  setorg(*deltri, neworg);
  if (!m_b.nobisect) {
    testtriangle(deltri);
  }

  // Delete the two spliced-out triangles.
  triangledealloc(lefttri.tri);
  triangledealloc(righttri.tri);
}

//***************************************************************************
//
//  undovertex()   Undo the most recent Vertex insertion.
//
//  Walks through the list of transformations (flips and a Vertex insertion)
//  in the reverse of the order in which they were done, and undoes them.
//  The inserted Vertex is removed from the triangulation and deallocated.
//  Two triangles (possibly just one) are also deallocated.
//
//***************************************************************************
void Mesh::undovertex() {
  otri fliptri;
  otri botleft, botright, topright;
  otri botlcasing, botrcasing, toprcasing;
  otri gluetri;
  osub botlsubseg, botrsubseg, toprsubseg;
  Vertex botvertex, rightvertex;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  // Walk through the list of transformations (flips and a Vertex insertion)
  // in the reverse of the order in which they were done, and undo them.
  while (lastflip != NULL) {
    // Find a Triangle involved in the last unreversed transformation.
    decode(lastflip->flippedtri, fliptri);

    // We are reversing one of three transformations:  a trisection of one
    // Triangle into three (by inserting a Vertex in the Triangle), a
    // bisection of two triangles into four (by inserting a Vertex in an
    // edge), or an edge flip.
    if (lastflip->prevflip == NULL) {
      // Restore a Triangle that was split into three triangles,
      // so it is again one Triangle.
      dprev(fliptri, botleft);
      lnextself(botleft);
      onext(fliptri, botright);
      lprevself(botright);
      sym(botleft, botlcasing);
      sym(botright, botrcasing);
      dest(botleft, botvertex);

      setapex(fliptri, botvertex);
      lnextself(fliptri);
      bond(fliptri, botlcasing);
      tspivot(botleft, botlsubseg);
      tsbond(fliptri, botlsubseg);
      lnextself(fliptri);
      bond(fliptri, botrcasing);
      tspivot(botright, botrsubseg);
      tsbond(fliptri, botrsubseg);

      // Delete the two spliced-out triangles.
      triangledealloc(botleft.tri);
      triangledealloc(botright.tri);
    } else if (lastflip->prevflip == FLIPSTACKER_MERGE4) {
      // Restore two triangles that were split into four triangles, so they are again two triangles.
      lprev(fliptri, gluetri);
      sym(gluetri, botright);
      lnextself(botright);
      sym(botright, botrcasing);
      dest(botright, rightvertex);

      setorg(fliptri, rightvertex);
      bond(gluetri, botrcasing);
      tspivot(botright, botrsubseg);
      tsbond(gluetri, botrsubseg);

      // Delete the spliced-out Triangle.
      triangledealloc(botright.tri);

      sym(fliptri, gluetri);
      if (gluetri.tri != dummytri) {
        lnextself(gluetri);
        dnext(gluetri, topright);
        sym(topright, toprcasing);

        setorg(gluetri, rightvertex);
        bond(gluetri, toprcasing);
        tspivot(topright, toprsubseg);
        tsbond(gluetri, toprsubseg);

        // Delete the spliced-out Triangle.
        triangledealloc(topright.tri);
      }

      // This is the end of the list, sneakily encoded.
      lastflip->prevflip = NULL;
    } else {
      // Undo an edge flip.
      unflip(&fliptri);
    }

    // Go on and process the next transformation.
    lastflip = lastflip->prevflip;
  }
}

#endif // not CDT_ONLY

//******** Mesh transformation routines end here

//******** Divide-and-conquer Delaunay triangulation begins here

//***************************************************************************
//
//  The divide-and-conquer bounding box
//
//  I originally implemented the divide-and-conquer and incremental Delaunay
//  triangulations using the edge-based data structure presented by Guibas
//  and Stolfi. Switching to a Triangle-based data structure doubled the
//  speed. However, I had to think of a few extra tricks to maintain the
//  elegance of the original algorithms.
//
//  The "bounding box" used by my variant of the divide-and-conquer
//  algorithm uses one Triangle for each edge of the convex hull of the
//  triangulation. These bounding triangles all share a common apical
//  Vertex, which is represented by NULL and which represents nothing.
//  The bounding triangles are linked in a circular fan about this NULL
//  Vertex, and the edges on the convex hull of the triangulation appear
//  opposite the NULL Vertex. You might find it easiest to imagine that
//  the NULL Vertex is a point in 3D space behind the center of the
//  triangulation, and that the bounding triangles form a sort of cone.
//
//  This bounding box makes it easy to represent degenerate cases. For
//  instance, the triangulation of two vertices is a single edge. This edge
//  is represented by two bounding box triangles, one on each "side" of the
//  edge. These triangles are also linked together in a fan about the NULL
//  Vertex.
//
//  The bounding box also makes it easy to traverse the convex hull, as the
//  divide-and-conquer algorithm needs to do.
//
//***************************************************************************

//***************************************************************************
//
//  vertexsort()   Sort an array of vertices by x-coordinate, using the
//                 y-coordinate as a secondary key.
//
//  Uses quicksort. Randomized O(n log n) time. No, I did not make any of
//  the usual quicksort mistakes.
//
//***************************************************************************
void vertexsort(Vertex *sortarray, int arraysize) {
  int left, right;
  int pivot;
  REAL pivotx, pivoty;
  Vertex temp;

  if (arraysize == 2) {
    // Recursive base case.
    if ((sortarray[0][0] > sortarray[1][0]) ||
        ((sortarray[0][0] == sortarray[1][0]) &&
         (sortarray[0][1] > sortarray[1][1]))) {
      temp = sortarray[1];
      sortarray[1] = sortarray[0];
      sortarray[0] = temp;
    }
    return;
  }
  // Choose a random pivot to split the array.
  pivot = (int) randInt((UINT) arraysize);
  pivotx = sortarray[pivot][0];
  pivoty = sortarray[pivot][1];
  // Split the array.
  left = -1;
  right = arraysize;
  while (left < right) {
    // Search for a Vertex whose x-coordinate is too large for the left.
    do {
      left++;
    } while ((left <= right) && ((sortarray[left][0] < pivotx) ||
                                 ((sortarray[left][0] == pivotx) &&
                                  (sortarray[left][1] < pivoty))));
    // Search for a Vertex whose x-coordinate is too small for the right.
    do {
      right--;
    } while ((left <= right) && ((sortarray[right][0] > pivotx) ||
                                 ((sortarray[right][0] == pivotx) &&
                                  (sortarray[right][1] > pivoty))));
    if (left < right) {
      // Swap the left and right vertices.
      temp = sortarray[left];
      sortarray[left] = sortarray[right];
      sortarray[right] = temp;
    }
  }
  if (left > 1) {
    // Recursively sort the left subset.
    vertexsort(sortarray, left);
  }
  if (right < arraysize - 2) {
    // Recursively sort the right subset.
    vertexsort(&sortarray[right + 1], arraysize - right - 1);
  }
}

//***************************************************************************
//
//  vertexmedian()   An order statistic algorithm, almost. Shuffles an
//                   array of vertices so that the first `median' vertices
//                   occur lexicographically before the remaining vertices.
//
//  Uses the x-coordinate as the primary key if axis == 0; the y-coordinate
//  if axis == 1. Very similar to the vertexsort() procedure, but runs in
//  randomized linear time.
//
//***************************************************************************
void vertexmedian(Vertex *sortarray, int arraysize, int median, int axis) {
  int left, right;
  int pivot;
  REAL pivot1, pivot2;
  Vertex temp;

  if (arraysize == 2) {
    // Recursive base case.
    if ((sortarray[0][axis] > sortarray[1][axis]) ||
        ((sortarray[0][axis] == sortarray[1][axis]) &&
         (sortarray[0][1 - axis] > sortarray[1][1 - axis]))) {
      temp = sortarray[1];
      sortarray[1] = sortarray[0];
      sortarray[0] = temp;
    }
    return;
  }
  // Choose a random pivot to split the array.
  pivot = (int) randInt((UINT) arraysize);
  pivot1 = sortarray[pivot][axis];
  pivot2 = sortarray[pivot][1 - axis];
  // Split the array.
  left = -1;
  right = arraysize;
  while (left < right) {
    // Search for a Vertex whose x-coordinate is too large for the left.
    do {
      left++;
    } while ((left <= right) && ((sortarray[left][axis] < pivot1) ||
                                 ((sortarray[left][axis] == pivot1) &&
                                  (sortarray[left][1 - axis] < pivot2))));
    // Search for a Vertex whose x-coordinate is too small for the right.
    do {
      right--;
    } while ((left <= right) && ((sortarray[right][axis] > pivot1) ||
                                 ((sortarray[right][axis] == pivot1) &&
                                  (sortarray[right][1 - axis] > pivot2))));
    if (left < right) {
      // Swap the left and right vertices.
      temp = sortarray[left];
      sortarray[left] = sortarray[right];
      sortarray[right] = temp;
    }
  }
  // Unlike in vertexsort(), at most one of the following conditionals is true.
  if (left > median) {
    // Recursively shuffle the left subset.
    vertexmedian(sortarray, left, median, axis);
  }
  if (right < median - 1) {
    // Recursively shuffle the right subset.
    vertexmedian(&sortarray[right + 1], arraysize - right - 1, median - right - 1, axis);
  }
}

//***************************************************************************
//
//  alternateaxes()   Sorts the vertices as appropriate for the divide-and-
//                    conquer algorithm with alternating cuts.
//
//  Partitions by x-coordinate if axis == 0; by y-coordinate if axis == 1.
//  For the base case, subsets containing only two or three vertices are
//  always sorted by x-coordinate.
//
//***************************************************************************
void alternateaxes(Vertex *sortarray, int arraysize, int axis) {
  int divider;

  divider = arraysize >> 1;
  if (arraysize <= 3) {
    // Recursive base case:  subsets of two or three vertices will be
    // handled specially, and should always be sorted by x-coordinate.
    axis = 0;
  }
  // Partition with a horizontal or vertical cut.
  vertexmedian(sortarray, arraysize, divider, axis);
  // Recursively partition the subsets with a cross cut.
  if (arraysize - divider >= 2) {
    if (divider >= 2) {
      alternateaxes(sortarray, divider, 1 - axis);
    }
    alternateaxes(&sortarray[divider], arraysize - divider, 1 - axis);
  }
}

//***************************************************************************
//
//  mergehulls()   Merge two adjacent Delaunay triangulations into a
//                 single Delaunay triangulation.
//
//  This is similar to the algorithm given by Guibas and Stolfi, but uses
//  a Triangle-based, rather than edge-based, data structure.
//
//  The algorithm walks up the gap between the two triangulations, knitting
//  them together. As they are merged, some of their bounding triangles
//  are converted into real triangles of the triangulation. The procedure
//  pulls each hull's bounding triangles apart, then knits them together
//  like the teeth of two gears. The Delaunay property determines, at each
//  step, whether the next "tooth" is a bounding Triangle of the left hull
//  or the right. When a bounding Triangle becomes real, its apex is
//  changed from NULL to a real Vertex.
//
//  Only two new triangles need to be allocated. These become new bounding
//  triangles at the top and bottom of the seam. They are used to connect
//  the remaining bounding triangles (those that have not been converted
//  into real triangles) into a single fan.
//
//  On entry, `farleft' and `innerleft' are bounding triangles of the left
//  triangulation. The origin of `farleft' is the leftmost Vertex, and
//  the destination of `innerleft' is the rightmost Vertex of the
//  triangulation. Similarly, `innerright' and `farright' are bounding
//  triangles of the right triangulation. The origin of `innerright' and
//  destination of `farright' are the leftmost and rightmost vertices.
//
//  On completion, the origin of `farleft' is the leftmost Vertex of the
//  merged triangulation, and the destination of `farright' is the rightmost
//  Vertex.
//
//***************************************************************************
void Mesh::mergehulls(otri     *farleft
                     ,otri     *innerleft
                     ,otri     *innerright
                     ,otri     *farright
                     ,int       axis)
{
  otri leftcand, rightcand;
  otri baseedge;
  otri nextedge;
  otri sidecasing, topcasing, outercasing;
  otri checkedge;
  Vertex innerleftdest;
  Vertex innerrightorg;
  Vertex innerleftapex, innerrightapex;
  Vertex farleftpt, farrightpt;
  Vertex farleftapex, farrightapex;
  Vertex lowerleft, lowerright;
  Vertex upperleft, upperright;
  Vertex nextapex;
  Vertex checkvertex;
  int changemade;
  int badedge;
  int leftfinished, rightfinished;
  Triangle ptr;                         // Temporary variable used by sym().

  dest(*innerleft, innerleftdest);
  apex(*innerleft, innerleftapex);
  org(*innerright, innerrightorg);
  apex(*innerright, innerrightapex);
  // Special treatment for horizontal cuts.
  if (m_b.dwyer && (axis == 1)) {
    org(*farleft, farleftpt);
    apex(*farleft, farleftapex);
    dest(*farright, farrightpt);
    apex(*farright, farrightapex);
    // The pointers to the extremal vertices are shifted to point to the
    // topmost and bottommost Vertex of each hull, rather than the
    // leftmost and rightmost vertices.
    while (farleftapex[1] < farleftpt[1]) {
      lnextself(*farleft);
      symself(*farleft);
      farleftpt = farleftapex;
      apex(*farleft, farleftapex);
    }
    sym(*innerleft, checkedge);
    apex(checkedge, checkvertex);
    while (checkvertex[1] > innerleftdest[1]) {
      lnext(checkedge, *innerleft);
      innerleftapex = innerleftdest;
      innerleftdest = checkvertex;
      sym(*innerleft, checkedge);
      apex(checkedge, checkvertex);
    }
    while (innerrightapex[1] < innerrightorg[1]) {
      lnextself(*innerright);
      symself(*innerright);
      innerrightorg = innerrightapex;
      apex(*innerright, innerrightapex);
    }
    sym(*farright, checkedge);
    apex(checkedge, checkvertex);
    while (checkvertex[1] > farrightpt[1]) {
      lnext(checkedge, *farright);
      farrightapex = farrightpt;
      farrightpt = checkvertex;
      sym(*farright, checkedge);
      apex(checkedge, checkvertex);
    }
  }
  // Find a line tangent to and below both hulls.
  do {
    changemade = 0;
    // Make innerleftdest the "bottommost" Vertex of the left hull.
    if (counterclockwise(innerleftdest, innerleftapex, innerrightorg) > 0.0) {
      lprevself(*innerleft);
      symself(*innerleft);
      innerleftdest = innerleftapex;
      apex(*innerleft, innerleftapex);
      changemade = 1;
    }
    // Make innerrightorg the "bottommost" Vertex of the right hull.
    if (counterclockwise(innerrightapex, innerrightorg, innerleftdest) > 0.0) {
      lnextself(*innerright);
      symself(*innerright);
      innerrightorg = innerrightapex;
      apex(*innerright, innerrightapex);
      changemade = 1;
    }
  } while (changemade);
  // Find the two candidates to be the next "gear tooth."
  sym(*innerleft, leftcand);
  sym(*innerright, rightcand);
  // Create the bottom new bounding Triangle.
  maketriangle(&baseedge);
  // Connect it to the bounding boxes of the left and right triangulations.
  bond(baseedge, *innerleft);
  lnextself(baseedge);
  bond(baseedge, *innerright);
  lnextself(baseedge);
  setorg(baseedge, innerrightorg);
  setdest(baseedge, innerleftdest);
  // Apex is intentionally left NULL.
  if (m_b.verbose > 2) {
    printf("  Creating base bounding ");
    printtriangle(&baseedge);
  }
  // Fix the extreme triangles if necessary.
  org(*farleft, farleftpt);
  if (innerleftdest == farleftpt) {
    lnext(baseedge, *farleft);
  }
  dest(*farright, farrightpt);
  if (innerrightorg == farrightpt) {
    lprev(baseedge, *farright);
  }
  // The vertices of the current knitting edge.
  lowerleft = innerleftdest;
  lowerright = innerrightorg;
  // The candidate vertices for knitting.
  apex(leftcand, upperleft);
  apex(rightcand, upperright);
  // Walk up the gap between the two triangulations, knitting them together.
  while (1) {
    // Have we reached the top?  (This isn't quite the right question,
    // because even though the left triangulation might seem finished now,
    // moving up on the right triangulation might reveal a new Vertex of
    // the left triangulation. And vice-versa.)
    leftfinished = counterclockwise(upperleft, lowerleft, lowerright) <= 0.0;
    rightfinished = counterclockwise(upperright, lowerleft, lowerright) <= 0.0;
    if (leftfinished && rightfinished) {
      // Create the top new bounding Triangle.
      maketriangle(&nextedge);
      setorg(nextedge, lowerleft);
      setdest(nextedge, lowerright);
      // Apex is intentionally left NULL.
      // Connect it to the bounding boxes of the two triangulations.
      bond(nextedge, baseedge);
      lnextself(nextedge);
      bond(nextedge, rightcand);
      lnextself(nextedge);
      bond(nextedge, leftcand);
      if (m_b.verbose > 2) {
        printf("  Creating top bounding ");
        printtriangle(&nextedge);
      }
      // Special treatment for horizontal cuts.
      if (m_b.dwyer && (axis == 1)) {
        org(*farleft, farleftpt);
        apex(*farleft, farleftapex);
        dest(*farright, farrightpt);
        apex(*farright, farrightapex);
        sym(*farleft, checkedge);
        apex(checkedge, checkvertex);
        // The pointers to the extremal vertices are restored to the
        // leftmost and rightmost vertices (rather than topmost and
        // bottommost).
        while (checkvertex[0] < farleftpt[0]) {
          lprev(checkedge, *farleft);
          farleftapex = farleftpt;
          farleftpt = checkvertex;
          sym(*farleft, checkedge);
          apex(checkedge, checkvertex);
        }
        while (farrightapex[0] > farrightpt[0]) {
          lprevself(*farright);
          symself(*farright);
          farrightpt = farrightapex;
          apex(*farright, farrightapex);
        }
      }
      return;
    }
    // Consider eliminating edges from the left triangulation.
    if (!leftfinished) {
      // What Vertex would be exposed if an edge were deleted?
      lprev(leftcand, nextedge);
      symself(nextedge);
      apex(nextedge, nextapex);
      // If nextapex is NULL, then no Vertex would be exposed; the
      // triangulation would have been eaten right through.
      if (nextapex != NULL) {
        // Check whether the edge is Delaunay.
        badedge = incircle(lowerleft, lowerright, upperleft, nextapex) > 0.0;
        while (badedge) {
          // Eliminate the edge with an edge flip. As a result,the
          // left triangulation will have one more boundary Triangle.
          lnextself(nextedge);
          sym(nextedge, topcasing);
          lnextself(nextedge);
          sym(nextedge, sidecasing);
          bond(nextedge, topcasing);
          bond(leftcand, sidecasing);
          lnextself(leftcand);
          sym(leftcand, outercasing);
          lprevself(nextedge);
          bond(nextedge, outercasing);
          // Correct the vertices to reflect the edge flip.
          setorg(leftcand, lowerleft);
          setdest(leftcand, NULL);
          setapex(leftcand, nextapex);
          setorg(nextedge, NULL);
          setdest(nextedge, upperleft);
          setapex(nextedge, nextapex);
          // Consider the newly exposed Vertex.
          upperleft = nextapex;
          // What Vertex would be exposed if another edge were deleted?
          otricopy(sidecasing, nextedge);
          apex(nextedge, nextapex);
          if (nextapex != NULL) {
            // Check whether the edge is Delaunay.
            badedge = incircle(lowerleft, lowerright, upperleft, nextapex) > 0.0;
          } else {
            // Avoid eating right through the triangulation.
            badedge = 0;
          }
        }
      }
    }
    // Consider eliminating edges from the right triangulation.
    if (!rightfinished) {
      // What Vertex would be exposed if an edge were deleted?
      lnext(rightcand, nextedge);
      symself(nextedge);
      apex(nextedge, nextapex);
      // If nextapex is NULL, then no Vertex would be exposed; the
      // triangulation would have been eaten right through.
      if (nextapex != NULL) {
        // Check whether the edge is Delaunay.
        badedge = incircle(lowerleft, lowerright, upperright, nextapex) > 0.0;
        while (badedge) {
          // Eliminate the edge with an edge flip. As a result, the
          // right triangulation will have one more boundary Triangle.
          lprevself(nextedge);
          sym(nextedge, topcasing);
          lprevself(nextedge);
          sym(nextedge, sidecasing);
          bond(nextedge, topcasing);
          bond(rightcand, sidecasing);
          lprevself(rightcand);
          sym(rightcand, outercasing);
          lnextself(nextedge);
          bond(nextedge, outercasing);
          // Correct the vertices to reflect the edge flip.
          setorg(rightcand, NULL);
          setdest(rightcand, lowerright);
          setapex(rightcand, nextapex);
          setorg(nextedge, upperright);
          setdest(nextedge, NULL);
          setapex(nextedge, nextapex);
          // Consider the newly exposed Vertex.
          upperright = nextapex;
          // What Vertex would be exposed if another edge were deleted?
          otricopy(sidecasing, nextedge);
          apex(nextedge, nextapex);
          if (nextapex != NULL) {
            // Check whether the edge is Delaunay.
            badedge = incircle(lowerleft, lowerright, upperright, nextapex) > 0.0;
          } else {
            // Avoid eating right through the triangulation.
            badedge = 0;
          }
        }
      }
    }
    if (leftfinished || (!rightfinished &&
           (incircle(upperleft, lowerleft, lowerright, upperright) > 0.0))) {
      // Knit the triangulations, adding an edge from `lowerleft' to `upperright'.
      bond(baseedge, rightcand);
      lprev(rightcand, baseedge);
      setdest(baseedge, lowerleft);
      lowerright = upperright;
      sym(baseedge, rightcand);
      apex(rightcand, upperright);
    } else {
      // Knit the triangulations, adding an edge from `upperleft' to `lowerright'.
      bond(baseedge, leftcand);
      lnext(leftcand, baseedge);
      setorg(baseedge, lowerright);
      lowerleft = upperleft;
      sym(baseedge, leftcand);
      apex(leftcand, upperleft);
    }
    if (m_b.verbose > 2) {
      printf("  Connecting ");
      printtriangle(&baseedge);
    }
  }
}

//***************************************************************************
//
//  divconqrecurse()   Recursively form a Delaunay triangulation by the
//                     divide-and-conquer method.
//
//  Recursively breaks down the problem into smaller pieces, which are
//  knitted together by mergehulls(). The base cases (problems of two or
//  three vertices) are handled specially here.
//
//  On completion, `farleft' and `farright' are bounding triangles such that
//  the origin of `farleft' is the leftmost Vertex (breaking ties by
//  choosing the highest leftmost Vertex), and the destination of
//  `farright' is the rightmost Vertex (breaking ties by choosing the
//  lowest rightmost Vertex).
//
//***************************************************************************
void Mesh::divconqrecurse(Vertex   *sortarray
                         ,int       vertices
                         ,int       axis
                         ,otri     *farleft
                         ,otri     *farright)
{
  otri midtri, tri1, tri2, tri3;
  otri innerleft, innerright;
  REAL area;
  int divider;

  if (m_b.verbose > 2) {
    printf("  Triangulating %d vertices.\n", vertices);
  }
  if (vertices == 2) {
    // The triangulation of two vertices is an edge. An edge is
    // represented by two bounding triangles.
    maketriangle(farleft);
    setorg(*farleft, sortarray[0]);
    setdest(*farleft, sortarray[1]);
    // The apex is intentionally left NULL.
    maketriangle(farright);
    setorg(*farright, sortarray[1]);
    setdest(*farright, sortarray[0]);
    // The apex is intentionally left NULL.
    bond(*farleft, *farright);
    lprevself(*farleft);
    lnextself(*farright);
    bond(*farleft, *farright);
    lprevself(*farleft);
    lnextself(*farright);
    bond(*farleft, *farright);
    if (m_b.verbose > 2) {
      printf("  Creating ");
      printtriangle(farleft);
      printf("  Creating ");
      printtriangle(farright);
    }
    // Ensure that the origin of `farleft' is sortarray[0].
    lprev(*farright, *farleft);
    return;
  } else if (vertices == 3) {
    // The triangulation of three vertices is either a Triangle (with
    // three bounding triangles) or two edges (with four bounding
    // triangles). In either case, four triangles are created.
    maketriangle(&midtri);
    maketriangle(&tri1);
    maketriangle(&tri2);
    maketriangle(&tri3);
    area = counterclockwise(sortarray[0], sortarray[1], sortarray[2]);
    if (area == 0.0) {
      // Three collinear vertices; the triangulation is two edges.
      setorg(midtri, sortarray[0]);
      setdest(midtri, sortarray[1]);
      setorg(tri1, sortarray[1]);
      setdest(tri1, sortarray[0]);
      setorg(tri2, sortarray[2]);
      setdest(tri2, sortarray[1]);
      setorg(tri3, sortarray[1]);
      setdest(tri3, sortarray[2]);
      // All apices are intentionally left NULL.
      bond(midtri, tri1);
      bond(tri2, tri3);
      lnextself(midtri);
      lprevself(tri1);
      lnextself(tri2);
      lprevself(tri3);
      bond(midtri, tri3);
      bond(tri1, tri2);
      lnextself(midtri);
      lprevself(tri1);
      lnextself(tri2);
      lprevself(tri3);
      bond(midtri, tri1);
      bond(tri2, tri3);
      // Ensure that the origin of `farleft' is sortarray[0].
      otricopy(tri1, *farleft);
      // Ensure that the destination of `farright' is sortarray[2].
      otricopy(tri2, *farright);
    } else {
      // The three vertices are not collinear; the triangulation is one Triangle, namely `midtri'.
      setorg(midtri, sortarray[0]);
      setdest(tri1, sortarray[0]);
      setorg(tri3, sortarray[0]);
      // Apices of tri1, tri2, and tri3 are left NULL.
      if (area > 0.0) {
        // The vertices are in counterclockwise order.
        setdest(midtri, sortarray[1]);
        setorg(tri1, sortarray[1]);
        setdest(tri2, sortarray[1]);
        setapex(midtri, sortarray[2]);
        setorg(tri2, sortarray[2]);
        setdest(tri3, sortarray[2]);
      } else {
        // The vertices are in clockwise order.
        setdest(midtri, sortarray[2]);
        setorg(tri1, sortarray[2]);
        setdest(tri2, sortarray[2]);
        setapex(midtri, sortarray[1]);
        setorg(tri2, sortarray[1]);
        setdest(tri3, sortarray[1]);
      }
      // The topology does not depend on how the vertices are ordered.
      bond(midtri, tri1);
      lnextself(midtri);
      bond(midtri, tri2);
      lnextself(midtri);
      bond(midtri, tri3);
      lprevself(tri1);
      lnextself(tri2);
      bond(tri1, tri2);
      lprevself(tri1);
      lprevself(tri3);
      bond(tri1, tri3);
      lnextself(tri2);
      lprevself(tri3);
      bond(tri2, tri3);
      // Ensure that the origin of `farleft' is sortarray[0].
      otricopy(tri1, *farleft);
      // Ensure that the destination of `farright' is sortarray[2].
      if (area > 0.0) {
        otricopy(tri2, *farright);
      } else {
        lnext(*farleft, *farright);
      }
    }
    if (m_b.verbose > 2) {
      printf("  Creating ");
      printtriangle(&midtri);
      printf("  Creating ");
      printtriangle(&tri1);
      printf("  Creating ");
      printtriangle(&tri2);
      printf("  Creating ");
      printtriangle(&tri3);
    }
    return;
  } else {
    // Split the vertices in half.
    divider = vertices >> 1;
    // Recursively triangulate each half.
    divconqrecurse(sortarray, divider, 1 - axis, farleft, &innerleft);
    divconqrecurse(&sortarray[divider], vertices - divider, 1 - axis, &innerright, farright);
    if (m_b.verbose > 1) {
      printf("  Joining triangulations with %d and %d vertices.\n", divider, vertices - divider);
    }
    // Merge the two triangulations into one.
    mergehulls(farleft, &innerleft, &innerright, farright, axis);
  }
}

long Mesh::removeghosts(otri *startghost) {
  otri searchedge;
  otri dissolveedge;
  otri deadtriangle;
  Vertex markorg;
  long hullsize;
  Triangle ptr;                         // Temporary variable used by sym().

  if (m_b.verbose) {
    printf("  Removing ghost triangles.\n");
  }
  // Find an edge on the convex hull to start point location from.
  lprev(*startghost, searchedge);
  symself(searchedge);
  dummytri[0] = encode(searchedge);
  // Remove the bounding box and count the convex hull edges.
  otricopy(*startghost, dissolveedge);
  hullsize = 0;
  do {
    hullsize++;
    lnext(dissolveedge, deadtriangle);
    lprevself(dissolveedge);
    symself(dissolveedge);
    // If no PSLG is involved, set the boundary markers of all the vertices
    // on the convex hull. If a PSLG is used, this step is done later.
    if (!m_b.poly) {
      // Watch out for the case where all the input vertices are collinear.
      if (dissolveedge.tri != dummytri) {
        org(dissolveedge, markorg);
        if (vertexmark(markorg) == 0) {
          setvertexmark(markorg, 1);
        }
      }
    }
    // Remove a bounding Triangle from a convex hull Triangle.
    dissolve(dissolveedge);
    // Find the next bounding Triangle.
    sym(deadtriangle, dissolveedge);
    // Delete the bounding Triangle.
    triangledealloc(deadtriangle.tri);
  } while (!otriequal(dissolveedge, *startghost));
  return hullsize;
}

//***************************************************************************
//
//  divconqdelaunay()   Form a Delaunay triangulation by the divide-and-
//                      conquer method.
//
//  Sorts the vertices, calls a recursive procedure to triangulate them, and
//  removes the bounding box, setting boundary markers as appropriate.
//
//***************************************************************************
long Mesh::divconqdelaunay() {
  Vertex *sortarray;
  otri hullleft, hullright;
  int divider;
  int i, j;

  if (m_b.verbose) {
    printf("  Sorting vertices.\n");
  }

  // Allocate an array of pointers to vertices for sorting.
  sortarray = MALLOC(Vertex, invertices);
  vertices.traversalinit();
  for (i = 0; i < invertices; i++) {
    sortarray[i] = vertextraverse();
  }
  // Sort the vertices.
  vertexsort(sortarray, invertices);
  // Discard duplicate vertices, which can really mess up the algorithm.
  i = 0;
  for (j = 1; j < invertices; j++) {
    if ((sortarray[i][0] == sortarray[j][0]) && (sortarray[i][1] == sortarray[j][1])) {
      if (!m_b.quiet) {
        duplicateVertexWarning(sortarray[j]);
      }
      setvertextype(sortarray[j], UNDEADVERTEX);
      undeads++;
    } else {
      i++;
      sortarray[i] = sortarray[j];
    }
  }
  i++;
  if (m_b.dwyer) {
    // Re-sort the array of vertices to accommodate alternating cuts.
    divider = i >> 1;
    if (i - divider >= 2) {
      if (divider >= 2) {
        alternateaxes(sortarray, divider, 1);
      }
      alternateaxes(&sortarray[divider], i - divider, 1);
    }
  }

  if (m_b.verbose) {
    printf("  Forming triangulation.\n");
  }

  // Form the Delaunay triangulation.
  divconqrecurse(sortarray, i, 0, &hullleft, &hullright);
  trifree((VOID *) sortarray);

  return removeghosts(&hullleft);
}

//******** Incremental Delaunay triangulation begins here

#ifndef REDUCED

//***************************************************************************
//
//  boundingbox()   Form an "infinite" bounding Triangle to insert vertices
//                  into.
//
//  The vertices at "infinity" are assigned finite coordinates, which are
//  used by the point location routines, but (mostly) ignored by the
//  Delaunay edge flip routines.
//
//***************************************************************************
void Mesh::boundingbox() {
  otri inftri;          // Handle for the triangular bounding box.
  REAL width;

  if (m_b.verbose) {
    printf("  Creating triangular bounding box.\n");
  }
  // Find the width (or height, whichever is larger) of the triangulation.
  width = xmax - xmin;
  if (ymax - ymin > width) {
    width = ymax - ymin;
  }
  if (width == 0.0) {
    width = 1.0;
  }
  // Create the vertices of the bounding box.
  infvertex1 = (Vertex) trimalloc(vertices.getItemBytes());
  infvertex2 = (Vertex) trimalloc(vertices.getItemBytes());
  infvertex3 = (Vertex) trimalloc(vertices.getItemBytes());
  infvertex1[0] = xmin - 50.0 * width;
  infvertex1[1] = ymin - 40.0 * width;
  infvertex2[0] = xmax + 50.0 * width;
  infvertex2[1] = ymin - 40.0 * width;
  infvertex3[0] = 0.5 * (xmin + xmax);
  infvertex3[1] = ymax + 60.0 * width;

  // Create the bounding box.
  maketriangle(&inftri);
  setorg(inftri, infvertex1);
  setdest(inftri, infvertex2);
  setapex(inftri, infvertex3);
  // Link dummytri to the bounding box so we can always find an
  // edge to begin searching (point location) from.
  dummytri[0] = (Triangle) inftri.tri;
  if (m_b.verbose > 2) {
    printf("  Creating ");
    printtriangle(&inftri);
  }
}

//***************************************************************************
//
//  removebox()   Remove the "infinite" bounding Triangle, setting boundary
//                markers as appropriate.
//
//  The triangular bounding box has three boundary triangles (one for each
//  side of the bounding box), and a bunch of triangles fanning out from
//  the three bounding box vertices (one Triangle for each edge of the
//  convex hull of the inner mesh). This routine removes these triangles.
//
//  Returns the number of edges on the convex hull of the triangulation.
//
//***************************************************************************
long Mesh::removebox() {
  otri deadtriangle;
  otri searchedge;
  otri checkedge;
  otri nextedge, finaledge, dissolveedge;
  Vertex markorg;
  long hullsize;
  Triangle ptr;                         // Temporary variable used by sym().

  if (m_b.verbose) {
    printf("  Removing triangular bounding box.\n");
  }
  // Find a boundary Triangle.
  nextedge.tri = dummytri;
  nextedge.orient = 0;
  symself(nextedge);
  // Mark a place to stop.
  lprev(nextedge, finaledge);
  lnextself(nextedge);
  symself(nextedge);
  // Find a Triangle (on the boundary of the Vertex set) that isn't
  // a bounding box Triangle.
  lprev(nextedge, searchedge);
  symself(searchedge);
  // Check whether nextedge is another boundary Triangle
  // adjacent to the first one.
  lnext(nextedge, checkedge);
  symself(checkedge);
  if (checkedge.tri == dummytri) {
    // Go on to the next Triangle. There are only three boundary
    // triangles, and this next Triangle cannot be the third one,
    // so it's safe to stop here.
    lprevself(searchedge);
    symself(searchedge);
  }
  // Find a new boundary edge to search from, as the current search
  // edge lies on a bounding box Triangle and will be deleted.
  dummytri[0] = encode(searchedge);
  hullsize = -2l;
  while (!otriequal(nextedge, finaledge)) {
    hullsize++;
    lprev(nextedge, dissolveedge);
    symself(dissolveedge);
    // If not using a PSLG, the vertices should be marked now.
    // (If using a PSLG, markhull() will do the job.)
    if (!m_b.poly) {
      // Be careful!  One must check for the case where all the input
      // vertices are collinear, and thus all the triangles are part of
      // the bounding box. Otherwise, the setvertexmark() call below
      // will cause a bad pointer reference.
      if (dissolveedge.tri != dummytri) {
        org(dissolveedge, markorg);
        if (vertexmark(markorg) == 0) {
          setvertexmark(markorg, 1);
        }
      }
    }
    // Disconnect the bounding box Triangle from the mesh Triangle.
    dissolve(dissolveedge);
    lnext(nextedge, deadtriangle);
    sym(deadtriangle, nextedge);
    // Get rid of the bounding box Triangle.
    triangledealloc(deadtriangle.tri);
    // Do we need to turn the corner?
    if (nextedge.tri == dummytri) {
      // Turn the corner.
      otricopy(dissolveedge, nextedge);
    }
  }
  triangledealloc(finaledge.tri);

  trifree((VOID *) infvertex1);  // Deallocate the bounding box vertices.
  trifree((VOID *) infvertex2);
  trifree((VOID *) infvertex3);

  return hullsize;
}

//***************************************************************************
//
//  incrementaldelaunay()   Form a Delaunay triangulation by incrementally
//                          inserting vertices.
//
//  Returns the number of edges on the convex hull of the triangulation.
//
//***************************************************************************
long Mesh::incrementaldelaunay() {
  otri starttri;
  Vertex vertexloop;

  // Create a triangular bounding box.
  boundingbox();
  if (m_b.verbose) {
    printf("  Incrementally inserting vertices.\n");
  }
  vertices.traversalinit();
  vertexloop = vertextraverse();
  while (vertexloop != NULL) {
    starttri.tri = dummytri;
    if (insertvertex(vertexloop, &starttri, NULL, 0, 0) == DUPLICATEVERTEX) {
      if (!m_b.quiet) {
        duplicateVertexWarning(vertexloop);
      }
      setvertextype(vertexloop, UNDEADVERTEX);
      undeads++;
    }
    vertexloop = vertextraverse();
  }
  // Remove the bounding box.
  return removebox();
}

void eventheapinsert(Event **heap, int heapsize, Event *newevent) {
  REAL eventx, eventy;
  int eventnum;
  int parent;
  int notdone;

  eventx = newevent->xkey;
  eventy = newevent->ykey;
  eventnum = heapsize;
  notdone = eventnum > 0;
  while (notdone) {
    parent = (eventnum - 1) >> 1;
    if ((heap[parent]->ykey < eventy) ||
        ((heap[parent]->ykey == eventy)
         && (heap[parent]->xkey <= eventx))) {
      notdone = 0;
    } else {
      heap[eventnum] = heap[parent];
      heap[eventnum]->heapposition = eventnum;

      eventnum = parent;
      notdone = eventnum > 0;
    }
  }
  heap[eventnum] = newevent;
  newevent->heapposition = eventnum;
}

void eventheapify(Event **heap, int heapsize, int eventnum) {
  Event *thisevent;
  REAL eventx, eventy;
  int leftchild, rightchild;
  int smallest;
  int notdone;

  thisevent = heap[eventnum];
  eventx = thisevent->xkey;
  eventy = thisevent->ykey;
  leftchild = 2 * eventnum + 1;
  notdone = leftchild < heapsize;
  while (notdone) {
    if ((heap[leftchild]->ykey < eventy) ||
        ((heap[leftchild]->ykey == eventy)
         && (heap[leftchild]->xkey < eventx))) {
      smallest = leftchild;
    } else {
      smallest = eventnum;
    }
    rightchild = leftchild + 1;
    if (rightchild < heapsize) {
      if ((heap[rightchild]->ykey < heap[smallest]->ykey) ||
          ((heap[rightchild]->ykey == heap[smallest]->ykey)
           && (heap[rightchild]->xkey < heap[smallest]->xkey))) {
        smallest = rightchild;
      }
    }
    if (smallest == eventnum) {
      notdone = 0;
    } else {
      heap[eventnum] = heap[smallest];
      heap[eventnum]->heapposition = eventnum;
      heap[smallest] = thisevent;
      thisevent->heapposition = smallest;

      eventnum = smallest;
      leftchild = 2 * eventnum + 1;
      notdone = leftchild < heapsize;
    }
  }
}

void eventheapdelete(Event **heap, int heapsize, int eventnum) {
  Event *moveevent;
  REAL eventx, eventy;
  int parent;
  int notdone;

  moveevent = heap[heapsize - 1];
  if (eventnum > 0) {
    eventx = moveevent->xkey;
    eventy = moveevent->ykey;
    do {
      parent = (eventnum - 1) >> 1;
      if ((heap[parent]->ykey < eventy) ||
          ((heap[parent]->ykey == eventy)
           && (heap[parent]->xkey <= eventx))) {
        notdone = 0;
      } else {
        heap[eventnum] = heap[parent];
        heap[eventnum]->heapposition = eventnum;

        eventnum = parent;
        notdone = eventnum > 0;
      }
    } while (notdone);
  }
  heap[eventnum] = moveevent;
  moveevent->heapposition = eventnum;
  eventheapify(heap, heapsize - 1, eventnum);
}

void Mesh::createeventheap(Event ***eventheap, Event **events, Event **freeevents) {
  Vertex thisvertex;
  int maxevents;
  int i;

  maxevents = (3 * invertices) / 2;
  *eventheap = MALLOC(Event*, maxevents);
  *events = MALLOC(Event, maxevents);
  vertices.traversalinit();
  for (i = 0; i < invertices; i++) {
    thisvertex = vertextraverse();
    (*events)[i].eventptr = (VOID *) thisvertex;
    (*events)[i].xkey = thisvertex[0];
    (*events)[i].ykey = thisvertex[1];
    eventheapinsert(*eventheap, i, *events + i);
  }
  *freeevents = NULL;
  for (i = maxevents - 1; i >= invertices; i--) {
    (*events)[i].eventptr = (VOID *) *freeevents;
    *freeevents = *events + i;
  }
}

int Mesh::rightofhyperbola(otri *fronttri, Vertex newsite) {
  Vertex leftvertex, rightvertex;
  REAL dxa, dya, dxb, dyb;

  hyperbolacount++;

  dest(*fronttri, leftvertex);
  apex(*fronttri, rightvertex);
  if ((leftvertex[1] < rightvertex[1]) ||
      ((leftvertex[1] == rightvertex[1]) &&
       (leftvertex[0] < rightvertex[0]))) {
    if (newsite[0] >= rightvertex[0]) {
      return 1;
    }
  } else {
    if (newsite[0] <= leftvertex[0]) {
      return 0;
    }
  }
  dxa = leftvertex[0] - newsite[0];
  dya = leftvertex[1] - newsite[1];
  dxb = rightvertex[0] - newsite[0];
  dyb = rightvertex[1] - newsite[1];
  return dya * (dxb * dxb + dyb * dyb) > dyb * (dxa * dxa + dya * dya);
}

REAL Mesh::circletop(Vertex pa, Vertex pb, Vertex pc, REAL ccwabc) {
  REAL xac, yac, xbc, ybc, xab, yab;
  REAL aclen2, bclen2, ablen2;

  circletopcount++;

  xac = pa[0] - pc[0];
  yac = pa[1] - pc[1];
  xbc = pb[0] - pc[0];
  ybc = pb[1] - pc[1];
  xab = pa[0] - pb[0];
  yab = pa[1] - pb[1];
  aclen2 = xac * xac + yac * yac;
  bclen2 = xbc * xbc + ybc * ybc;
  ablen2 = xab * xab + yab * yab;
  return pc[1] + (xac * bclen2 - xbc * aclen2 + sqrt(aclen2 * bclen2 * ablen2))
               / (2.0 * ccwabc);
}

void check4deadevent(otri *checktri, Event **freeevents, Event **eventheap, int *heapsize) {
  Event *deadevent;
  Vertex eventvertex;
  int eventnum;

  org(*checktri, eventvertex);
  if (eventvertex != NULL) {
    deadevent = (Event*) eventvertex;
    eventnum = deadevent->heapposition;
    deadevent->eventptr = (VOID *) *freeevents;
    *freeevents = deadevent;
    eventheapdelete(eventheap, *heapsize, eventnum);
    (*heapsize)--;
    setorg(*checktri, NULL);
  }
}

SplayNode *Mesh::splay(SplayNode *splaytree, Vertex searchpoint, otri *searchtri) {
  SplayNode *child, *grandchild;
  SplayNode *lefttree, *righttree;
  SplayNode *leftright;
  Vertex checkvertex;
  int rightofroot, rightofchild;

  if (splaytree == NULL) {
    return NULL;
  }
  dest(splaytree->keyedge, checkvertex);
  if (checkvertex == splaytree->keydest) {
    rightofroot = rightofhyperbola(&splaytree->keyedge, searchpoint);
    if (rightofroot) {
      otricopy(splaytree->keyedge, *searchtri);
      child = splaytree->rchild;
    } else {
      child = splaytree->lchild;
    }
    if (child == NULL) {
      return splaytree;
    }
    dest(child->keyedge, checkvertex);
    if (checkvertex != child->keydest) {
      child = splay(child, searchpoint, searchtri);
      if (child == NULL) {
        if (rightofroot) {
          splaytree->rchild = NULL;
        } else {
          splaytree->lchild = NULL;
        }
        return splaytree;
      }
    }
    rightofchild = rightofhyperbola(&child->keyedge, searchpoint);
    if (rightofchild) {
      otricopy(child->keyedge, *searchtri);
      grandchild = splay(child->rchild, searchpoint, searchtri);
      child->rchild = grandchild;
    } else {
      grandchild = splay(child->lchild, searchpoint, searchtri);
      child->lchild = grandchild;
    }
    if (grandchild == NULL) {
      if (rightofroot) {
        splaytree->rchild = child->lchild;
        child->lchild = splaytree;
      } else {
        splaytree->lchild = child->rchild;
        child->rchild = splaytree;
      }
      return child;
    }
    if (rightofchild) {
      if (rightofroot) {
        splaytree->rchild = child->lchild;
        child->lchild = splaytree;
      } else {
        splaytree->lchild = grandchild->rchild;
        grandchild->rchild = splaytree;
      }
      child->rchild = grandchild->lchild;
      grandchild->lchild = child;
    } else {
      if (rightofroot) {
        splaytree->rchild = grandchild->lchild;
        grandchild->lchild = splaytree;
      } else {
        splaytree->lchild = child->rchild;
        child->rchild = splaytree;
      }
      child->lchild = grandchild->rchild;
      grandchild->rchild = child;
    }
    return grandchild;
  } else {
    lefttree = splay(splaytree->lchild, searchpoint, searchtri);
    righttree = splay(splaytree->rchild, searchpoint, searchtri);

    splaynodes.dealloc(splaytree);
    if (lefttree == NULL) {
      return righttree;
    } else if (righttree == NULL) {
      return lefttree;
    } else if (lefttree->rchild == NULL) {
      lefttree->rchild = righttree->lchild;
      righttree->lchild = lefttree;
      return righttree;
    } else if (righttree->lchild == NULL) {
      righttree->lchild = lefttree->rchild;
      lefttree->rchild = righttree;
      return lefttree;
    } else {
//      printf("Holy Toledo!!!\n");
      leftright = lefttree->rchild;
      while (leftright->rchild != NULL) {
        leftright = leftright->rchild;
      }
      leftright->rchild = righttree;
      return lefttree;
    }
  }
}

SplayNode *Mesh::splayinsert(SplayNode *splayroot, otri *newkey, Vertex searchpoint) {
  SplayNode *newsplaynode;

  newsplaynode = splaynodes.alloc();
  otricopy(*newkey, newsplaynode->keyedge);
  dest(*newkey, newsplaynode->keydest);
  if (splayroot == NULL) {
    newsplaynode->lchild = NULL;
    newsplaynode->rchild = NULL;
  } else if (rightofhyperbola(&splayroot->keyedge, searchpoint)) {
    newsplaynode->lchild = splayroot;
    newsplaynode->rchild = splayroot->rchild;
    splayroot->rchild = NULL;
  } else {
    newsplaynode->lchild = splayroot->lchild;
    newsplaynode->rchild = splayroot;
    splayroot->lchild = NULL;
  }
  return newsplaynode;
}

SplayNode *Mesh::circletopinsert(SplayNode *splayroot,
                                 otri *newkey,
                                 Vertex pa, Vertex pb, Vertex pc, REAL topy)
{
  REAL ccwabc;
  REAL xac, yac, xbc, ybc;
  REAL aclen2, bclen2;
  REAL searchpoint[2];
  otri dummytri;

  ccwabc = counterclockwise(pa, pb, pc);
  xac = pa[0] - pc[0];
  yac = pa[1] - pc[1];
  xbc = pb[0] - pc[0];
  ybc = pb[1] - pc[1];
  aclen2 = xac * xac + yac * yac;
  bclen2 = xbc * xbc + ybc * ybc;
  searchpoint[0] = pc[0] - (yac * bclen2 - ybc * aclen2) / (2.0 * ccwabc);
  searchpoint[1] = topy;
  return splayinsert(splay(splayroot, (Vertex) searchpoint, &dummytri), newkey, (Vertex) searchpoint);
}

SplayNode *Mesh::frontlocate(SplayNode *splayroot,
                             otri *bottommost, Vertex searchvertex,
                             otri *searchtri, int *farright)
{
  int farrightflag;
  Triangle ptr;                       // Temporary variable used by onext().

  otricopy(*bottommost, *searchtri);
  splayroot = splay(splayroot, searchvertex, searchtri);

  farrightflag = 0;
  while (!farrightflag && rightofhyperbola(searchtri, searchvertex)) {
    onextself(*searchtri);
    farrightflag = otriequal(*searchtri, *bottommost);
  }
  *farright = farrightflag;
  return splayroot;
}

long Mesh::sweeplinedelaunay() {
  Event **eventheap;
  Event *events;
  Event *freeevents;
  Event *nextevent;
  Event *newevent;
  SplayNode *splayroot;
  otri bottommost;
  otri searchtri;
  otri fliptri;
  otri lefttri, righttri, farlefttri, farrighttri;
  otri inserttri;
  Vertex firstvertex, secondvertex;
  Vertex nextvertex, lastvertex;
  Vertex connectvertex;
  Vertex leftvertex, midvertex, rightvertex;
  REAL lefttest, righttest;
  int heapsize;
  int check4events, farrightflag;
  Triangle ptr;   // Temporary variable used by sym(), onext(), and oprev().

  splaynodes.poolinit(sizeof(SplayNode), SPLAYNODEPERBLOCK, SPLAYNODEPERBLOCK, 0);
  splayroot = NULL;

  if (m_b.verbose) {
    printf("  Placing vertices in event heap.\n");
  }
  createeventheap(&eventheap, &events, &freeevents);
  heapsize = invertices;

  if (m_b.verbose) {
    printf("  Forming triangulation.\n");
  }
  maketriangle(&lefttri);
  maketriangle(&righttri);
  bond(lefttri, righttri);
  lnextself(lefttri);
  lprevself(righttri);
  bond(lefttri, righttri);
  lnextself(lefttri);
  lprevself(righttri);
  bond(lefttri, righttri);
  firstvertex = (Vertex) eventheap[0]->eventptr;
  eventheap[0]->eventptr = (VOID *) freeevents;
  freeevents = eventheap[0];
  eventheapdelete(eventheap, heapsize, 0);
  heapsize--;
  do {
    if (heapsize == 0) {
      triError(_T("Input vertices are all identical"));
    }
    secondvertex = (Vertex) eventheap[0]->eventptr;
    eventheap[0]->eventptr = (VOID *) freeevents;
    freeevents = eventheap[0];
    eventheapdelete(eventheap, heapsize, 0);
    heapsize--;
    if ((firstvertex[0] == secondvertex[0]) &&
        (firstvertex[1] == secondvertex[1])) {
      if (!m_b.quiet) {
        duplicateVertexWarning(secondvertex);
      }
      setvertextype(secondvertex, UNDEADVERTEX);
      undeads++;
    }
  } while ((firstvertex[0] == secondvertex[0]) &&
           (firstvertex[1] == secondvertex[1]));
  setorg(lefttri, firstvertex);
  setdest(lefttri, secondvertex);
  setorg(righttri, secondvertex);
  setdest(righttri, firstvertex);
  lprev(lefttri, bottommost);
  lastvertex = secondvertex;
  while (heapsize > 0) {
    nextevent = eventheap[0];
    eventheapdelete(eventheap, heapsize, 0);
    heapsize--;
    check4events = 1;
    if (nextevent->xkey < xmin) {
      decode(nextevent->eventptr, fliptri);
      oprev(fliptri, farlefttri);
      check4deadevent(&farlefttri, &freeevents, eventheap, &heapsize);
      onext(fliptri, farrighttri);
      check4deadevent(&farrighttri, &freeevents, eventheap, &heapsize);

      if (otriequal(farlefttri, bottommost)) {
        lprev(fliptri, bottommost);
      }
      flip(&fliptri);
      setapex(fliptri, NULL);
      lprev(fliptri, lefttri);
      lnext(fliptri, righttri);
      sym(lefttri, farlefttri);

      if (randInt(SAMPLERATE) == 0) {
        symself(fliptri);
        dest(fliptri, leftvertex);
        apex(fliptri, midvertex);
        org(fliptri, rightvertex);
        splayroot = circletopinsert(splayroot, &lefttri, leftvertex, midvertex, rightvertex, nextevent->ykey);
      }
    } else {
      nextvertex = (Vertex) nextevent->eventptr;
      if ((nextvertex[0] == lastvertex[0]) && (nextvertex[1] == lastvertex[1])) {
        if (!m_b.quiet) {
          duplicateVertexWarning(nextvertex);
        }
        setvertextype(nextvertex, UNDEADVERTEX);
        undeads++;
        check4events = 0;
      } else {
        lastvertex = nextvertex;

        splayroot = frontlocate(splayroot, &bottommost, nextvertex, &searchtri, &farrightflag);
//
        otricopy(bottommost, searchtri);
        farrightflag = 0;
        while (!farrightflag && rightofhyperbola(&searchtri, nextvertex)) {
          onextself(searchtri);
          farrightflag = otriequal(searchtri, bottommost);
        }


        check4deadevent(&searchtri, &freeevents, eventheap, &heapsize);

        otricopy(searchtri, farrighttri);
        sym(searchtri, farlefttri);
        maketriangle(&lefttri);
        maketriangle(&righttri);
        dest(farrighttri, connectvertex);
        setorg(lefttri, connectvertex);
        setdest(lefttri, nextvertex);
        setorg(righttri, nextvertex);
        setdest(righttri, connectvertex);
        bond(lefttri, righttri);
        lnextself(lefttri);
        lprevself(righttri);
        bond(lefttri, righttri);
        lnextself(lefttri);
        lprevself(righttri);
        bond(lefttri, farlefttri);
        bond(righttri, farrighttri);
        if (!farrightflag && otriequal(farrighttri, bottommost)) {
          otricopy(lefttri, bottommost);
        }

        if (randInt(SAMPLERATE) == 0) {
          splayroot = splayinsert(splayroot, &lefttri, nextvertex);
        } else if (randInt(SAMPLERATE) == 0) {
          lnext(righttri, inserttri);
          splayroot = splayinsert(splayroot, &inserttri, nextvertex);
        }
      }
    }
    nextevent->eventptr = (VOID *) freeevents;
    freeevents = nextevent;

    if (check4events) {
      apex(farlefttri, leftvertex);
      dest(lefttri, midvertex);
      apex(lefttri, rightvertex);
      lefttest = counterclockwise(leftvertex, midvertex, rightvertex);
      if (lefttest > 0.0) {
        newevent = freeevents;
        freeevents = (Event *) freeevents->eventptr;
        newevent->xkey = xminextreme;
        newevent->ykey = circletop(leftvertex, midvertex, rightvertex, lefttest);
        newevent->eventptr = (VOID *) encode(lefttri);
        eventheapinsert(eventheap, heapsize, newevent);
        heapsize++;
        setorg(lefttri, newevent);
      }
      apex(righttri, leftvertex);
      org(righttri, midvertex);
      apex(farrighttri, rightvertex);
      righttest = counterclockwise(leftvertex, midvertex, rightvertex);
      if (righttest > 0.0) {
        newevent = freeevents;
        freeevents = (Event *) freeevents->eventptr;
        newevent->xkey = xminextreme;
        newevent->ykey = circletop(leftvertex, midvertex, rightvertex, righttest);
        newevent->eventptr = (VOID *) encode(farrighttri);
        eventheapinsert(eventheap, heapsize, newevent);
        heapsize++;
        setorg(farrighttri, newevent);
      }
    }
  }

  splaynodes.pooldeinit();
  lprevself(bottommost);
  return removeghosts(&bottommost);
}

#endif // not REDUCED

//******** General mesh construction routines begin here

//  Form a Delaunay triangulation.
long Mesh::delaunay() {
  long hulledges;

  eextras = 0;
  initializetrisubpools();

#ifdef REDUCED
  if (!m_b.quiet) {
    printf("Constructing Delaunay triangulation by divide-and-conquer method.\n");
  }
  hulledges = divconqdelaunay();
#else // not REDUCED
  if (!m_b.quiet) {
    printf("Constructing Delaunay triangulation ");
    if (m_b.incremental) {
      printf("by incremental method.\n");
    } else if (m_b.sweepline) {
      printf("by sweepline method.\n");
    } else {
      printf("by divide-and-conquer method.\n");
    }
  }
  if (m_b.incremental) {
    hulledges = incrementaldelaunay();
  } else if (m_b.sweepline) {
    hulledges = sweeplinedelaunay();
  } else {
    hulledges = divconqdelaunay();
  }
#endif // not REDUCED

  if (triangles.getItems() == 0) {
    // The input vertices were all collinear, so there are no triangles.
    return 0l;
  } else {
    return hulledges;
  }
}

#ifndef CDT_ONLY

//***************************************************************************
//
//  reconstruct()   Reconstruct a triangulation from its .ele (and possibly
//                  .poly) file. Used when the -r switch is used.
//
//  Reads an .ele file and reconstructs the original mesh. If the -p switch
//  is used, this procedure will also read a .poly file and reconstruct the
//  subsegments of the original mesh. If the -a switch is used, this
//  procedure will also read an .area file and set a maximum area constraint
//  on each Triangle.
//
//  Vertices that are not corners of triangles, such as nodes on edges of
//  subparametric elements, are discarded.
//
//  This routine finds the adjacencies between triangles (and subsegments)
//  by forming one stack of triangles for each Vertex. Each Triangle is on
//  three different stacks simultaneously. Each Triangle's subsegment
//  pointers are used to link the items in each stack. This memory-saving
//  feature makes the code harder to read. The most important thing to keep
//  in mind is that each Triangle is removed from a stack precisely when
//  the corresponding pointer is adjusted to refer to a subsegment rather
//  than the next Triangle of the stack.
//
//***************************************************************************
#ifdef TRILIBRARY
int Mesh::reconstruct(int      *trianglelist
                     ,REAL     *triangleattriblist
                     ,REAL     *trianglearealist
                     ,int       elements
                     ,int       corners
                     ,int       attribs
                     ,int      *segmentlist
                     ,int      *segmentmarkerlist
                     ,int       numberofsegments)
#else // not TRILIBRARY
long Mesh::reconstruct(char     *elefilename
                      ,char     *areafilename
                      ,char     *polyfilename
                      ,FILE     *polyfile)
#endif // not TRILIBRARY
{
#ifdef TRILIBRARY
  int vertexindex;
  int attribindex;
#else // not TRILIBRARY
  FILE *elefile;
  FILE *areafile = NULL;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  int areaelements;
#endif // not TRILIBRARY
  otri triangleloop;
  otri triangleleft;
  otri checktri;
  otri checkleft;
  otri checkneighbor;
  osub subsegloop;
  Triangle *vertexarray;
  Triangle *prevlink;
  Triangle nexttri;
  Vertex tdest, tapex;
  Vertex checkdest, checkapex;
  Vertex shorg;
  Vertex killvertex;
  Vertex segmentorg, segmentdest;
  REAL area;
  int corner[3];
  int end[2];
  int killvertexindex;
  int incorners;
  int segmentmarkers;
  int boundmarker;
  int aroundvertex;
  long hullsize;
  int notfound;
  long elementnumber, segmentnumber;
  int i, j;
  Triangle ptr;                         // Temporary variable used by sym().

#ifdef TRILIBRARY
  inelements = elements;
  incorners = corners;
  if(incorners < 3) {
    triError(_T("Triangles must have at least 3 vertices"));
  }
  eextras = attribs;
#else // not TRILIBRARY
  // Read the triangles from an .ele file.
  if(!m_b.quiet) {
    printf("Opening %s.\n", elefilename);
  }
  elefile = FOPEN(elefilename, "r");
  // Read number of triangles, number of vertices per Triangle, and
  // number of Triangle attributes from .ele file.
  stringptr = readline(inputline, elefile, elefilename);
  inelements = (int)strtol(stringptr, &stringptr, 0);
  stringptr = findfield(stringptr);
  if(*stringptr == '\0') {
    incorners = 3;
  } else {
    incorners = (int)strtol(stringptr, &stringptr, 0);
    if(incorners < 3) {
      triError(_T("Triangles in %s must have at least 3 vertices"), String(elefilename).cstr());
    }
  }
  stringptr = findfield(stringptr);
  if (*stringptr == '\0') {
    eextras = 0;
  } else {
    eextras = (int) strtol(stringptr, &stringptr, 0);
  }
#endif // not TRILIBRARY

  initializetrisubpools();

  // Create the triangles.
  for (elementnumber = 1; elementnumber <= inelements; elementnumber++) {
    maketriangle(&triangleloop);
    // Mark the Triangle as living.
    triangleloop.tri[3] = (Triangle) triangleloop.tri;
  }

  segmentmarkers = 0;
  if (m_b.poly) {
#ifdef TRILIBRARY
    insegments = numberofsegments;
    segmentmarkers = segmentmarkerlist != NULL;
#else // not TRILIBRARY
    // Read number of segments and number of segment boundary markers from .poly file.
    stringptr = readline(inputline, polyfile, m_b.inpolyfilename);
    insegments = (int) strtol(stringptr, &stringptr, 0);
    stringptr = findfield(stringptr);
    if (*stringptr != '\0') {
      segmentmarkers = (int) strtol(stringptr, &stringptr, 0);
    }
#endif // not TRILIBRARY

    // Create the subsegments.
    for (segmentnumber = 1; segmentnumber <= insegments; segmentnumber++) {
      makesubseg(&subsegloop);
      // Mark the subsegment as living.
      subsegloop.ss[2] = (SubSeg) subsegloop.ss;
    }
  }

#ifdef TRILIBRARY
  vertexindex = 0;
  attribindex = 0;
#else // not TRILIBRARY
  if (m_b.vararea) {
    // Open an .area file, check for consistency with the .ele file.
    if (!m_b.quiet) {
      printf("Opening %s.\n", areafilename);
    }
    areafile = FOPEN(areafilename, "r");
    stringptr = readline(inputline, areafile, areafilename);
    areaelements = (int) strtol(stringptr, &stringptr, 0);
    if (areaelements != inelements) {
      triError(_T("%s and %s disagree on number of triangles"), String(elefilename).cstr(), String(areafilename).cstr());
    }
  }
#endif // not TRILIBRARY

  if (!m_b.quiet) {
    printf("Reconstructing mesh.\n");
  }
  // Allocate a temporary array that maps each Vertex to some adjacent
  // Triangle. I took care to allocate all the permanent memory for
  // triangles and subsegments first.
  vertexarray = MALLOC(Triangle, vertices.getItems());
  // Each Vertex is initially unrepresented.
  for (i = 0; i < vertices.getItems(); i++) {
    vertexarray[i] = (Triangle) dummytri;
  }

  if (m_b.verbose) {
    printf("  Assembling triangles.\n");
  }
  // Read the triangles from the .ele file, and link
  // together those that share an edge.
  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  elementnumber = m_b.firstnumber;
  while (triangleloop.tri != NULL) {
#ifdef TRILIBRARY
    // Copy the Triangle's three corners.
    for (j = 0; j < 3; j++) {
      corner[j] = trianglelist[vertexindex++];
      if ((corner[j] < m_b.firstnumber) || (corner[j] >= m_b.firstnumber + invertices)) {
        triError(_T("Triangle %ld has an invalid Vertex index"), elementnumber);
      }
    }
#else // not TRILIBRARY
    // Read Triangle number and the Triangle's three corners.
    stringptr = readline(inputline, elefile, elefilename);
    for (j = 0; j < 3; j++) {
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        triError(_T("Triangle %ld is missing Vertex %d in %s"), elementnumber, j + 1, String(elefilename).cstr());
      } else {
        corner[j] = (int) strtol(stringptr, &stringptr, 0);
        if((corner[j] < m_b.firstnumber) || (corner[j] >= m_b.firstnumber + invertices)) {
          triError(_T("Triangle %ld has an invalid Vertex index"), elementnumber);
        }
      }
    }
#endif // not TRILIBRARY

    // Find out about (and throw away) extra nodes.
    for (j = 3; j < incorners; j++) {
#ifdef TRILIBRARY
      killvertexindex = trianglelist[vertexindex++];
#else // not TRILIBRARY
      stringptr = findfield(stringptr);
      if (*stringptr != '\0') {
        killvertexindex = (int) strtol(stringptr, &stringptr, 0);
#endif // not TRILIBRARY
        if ((killvertexindex >= m_b.firstnumber) &&
            (killvertexindex < m_b.firstnumber + invertices)) {
          // Delete the non-corner Vertex if it's not already deleted.
          killvertex = getvertex(killvertexindex);
          if (vertextype(killvertex) != DEADVERTEX) {
            vertexdealloc(killvertex);
          }
        }
#ifndef TRILIBRARY
      }
#endif // not TRILIBRARY
    }

    // Read the Triangle's attributes.
    for (j = 0; j < eextras; j++) {
#ifdef TRILIBRARY
      setelemattribute(triangleloop, j, triangleattriblist[attribindex++]);
#else // not TRILIBRARY
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        setelemattribute(triangleloop, j, 0);
      } else {
        setelemattribute(triangleloop, j, (REAL) strtod(stringptr, &stringptr));
      }
#endif // not TRILIBRARY
    }

    if (m_b.vararea) {
#ifdef TRILIBRARY
      area = trianglearealist[elementnumber - m_b.firstnumber];
#else // not TRILIBRARY
      // Read an area constraint from the .area file.
      stringptr = readline(inputline, areafile, areafilename);
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        area = -1.0;                      // No constraint on this Triangle.
      } else {
        area = (REAL) strtod(stringptr, &stringptr);
      }
#endif // not TRILIBRARY
      setareabound(triangleloop, area);
    }

    // Set the Triangle's vertices.
    triangleloop.orient = 0;
    setorg(triangleloop, getvertex(corner[0]));
    setdest(triangleloop, getvertex(corner[1]));
    setapex(triangleloop, getvertex(corner[2]));
    // Try linking the Triangle to others that share these vertices.
    for (triangleloop.orient = 0; triangleloop.orient < 3; triangleloop.orient++) {
      // Take the number for the origin of triangleloop.
      aroundvertex = corner[triangleloop.orient];
      // Look for other triangles having this Vertex.
      nexttri = vertexarray[aroundvertex - m_b.firstnumber];
      // Link the current Triangle to the next one in the stack.
      triangleloop.tri[6 + triangleloop.orient] = nexttri;
      // Push the current Triangle onto the stack.
      vertexarray[aroundvertex - m_b.firstnumber] = encode(triangleloop);
      decode(nexttri, checktri);
      if (checktri.tri != dummytri) {
        dest(triangleloop, tdest);
        apex(triangleloop, tapex);
        // Look for other triangles that share an edge.
        do {
          dest(checktri, checkdest);
          apex(checktri, checkapex);
          if (tapex == checkdest) {
            // The two triangles share an edge; bond them together.
            lprev(triangleloop, triangleleft);
            bond(triangleleft, checktri);
          }
          if (tdest == checkapex) {
            // The two triangles share an edge; bond them together.
            lprev(checktri, checkleft);
            bond(triangleloop, checkleft);
          }
          // Find the next Triangle in the stack.
          nexttri = checktri.tri[6 + checktri.orient];
          decode(nexttri, checktri);
        } while (checktri.tri != dummytri);
      }
    }
    triangleloop.tri = triangletraverse();
    elementnumber++;
  }

#ifdef TRILIBRARY
  vertexindex = 0;
#else // not TRILIBRARY
  fclose(elefile);
  if (m_b.vararea) {
    fclose(areafile);
  }
#endif // not TRILIBRARY

  hullsize = 0;                      // Prepare to count the boundary edges.
  if (m_b.poly) {
    if (m_b.verbose) {
      printf("  Marking segments in triangulation.\n");
    }
    // Read the segments from the .poly file, and link them to their neighboring triangles.
    boundmarker = 0;
    subsegs.traversalinit();
    subsegloop.ss = subsegtraverse();
    segmentnumber = m_b.firstnumber;
    while (subsegloop.ss != NULL) {
#ifdef TRILIBRARY
      end[0] = segmentlist[vertexindex++];
      end[1] = segmentlist[vertexindex++];
      if (segmentmarkers) {
        boundmarker = segmentmarkerlist[segmentnumber - m_b.firstnumber];
      }
#else // not TRILIBRARY
      // Read the endpoints of each segment, and possibly a boundary marker.
      stringptr = readline(inputline, polyfile, m_b.inpolyfilename);
      // Skip the first (segment number) field.
      stringptr = findfield(stringptr);
      if(*stringptr == '\0') {
        triError(_T("Segment %ld has no endpoints in %s"), segmentnumber, String(polyfilename).cstr());
      } else {
        end[0] = (int)strtol(stringptr, &stringptr, 0);
      }
      stringptr = findfield(stringptr);
      if(*stringptr == '\0') {
        triError(_T("Segment %ld is missing its second endpoint in %s"), segmentnumber, String(polyfilename).cstr());
      } else {
        end[1] = (int) strtol(stringptr, &stringptr, 0);
      }
      if (segmentmarkers) {
        stringptr = findfield(stringptr);
        if (*stringptr == '\0') {
          boundmarker = 0;
        } else {
          boundmarker = (int) strtol(stringptr, &stringptr, 0);
        }
      }
#endif // not TRILIBRARY
      for (j = 0; j < 2; j++) {
        if ((end[j] < m_b.firstnumber) || (end[j] >= m_b.firstnumber + invertices)) {
          triError(_T("Segment %ld has an invalid Vertex index"), segmentnumber);
        }
      }

      // set the subsegment's vertices.
      subsegloop.ssorient = 0;
      segmentorg = getvertex(end[0]);
      segmentdest = getvertex(end[1]);
      setsorg(subsegloop, segmentorg);
      setsdest(subsegloop, segmentdest);
      setsegorg(subsegloop, segmentorg);
      setsegdest(subsegloop, segmentdest);
      setmark(subsegloop, boundmarker);
      // Try linking the subsegment to triangles that share these vertices.
      for (subsegloop.ssorient = 0; subsegloop.ssorient < 2; subsegloop.ssorient++) {
        // Take the number for the destination of subsegloop.
        aroundvertex = end[1 - subsegloop.ssorient];
        // Look for triangles having this Vertex.
        prevlink = &vertexarray[aroundvertex - m_b.firstnumber];
        nexttri = vertexarray[aroundvertex - m_b.firstnumber];
        decode(nexttri, checktri);
        sorg(subsegloop, shorg);
        notfound = 1;
        // Look for triangles having this edge. Note that I'm only
        // comparing each Triangle's destination with the subsegment;
        // each Triangle's apex is handled through a different Vertex.
        // Because each Triangle appears on three vertices' lists, each
        // occurrence of a Triangle on a list can (and does) represent
        // an edge. In this way, most edges are represented twice, and
        // every Triangle-subsegment bond is represented once.
        while (notfound && (checktri.tri != dummytri)) {
          dest(checktri, checkdest);
          if (shorg == checkdest) {
            // We have a match. Remove this Triangle from the list.
            *prevlink = checktri.tri[6 + checktri.orient];
            // Bond the subsegment to the Triangle.
            tsbond(checktri, subsegloop);
            // Check if this is a boundary edge.
            sym(checktri, checkneighbor);
            if (checkneighbor.tri == dummytri) {
              // The next line doesn't insert a subsegment (because there's
              // already one there), but it sets the boundary markers of
              // the existing subsegment and its vertices.
              insertsubseg(&checktri, 1);
              hullsize++;
            }
            notfound = 0;
          }
          // Find the next Triangle in the stack.
          prevlink = &checktri.tri[6 + checktri.orient];
          nexttri = checktri.tri[6 + checktri.orient];
          decode(nexttri, checktri);
        }
      }
      subsegloop.ss = subsegtraverse();
      segmentnumber++;
    }
  }

  // Mark the remaining edges as not being attached to any subsegment.
  // Also, count the (yet uncounted) boundary edges.
  for (i = 0; i < vertices.getItems(); i++) {
    // Search the stack of triangles adjacent to a Vertex.
    nexttri = vertexarray[i];
    decode(nexttri, checktri);
    while (checktri.tri != dummytri) {
      // Find the next Triangle in the stack before this information gets overwritten.
      nexttri = checktri.tri[6 + checktri.orient];
      // No adjacent subsegment.(This overwrites the stack info.)
      tsdissolve(checktri);
      sym(checktri, checkneighbor);
      if (checkneighbor.tri == dummytri) {
        insertsubseg(&checktri, 1);
        hullsize++;
      }
      decode(nexttri, checktri);
    }
  }

  trifree((VOID *) vertexarray);
  return hullsize;
}

#endif // not CDT_ONLY

//******** Segment insertion begins here

//***************************************************************************
//
//  finddirection()   Find the first Triangle on the path from one point
//                    to another.
//
//  Finds the Triangle that intersects a line segment drawn from the
//  origin of `searchtri' to the point `searchpoint', and returns the result
//  in `searchtri'. The origin of `searchtri' does not change, even though
//  the Triangle returned may differ from the one passed in. This routine
//  is used to find the direction to move in to get from one point to
//  another.
//
//  The return value notes whether the destination or apex of the found
//  Triangle is collinear with the two points in question.
//
//***************************************************************************
FindDirectionResult Mesh::finddirection(otri *searchtri, Vertex searchpoint) {
  DEFINEMETHODNAME;
  otri checktri;
  Vertex startvertex;
  Vertex leftvertex, rightvertex;
  REAL leftccw, rightccw;
  int leftflag, rightflag;
  Triangle ptr;           // Temporary variable used by onext() and oprev().

  org(*searchtri, startvertex);
  dest(*searchtri, rightvertex);
  apex(*searchtri, leftvertex);
  // Is `searchpoint' to the left?
  leftccw = counterclockwise(searchpoint, startvertex, leftvertex);
  leftflag = leftccw > 0.0;
  // Is `searchpoint' to the right?
  rightccw = counterclockwise(startvertex, searchpoint, rightvertex);
  rightflag = rightccw > 0.0;
  if (leftflag && rightflag) {
    // `searchtri' faces directly away from `searchpoint'. We could go left
    // or right. Ask whether it's a Triangle or a boundary on the left.
    onext(*searchtri, checktri);
    if (checktri.tri == dummytri) {
      leftflag = 0;
    } else {
      rightflag = 0;
    }
  }
  while (leftflag) {
    // Turn left until satisfied.
    onextself(*searchtri);
    if (searchtri->tri == dummytri) {
      internalerrorNoTriangleFound(method, startvertex, searchpoint);
    }
    apex(*searchtri, leftvertex);
    rightccw = leftccw;
    leftccw = counterclockwise(searchpoint, startvertex, leftvertex);
    leftflag = leftccw > 0.0;
  }
  while (rightflag) {
    // Turn right until satisfied.
    oprevself(*searchtri);
    if (searchtri->tri == dummytri) {
      internalerrorNoTriangleFound(method, startvertex, searchpoint);
    }
    dest(*searchtri, rightvertex);
    leftccw = rightccw;
    rightccw = counterclockwise(startvertex, searchpoint, rightvertex);
    rightflag = rightccw > 0.0;
  }
  if (leftccw == 0.0) {
    return LEFTCOLLINEAR;
  } else if (rightccw == 0.0) {
    return RIGHTCOLLINEAR;
  } else {
    return WITHIN;
  }
}

//***************************************************************************
//
//  segmentintersection()   Find the intersection of an existing segment
//                          and a segment that is being inserted. Insert
//                          a Vertex at the intersection, splitting an
//                          existing subsegment.
//
//  The segment being inserted connects the apex of splittri to endpoint2.
//  splitsubseg is the subsegment being split, and MUST adjoin splittri.
//  Hence, endpoints of the subsegment being split are the origin and
//  destination of splittri.
//
//  On completion, splittri is a handle having the newly inserted
//  intersection point as its origin, and endpoint1 as its destination.
//
//***************************************************************************
void Mesh::segmentintersection(otri *splittri, osub *splitsubseg, Vertex endpoint2) {
  DEFINEMETHODNAME;
  osub opposubseg;
  Vertex endpoint1;
  Vertex torg, tdest;
  Vertex leftvertex, rightvertex;
  Vertex newvertex;
  InserVvertexResult success;
  FindDirectionResult collinear;
  REAL ex, ey;
  REAL tx, ty;
  REAL etx, ety;
  REAL split, denom;
  int i;
  Triangle ptr;                       // Temporary variable used by onext().
  SubSeg sptr;                        // Temporary variable used by snext().

  // Find the other three segment endpoints.
  apex(*splittri, endpoint1);
  org(*splittri, torg);
  dest(*splittri, tdest);
  // Segment intersection formulae; see the Antonio reference.
  tx = tdest[0] - torg[0];
  ty = tdest[1] - torg[1];
  ex = endpoint2[0] - endpoint1[0];
  ey = endpoint2[1] - endpoint1[1];
  etx = torg[0] - endpoint2[0];
  ety = torg[1] - endpoint2[1];
  denom = ty * ex - tx * ey;
  if (denom == 0.0) {
    internalerror(method, _T("Attempt to find intersection of parallel segments"));
  }
  split = (ey * etx - ex * ety) / denom;
  // Create the new Vertex.
  newvertex = vertices.alloc();
  // Interpolate its coordinate and attributes.
  for (i = 0; i < 2 + nextras; i++) {
    newvertex[i] = torg[i] + split * (tdest[i] - torg[i]);
  }
  setvertexmark(newvertex, mark(*splitsubseg));
  setvertextype(newvertex, INPUTVERTEX);
  if (m_b.verbose > 1) {
    _tprintf(_T("  Splitting subsegment %s\n"), triangleToString(torg, tdest, newvertex).cstr());
  }
  // Insert the intersection Vertex. This should always succeed.
  success = insertvertex(newvertex, splittri, splitsubseg, 0, 0);
  if (success != SUCCESSFULVERTEX) {
    internalerror(method, _T("Failure to split a segment"));
  }
  // Record a Triangle whose origin is the new Vertex.
  setvertex2tri(newvertex, encode(*splittri));
  if (steinerleft > 0) {
    steinerleft--;
  }

  // Divide the segment into two, and correct the segment endpoints.
  ssymself(*splitsubseg);
  spivot(*splitsubseg, opposubseg);
  sdissolve(*splitsubseg);
  sdissolve(opposubseg);
  do {
    setsegorg(*splitsubseg, newvertex);
    snextself(*splitsubseg);
  } while (splitsubseg->ss != dummysub);
  do {
    setsegorg(opposubseg, newvertex);
    snextself(opposubseg);
  } while (opposubseg.ss != dummysub);

  // Inserting the Vertex may have caused edge flips. We wish to rediscover
  // the edge connecting endpoint1 to the new intersection Vertex.
  collinear = finddirection(splittri, endpoint1);
  dest(*splittri, rightvertex);
  apex(*splittri, leftvertex);
  if ((leftvertex[0] == endpoint1[0]) && (leftvertex[1] == endpoint1[1])) {
    onextself(*splittri);
  } else if ((rightvertex[0] != endpoint1[0]) || (rightvertex[1] != endpoint1[1])) {
    internalerror(method, _T("Topological inconsistency after splitting a segment"));
  }
  // `splittri' should have destination endpoint1.
}

//***************************************************************************
//
//  scoutsegment()   Scout the first Triangle on the path from one endpoint
//                   to another, and check for completion (reaching the
//                   second endpoint), a collinear Vertex, or the
//                   intersection of two segments.
//
//  Returns one if the entire segment is successfully inserted, and zero if
//  the job must be finished by conformingedge() or constrainededge().
//
//  If the first Triangle on the path has the second endpoint as its
//  destination or apex, a subsegment is inserted and the job is done.
//
//  If the first Triangle on the path has a destination or apex that lies on
//  the segment, a subsegment is inserted connecting the first endpoint to
//  the collinear Vertex, and the search is continued from the collinear
//  Vertex.
//
//  If the first Triangle on the path has a subsegment opposite its origin,
//  then there is a segment that intersects the segment being inserted.
//  Their intersection Vertex is inserted, splitting the subsegment.
//
//***************************************************************************
int Mesh::scoutsegment(otri *searchtri, Vertex endpoint2, int newmark) {
  otri crosstri;
  osub crosssubseg;
  Vertex leftvertex, rightvertex;
  FindDirectionResult collinear;
  SubSeg sptr;                      // Temporary variable used by tspivot().

  collinear = finddirection(searchtri, endpoint2);
  dest(*searchtri, rightvertex);
  apex(*searchtri, leftvertex);
  if (((leftvertex[0] == endpoint2[0]) && (leftvertex[1] == endpoint2[1])) ||
      ((rightvertex[0] == endpoint2[0]) && (rightvertex[1] == endpoint2[1]))) {
    // The segment is already an edge in the mesh.
    if ((leftvertex[0] == endpoint2[0]) && (leftvertex[1] == endpoint2[1])) {
      lprevself(*searchtri);
    }
    // Insert a subsegment, if there isn't already one there.
    insertsubseg(searchtri, newmark);
    return 1;
  } else if (collinear == LEFTCOLLINEAR) {
    // We've collided with a Vertex between the segment's endpoints.
    // Make the collinear Vertex be the Triangle's origin.
    lprevself(*searchtri);
    insertsubseg(searchtri, newmark);
    // Insert the remainder of the segment.
    return scoutsegment(searchtri, endpoint2, newmark);
  } else if (collinear == RIGHTCOLLINEAR) {
    // We've collided with a Vertex between the segment's endpoints.
    insertsubseg(searchtri, newmark);
    // Make the collinear Vertex be the Triangle's origin.
    lnextself(*searchtri);
    // Insert the remainder of the segment.
    return scoutsegment(searchtri, endpoint2, newmark);
  } else {
    lnext(*searchtri, crosstri);
    tspivot(crosstri, crosssubseg);
    // Check for a crossing segment.
    if (crosssubseg.ss == dummysub) {
      return 0;
    } else {
      // Insert a Vertex at the intersection.
      segmentintersection(&crosstri, &crosssubseg, endpoint2);
      otricopy(crosstri, *searchtri);
      insertsubseg(searchtri, newmark);
      // Insert the remainder of the segment.
      return scoutsegment(searchtri, endpoint2, newmark);
    }
  }
}

#ifndef REDUCED
#ifndef CDT_ONLY

//***************************************************************************
//
//  conformingedge()   Force a segment into a conforming Delaunay
//                     triangulation by inserting a Vertex at its midpoint,
//                     and recursively forcing in the two half-segments if
//                     necessary.
//
//  Generates a sequence of subsegments connecting `endpoint1' to
//  `endpoint2'. `newmark' is the boundary marker of the segment, assigned
//  to each new splitting Vertex and subsegment.
//
//  Note that conformingedge() does not always maintain the conforming
//  Delaunay property. Once inserted, segments are locked into place;
//  vertices inserted later (to force other segments in) may render these
//  fixed segments non-Delaunay. The conforming Delaunay property will be
//  restored by enforcequality() by splitting encroached subsegments.
//
//***************************************************************************
void Mesh::conformingedge(Vertex endpoint1, Vertex endpoint2, int newmark) {
  otri searchtri1, searchtri2;
  osub brokensubseg;
  Vertex newvertex;
  Vertex midvertex1, midvertex2;
  InserVvertexResult success;
  int i;
  SubSeg sptr;                      // Temporary variable used by tspivot().

  if (m_b.verbose > 2) {
    _tprintf(_T("Forcing segment into triangulation by recursive splitting:%s\n")
            ,edgeToString(endpoint1, endpoint2).cstr());
  }
  // Create a new Vertex to insert in the middle of the segment.
  newvertex = vertices.alloc();
  // Interpolate coordinates and attributes.
  for (i = 0; i < 2 + nextras; i++) {
    newvertex[i] = 0.5 * (endpoint1[i] + endpoint2[i]);
  }
  setvertexmark(newvertex, newmark);
  setvertextype(newvertex, SEGMENTVERTEX);
  // No known Triangle to search from.
  searchtri1.tri = dummytri;
  // Attempt to insert the new Vertex.
  success = insertvertex(newvertex, &searchtri1, NULL, 0, 0);
  if (success == DUPLICATEVERTEX) {
    if (m_b.verbose > 2) {
      _tprintf(_T("  Segment intersects existing vertex %s\n"), vertexToString(newvertex).cstr());
    }
    // Use the Vertex that's already there.
    vertexdealloc(newvertex);
    org(searchtri1, newvertex);
  } else {
    if (success == VIOLATINGVERTEX) {
      if (m_b.verbose > 2) {
        _tprintf(_T("  Two segments intersect at %s\n"), vertexToString(newvertex).cstr());
      }
      // By fluke, we've landed right on another segment. Split it.
      tspivot(searchtri1, brokensubseg);
      success = insertvertex(newvertex, &searchtri1, &brokensubseg, 0, 0);
      if (success != SUCCESSFULVERTEX) {
        internalerror(__TFUNCTION__, _T("Failure to split a segment"));
      }
    }
    // The Vertex has been inserted successfully.
    if (steinerleft > 0) {
      steinerleft--;
    }
  }
  otricopy(searchtri1, searchtri2);
  // `searchtri1' and `searchtri2' are fastened at their origins to
  // `newvertex', and will be directed toward `endpoint1' and `endpoint2'
  // respectively. First, we must get `searchtri2' out of the way so it
  // won't be invalidated during the insertion of the first half of the segment.
  finddirection(&searchtri2, endpoint2);
  if (!scoutsegment(&searchtri1, endpoint1, newmark)) {
    // The origin of searchtri1 may have changed if a collision with an
    // intervening Vertex on the segment occurred.
    org(searchtri1, midvertex1);
    conformingedge(midvertex1, endpoint1, newmark);
  }
  if (!scoutsegment(&searchtri2, endpoint2, newmark)) {
    // The origin of searchtri2 may have changed if a collision with an
    // intervening Vertex on the segment occurred.
    org(searchtri2, midvertex2);
    conformingedge(midvertex2, endpoint2, newmark);
  }
}

#endif // not CDT_ONLY
#endif // not REDUCED

//***************************************************************************
//
//  delaunayfixup()   Enforce the Delaunay condition at an edge, fanning out
//                    recursively from an existing Vertex. Pay special
//                    attention to stacking inverted triangles.
//
//  This is a support routine for inserting segments into a constrained
//  Delaunay triangulation.
//
//  The origin of fixuptri is treated as if it has just been inserted, and
//  the local Delaunay condition needs to be enforced. It is only enforced
//  in one sector, however, that being the angular range defined by
//  fixuptri.
//
//  This routine also needs to make decisions regarding the "stacking" of
//  triangles. (Read the description of constrainededge() below before
//  reading on here, so you understand the algorithm.)  If the position of
//  the new Vertex (the origin of fixuptri) indicates that the Vertex before
//  it on the polygon is a reflex Vertex, then "stack" the Triangle by
//  doing nothing. (fixuptri is an inverted Triangle, which is how stacked
//  triangles are identified.)
//
//  Otherwise, check whether the Vertex before that was a reflex Vertex.
//  If so, perform an edge flip, thereby eliminating an inverted Triangle
//  (popping it off the stack). The edge flip may result in the creation
//  of a new inverted Triangle, depending on whether or not the new Vertex
//  is visible to the Vertex three edges behind on the polygon.
//
//  If neither of the two vertices behind the new Vertex are reflex
//  vertices, fixuptri and fartri, the Triangle opposite it, are not
//  inverted; hence, ensure that the edge between them is locally Delaunay.
//
//  `leftside' indicates whether or not fixuptri is to the left of the
//  segment being inserted. (Imagine that the segment is pointing up from
//  endpoint1 to endpoint2.)
//
//***************************************************************************
void Mesh::delaunayfixup(otri *fixuptri, int leftside) {
  otri neartri;
  otri fartri;
  osub faredge;
  Vertex nearvertex, leftvertex, rightvertex, farvertex;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  lnext(*fixuptri, neartri);
  sym(neartri, fartri);
  // Check if the edge opposite the origin of fixuptri can be flipped.
  if (fartri.tri == dummytri) {
    return;
  }
  tspivot(neartri, faredge);
  if (faredge.ss != dummysub) {
    return;
  }
  // Find all the relevant vertices.
  apex(neartri, nearvertex);
  org(neartri, leftvertex);
  dest(neartri, rightvertex);
  apex(fartri, farvertex);
  // Check whether the previous polygon Vertex is a reflex Vertex.
  if (leftside) {
    if (counterclockwise(nearvertex, leftvertex, farvertex) <= 0.0) {
      // leftvertex is a reflex Vertex too. Nothing can
      // be done until a convex section is found.
      return;
    }
  } else {
    if (counterclockwise(farvertex, rightvertex, nearvertex) <= 0.0) {
      // rightvertex is a reflex Vertex too. Nothing can
      // be done until a convex section is found.
      return;
    }
  }
  if (counterclockwise(rightvertex, leftvertex, farvertex) > 0.0) {
    // fartri is not an inverted Triangle, and farvertex is not a reflex
    // Vertex. As there are no reflex vertices, fixuptri isn't an
    // inverted Triangle, either. Hence, test the edge between the
    // triangles to ensure it is locally Delaunay.
    if (incircle(leftvertex, farvertex, rightvertex, nearvertex) <= 0.0) {
      return;
    }
    // Not locally Delaunay; go on to an edge flip.
  } // else fartri is inverted; remove it from the stack by flipping.
  flip(&neartri);
  lprevself(*fixuptri);    // Restore the origin of fixuptri after the flip.
  // Recursively process the two triangles that result from the flip.
  delaunayfixup(fixuptri, leftside);
  delaunayfixup(&fartri, leftside);
}

//***************************************************************************
//
//  constrainededge()   Force a segment into a constrained Delaunay
//                      triangulation by deleting the triangles it
//                      intersects, and triangulating the polygons that
//                      form on each side of it.
//
//  Generates a single subsegment connecting `endpoint1' to `endpoint2'.
//  The Triangle `starttri' has `endpoint1' as its origin. `newmark' is the
//  boundary marker of the segment.
//
//  To insert a segment, every Triangle whose interior intersects the
//  segment is deleted. The union of these deleted triangles is a polygon
//  (which is not necessarily monotone, but is close enough), which is
//  divided into two polygons by the new segment. This routine's task is
//  to generate the Delaunay triangulation of these two polygons.
//
//  You might think of this routine's behavior as a two-step process. The
//  first step is to walk from endpoint1 to endpoint2, flipping each edge
//  encountered. This step creates a fan of edges connected to endpoint1,
//  including the desired edge to endpoint2. The second step enforces the
//  Delaunay condition on each side of the segment in an incremental manner:
//  proceeding along the polygon from endpoint1 to endpoint2 (this is done
//  independently on each side of the segment), each Vertex is "enforced"
//  as if it had just been inserted, but affecting only the previous
//  vertices. The result is the same as if the vertices had been inserted
//  in the order they appear on the polygon, so the result is Delaunay.
//
//  In truth, constrainededge() interleaves these two steps. The procedure
//  walks from endpoint1 to endpoint2, and each time an edge is encountered
//  and flipped, the newly exposed Vertex (at the far end of the flipped
//  edge) is "enforced" upon the previously flipped edges, usually affecting
//  only one side of the polygon (depending upon which side of the segment
//  the Vertex falls on).
//
//  The algorithm is complicated by the need to handle polygons that are not
//  convex. Although the polygon is not necessarily monotone, it can be
//  triangulated in a manner similar to the stack-based algorithms for
//  monotone polygons. For each reflex Vertex (local concavity) of the
//  polygon, there will be an inverted Triangle formed by one of the edge
//  flips. (An inverted Triangle is one with negative area - that is, its
//  vertices are arranged in clockwise order - and is best thought of as a
//  wrinkle in the fabric of the mesh.)  Each inverted Triangle can be
//  thought of as a reflex Vertex pushed on the stack, waiting to be fixed
//  later.
//
//  A reflex Vertex is popped from the stack when a Vertex is inserted that
//  is visible to the reflex Vertex. (However, if the Vertex behind the
//  reflex Vertex is not visible to the reflex Vertex, a new inverted
//  Triangle will take its place on the stack.)  These details are handled
//  by the delaunayfixup() routine above.
//
//***************************************************************************
void Mesh::constrainededge(otri *starttri, Vertex endpoint2, int newmark) {
  otri fixuptri, fixuptri2;
  osub crosssubseg;
  Vertex endpoint1;
  Vertex farvertex;
  REAL area;
  int collision;
  int done;
  Triangle ptr;             // Temporary variable used by sym() and oprev().
  SubSeg sptr;              // Temporary variable used by tspivot().

  org(*starttri, endpoint1);
  lnext(*starttri, fixuptri);
  flip(&fixuptri);
  // `collision' indicates whether we have found a Vertex directly
  // between endpoint1 and endpoint2.
  collision = 0;
  done = 0;
  do {
    org(fixuptri, farvertex);
    // `farvertex' is the extreme point of the polygon we are "digging"
    // to get from endpoint1 to endpoint2.
    if ((farvertex[0] == endpoint2[0]) && (farvertex[1] == endpoint2[1])) {
      oprev(fixuptri, fixuptri2);
      // Enforce the Delaunay condition around endpoint2.
      delaunayfixup(&fixuptri, 0);
      delaunayfixup(&fixuptri2, 1);
      done = 1;
    } else {
      // Check whether farvertex is to the left or right of the segment
      // being inserted, to decide which edge of fixuptri to dig
      // through next.
      area = counterclockwise(endpoint1, endpoint2, farvertex);
      if (area == 0.0) {
        // We've collided with a Vertex between endpoint1 and endpoint2.
        collision = 1;
        oprev(fixuptri, fixuptri2);
        // Enforce the Delaunay condition around farvertex.
        delaunayfixup(&fixuptri, 0);
        delaunayfixup(&fixuptri2, 1);
        done = 1;
      } else {
        if (area > 0.0) {        // farvertex is to the left of the segment.
          oprev(fixuptri, fixuptri2);
          // Enforce the Delaunay condition around farvertex, on the
          // left side of the segment only.
          delaunayfixup(&fixuptri2, 1);
          // Flip the edge that crosses the segment. After the edge is
          // flipped, one of its endpoints is the fan Vertex, and the
          // destination of fixuptri is the fan Vertex.
          lprevself(fixuptri);
        } else {                // farvertex is to the right of the segment.
          delaunayfixup(&fixuptri, 0);
          // Flip the edge that crosses the segment. After the edge is
          // flipped, one of its endpoints is the fan Vertex, and the
          // destination of fixuptri is the fan Vertex.
          oprevself(fixuptri);
        }
        // Check for two intersecting segments.
        tspivot(fixuptri, crosssubseg);
        if (crosssubseg.ss == dummysub) {
          flip(&fixuptri);    // May create inverted Triangle at left.
        } else { // We've collided with a segment between endpoint1 and endpoint2.
          collision = 1;
          // Insert a Vertex at the intersection.
          segmentintersection(&fixuptri, &crosssubseg, endpoint2);
          done = 1;
        }
      }
    }
  } while (!done);
  // Insert a subsegment to make the segment permanent.
  insertsubseg(&fixuptri, newmark);
  // If there was a collision with an interceding Vertex, install another
  // segment connecting that Vertex with endpoint2.
  if (collision) {
    // Insert the remainder of the segment.
    if (!scoutsegment(&fixuptri, endpoint2, newmark)) {
      constrainededge(&fixuptri, endpoint2, newmark);
    }
  }
}

//  Insert a PSLG segment into a triangulation.
void Mesh::insertsegment(Vertex endpoint1, Vertex endpoint2, int newmark) {
  DEFINEMETHODNAME;
  otri searchtri1, searchtri2;
  Triangle encodedtri;
  Vertex checkvertex;
  Triangle ptr;                         // Temporary variable used by sym().

  if (m_b.verbose > 1) {
    _tprintf(_T("  Connecting %s to %s\n")
            ,vertexToString(endpoint1).cstr(), vertexToString(endpoint2).cstr());
  }

  // Find a Triangle whose origin is the segment's first endpoint.
  checkvertex = NULL;
  encodedtri = vertex2tri(endpoint1);
  if (encodedtri != NULL) {
    decode(encodedtri, searchtri1);
    org(searchtri1, checkvertex);
  }
  if (checkvertex != endpoint1) {
    // Find a boundary Triangle to search from.
    searchtri1.tri = dummytri;
    searchtri1.orient = 0;
    symself(searchtri1);
    // Search for the segment's first endpoint by point location.
    if (locate(endpoint1, &searchtri1) != ONVERTEX) {
      internalerrorNoPSLGVertex(method, endpoint1);
    }
  }
  // Remember this Triangle to improve subsequent point location.
  otricopy(searchtri1, recenttri);
  // Scout the beginnings of a path from the first endpoint toward the second.
  if (scoutsegment(&searchtri1, endpoint2, newmark)) {
    // The segment was easily inserted.
    return;
  }
  // The first endpoint may have changed if a collision with an intervening
  // Vertex on the segment occurred.
  org(searchtri1, endpoint1);

  // Find a Triangle whose origin is the segment's second endpoint.
  checkvertex = NULL;
  encodedtri = vertex2tri(endpoint2);
  if (encodedtri != NULL) {
    decode(encodedtri, searchtri2);
    org(searchtri2, checkvertex);
  }
  if (checkvertex != endpoint2) {
    // Find a boundary Triangle to search from.
    searchtri2.tri = dummytri;
    searchtri2.orient = 0;
    symself(searchtri2);
    // Search for the segment's second endpoint by point location.
    if (locate(endpoint2, &searchtri2) != ONVERTEX) {
      internalerrorNoPSLGVertex(method, endpoint2);
    }
  }
  // Remember this Triangle to improve subsequent point location.
  otricopy(searchtri2, recenttri);
  // Scout the beginnings of a path from the second endpoint
  // toward the first.
  if (scoutsegment(&searchtri2, endpoint1, newmark)) {
    // The segment was easily inserted.
    return;
  }
  // The second endpoint may have changed if a collision with an intervening
  // Vertex on the segment occurred.
  org(searchtri2, endpoint2);

#ifndef REDUCED
#ifndef CDT_ONLY
  if (m_b.splitseg) {
    // Insert vertices to force the segment into the triangulation.
    conformingedge(endpoint1, endpoint2, newmark);
  } else {
#endif // not CDT_ONLY
#endif // not REDUCED
    // Insert the segment directly into the triangulation.
    constrainededge(&searchtri1, endpoint2, newmark);
#ifndef REDUCED
#ifndef CDT_ONLY
  }
#endif // not CDT_ONLY
#endif // not REDUCED
}

//  Cover the convex hull of a triangulation with subsegments.
void Mesh::markhull() {
  otri hulltri;
  otri nexttri;
  otri starttri;
  Triangle ptr;             // Temporary variable used by sym() and oprev().

  // Find a Triangle handle on the hull.
  hulltri.tri = dummytri;
  hulltri.orient = 0;
  symself(hulltri);
  // Remember where we started so we know when to stop.
  otricopy(hulltri, starttri);
  // Go once counterclockwise around the convex hull.
  do {
    // Create a subsegment if there isn't already one here.
    insertsubseg(&hulltri, 1);
    // To find the next hull edge, go clockwise around the next Vertex.
    lnextself(hulltri);
    oprev(hulltri, nexttri);
    while (nexttri.tri != dummytri) {
      otricopy(nexttri, hulltri);
      oprev(hulltri, nexttri);
    }
  } while (!otriequal(hulltri, starttri));
}

//***************************************************************************
//
//  formskeleton()   Create the segments of a triangulation, including PSLG
//                   segments and edges on the convex hull.
//
//  The PSLG segments are read from a .poly file. The return value is the
//  number of segments in the file.
//
//***************************************************************************
#ifdef TRILIBRARY
void Mesh::formskeleton(int *segmentlist, int *segmentmarkerlist, int numberofsegments)
#else // not TRILIBRARY
void Mesh::formskeleton(FILE *polyfile, char *polyfilename)
#endif // not TRILIBRARY
{
#ifdef TRILIBRARY
  char polyfilename[6];
  int index;
#else // not TRILIBRARY
  char inputline[INPUTLINESIZE];
  char *stringptr;
#endif // not TRILIBRARY
  Vertex endpoint1, endpoint2;
  int segmentmarkers;
  int end1, end2;
  int boundmarker;
  int i;

  if (m_b.poly) {
    if (!m_b.quiet) {
      printf("Recovering segments in Delaunay triangulation.\n");
    }
#ifdef TRILIBRARY
    strcpy(polyfilename, "input");
    insegments = numberofsegments;
    segmentmarkers = segmentmarkerlist != NULL;
    index = 0;
#else // not TRILIBRARY
    // Read the segments from a .poly file.
    // Read number of segments and number of boundary markers.
    stringptr = readline(inputline, polyfile, polyfilename);
    insegments = (int) strtol(stringptr, &stringptr, 0);
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      segmentmarkers = 0;
    } else {
      segmentmarkers = (int) strtol(stringptr, &stringptr, 0);
    }
#endif // not TRILIBRARY

    // If the input vertices are collinear, there is no triangulation,
    // so don't try to insert segments.
    if (triangles.getItems() == 0) {
      return;
    }

    // If segments are to be inserted, compute a mapping from vertices to triangles.
    if (insegments > 0) {
      makevertexmap();
      if (m_b.verbose) {
        printf("  Recovering PSLG segments.\n");
      }
    }

    boundmarker = 0;
    // Read and insert the segments.
    for (i = 0; i < insegments; i++) {
#ifdef TRILIBRARY
      end1 = segmentlist[index++];
      end2 = segmentlist[index++];
      if (segmentmarkers) {
        boundmarker = segmentmarkerlist[i];
      }
#else // not TRILIBRARY
      stringptr = readline(inputline, polyfile, m_b.inpolyfilename);
      stringptr = findfield(stringptr);
      if(*stringptr == '\0') {
        triError(_T("Segment %d has no endpoints in %s"), m_b.firstnumber + i, String(polyfilename).cstr());
      } else {
        end1 = (int) strtol(stringptr, &stringptr, 0);
      }
      stringptr = findfield(stringptr);
      if(*stringptr == '\0') {
        triError(_T("Segment %d is missing its second endpoint in %s"), m_b.firstnumber + i, String(polyfilename).cstr());
      } else {
        end2 = (int)strtol(stringptr, &stringptr, 0);
      }
      if(segmentmarkers) {
        stringptr = findfield(stringptr);
        if(*stringptr == '\0') {
          boundmarker = 0;
        } else {
          boundmarker = (int)strtol(stringptr, &stringptr, 0);
        }
      }
#endif // not TRILIBRARY
      if((end1 < m_b.firstnumber) || (end1 >= m_b.firstnumber + invertices)) {
        if(!m_b.quiet) {
          printf("Warning:  Invalid first endpoint of segment %d in %s.\n", m_b.firstnumber + i, polyfilename);
        }
      } else if ((end2 < m_b.firstnumber) || (end2 >= m_b.firstnumber + invertices)) {
        if (!m_b.quiet) {
          printf("Warning:  Invalid second endpoint of segment %d in %s.\n"
                ,m_b.firstnumber + i, polyfilename);
        }
      } else {
        // Find the vertices numbered `end1' and `end2'.
        endpoint1 = getvertex(end1);
        endpoint2 = getvertex(end2);
        if((endpoint1[0] == endpoint2[0]) && (endpoint1[1] == endpoint2[1])) {
          if(!m_b.quiet) {
            printf("Warning:  Endpoints of segment %d are coincident in %s.\n"
                  ,m_b.firstnumber + i, polyfilename);
          }
        } else {
          insertsegment(endpoint1, endpoint2, boundmarker);
        }
      }
    }
  } else {
    insegments = 0;
  }
  if(m_b.convex || !m_b.poly) {
    // Enclose the convex hull with subsegments.
    if(m_b.verbose) {
      printf("  Enclosing convex hull with segments.\n");
    }
    markhull();
  }
}

//******** Carving out holes and concavities begins here

//***************************************************************************
//
//  infecthull()   Virally infect all of the triangles of the convex hull
//                 that are not protected by subsegments. Where there are
//                 subsegments, set boundary markers as appropriate.
//
//***************************************************************************
void Mesh::infecthull() {
  otri hulltri;
  otri nexttri;
  otri starttri;
  osub hullsubseg;
  Triangle **deadtriangle;
  Vertex horg, hdest;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  if (m_b.verbose) {
    printf("  Marking concavities (external triangles) for elimination.\n");
  }
  // Find a Triangle handle on the hull.
  hulltri.tri = dummytri;
  hulltri.orient = 0;
  symself(hulltri);
  // Remember where we started so we know when to stop.
  otricopy(hulltri, starttri);
  // Go once counterclockwise around the convex hull.
  do {
    // Ignore triangles that are already infected.
    if (!infected(hulltri)) {
      // Is the Triangle protected by a subsegment?
      tspivot(hulltri, hullsubseg);
      if (hullsubseg.ss == dummysub) {
        // The Triangle is not protected; infect it.
        if (!infected(hulltri)) {
          infect(hulltri);
          deadtriangle = viri.alloc();
          *deadtriangle = hulltri.tri;
        }
      } else {
        // The Triangle is protected; set boundary markers if appropriate.
        if (mark(hullsubseg) == 0) {
          setmark(hullsubseg, 1);
          org(hulltri, horg);
          dest(hulltri, hdest);
          if (vertexmark(horg) == 0) {
            setvertexmark(horg, 1);
          }
          if (vertexmark(hdest) == 0) {
            setvertexmark(hdest, 1);
          }
        }
      }
    }
    // To find the next hull edge, go clockwise around the next Vertex.
    lnextself(hulltri);
    oprev(hulltri, nexttri);
    while (nexttri.tri != dummytri) {
      otricopy(nexttri, hulltri);
      oprev(hulltri, nexttri);
    }
  } while (!otriequal(hulltri, starttri));
}

//***************************************************************************
//
//  plague()   Spread the virus from all infected triangles to any neighbors
//             not protected by subsegments. Delete all infected triangles.
//
//  This is the procedure that actually creates holes and concavities.
//
//  This procedure operates in two phases. The first phase identifies all
//  the triangles that will die, and marks them as infected. They are
//  marked to ensure that each Triangle is added to the virus pool only
//  once, so the procedure will terminate.
//
//  The second phase actually eliminates the infected triangles. It also
//  eliminates orphaned vertices.
//
//***************************************************************************
void Mesh::plague() {
  otri testtri;
  otri neighbor;
  Triangle **virusloop;
  Triangle **deadtriangle;
  osub neighborsubseg;
  Vertex testvertex;
  Vertex norg, ndest;
  Vertex deadorg, deaddest, deadapex;
  int killorg;
  Triangle ptr;             // Temporary variable used by sym() and onext().
  SubSeg sptr;              // Temporary variable used by tspivot().

  if (m_b.verbose) {
    printf("  Marking neighbors of marked triangles.\n");
  }
  // Loop through all the infected triangles, spreading the virus to
  // their neighbors, then to their neighbors' neighbors.
  viri.traversalinit();
  virusloop = viri.traverse();
  while (virusloop != NULL) {
    testtri.tri = *virusloop;
    // A Triangle is marked as infected by messing with one of its pointers
    // to subsegments, setting it to an illegal value. Hence, we have to
    // temporarily uninfect this Triangle so that we can examine its
    // adjacent subsegments.
    uninfect(testtri);
    if (m_b.verbose > 2) {
      // Assign the Triangle an orientation for convenience in
      // checking its vertices.
      testtri.orient = 0;
      org(testtri, deadorg);
      dest(testtri, deaddest);
      apex(testtri, deadapex);
      _tprintf(_T("    Checking %s\n"), triangleToString(deadorg,deaddest,deadapex).cstr());
    }
    // Check each of the Triangle's three neighbors.
    for (testtri.orient = 0; testtri.orient < 3; testtri.orient++) {
      // Find the neighbor.
      sym(testtri, neighbor);
      // Check for a subsegment between the Triangle and its neighbor.
      tspivot(testtri, neighborsubseg);
      // Check if the neighbor is nonexistent or already infected.
      if ((neighbor.tri == dummytri) || infected(neighbor)) {
        if (neighborsubseg.ss != dummysub) {
          // There is a subsegment separating the Triangle from its
          // neighbor, but both triangles are dying, so the subsegment dies too.
          subsegdealloc(neighborsubseg.ss);
          if (neighbor.tri != dummytri) {
            // Make sure the subsegment doesn't get deallocated again
            // later when the infected neighbor is visited.
            uninfect(neighbor);
            tsdissolve(neighbor);
            infect(neighbor);
          }
        }
      } else {                   // The neighbor exists and is not infected.
        if (neighborsubseg.ss == dummysub) {
          // There is no subsegment protecting the neighbor, so the neighbor becomes infected.
          if (m_b.verbose > 2) {
            org(neighbor, deadorg);
            dest(neighbor, deaddest);
            apex(neighbor, deadapex);
            _tprintf(_T("    Marking %s\n"), triangleToString(deadorg, deaddest, deadapex).cstr());
          }
          infect(neighbor);
          // Ensure that the neighbor's neighbors will be infected.
          deadtriangle = viri.alloc();
          *deadtriangle = neighbor.tri;
        } else {               // The neighbor is protected by a subsegment.
          // Remove this Triangle from the subsegment.
          stdissolve(neighborsubseg);
          // The subsegment becomes a boundary. Set markers accordingly.
          if (mark(neighborsubseg) == 0) {
            setmark(neighborsubseg, 1);
          }
          org(neighbor, norg);
          dest(neighbor, ndest);
          if (vertexmark(norg) == 0) {
            setvertexmark(norg, 1);
          }
          if (vertexmark(ndest) == 0) {
            setvertexmark(ndest, 1);
          }
        }
      }
    }
    // Remark the Triangle as infected, so it doesn't get added to the virus pool again.
    infect(testtri);
    virusloop = viri.traverse();
  }

  if (m_b.verbose) {
    printf("  Deleting marked triangles.\n");
  }

  viri.traversalinit();
  virusloop = viri.traverse();
  while (virusloop != NULL) {
    testtri.tri = *virusloop;

    // Check each of the three corners of the Triangle for elimination.
    // This is done by walking around each Vertex, checking if it is
    // still connected to at least one live Triangle.
    for (testtri.orient = 0; testtri.orient < 3; testtri.orient++) {
      org(testtri, testvertex);
      // Check if the Vertex has already been tested.
      if (testvertex != NULL) {
        killorg = 1;
        // Mark the corner of the Triangle as having been tested.
        setorg(testtri, NULL);
        // Walk counterclockwise about the Vertex.
        onext(testtri, neighbor);
        // Stop upon reaching a boundary or the starting Triangle.
        while ((neighbor.tri != dummytri) && (!otriequal(neighbor, testtri))) {
          if (infected(neighbor)) {
            // Mark the corner of this Triangle as having been tested.
            setorg(neighbor, NULL);
          } else {
            // A live Triangle. The Vertex survives.
            killorg = 0;
          }
          // Walk counterclockwise about the Vertex.
          onextself(neighbor);
        }
        // If we reached a boundary, we must walk clockwise as well.
        if (neighbor.tri == dummytri) {
          // Walk clockwise about the Vertex.
          oprev(testtri, neighbor);
          // Stop upon reaching a boundary.
          while (neighbor.tri != dummytri) {
            if (infected(neighbor)) {
            // Mark the corner of this Triangle as having been tested.
              setorg(neighbor, NULL);
            } else {
              // A live Triangle. The Vertex survives.
              killorg = 0;
            }
            // Walk clockwise about the Vertex.
            oprevself(neighbor);
          }
        }
        if (killorg) {
          if (m_b.verbose > 1) {
            _tprintf(_T("    Deleting Vertex %s\n"), vertexToString(testvertex).cstr());
          }
          setvertextype(testvertex, UNDEADVERTEX);
          undeads++;
        }
      }
    }

    // Record changes in the number of boundary edges, and disconnect
    // dead triangles from their neighbors.
    for (testtri.orient = 0; testtri.orient < 3; testtri.orient++) {
      sym(testtri, neighbor);
      if (neighbor.tri == dummytri) {
        // There is no neighboring Triangle on this edge, so this edge
        // is a boundary edge. This Triangle is being deleted, so this
        // boundary edge is deleted.
        hullsize--;
      } else {
        // Disconnect the Triangle from its neighbor.
        dissolve(neighbor);
        // There is a neighboring Triangle on this edge, so this edge
        // becomes a boundary edge when this Triangle is deleted.
        hullsize++;
      }
    }
    // Return the dead Triangle to the pool of triangles.
    triangledealloc(testtri.tri);
    virusloop = viri.traverse();
  }
  // Empty the virus pool.
  viri.restart();
}

//***************************************************************************
//
//  regionplague()   Spread regional attributes and/or area constraints
//                   (from a .poly file) throughout the mesh.
//
//  This procedure operates in two phases. The first phase spreads an
//  attribute and/or an area constraint through a (segment-bounded) region.
//  The triangles are marked to ensure that each Triangle is added to the
//  virus pool only once, so the procedure will terminate.
//
//  The second phase uninfects all infected triangles, returning them to
//  normal.
//
//***************************************************************************
void Mesh::regionplague(REAL attribute, REAL area) {
  otri testtri;
  otri neighbor;
  Triangle **virusloop;
  Triangle **regiontri;
  osub neighborsubseg;
  Vertex regionorg, regiondest, regionapex;
  Triangle ptr;             // Temporary variable used by sym() and onext().
  SubSeg sptr;              // Temporary variable used by tspivot().

  if (m_b.verbose > 1) {
    printf("  Marking neighbors of marked triangles.\n");
  }
  // Loop through all the infected triangles, spreading the attribute
  // and/or area constraint to their neighbors, then to their neighbors' neighbors.
  viri.traversalinit();
  virusloop = viri.traverse();
  while (virusloop != NULL) {
    testtri.tri = *virusloop;
    // A Triangle is marked as infected by messing with one of its pointers
    // to subsegments, setting it to an illegal value. Hence, we have to
    // temporarily uninfect this Triangle so that we can examine its
    // adjacent subsegments.
    uninfect(testtri);
    if (m_b.regionattrib) {
      // Set an attribute.
      setelemattribute(testtri, eextras, attribute);
    }
    if (m_b.vararea) {
      // Set an area constraint.
      setareabound(testtri, area);
    }
    if (m_b.verbose > 2) {
      // Assign the Triangle an orientation for convenience in
      // checking its vertices.
      testtri.orient = 0;
      org(testtri, regionorg);
      dest(testtri, regiondest);
      apex(testtri, regionapex);
      _tprintf(_T("    Checking %s\n"), triangleToString(regionorg, regiondest, regionapex).cstr());
    }
    // Check each of the Triangle's three neighbors.
    for (testtri.orient = 0; testtri.orient < 3; testtri.orient++) {
      // Find the neighbor.
      sym(testtri, neighbor);
      // Check for a subsegment between the Triangle and its neighbor.
      tspivot(testtri, neighborsubseg);
      // Make sure the neighbor exists, is not already infected, and
      // isn't protected by a subsegment.
      if ((neighbor.tri != dummytri) && !infected(neighbor) && (neighborsubseg.ss == dummysub)) {
        if (m_b.verbose > 2) {
          org(neighbor, regionorg);
          dest(neighbor, regiondest);
          apex(neighbor, regionapex);
          _tprintf(_T("    Marking %s\n"), triangleToString(regionorg, regiondest, regionapex).cstr());
        }
        // Infect the neighbor.
        infect(neighbor);
        // Ensure that the neighbor's neighbors will be infected.
        regiontri = viri.alloc();
        *regiontri = neighbor.tri;
      }
    }
    // Remark the Triangle as infected, so it doesn't get added to the virus pool again.
    infect(testtri);
    virusloop = viri.traverse();
  }

  // Uninfect all triangles.
  if (m_b.verbose > 1) {
    printf("  Unmarking marked triangles.\n");
  }
  viri.traversalinit();
  virusloop = viri.traverse();
  while (virusloop != NULL) {
    testtri.tri = *virusloop;
    uninfect(testtri);
    virusloop = viri.traverse();
  }
  // Empty the virus pool.
  viri.restart();
}

//***************************************************************************
//
//  carveholes()   Find the holes and infect them. Find the area
//                 constraints and infect them. Infect the convex hull.
//                 Spread the infection and kill triangles. Spread the
//                 area constraints.
//
//  This routine mainly calls other routines to carry out all these functions.
//
//***************************************************************************
void Mesh::carveholes(REAL *holelist, int holes, REAL *regionlist, int regions) {
  otri searchtri;
  otri triangleloop;
  otri *regiontris;
  Triangle **holetri;
  Triangle **regiontri;
  Vertex searchorg, searchdest;
  LocateResult intersect;
  int i;
  Triangle ptr;                         // Temporary variable used by sym().

  if (!(m_b.quiet || (m_b.noholes && m_b.convex))) {
    printf("Removing unwanted triangles.\n");
    if (m_b.verbose && (holes > 0)) {
      printf("  Marking holes for elimination.\n");
    }
  }

  if (regions > 0) {
    // Allocate storage for the triangles in which region points fall.
    regiontris = MALLOC(otri, regions);
  } else {
    regiontris = NULL;
  }

  if (((holes > 0) && !m_b.noholes) || !m_b.convex || (regions > 0)) {
    // Initialize a pool of viri to be used for holes, concavities,
    // regional attributes, and/or regional area constraints.
    viri.poolinit(sizeof(Triangle *), VIRUSPERBLOCK, VIRUSPERBLOCK, 0);
  }

  if (!m_b.convex) {
    // Mark as infected any unprotected triangles on the boundary.
    // This is one way by which concavities are created.
    infecthull();
  }

  if ((holes > 0) && !m_b.noholes) {
    // Infect each Triangle in which a hole lies.
    for (i = 0; i < 2 * holes; i += 2) {
      // Ignore holes that aren't within the bounds of the mesh.
      if ((holelist[i] >= xmin) && (holelist[i] <= xmax)
          && (holelist[i + 1] >= ymin) && (holelist[i + 1] <= ymax)) {
        // Start searching from some Triangle on the outer boundary.
        searchtri.tri = dummytri;
        searchtri.orient = 0;
        symself(searchtri);
        // Ensure that the hole is to the left of this boundary edge;
        // otherwise, locate() will falsely report that the hole
        // falls within the starting Triangle.
        org(searchtri, searchorg);
        dest(searchtri, searchdest);
        if (counterclockwise(searchorg, searchdest, &holelist[i]) > 0.0) {
          // Find a Triangle that contains the hole.
          intersect = locate(&holelist[i], &searchtri);
          if ((intersect != OUTSIDE) && (!infected(searchtri))) {
            // Infect the Triangle. This is done by marking the Triangle
            // as infected and including the Triangle in the virus pool.
            infect(searchtri);
            holetri  = viri.alloc();
            *holetri = searchtri.tri;
          }
        }
      }
    }
  }

  // Now, we have to find all the regions BEFORE we carve the holes, because
  // locate() won't work when the triangulation is no longer convex.
  // (Incidentally, this is the reason why regional attributes and area
  // constraints can't be used when refining a preexisting mesh, which
  // might not be convex; they can only be used with a freshly
  //  triangulated PSLG.)
  if (regions > 0) {
    // Find the starting Triangle for each region.
    for (i = 0; i < regions; i++) {
      regiontris[i].tri = dummytri;
      // Ignore region points that aren't within the bounds of the mesh.
      if ((regionlist[4 * i] >= xmin)     && (regionlist[4 * i]     <= xmax)
       && (regionlist[4 * i + 1] >= ymin) && (regionlist[4 * i + 1] <= ymax)) {
        // Start searching from some Triangle on the outer boundary.
        searchtri.tri = dummytri;
        searchtri.orient = 0;
        symself(searchtri);
        // Ensure that the region point is to the left of this boundary
        // edge; otherwise, locate() will falsely report that the
        // region point falls within the starting Triangle.
        org(searchtri, searchorg);
        dest(searchtri, searchdest);
        if (counterclockwise(searchorg, searchdest, &regionlist[4 * i]) > 0.0) {
          // Find a Triangle that contains the region point.
          intersect = locate(&regionlist[4 * i], &searchtri);
          if ((intersect != OUTSIDE) && (!infected(searchtri))) {
            // Record the Triangle for processing after the holes have been carved.
            otricopy(searchtri, regiontris[i]);
          }
        }
      }
    }
  }

  if (viri.getItems() > 0) {
    // Carve the holes and concavities.
    plague();
  }
  // The virus pool should be empty now.

  if (regions > 0) {
    if (!m_b.quiet) {
      if (m_b.regionattrib) {
        if (m_b.vararea) {
          printf("Spreading regional attributes and area constraints.\n");
        } else {
          printf("Spreading regional attributes.\n");
        }
      } else {
        printf("Spreading regional area constraints.\n");
      }
    }
    if (m_b.regionattrib && !m_b.refine) {
      // Assign every Triangle a regional attribute of zero.
      triangles.traversalinit();
      triangleloop.orient = 0;
      triangleloop.tri = triangletraverse();
      while (triangleloop.tri != NULL) {
        setelemattribute(triangleloop, eextras, 0.0);
        triangleloop.tri = triangletraverse();
      }
    }
    for (i = 0; i < regions; i++) {
      if (regiontris[i].tri != dummytri) {
        // Make sure the Triangle under consideration still exists.
        // It may have been eaten by the virus.
        if (!deadtri(regiontris[i].tri)) {
          // Put one Triangle in the virus pool.
          infect(regiontris[i]);
          regiontri = viri.alloc();
          *regiontri = regiontris[i].tri;
          // Apply one region's attribute and/or area constraint.
          regionplague(regionlist[4 * i + 2], regionlist[4 * i + 3]);
          // The virus pool should be empty now.
        }
      }
    }
    if (m_b.regionattrib && !m_b.refine) {
      // Note the fact that each Triangle has an additional attribute.
      eextras++;
    }
  }

  // Free up memory.
  if (((holes > 0) && !m_b.noholes) || !m_b.convex || (regions > 0)) {
    viri.pooldeinit();
  }
  if (regions > 0) {
    trifree((VOID *) regiontris);
  }
}

//******** Mesh quality maintenance begins here

#ifndef CDT_ONLY

//***************************************************************************
//
//  tallyencs()   Traverse the entire list of subsegments, and check each
//                to see if it is encroached. If so, add it to the list.
//
//***************************************************************************
void Mesh::tallyencs() {
  osub subsegloop;
  int dummy;

  subsegs.traversalinit();
  subsegloop.ssorient = 0;
  subsegloop.ss = subsegtraverse();
  while (subsegloop.ss != NULL) {
    // If the segment is encroached, add it to the list.
    dummy = checkseg4encroach(&subsegloop);
    subsegloop.ss = subsegtraverse();
  }
}

// return a message, telling what to do about precision problems
String getPrecisionMsg() {
  String msg = _T("Try increasing the area criterion and/or reducing the minimum"
                  " allowable angle so that tiny triangles are not created.");
#ifdef SINGLE
  msg += _T("\nAlternatively, try recompiling me with double precision"
            " arithmetic (by removing \"#define SINGLE\" from the"
            " source file or \"-DSINGLE\" from the makefile).");
#endif // SINGLE
  return msg;
}

//***************************************************************************
//
//  splitencsegs()   Split all the encroached subsegments.
//
//  Each encroached subsegment is repaired by splitting it - inserting a
//  Vertex at or near its midpoint. Newly inserted vertices may encroach
//  upon other subsegments; these are also repaired.
//
//  `triflaws' is a flag that specifies whether one should take note of new
//  bad triangles that result from inserting vertices to repair encroached
//  subsegments.
//
//***************************************************************************
void Mesh::splitencsegs(int triflaws) {
  otri enctri;
  otri testtri;
  osub testsh;
  osub currentenc;
  BadSubSeg *encloop;
  Vertex eorg, edest, eapex;
  Vertex newvertex;
  InserVvertexResult success;
  REAL segmentlength, nearestpoweroftwo;
  REAL split;
  REAL multiplier, divisor;
  int acuteorg, acuteorg2, acutedest, acutedest2;
  int dummy;
  int i;
  Triangle ptr;                       // Temporary variable used by stpivot().
  SubSeg sptr;                        // Temporary variable used by snext().

  // Note that steinerleft == -1 if an unlimited number of Steiner points is allowed.
  while ((badsubsegs.getItems() > 0) && (steinerleft != 0)) {
    badsubsegs.traversalinit();
    encloop = badsubsegtraverse();
    while ((encloop != NULL) && (steinerleft != 0)) {
      sdecode(encloop->encsubseg, currentenc);
      sorg(currentenc, eorg);
      sdest(currentenc, edest);
      // Make sure that this segment is still the same segment it was
      // when it was determined to be encroached. If the segment was
      // enqueued multiple times (because several newly inserted
      // vertices encroached it), it may have already been split.
      if (!deadsubseg(currentenc.ss) && (eorg == encloop->subsegorg) && (edest == encloop->subsegdest)) {
        // To decide where to split a segment, we need to know if the
        // segment shares an endpoint with an adjacent segment.
        // The concern is that, if we simply split every encroached
        // segment in its center, two adjacent segments with a small
        // angle between them might lead to an infinite loop; each
        // Vertex added to split one segment will encroach upon the
        // other segment, which must then be split with a Vertex that
        // will encroach upon the first segment, and so on forever.
        // To avoid this, imagine a set of concentric circles, whose
        // radii are powers of two, about each segment endpoint.
        // These concentric circles determine where the segment is
        // split. (If both endpoints are shared with adjacent
        // segments, split the segment in the middle, and apply the
        // concentric circles for later splittings.)

        // Is the origin shared with another segment?
        stpivot(currentenc, enctri);
        lnext(enctri, testtri);
        tspivot(testtri, testsh);
        acuteorg = testsh.ss != dummysub;
        // Is the destination shared with another segment?
        lnextself(testtri);
        tspivot(testtri, testsh);
        acutedest = testsh.ss != dummysub;

        // If we're using Chew's algorithm (rather than Ruppert's)
        // to define encroachment, delete free vertices from the
        // subsegment's diametral circle.
        if (!m_b.conformdel && !acuteorg && !acutedest) {
          apex(enctri, eapex);
          while ((vertextype(eapex) == FREEVERTEX) &&
                 ((eorg[0] - eapex[0]) * (edest[0] - eapex[0]) +
                  (eorg[1] - eapex[1]) * (edest[1] - eapex[1]) < 0.0)) {
            deletevertex(&testtri);
            stpivot(currentenc, enctri);
            apex(enctri, eapex);
            lprev(enctri, testtri);
          }
        }

        // Now, check the other side of the segment, if there's a Triangle there.
        sym(enctri, testtri);
        if (testtri.tri != dummytri) {
          // Is the destination shared with another segment?
          lnextself(testtri);
          tspivot(testtri, testsh);
          acutedest2 = testsh.ss != dummysub;
          acutedest = acutedest || acutedest2;
          // Is the origin shared with another segment?
          lnextself(testtri);
          tspivot(testtri, testsh);
          acuteorg2 = testsh.ss != dummysub;
          acuteorg = acuteorg || acuteorg2;

          // Delete free vertices from the subsegment's diametral circle.
          if (!m_b.conformdel && !acuteorg2 && !acutedest2) {
            org(testtri, eapex);
            while ((vertextype(eapex) == FREEVERTEX) &&
                   ((eorg[0] - eapex[0]) * (edest[0] - eapex[0]) +
                    (eorg[1] - eapex[1]) * (edest[1] - eapex[1]) < 0.0)) {
              deletevertex(&testtri);
              sym(enctri, testtri);
              apex(testtri, eapex);
              lprevself(testtri);
            }
          }
        }

        // Use the concentric circles if exactly one endpoint is shared
        // with another adjacent segment.
        if (acuteorg || acutedest) {
          segmentlength = sqrt((edest[0] - eorg[0]) * (edest[0] - eorg[0]) +
                               (edest[1] - eorg[1]) * (edest[1] - eorg[1]));
          // Find the power of two that most evenly splits the segment.
          // The worst case is a 2:1 ratio between subsegment lengths.
          nearestpoweroftwo = 1.0;
          while (segmentlength > 3.0 * nearestpoweroftwo) {
            nearestpoweroftwo *= 2.0;
          }
          while (segmentlength < 1.5 * nearestpoweroftwo) {
            nearestpoweroftwo *= 0.5;
          }
          // Where do we split the segment?
          split = nearestpoweroftwo / segmentlength;
          if (acutedest) {
            split = 1.0 - split;
          }
        } else {
          // If we're not worried about adjacent segments, split this segment in the middle.
          split = 0.5;
        }

        // Create the new Vertex.
        newvertex = vertices.alloc();
        // Interpolate its coordinate and attributes.
        for (i = 0; i < 2 + nextras; i++) {
          newvertex[i] = eorg[i] + split * (edest[i] - eorg[i]);
        }

        if (!m_b.noexact) {
          // Roundoff in the above calculation may yield a `newvertex'
          // that is not precisely collinear with `eorg' and `edest'.
          // Improve collinearity by one step of iterative refinement.
          multiplier = counterclockwise(eorg, edest, newvertex);
          divisor = ((eorg[0] - edest[0]) * (eorg[0] - edest[0]) +
                     (eorg[1] - edest[1]) * (eorg[1] - edest[1]));
          if ((multiplier != 0.0) && (divisor != 0.0)) {
            multiplier = multiplier / divisor;
            // Watch out for NANs.
            if (multiplier == multiplier) {
              newvertex[0] += multiplier * (edest[1] - eorg[1]);
              newvertex[1] += multiplier * (eorg[0] - edest[0]);
            }
          }
        }

        setvertexmark(newvertex, mark(currentenc));
        setvertextype(newvertex, SEGMENTVERTEX);
        if (m_b.verbose > 1) {
          _tprintf(_T("  Splitting subsegment %s at %s\n")
                  ,edgeToString(eorg, edest).cstr(), vertexToString(newvertex).cstr());
        }
        // Check whether the new Vertex lies on an endpoint.
        if (((newvertex[0] == eorg[0]) && (newvertex[1] == eorg[1])) ||
            ((newvertex[0] == edest[0]) && (newvertex[1] == edest[1]))) {
          triError(_T("Ran out of precision at %s.\n"
                      " Attempted to split a segment to a smaller size than"
                      " can be accommodated by the finite precision of floating point arithmetic.\n%s")
                    , vertexToString(newvertex).cstr()
                    , getPrecisionMsg().cstr());
        }
        // Insert the splitting Vertex. This should always succeed.
        success = insertvertex(newvertex, &enctri, &currentenc, 1, triflaws);
        if ((success != SUCCESSFULVERTEX) && (success != ENCROACHINGVERTEX)) {
          internalerror(__TFUNCTION__, _T("Failure to split a segment"));
        }
        if (steinerleft > 0) {
          steinerleft--;
        }
        // Check the two new subsegments to see if they're encroached.
        dummy = checkseg4encroach(&currentenc);
        snextself(currentenc);
        dummy = checkseg4encroach(&currentenc);
      }

      badsubsegdealloc(encloop);
      encloop = badsubsegtraverse();
    }
  }
}

//  Test every Triangle in the mesh for quality measures.
void Mesh::tallyfaces() {
  otri triangleloop;

  if (m_b.verbose) {
    printf("  Making a list of bad triangles.\n");
  }
  triangles.traversalinit();
  triangleloop.orient = 0;
  triangleloop.tri = triangletraverse();
  while (triangleloop.tri != NULL) {
    // If the Triangle is bad, enqueue it.
    testtriangle(&triangleloop);
    triangleloop.tri = triangletraverse();
  }
}

//***************************************************************************
//
//  splittriangle()   Inserts a Vertex at the circumcenter of a Triangle.
//                    Deletes the newly inserted Vertex if it encroaches
//                    upon a segment.
//
//***************************************************************************
void Mesh::splittriangle(BadTriangle *badtri) {
  otri badotri;
  Vertex borg, bdest, bapex;
  Vertex newvertex;
  REAL xi, eta;
  InserVvertexResult success;
  int errorflag;
  int i;

  decode(badtri->poortri, badotri);
  org(badotri, borg);
  dest(badotri, bdest);
  apex(badotri, bapex);
  // Make sure that this Triangle is still the same Triangle it was
  // when it was tested and determined to be of bad quality.
  // Subsequent transformations may have made it a different Triangle.
  if (!deadtri(badotri.tri) && (borg == badtri->triangorg) &&
      (bdest == badtri->triangdest) && (bapex == badtri->triangapex)) {
    if (m_b.verbose > 1) {
      _tprintf(_T("  Splitting this Triangle at its circumcenter:%s\n"), triangleToString(borg, bdest, bapex).cstr());
    }

    errorflag = 0;
    // Create a new Vertex at the Triangle's circumcenter.
    newvertex = vertices.alloc();
    findcircumcenter(borg, bdest, bapex, newvertex, &xi, &eta, 1);

    // Check whether the new Vertex lies on a Triangle Vertex.
    if (((newvertex[0] == borg[0]) && (newvertex[1] == borg[1])) ||
        ((newvertex[0] == bdest[0]) && (newvertex[1] == bdest[1])) ||
        ((newvertex[0] == bapex[0]) && (newvertex[1] == bapex[1]))) {
      if (!m_b.quiet) {
        vertexFallsOnExistingWarning(newvertex);
        errorflag = 1;
      }
      vertexdealloc(newvertex);
    } else {
      for (i = 2; i < 2 + nextras; i++) {
        // Interpolate the Vertex attributes at the circumcenter.
        newvertex[i] = borg[i] + xi * (bdest[i] - borg[i]) + eta * (bapex[i] - borg[i]);
      }
      // The new Vertex must be in the interior, and therefore is a
      //   free Vertex with a marker of zero.
      setvertexmark(newvertex, 0);
      setvertextype(newvertex, FREEVERTEX);

      // Ensure that the handle `badotri' does not represent the longest
      // edge of the Triangle. This ensures that the circumcenter must
      // fall to the left of this edge, so point location will work.
      // (If the angle org-apex-dest exceeds 90 degrees, then the
      // circumcenter lies outside the org-dest edge, and eta is
      // negative. Roundoff error might prevent eta from being
      // negative when it should be, so I test eta against xi.)
      if (eta < xi) {
        lprevself(badotri);
      }

      // Insert the circumcenter, searching from the edge of the Triangle,
      // and maintain the Delaunay property of the triangulation.
      success = insertvertex(newvertex, &badotri, NULL, 1, 1);
      if (success == SUCCESSFULVERTEX) {
        if (steinerleft > 0) {
          steinerleft--;
        }
      } else if (success == ENCROACHINGVERTEX) {
        // If the newly inserted Vertex encroaches upon a subsegment, delete the new Vertex.
        undovertex();
        if (m_b.verbose > 1) {
          _tprintf(_T("  Rejecting %s\n"), vertexToString(newvertex).cstr());
        }
        vertexdealloc(newvertex);
      } else if (success == VIOLATINGVERTEX) {
        // Failed to insert the new Vertex, but some subsegment was marked as being encroached.
        vertexdealloc(newvertex);
      } else { // success == DUPLICATEVERTEX
        // Couldn't insert the new Vertex because a Vertex is already there.
        if (!m_b.quiet) {
          vertexFallsOnExistingWarning(newvertex);
          errorflag = 1;
        }
        vertexdealloc(newvertex);
      }
    }
    if (errorflag) {
      if (m_b.verbose) {
        _tprintf(_T("The new vertex is at the circumcenter of triangle %s\n")
                ,triangleToString(borg, bdest, bapex).cstr());
      }
      _tprintf(_T("This probably means that I am trying to refine triangles"
               " to a smaller size than can be accommodated by the finite"
               " precision of floating point arithmetic. (You can be"
               " sure of this if I fail to terminate.)\n"));
      _tprintf(_T("%s"), getPrecisionMsg().cstr());
    }
  }
}

//  Remove all the encroached subsegments and bad triangles from the triangulation.
void Mesh::enforcequality() {
  BadTriangle *badtri;
  int i;

  if (!m_b.quiet) {
    printf("Adding Steiner points to enforce quality.\n");
  }
  // Initialize the pool of encroached subsegments.
  badsubsegs.poolinit(sizeof(BadSubSeg), BADSUBSEGPERBLOCK, BADSUBSEGPERBLOCK, 0);
  if (m_b.verbose) {
    printf("  Looking for encroached subsegments.\n");
  }
  // Test all segments to see if they're encroached.
  tallyencs();
  if (m_b.verbose && (badsubsegs.getItems() > 0)) {
    printf("  Splitting encroached subsegments.\n");
  }
  // Fix encroached subsegments without noting bad triangles.
  splitencsegs(0);
  // At this point, if we haven't run out of Steiner points, the
  // triangulation should be (conforming) Delaunay.

  // Next, we worry about enforcing Triangle quality.
  if ((m_b.minangle > 0.0) || m_b.vararea || m_b.fixedarea || m_b.usertest) {
    // Initialize the pool of bad triangles.
    badtriangles.poolinit(sizeof(BadTriangle), BADTRIPERBLOCK, BADTRIPERBLOCK, 0);
    // Initialize the queues of bad triangles.
    for (i = 0; i < 4096; i++) {
      queuefront[i] = NULL;
    }
    firstnonemptyq = -1;
    // Test all triangles to see if they're bad.
    tallyfaces();
    // Initialize the pool of recently flipped triangles.
    flipstackers.poolinit(sizeof(FlipStacker), FLIPSTACKERPERBLOCK, FLIPSTACKERPERBLOCK, 0);
    checkquality = 1;
    if (m_b.verbose) {
      printf("  Splitting bad triangles.\n");
    }
    while ((badtriangles.getItems() > 0) && (steinerleft != 0)) {
      // Fix one bad Triangle by inserting a Vertex at its circumcenter.
      badtri = dequeuebadtriang();
      splittriangle(badtri);
      if (badsubsegs.getItems() > 0) {
        // Put bad Triangle back in queue for another try later.
        enqueuebadtriang(badtri);
        // Fix any encroached subsegments that resulted.
        // Record any new bad triangles that result.
        splitencsegs(1);
      } else {
        // Return the bad Triangle to the pool.
        badtriangles.dealloc(badtri);
      }
    }
  }
  // At this point, if the "-D" switch was selected and we haven't run out
  // of Steiner points, the triangulation should be (conforming) Delaunay
  // and have no low-quality triangles.

  // Might we have run out of Steiner points too soon?
  if (!m_b.quiet && m_b.conformdel && (badsubsegs.getItems() > 0) && (steinerleft == 0)) {
    printf("\nWarning:  I ran out of Steiner points, but the mesh has\n");
    if (badsubsegs.getItems() == 1) {
      printf("  one encroached subsegment, and therefore might not be truly\n");
    } else {
      printf("  %ld encroached subsegments, and therefore might not be truly\n", badsubsegs.getItems());
    }
    printf("  Delaunay. If the Delaunay property is important to you,\n");
    printf("  try increasing the number of Steiner points (controlled by\n");
    printf("  the -S switch) slightly and try again.\n\n");
  }
}

#endif // not CDT_ONLY

//  Create extra nodes for quadratic subparametric elements.
void Mesh::highorder() {
  otri triangleloop, trisym;
  osub checkmark;
  Vertex newvertex;
  Vertex torg, tdest;
  int i;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

  if (!m_b.quiet) {
    printf("Adding vertices for second-order triangles.\n");
  }
  // The following line ensures that dead items in the pool of nodes
  // cannot be allocated for the extra nodes associated with high
  // order elements. This ensures that the primary nodes (at the
  // corners of elements) will occur earlier in the output files, and
  // have lower indices, than the extra nodes.
  vertices.resetDeadItems();

  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  // To loop over the set of edges, loop over all triangles, and look at
  // the three edges of each Triangle. If there isn't another Triangle
  // adjacent to the edge, operate on the edge. If there is another
  // adjacent Triangle, operate on the edge only if the current Triangle
  // has a smaller pointer than its neighbor. This way, each edge is
  // considered only once.
  while (triangleloop.tri != NULL) {
    for (triangleloop.orient = 0; triangleloop.orient < 3; triangleloop.orient++) {
      sym(triangleloop, trisym);
      if ((triangleloop.tri < trisym.tri) || (trisym.tri == dummytri)) {
        org(triangleloop, torg);
        dest(triangleloop, tdest);
        // Create a new node in the middle of the edge. Interpolate its attributes.
        newvertex = vertices.alloc();
        for (i = 0; i < 2 + nextras; i++) {
          newvertex[i] = 0.5 * (torg[i] + tdest[i]);
        }
        // Set the new node's marker to zero or one, depending on whether it lies on a boundary.
        setvertexmark(newvertex, trisym.tri == dummytri);
        setvertextype(newvertex, trisym.tri == dummytri ? FREEVERTEX : SEGMENTVERTEX);
        if (m_b.usesegments) {
          tspivot(triangleloop, checkmark);
          // If this edge is a segment, transfer the marker to the new node.
          if (checkmark.ss != dummysub) {
            setvertexmark(newvertex, mark(checkmark));
            setvertextype(newvertex, SEGMENTVERTEX);
          }
        }
        if (m_b.verbose > 1) {
          _tprintf(_T("  Creating %s\n"), vertexToString(newvertex).cstr());
        }
        // Record the new node in the (one or two) adjacent elements.
        triangleloop.tri[highorderindex + triangleloop.orient] = (Triangle)newvertex;
        if (trisym.tri != dummytri) {
          trisym.tri[highorderindex + trisym.orient] = (Triangle)newvertex;
        }
      }
    }
    triangleloop.tri = triangletraverse();
  }
}

//******** File I/O routines begin here

#ifndef TRILIBRARY

// Read the vertices from a file, which may be a node or.poly file.
void Mesh::readnodes(char *nodefilename, char *polyfilename, FILE **polyfile) {
  FILE *infile = NULL;
  Vertex vertexloop;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  char *infilename;
  REAL x, y;
  int firstnode;
  int nodemarkers;
  int currentmarker;
  int i, j;

  if (m_b.poly) {
    // Read the vertices from a .poly file.
    if (!m_b.quiet) {
      printf("Opening %s.\n", polyfilename);
    }
    *polyfile = FOPEN(polyfilename, "r");
    // Read number of vertices, number of dimensions, number of Vertex
    // attributes, and number of boundary markers.
    stringptr = readline(inputline, *polyfile, polyfilename);
    invertices = (int) strtol(stringptr, &stringptr, 0);
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      mesh_dim = 2;
    } else {
      mesh_dim = (int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      nextras = 0;
    } else {
      nextras = (int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      nodemarkers = 0;
    } else {
      nodemarkers = (int) strtol(stringptr, &stringptr, 0);
    }
    if (invertices > 0) {
      infile = *polyfile;
      infilename = polyfilename;
      readnodefile = 0;
    } else {
      // If the .poly file claims there are zero vertices, that means that
      // the vertices should be read from a separate .node file.
      readnodefile = 1;
      infilename = nodefilename;
    }
  } else {
    readnodefile = 1;
    infilename = nodefilename;
    *polyfile = NULL;
  }

  if (readnodefile) {
    // Read the vertices from a .node file.
    if (!m_b.quiet) {
      printf("Opening %s.\n", nodefilename);
    }
    infile = FOPEN(nodefilename, "r");
    // Read number of vertices, number of dimensions, number of Vertex
    // attributes, and number of boundary markers.
    stringptr = readline(inputline, infile, nodefilename);
    invertices = (int) strtol(stringptr, &stringptr, 0);
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      mesh_dim = 2;
    } else {
      mesh_dim = (int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      nextras = 0;
    } else {
      nextras = (int) strtol(stringptr, &stringptr, 0);
    }
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      nodemarkers = 0;
    } else {
      nodemarkers = (int) strtol(stringptr, &stringptr, 0);
    }
  }

  if (invertices < 3) {
    triError(_T("Input must have at least three input vertices"));
  }
  if (mesh_dim != 2) {
    triError(_T("Triangle only works with two-dimensional meshes"));
  }
  if (nextras == 0) {
    m_b.weighted = 0;
  }

  initializevertexpool();

  // Read the vertices.
  for (i = 0; i < invertices; i++) {
    vertexloop = vertices.alloc();
    stringptr = readline(inputline, infile, infilename);
    if (i == 0) {
      firstnode = (int) strtol(stringptr, &stringptr, 0);
      if ((firstnode == 0) || (firstnode == 1)) {
        m_b.firstnumber = firstnode;
      }
    }
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      triError(_T("Vertex %d has no x coordinate"), m_b.firstnumber + i);
    }
    x = (REAL) strtod(stringptr, &stringptr);
    stringptr = findfield(stringptr);
    if (*stringptr == '\0') {
      triError(_T("Vertex %d has no y coordinate"), m_b.firstnumber + i);
    }
    y = (REAL) strtod(stringptr, &stringptr);
    vertexloop[0] = x;
    vertexloop[1] = y;
    // Read the Vertex attributes.
    for (j = 2; j < 2 + nextras; j++) {
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        vertexloop[j] = 0.0;
      } else {
        vertexloop[j] = (REAL) strtod(stringptr, &stringptr);
      }
    }
    if (nodemarkers) {
      // Read a Vertex marker.
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        setvertexmark(vertexloop, 0);
      } else {
        currentmarker = (int) strtol(stringptr, &stringptr, 0);
        setvertexmark(vertexloop, currentmarker);
      }
    } else {
      // If no markers are specified in the file, they default to zero.
      setvertexmark(vertexloop, 0);
    }
    setvertextype(vertexloop, INPUTVERTEX);
    // Determine the smallest and largest x and y coordinates.
    if (i == 0) {
      xmin = xmax = x;
      ymin = ymax = y;
    } else {
      xmin = (x < xmin) ? x : xmin;
      xmax = (x > xmax) ? x : xmax;
      ymin = (y < ymin) ? y : ymin;
      ymax = (y > ymax) ? y : ymax;
    }
  }
  if (readnodefile) {
    fclose(infile);
  }

  // Nonexistent x value used as a flag to mark circle events in sweepline
  // Delaunay algorithm.
  xminextreme = 10 * xmin - 9 * xmax;
}

//***************************************************************************
//
//  readholes()   Read the holes, and possibly regional attributes and area
//                constraints, from a .poly file.
//
//***************************************************************************
void Mesh::readholes(FILE *polyfile, char *polyfilename, REAL **hlist, int *holes,
                     REAL **rlist, int *regions)
{
  REAL *holelist;
  REAL *regionlist;
  char inputline[INPUTLINESIZE];
  char *stringptr;
  int index;
  int i;

  // Read the holes.
  stringptr = readline(inputline, polyfile, polyfilename);
  *holes = (int) strtol(stringptr, &stringptr, 0);
  if (*holes > 0) {
    holelist = MALLOC(REAL, 2 * *holes);
    *hlist = holelist;
    for (i = 0; i < 2 * *holes; i += 2) {
      stringptr = readline(inputline, polyfile, polyfilename);
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        triError(_T("Hole %d has no x coordinate"), m_b.firstnumber + (i >> 1));
      } else {
        holelist[i] = (REAL) strtod(stringptr, &stringptr);
      }
      stringptr = findfield(stringptr);
      if (*stringptr == '\0') {
        triError(_T("Hole %d has no y coordinate"), m_b.firstnumber + (i >> 1));
      } else {
        holelist[i + 1] = (REAL) strtod(stringptr, &stringptr);
      }
    }
  } else {
    *hlist = NULL;
  }

#ifndef CDT_ONLY
  if ((m_b.regionattrib || m_b.vararea) && !m_b.refine) {
    // Read the area constraints.
    stringptr = readline(inputline, polyfile, polyfilename);
    *regions = (int) strtol(stringptr, &stringptr, 0);
    if (*regions > 0) {
      regionlist = MALLOC(REAL, 4 * *regions);
      *rlist = regionlist;
      index = 0;
      for (i = 0; i < *regions; i++) {
        stringptr = readline(inputline, polyfile, polyfilename);
        stringptr = findfield(stringptr);
        if (*stringptr == '\0') {
          triError(_T("Region %d has no x coordinate"), m_b.firstnumber + i);
        } else {
          regionlist[index++] = (REAL) strtod(stringptr, &stringptr);
        }
        stringptr = findfield(stringptr);
        if (*stringptr == '\0') {
          triError(_T("Region %d has no y coordinate"), m_b.firstnumber + i);
        } else {
          regionlist[index++] = (REAL) strtod(stringptr, &stringptr);
        }
        stringptr = findfield(stringptr);
        if (*stringptr == '\0') {
          triError(_T("Region %d has no region attribute or area constraint"), m_b.firstnumber + i);
        } else {
          regionlist[index++] = (REAL) strtod(stringptr, &stringptr);
        }
        stringptr = findfield(stringptr);
        if (*stringptr == '\0') {
          regionlist[index] = regionlist[index - 1];
        } else {
          regionlist[index] = (REAL) strtod(stringptr, &stringptr);
        }
        index++;
      }
    }
  } else {
    // Set `*regions' to zero to avoid an accidental free() later.
    *regions = 0;
    *rlist = NULL;
  }
#endif // not CDT_ONLY

  fclose(polyfile);
}

//***************************************************************************
//
//  finishfile()   Write the command line to the output file so the user
//                 can remember how the file was generated. Close the file.
//
//***************************************************************************
void finishfile(FILE *outfile, int argc, char **argv) {
  int i;

  fprintf(outfile, "# Generated by");
  for (i = 0; i < argc; i++) {
    fprintf(outfile, " ");
    fputs(argv[i], outfile);
  }
  fprintf(outfile, "\n");
  fclose(outfile);
}

#else // TRILIBRARY

//  Read the vertices from memory.
void Mesh::transfernodes(REAL     *pointlist
                        ,REAL     *pointattriblist
                        ,int      *pointmarkerlist
                        ,int       numberofpoints
                        ,int       numberofpointattribs)
{
  Vertex vertexloop;
  REAL x, y;
  int i, j;
  int coordindex;
  int attribindex;

  invertices = numberofpoints;
  mesh_dim = 2;
  nextras = numberofpointattribs;
  readnodefile = 0;
  if (invertices < 3) {
    triError(_T("Input must have at least three input vertices"));
  }
  if (nextras == 0) {
    m_b.weighted = 0;
  }

  initializevertexpool();

  // Read the vertices.
  coordindex = 0;
  attribindex = 0;
  for (i = 0; i < invertices; i++) {
    vertexloop = vertices.alloc();
    // Read the Vertex coordinates.
    x = vertexloop[0] = pointlist[coordindex++];
    y = vertexloop[1] = pointlist[coordindex++];
    // Read the Vertex attributes.
    for (j = 0; j < numberofpointattribs; j++) {
      vertexloop[2 + j] = pointattriblist[attribindex++];
    }
    if (pointmarkerlist != NULL) {
      // Read a Vertex marker.
      setvertexmark(vertexloop, pointmarkerlist[i]);
    } else {
      // If no markers are specified, they default to zero.
      setvertexmark(vertexloop, 0);
    }
    setvertextype(vertexloop, INPUTVERTEX);
    // Determine the smallest and largest x and y coordinates.
    if (i == 0) {
      xmin = xmax = x;
      ymin = ymax = y;
    } else {
      xmin = (x < xmin) ? x : xmin;
      xmax = (x > xmax) ? x : xmax;
      ymin = (y < ymin) ? y : ymin;
      ymax = (y > ymax) ? y : ymax;
    }
  }

  // Nonexistent x value used as a flag to mark circle events in sweepline
  // Delaunay algorithm.
  xminextreme = 10 * xmin - 9 * xmax;
}

#endif // not TRILIBRARY

//***************************************************************************
//
//  writenodes()   Number the vertices and write them to a .node file.
//
//  To save memory, the Vertex numbers are written over the boundary markers
//  after the vertices are written to a file.
//
//***************************************************************************
#ifdef TRILIBRARY
void Mesh::writenodes(REAL **pointlist, REAL **pointattriblist, int **pointmarkerlist)
#else // not TRILIBRARY
void Mesh::writenodes(char *nodefilename, int argc, char **argv)
#endif // not TRILIBRARY
{
#ifdef TRILIBRARY
  REAL *plist;
  REAL *palist;
  int *pmlist;
  int coordindex;
  int attribindex;
#else // not TRILIBRARY
  FILE *outfile;
#endif // not TRILIBRARY
  Vertex vertexloop;
  long outvertices;
  int vertexnumber;
  int i;

  if (m_b.jettison) {
    outvertices = vertices.getItems() - undeads;
  } else {
    outvertices = vertices.getItems();
  }

#ifdef TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing vertices.\n");
  }
  // Allocate memory for output vertices if necessary.
  if (*pointlist == NULL) {
    *pointlist = MALLOC(REAL, outvertices * 2);
  }
  // Allocate memory for output Vertex attributes if necessary.
  if ((nextras > 0) && (*pointattriblist == NULL)) {
    *pointattriblist = MALLOC(REAL, outvertices * nextras);
  }
  // Allocate memory for output Vertex markers if necessary.
  if (!m_b.nobound && (*pointmarkerlist == NULL)) {
    *pointmarkerlist = MALLOC(int, outvertices);
  }
  plist = *pointlist;
  palist = *pointattriblist;
  pmlist = *pointmarkerlist;
  coordindex = 0;
  attribindex = 0;
#else // not TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing %s.\n", nodefilename);
  }
  outfile = FOPEN(nodefilename, "w");
  // Number of vertices, number of dimensions, number of Vertex attributes,
  // and number of boundary markers (zero or one).
  fprintf(outfile, "%ld  %d  %d  %d\n", outvertices, mesh_dim,
          nextras, 1 - m_b.nobound);
#endif // not TRILIBRARY

  vertices.traversalinit();
  vertexnumber = m_b.firstnumber;
  vertexloop = vertextraverse();
  while (vertexloop != NULL) {
    if (!m_b.jettison || (vertextype(vertexloop) != UNDEADVERTEX)) {
#ifdef TRILIBRARY
      // X and y coordinates.
      plist[coordindex++] = vertexloop[0];
      plist[coordindex++] = vertexloop[1];
      // Vertex attributes.
      for (i = 0; i < nextras; i++) {
        palist[attribindex++] = vertexloop[2 + i];
      }
      if (!m_b.nobound) {
        // Copy the boundary marker.
        pmlist[vertexnumber - m_b.firstnumber] = vertexmark(vertexloop);
      }
#else // not TRILIBRARY
      // Vertex number, x and y coordinates.
      fprintf(outfile, "%4d    %.17g  %.17g", vertexnumber, vertexloop[0],
              vertexloop[1]);
      for (i = 0; i < nextras; i++) {
        // Write an attribute.
        fprintf(outfile, "  %.17g", vertexloop[i + 2]);
      }
      if (m_b.nobound) {
        fprintf(outfile, "\n");
      } else {
        // Write the boundary marker.
        fprintf(outfile, "    %d\n", vertexmark(vertexloop));
      }
#endif // not TRILIBRARY

      setvertexmark(vertexloop, vertexnumber);
      vertexnumber++;
    }
    vertexloop = vertextraverse();
  }

#ifndef TRILIBRARY
  finishfile(outfile, argc, argv);
#endif // not TRILIBRARY
}

//***************************************************************************
//
//  numbernodes()   Number the vertices.
//
//  Each Vertex is assigned a marker equal to its number.
//
//  Used when writenodes() is not called because no .node file is written.
//
//***************************************************************************
void Mesh::numbernodes() {
  Vertex vertexloop;
  int vertexnumber;

  vertices.traversalinit();
  vertexnumber = m_b.firstnumber;
  vertexloop = vertextraverse();
  while (vertexloop != NULL) {
    setvertexmark(vertexloop, vertexnumber);
    if (!m_b.jettison || (vertextype(vertexloop) != UNDEADVERTEX)) {
      vertexnumber++;
    }
    vertexloop = vertextraverse();
  }
}

//  Write the triangles to an .ele file.
#ifdef TRILIBRARY
void Mesh::writeelements(int     **trianglelist, REAL **triangleattriblist)
#else // not TRILIBRARY
void Mesh::writeelements(char *elefilename, int argc, char **argv)
#endif // not TRILIBRARY
{
#ifdef TRILIBRARY
  int *tlist;
  REAL *talist;
  int vertexindex;
  int attribindex;
#else // not TRILIBRARY
  FILE *outfile;
#endif // not TRILIBRARY
  otri triangleloop;
  Vertex p1, p2, p3;
  Vertex mid1, mid2, mid3;
  long elementnumber;
  int i;

#ifdef TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing triangles.\n");
  }
  // Allocate memory for output triangles if necessary.
  if (*trianglelist == NULL) {
    *trianglelist = MALLOC(int, triangles.getItems() * ((m_b.order + 1) * (m_b.order + 2) / 2));
  }
  // Allocate memory for output Triangle attributes if necessary.
  if ((eextras > 0) && (*triangleattriblist == NULL)) {
    *triangleattriblist = MALLOC(REAL, triangles.getItems() * eextras);
  }
  tlist = *trianglelist;
  talist = *triangleattriblist;
  vertexindex = 0;
  attribindex = 0;
#else // not TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing %s.\n", elefilename);
  }
  outfile = FOPEN(elefilename, "w");
  // Number of triangles, vertices per Triangle, attributes per Triangle.
  fprintf(outfile, "%ld  %d  %d\n", triangles.getItems(),
          (m_b.order + 1) * (m_b.order + 2) / 2, eextras);
#endif // not TRILIBRARY

  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  triangleloop.orient = 0;
  elementnumber = m_b.firstnumber;
  while (triangleloop.tri != NULL) {
    org(triangleloop, p1);
    dest(triangleloop, p2);
    apex(triangleloop, p3);
    if (m_b.order == 1) {
#ifdef TRILIBRARY
      tlist[vertexindex++] = vertexmark(p1);
      tlist[vertexindex++] = vertexmark(p2);
      tlist[vertexindex++] = vertexmark(p3);
#else // not TRILIBRARY
      // Triangle number, indices for three vertices.
      fprintf(outfile, "%4ld    %4d  %4d  %4d", elementnumber, vertexmark(p1), vertexmark(p2), vertexmark(p3));
#endif // not TRILIBRARY
    } else {
      mid1 = (Vertex) triangleloop.tri[highorderindex + 1];
      mid2 = (Vertex) triangleloop.tri[highorderindex + 2];
      mid3 = (Vertex) triangleloop.tri[highorderindex];
#ifdef TRILIBRARY
      tlist[vertexindex++] = vertexmark(p1);
      tlist[vertexindex++] = vertexmark(p2);
      tlist[vertexindex++] = vertexmark(p3);
      tlist[vertexindex++] = vertexmark(mid1);
      tlist[vertexindex++] = vertexmark(mid2);
      tlist[vertexindex++] = vertexmark(mid3);
#else // not TRILIBRARY
      // Triangle number, indices for six vertices.
      fprintf(outfile, "%4ld    %4d  %4d  %4d  %4d  %4d  %4d", elementnumber,
              vertexmark(p1), vertexmark(p2), vertexmark(p3), vertexmark(mid1),
              vertexmark(mid2), vertexmark(mid3));
#endif // not TRILIBRARY
    }

#ifdef TRILIBRARY
    for (i = 0; i < eextras; i++) {
      talist[attribindex++] = elemattribute(triangleloop, i);
    }
#else // not TRILIBRARY
    for (i = 0; i < eextras; i++) {
      fprintf(outfile, "  %.17g", elemattribute(triangleloop, i));
    }
    fprintf(outfile, "\n");
#endif // not TRILIBRARY

    triangleloop.tri = triangletraverse();
    elementnumber++;
  }

#ifndef TRILIBRARY
  finishfile(outfile, argc, argv);
#endif // not TRILIBRARY
}

//  Write the segments and holes to a .poly file.
#ifdef TRILIBRARY
void Mesh::writepoly(int     **segmentlist, int **segmentmarkerlist)
#else // not TRILIBRARY
void Mesh::writepoly(char     *polyfilename
                    ,REAL     *holelist
                    ,int       holes
                    ,REAL     *regionlist
                    ,int       regions
                    ,int       argc, char **argv)
#endif // not TRILIBRARY
{
#ifdef TRILIBRARY
  int *slist;
  int *smlist;
  int index;
#else // not TRILIBRARY
  FILE *outfile;
  long holenumber, regionnumber;
#endif // not TRILIBRARY
  osub subsegloop;
  Vertex endpoint1, endpoint2;
  long subsegnumber;

#ifdef TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing segments.\n");
  }
  // Allocate memory for output segments if necessary.
  if (*segmentlist == NULL) {
    *segmentlist = MALLOC(int, subsegs.getItems() * 2);
  }
  // Allocate memory for output segment markers if necessary.
  if (!m_b.nobound && (*segmentmarkerlist == NULL)) {
    *segmentmarkerlist = MALLOC(int, subsegs.getItems());
  }
  slist = *segmentlist;
  smlist = *segmentmarkerlist;
  index = 0;
#else // not TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing %s.\n", polyfilename);
  }
  outfile = FOPEN(polyfilename, "w");
  // The zero indicates that the vertices are in a separate .node file.
  // Followed by number of dimensions, number of Vertex attributes,
  // and number of boundary markers (zero or one).
  fprintf(outfile, "%d  %d  %d  %d\n", 0, mesh_dim, nextras, 1 - m_b.nobound);
  // Number of segments, number of boundary markers (zero or one).
  fprintf(outfile, "%ld  %d\n", subsegs.getItems(), 1 - m_b.nobound);
#endif // not TRILIBRARY

  subsegs.traversalinit();
  subsegloop.ss = subsegtraverse();
  subsegloop.ssorient = 0;
  subsegnumber = m_b.firstnumber;
  while (subsegloop.ss != NULL) {
    sorg(subsegloop, endpoint1);
    sdest(subsegloop, endpoint2);
#ifdef TRILIBRARY
    // Copy indices of the segment's two endpoints.
    slist[index++] = vertexmark(endpoint1);
    slist[index++] = vertexmark(endpoint2);
    if (!m_b.nobound) {
      // Copy the boundary marker.
      smlist[subsegnumber - m_b.firstnumber] = mark(subsegloop);
    }
#else // not TRILIBRARY
    // Segment number, indices of its two endpoints, and possibly a marker.
    if (m_b.nobound) {
      fprintf(outfile, "%4ld    %4d  %4d\n", subsegnumber, vertexmark(endpoint1), vertexmark(endpoint2));
    } else {
      fprintf(outfile, "%4ld    %4d  %4d    %4d\n"
             ,subsegnumber, vertexmark(endpoint1), vertexmark(endpoint2), mark(subsegloop));
    }
#endif // not TRILIBRARY

    subsegloop.ss = subsegtraverse();
    subsegnumber++;
  }

#ifndef TRILIBRARY
#ifndef CDT_ONLY
  fprintf(outfile, "%d\n", holes);
  if (holes > 0) {
    for (holenumber = 0; holenumber < holes; holenumber++) {
      // Hole number, x and y coordinates.
      fprintf(outfile, "%4ld   %.17g  %.17g\n"
             ,m_b.firstnumber + holenumber
             ,holelist[2 * holenumber]
             ,holelist[2 * holenumber + 1]);
    }
  }
  if (regions > 0) {
    fprintf(outfile, "%d\n", regions);
    for (regionnumber = 0; regionnumber < regions; regionnumber++) {
      // Region number, x and y coordinates, attribute, maximum area.
      fprintf(outfile, "%4ld   %.17g  %.17g  %.17g  %.17g\n"
             ,m_b.firstnumber + regionnumber
             ,regionlist[4 * regionnumber]
             ,regionlist[4 * regionnumber + 1]
             ,regionlist[4 * regionnumber + 2]
             ,regionlist[4 * regionnumber + 3]);
    }
  }
#endif // not CDT_ONLY

  finishfile(outfile, argc, argv);
#endif // not TRILIBRARY
}

//  Write the edges to an .edge file.
#ifdef TRILIBRARY
void Mesh::writeedges(int **edgelist, int **edgemarkerlist)
#else // not TRILIBRARY
void Mesh::writeedges(char *edgefilename, int argc, char **argv)
#endif // not TRILIBRARY
{
#ifdef TRILIBRARY
  int *elist;
  int *emlist;
  int index;
#else // not TRILIBRARY
  FILE *outfile;
#endif // not TRILIBRARY
  otri triangleloop, trisym;
  osub checkmark;
  Vertex p1, p2;
  long edgenumber;
  Triangle ptr;                     // Temporary variable used by sym().
  SubSeg sptr;                      // Temporary variable used by tspivot().

#ifdef TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing edges.\n");
  }
  // Allocate memory for edges if necessary.
  if (*edgelist == NULL) {
    *edgelist = MALLOC(int, edges * 2);
  }
  // Allocate memory for edge markers if necessary.
  if (!m_b.nobound && (*edgemarkerlist == NULL)) {
    *edgemarkerlist = MALLOC(int, edges);
  }
  elist = *edgelist;
  emlist = *edgemarkerlist;
  index = 0;
#else // not TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing %s.\n", edgefilename);
  }
  outfile = FOPEN(edgefilename, "w");
  // Number of edges, number of boundary markers (zero or one).
  fprintf(outfile, "%ld  %d\n", edges, 1 - m_b.nobound);
#endif // not TRILIBRARY

  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  edgenumber = m_b.firstnumber;
  // To loop over the set of edges, loop over all triangles, and look at
  // the three edges of each Triangle. If there isn't another Triangle
  // adjacent to the edge, operate on the edge. If there is another
  // adjacent Triangle, operate on the edge only if the current Triangle
  // has a smaller pointer than its neighbor. This way, each edge is
  // considered only once.
  while (triangleloop.tri != NULL) {
    for (triangleloop.orient = 0; triangleloop.orient < 3; triangleloop.orient++) {
      sym(triangleloop, trisym);
      if ((triangleloop.tri < trisym.tri) || (trisym.tri == dummytri)) {
        org(triangleloop, p1);
        dest(triangleloop, p2);
#ifdef TRILIBRARY
        elist[index++] = vertexmark(p1);
        elist[index++] = vertexmark(p2);
#endif // TRILIBRARY
        if (m_b.nobound) {
#ifndef TRILIBRARY
          // Edge number, indices of two endpoints.
          fprintf(outfile, "%4ld   %d  %d\n", edgenumber, vertexmark(p1), vertexmark(p2));
#endif // not TRILIBRARY
        } else {
          // Edge number, indices of two endpoints, and a boundary marker.
          // If there's no subsegment, the boundary marker is zero.
          if (m_b.usesegments) {
            tspivot(triangleloop, checkmark);
            if (checkmark.ss == dummysub) {
#ifdef TRILIBRARY
              emlist[edgenumber - m_b.firstnumber] = 0;
#else // not TRILIBRARY
              fprintf(outfile, "%4ld   %d  %d  %d\n", edgenumber, vertexmark(p1), vertexmark(p2), 0);
#endif // not TRILIBRARY
            } else {
#ifdef TRILIBRARY
              emlist[edgenumber - m_b.firstnumber] = mark(checkmark);
#else // not TRILIBRARY
              fprintf(outfile, "%4ld   %d  %d  %d\n", edgenumber, vertexmark(p1), vertexmark(p2), mark(checkmark));
#endif // not TRILIBRARY
            }
          } else {
#ifdef TRILIBRARY
            emlist[edgenumber - m_b.firstnumber] = trisym.tri == dummytri;
#else // not TRILIBRARY
            fprintf(outfile, "%4ld   %d  %d  %d\n", edgenumber, vertexmark(p1), vertexmark(p2), trisym.tri == dummytri);
#endif // not TRILIBRARY
          }
        }
        edgenumber++;
      }
    }
    triangleloop.tri = triangletraverse();
  }

#ifndef TRILIBRARY
  finishfile(outfile, argc, argv);
#endif // not TRILIBRARY
}

//***************************************************************************
//
//  writevoronoi()   Write the Voronoi diagram to a .v.node and .v.edge
//                   file.
//
//  The Voronoi diagram is the geometric dual of the Delaunay triangulation.
//  Hence, the Voronoi vertices are listed by traversing the Delaunay
//  triangles, and the Voronoi edges are listed by traversing the Delaunay
//  edges.
//
//  WARNING:  In order to assign numbers to the Voronoi vertices, this
//  procedure messes up the subsegments or the extra nodes of every
//  element. Hence, you should call this procedure last.
//
//***************************************************************************
#ifdef TRILIBRARY
void Mesh::writevoronoi(REAL    **vpointlist
                       ,REAL    **vpointattriblist
                       ,int     **vpointmarkerlist
                       ,int     **vedgelist
                       ,int     **vedgemarkerlist
                       ,REAL    **vnormlist)
#else // not TRILIBRARY
void Mesh::writevoronoi(char *vnodefilename, char *vedgefilename, int argc, char **argv)
#endif // not TRILIBRARY
{
#ifdef TRILIBRARY
  REAL *plist;
  REAL *palist;
  int *elist;
  REAL *normlist;
  int coordindex;
  int attribindex;
#else // not TRILIBRARY
  FILE *outfile;
#endif // not TRILIBRARY
  otri triangleloop, trisym;
  Vertex torg, tdest, tapex;
  REAL circumcenter[2];
  REAL xi, eta;
  long vnodenumber, vedgenumber;
  int p1, p2;
  int i;
  Triangle ptr;                         // Temporary variable used by sym().

#ifdef TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing Voronoi vertices.\n");
  }
  // Allocate memory for Voronoi vertices if necessary.
  if (*vpointlist == NULL) {
    *vpointlist = MALLOC(REAL, triangles.getItems() * 2);
  }
  // Allocate memory for Voronoi Vertex attributes if necessary.
  if (*vpointattriblist == NULL) {
    *vpointattriblist = MALLOC(REAL, triangles.getItems() * nextras);
  }
  *vpointmarkerlist = NULL;
  plist = *vpointlist;
  palist = *vpointattriblist;
  coordindex = 0;
  attribindex = 0;
#else // not TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing %s.\n", vnodefilename);
  }
  outfile = FOPEN(vnodefilename, "w");
  // Number of triangles, two dimensions, number of Vertex attributes, no markers.
  fprintf(outfile, "%ld  %d  %d  %d\n", triangles.getItems(), 2, nextras, 0);
#endif // not TRILIBRARY

  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  triangleloop.orient = 0;
  vnodenumber = m_b.firstnumber;
  while (triangleloop.tri != NULL) {
    org(triangleloop, torg);
    dest(triangleloop, tdest);
    apex(triangleloop, tapex);
    findcircumcenter(torg, tdest, tapex, circumcenter, &xi, &eta, 0);
#ifdef TRILIBRARY
    // X and y coordinates.
    plist[coordindex++] = circumcenter[0];
    plist[coordindex++] = circumcenter[1];
    for (i = 2; i < 2 + nextras; i++) {
      // Interpolate the Vertex attributes at the circumcenter.
      palist[attribindex++] = torg[i] + xi * (tdest[i] - torg[i]) + eta * (tapex[i] - torg[i]);
    }
#else // not TRILIBRARY
    // Voronoi Vertex number, x and y coordinates.
    fprintf(outfile, "%4ld    %.17g  %.17g", vnodenumber, circumcenter[0], circumcenter[1]);
    for (i = 2; i < 2 + nextras; i++) {
      // Interpolate the Vertex attributes at the circumcenter.
      fprintf(outfile, "  %.17g", torg[i] + xi * (tdest[i] - torg[i]) + eta * (tapex[i] - torg[i]));
    }
    fprintf(outfile, "\n");
#endif // not TRILIBRARY

    * (int *) (triangleloop.tri + 6) = (int) vnodenumber;
    triangleloop.tri = triangletraverse();
    vnodenumber++;
  }

#ifndef TRILIBRARY
  finishfile(outfile, argc, argv);
#endif // not TRILIBRARY

#ifdef TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing Voronoi edges.\n");
  }
  // Allocate memory for output Voronoi edges if necessary.
  if (*vedgelist == NULL) {
    *vedgelist = MALLOC(int, edges * 2);
  }
  *vedgemarkerlist = NULL;
  // Allocate memory for output Voronoi norms if necessary.
  if (*vnormlist == NULL) {
    *vnormlist = MALLOC(REAL, edges * 2);
  }
  elist = *vedgelist;
  normlist = *vnormlist;
  coordindex = 0;
#else // not TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing %s.\n", vedgefilename);
  }
  outfile = FOPEN(vedgefilename, "w");
  // Number of edges, zero boundary markers.
  fprintf(outfile, "%ld  %d\n", edges, 0);
#endif // not TRILIBRARY

  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  vedgenumber = m_b.firstnumber;
  // To loop over the set of edges, loop over all triangles, and look at
  // the three edges of each Triangle. If there isn't another Triangle
  // adjacent to the edge, operate on the edge. If there is another
  // adjacent Triangle, operate on the edge only if the current Triangle
  // has a smaller pointer than its neighbor. This way, each edge is
  // considered only once.
  while (triangleloop.tri != NULL) {
    for (triangleloop.orient = 0; triangleloop.orient < 3; triangleloop.orient++) {
      sym(triangleloop, trisym);
      if ((triangleloop.tri < trisym.tri) || (trisym.tri == dummytri)) {
        // Find the number of this Triangle (and Voronoi Vertex).
        p1 = * (int *) (triangleloop.tri + 6);
        if (trisym.tri == dummytri) {
          org(triangleloop, torg);
          dest(triangleloop, tdest);
#ifdef TRILIBRARY
          // Copy an infinite ray. Index of one endpoint, and -1.
          elist[coordindex] = p1;
          normlist[coordindex++] = tdest[1] - torg[1];
          elist[coordindex] = -1;
          normlist[coordindex++] = torg[0] - tdest[0];
#else // not TRILIBRARY
          // Write an infinite ray. Edge number, index of one endpoint, -1,
          // and x and y coordinates of a vector representing the direction of the ray.
          fprintf(outfile, "%4ld   %d  %d   %.17g  %.17g\n"
                 ,vedgenumber, p1, -1, tdest[1] - torg[1], torg[0] - tdest[0]);
#endif // not TRILIBRARY
        } else {
          // Find the number of the adjacent Triangle (and Voronoi Vertex).
          p2 = * (int *) (trisym.tri + 6);
          // Finite edge. Write indices of two endpoints.
#ifdef TRILIBRARY
          elist[coordindex] = p1;
          normlist[coordindex++] = 0.0;
          elist[coordindex] = p2;
          normlist[coordindex++] = 0.0;
#else // not TRILIBRARY
          fprintf(outfile, "%4ld   %d  %d\n", vedgenumber, p1, p2);
#endif // not TRILIBRARY
        }
        vedgenumber++;
      }
    }
    triangleloop.tri = triangletraverse();
  }

#ifndef TRILIBRARY
  finishfile(outfile, argc, argv);
#endif // not TRILIBRARY
}

#ifdef TRILIBRARY
void Mesh::writeneighbors(int **neighborlist)
#else // not TRILIBRARY
void Mesh::writeneighbors(char *neighborfilename, int argc, char **argv)
#endif // not TRILIBRARY
{
#ifdef TRILIBRARY
  int *nlist;
  int index;
#else // not TRILIBRARY
  FILE *outfile;
#endif // not TRILIBRARY
  otri triangleloop, trisym;
  long elementnumber;
  int neighbor1, neighbor2, neighbor3;
  Triangle ptr;                         // Temporary variable used by sym().

#ifdef TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing neighbors.\n");
  }
  // Allocate memory for neighbors if necessary.
  if (*neighborlist == NULL) {
    *neighborlist = MALLOC(int, triangles.getItems() * 3);
  }
  nlist = *neighborlist;
  index = 0;
#else // not TRILIBRARY
  if (!m_b.quiet) {
    printf("Writing %s.\n", neighborfilename);
  }
  outfile = FOPEN(neighborfilename, "w");
  // Number of triangles, three neighbors per Triangle.
  fprintf(outfile, "%ld  %d\n", triangles.getItems(), 3);
#endif // not TRILIBRARY

  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  triangleloop.orient = 0;
  elementnumber = m_b.firstnumber;
  while (triangleloop.tri != NULL) {
    * (int *) (triangleloop.tri + 6) = (int) elementnumber;
    triangleloop.tri = triangletraverse();
    elementnumber++;
  }
  * (int *) (dummytri + 6) = -1;

  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  elementnumber = m_b.firstnumber;
  while (triangleloop.tri != NULL) {
    triangleloop.orient = 1;
    sym(triangleloop, trisym);
    neighbor1 = * (int *) (trisym.tri + 6);
    triangleloop.orient = 2;
    sym(triangleloop, trisym);
    neighbor2 = * (int *) (trisym.tri + 6);
    triangleloop.orient = 0;
    sym(triangleloop, trisym);
    neighbor3 = * (int *) (trisym.tri + 6);
#ifdef TRILIBRARY
    nlist[index++] = neighbor1;
    nlist[index++] = neighbor2;
    nlist[index++] = neighbor3;
#else // not TRILIBRARY
    // Triangle number, neighboring Triangle numbers.
    fprintf(outfile, "%4ld    %d  %d  %d\n", elementnumber, neighbor1, neighbor2, neighbor3);
#endif // not TRILIBRARY

    triangleloop.tri = triangletraverse();
    elementnumber++;
  }

#ifndef TRILIBRARY
  finishfile(outfile, argc, argv);
#endif // not TRILIBRARY
}

#ifndef TRILIBRARY
//***************************************************************************
//
//  writeoff()   Write the triangulation to an .off file.
//
//  OFF stands for the Object File Format, a format used by the Geometry
//  Center's Geomview package.
//
//***************************************************************************
void Mesh::writeoff(char *offfilename, int argc, char **argv) {
  FILE *outfile;
  otri triangleloop;
  Vertex vertexloop;
  Vertex p1, p2, p3;
  long outvertices;

  if (!m_b.quiet) {
    printf("Writing %s.\n", offfilename);
  }

  if (m_b.jettison) {
    outvertices = vertices.getItems() - undeads;
  } else {
    outvertices = vertices.getItems();
  }

  outfile = FOPEN(offfilename, "w");
  // Number of vertices, triangles, and edges.
  fprintf(outfile, "OFF\n%ld  %ld  %ld\n", outvertices, triangles.getItems(), edges);

  // Write the vertices.
  vertices.traversalinit();
  vertexloop = vertextraverse();
  while (vertexloop != NULL) {
    if (!m_b.jettison || (vertextype(vertexloop) != UNDEADVERTEX)) {
      // The "0.0" is here because the OFF format uses 3D coordinates.
      fprintf(outfile, " %.17g  %.17g  %.17g\n", vertexloop[0], vertexloop[1], 0.0);
    }
    vertexloop = vertextraverse();
  }

  // Write the triangles.
  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  triangleloop.orient = 0;
  while (triangleloop.tri != NULL) {
    org(triangleloop, p1);
    dest(triangleloop, p2);
    apex(triangleloop, p3);
    // The "3" means a three-Vertex polygon.
    fprintf(outfile, " 3   %4d  %4d  %4d\n"
                   , vertexmark(p1) - m_b.firstnumber
                   , vertexmark(p2) - m_b.firstnumber
                   , vertexmark(p3) - m_b.firstnumber);
    triangleloop.tri = triangletraverse();
  }
  finishfile(outfile, argc, argv);
}

#endif // not TRILIBRARY

// Return a String with statistics about the quality of the mesh
String Mesh::getQualityStatistics() {
  otri triangleloop;
  Vertex p[3];
  REAL cossquaretable[8];
  REAL ratiotable[16];
  REAL dx[3], dy[3];
  REAL edgelength[3];
  REAL dotproduct;
  REAL cossquare;
  REAL triarea;
  REAL shortest, longest;
  REAL trilongest2;
  REAL smallestarea, biggestarea;
  REAL triminaltitude2;
  REAL minaltitude;
  REAL triaspect2;
  REAL worstaspect;
  REAL smallestangle, biggestangle;
  REAL radconst, degconst;
  int angletable[18];
  int aspecttable[16];
  int aspectindex;
  int tendegree;
  int acutebiggest;
  int i, ii, j, k;

  String result;
  result = _T("Mesh quality statistics:\n\n");
  radconst = PI / 18.0;
  degconst = 180.0 / PI;
  for (i = 0; i < 8; i++) {
    cossquaretable[i] = cos(radconst * (REAL) (i + 1));
    cossquaretable[i] = cossquaretable[i] * cossquaretable[i];
  }
  for (i = 0; i < 18; i++) {
    angletable[i] = 0;
  }

  ratiotable[0]  =      1.5;      ratiotable[1]  =     2.0;
  ratiotable[2]  =      2.5;      ratiotable[3]  =     3.0;
  ratiotable[4]  =      4.0;      ratiotable[5]  =     6.0;
  ratiotable[6]  =     10.0;      ratiotable[7]  =    15.0;
  ratiotable[8]  =     25.0;      ratiotable[9]  =    50.0;
  ratiotable[10] =    100.0;      ratiotable[11] =   300.0;
  ratiotable[12] =   1000.0;      ratiotable[13] = 10000.0;
  ratiotable[14] = 100000.0;      ratiotable[15] =     0.0;
  for (i = 0; i < 16; i++) {
    aspecttable[i] = 0;
  }

  worstaspect = 0.0;
  minaltitude = xmax - xmin + ymax - ymin;
  minaltitude = minaltitude * minaltitude;
  shortest = minaltitude;
  longest = 0.0;
  smallestarea = minaltitude;
  biggestarea = 0.0;
  worstaspect = 0.0;
  smallestangle = 0.0;
  biggestangle = 2.0;
  acutebiggest = 1;

  triangles.traversalinit();
  triangleloop.tri = triangletraverse();
  triangleloop.orient = 0;
  while (triangleloop.tri != NULL) {
    org(triangleloop, p[0]);
    dest(triangleloop, p[1]);
    apex(triangleloop, p[2]);
    trilongest2 = 0.0;

    for (i = 0; i < 3; i++) {
      j = plus1mod3[i];
      k = minus1mod3[i];
      dx[i] = p[j][0] - p[k][0];
      dy[i] = p[j][1] - p[k][1];
      edgelength[i] = dx[i] * dx[i] + dy[i] * dy[i];
      if (edgelength[i] > trilongest2) {
        trilongest2 = edgelength[i];
      }
      if (edgelength[i] > longest) {
        longest = edgelength[i];
      }
      if (edgelength[i] < shortest) {
        shortest = edgelength[i];
      }
    }

    triarea = counterclockwise(p[0], p[1], p[2]);
    if (triarea < smallestarea) {
      smallestarea = triarea;
    }
    if (triarea > biggestarea) {
      biggestarea = triarea;
    }
    triminaltitude2 = triarea * triarea / trilongest2;
    if (triminaltitude2 < minaltitude) {
      minaltitude = triminaltitude2;
    }
    triaspect2 = trilongest2 / triminaltitude2;
    if (triaspect2 > worstaspect) {
      worstaspect = triaspect2;
    }
    aspectindex = 0;
    while ((triaspect2 > ratiotable[aspectindex] * ratiotable[aspectindex]) && (aspectindex < 15)) {
      aspectindex++;
    }
    aspecttable[aspectindex]++;

    for (i = 0; i < 3; i++) {
      j = plus1mod3[i];
      k = minus1mod3[i];
      dotproduct = dx[j] * dx[k] + dy[j] * dy[k];
      cossquare = dotproduct * dotproduct / (edgelength[j] * edgelength[k]);
      tendegree = 8;
      for (ii = 7; ii >= 0; ii--) {
        if (cossquare > cossquaretable[ii]) {
          tendegree = ii;
        }
      }
      if (dotproduct <= 0.0) {
        angletable[tendegree]++;
        if (cossquare > smallestangle) {
          smallestangle = cossquare;
        }
        if (acutebiggest && (cossquare < biggestangle)) {
          biggestangle = cossquare;
        }
      } else {
        angletable[17 - tendegree]++;
        if (acutebiggest || (cossquare > biggestangle)) {
          biggestangle = cossquare;
          acutebiggest = 0;
        }
      }
    }
    triangleloop.tri = triangletraverse();
  }

  shortest = sqrt(shortest);
  longest = sqrt(longest);
  minaltitude = sqrt(minaltitude);
  worstaspect = sqrt(worstaspect);
  smallestarea *= 0.5;
  biggestarea *= 0.5;
  if (smallestangle >= 1.0) {
    smallestangle = 0.0;
  } else {
    smallestangle = degconst * acos(sqrt(smallestangle));
  }
  if (biggestangle >= 1.0) {
    biggestangle = 180.0;
  } else {
    if (acutebiggest) {
      biggestangle = degconst * acos(sqrt(biggestangle));
    } else {
      biggestangle = 180.0 - degconst * acos(sqrt(biggestangle));
    }
  }

  result += format(_T("  Smallest area: %16.5g   |  Largest area: %16.5g\n"), smallestarea, biggestarea);
  result += format(_T("  Shortest edge: %16.5g   |  Longest edge: %16.5g\n"), shortest, longest);
  result += format(_T("  Shortest altitude: %12.5g   |  Largest aspect ratio: %8.5g\n\n")
                  , minaltitude, worstaspect);

  result += format(_T("  Triangle aspect ratio histogram:\n"));
  result += format(_T("  1.1547 - %-6.6g    :  %8d    | %6.6g - %-6.6g     :  %8d\n")
                  , ratiotable[0], aspecttable[0], ratiotable[7], ratiotable[8]
                  , aspecttable[8]);
  for (i = 1; i < 7; i++) {
    result += format(_T("  %6.6g - %-6.6g    :  %8d    | %6.6g - %-6.6g     :  %8d\n")
                    , ratiotable[i - 1], ratiotable[i], aspecttable[i]
                    , ratiotable[i + 7], ratiotable[i + 8], aspecttable[i + 8]);
  }
  result += format(_T("  %6.6g - %-6.6g    :  %8d    | %6.6g -            :  %8d\n")
                  , ratiotable[6], ratiotable[7], aspecttable[7], ratiotable[14]
                  , aspecttable[15]);
  result += format(_T("  (Aspect ratio is longest edge divided by shortest altitude)\n\n"));
  result += format(_T("  Smallest angle: %15.5g   |  Largest angle: %15.5g\n\n"), smallestangle, biggestangle);
  result += _T("  Angle histogram:\n");
  for (i = 0; i < 9; i++) {
    result += format(_T("    %3d - %3d degrees:  %8d    |    %3d - %3d degrees:  %8d\n")
                    ,i * 10, i * 10 + 10, angletable[i]
                    ,i * 10 + 90, i * 10 + 100, angletable[i + 9]);
  }
  result += _T("\n");
  return result;
}

// statistics()   return a string  with all sorts of cool facts.
String Mesh::getStatistics() {
  String result;
  result = _T("\nStatistics:\n\n");
  result += format(_T("  Input vertices: %d\n"), invertices);
  if (m_b.refine) {
    result += format(_T("  Input triangles: %d\n"), inelements);
  }
  if (m_b.poly) {
    result += format(_T("  Input segments: %d\n"), insegments);
    if (!m_b.refine) {
      result += format(_T("  Input holes: %d\n"), holes);
    }
  }

  result += format(_T("\n  Mesh vertices: %ld\n"), vertices.getItems() - undeads);
  result += format(_T("  Mesh triangles: %ld\n"), triangles.getItems());
  result += format(_T("  Mesh edges: %ld\n"), edges);
  result += format(_T("  Mesh exterior boundary edges: %ld\n"), hullsize);
  if (m_b.poly || m_b.refine) {
    result += format(_T("  Mesh interior boundary edges: %ld\n"), subsegs.getItems() - hullsize);
    result += format(_T("  Mesh subsegments (constrained edges): %ld\n"), subsegs.getItems());
  }
  result += _T("\n");

  if (m_b.verbose) {
    result += getQualityStatistics();
    result += _T("Memory allocation statistics\n\n");
    result += format(_T("  Maximum number of vertices: %ld\n"), vertices.getMaxItems());
    result += format(_T("  Maximum number of triangles: %ld\n"), triangles.getMaxItems());
    if (subsegs.getMaxItems() > 0) {
      result += format(_T("  Maximum number of subsegments: %ld\n"), subsegs.getMaxItems());
    }
    if (viri.getMaxItems() > 0) {
      result += format(_T("  Maximum number of viri: %ld\n"), viri.getMaxItems());
    }
    if (badsubsegs.getMaxItems() > 0) {
      result += format(_T("  Maximum number of encroached subsegments: %ld\n"), badsubsegs.getMaxItems());
    }
    if (badtriangles.getMaxItems() > 0) {
      result += format(_T("  Maximum number of bad triangles: %ld\n"), badtriangles.getMaxItems());
    }
    if (flipstackers.getMaxItems() > 0) {
      result += format(_T("  Maximum number of stacked Triangle flips: %ld\n"), flipstackers.getMaxItems());
    }
    if (splaynodes.getMaxItems() > 0) {
      result += format(_T("  Maximum number of splay tree nodes: %ld\n"), splaynodes.getMaxItems());
    }
    result += format(_T("  Approximate heap memory use (bytes): %ld\n\n")
                    ,vertices.getMaxItems()     * vertices.getItemBytes()
                   + triangles.getMaxItems()    * triangles.getItemBytes()
                   + subsegs.getMaxItems()      * subsegs.getItemBytes()
                   + viri.getMaxItems()         * viri.getItemBytes()
                   + badsubsegs.getMaxItems()   * badsubsegs.getItemBytes()
                   + badtriangles.getMaxItems() * badtriangles.getItemBytes()
                   + flipstackers.getMaxItems() * flipstackers.getItemBytes()
                   + splaynodes.getMaxItems()   * splaynodes.getItemBytes());

    result += _T("Algorithmic statistics:\n\n");
    if (!m_b.weighted) {
      result += format(_T("  Number of incircle tests: %ld\n"), incirclecount);
    } else {
      result += format(_T("  Number of 3D orientation tests: %ld\n"), orient3dcount);
    }
    result += format(_T("  Number of 2D orientation tests: %ld\n"), counterclockcount);
    if (hyperbolacount > 0) {
      result += format(_T("  Number of right-of-hyperbola tests: %ld\n"), hyperbolacount);
    }
    if (circletopcount > 0) {
      result += format(_T("  Number of circle top computations: %ld\n"), circletopcount);
    }
    if (circumcentercount > 0) {
      result += format(_T("  Number of Triangle circumcenter computations: %ld\n"), circumcentercount);
    }
    result += _T("\n");
  }
  return result;
}

//***************************************************************************
//
//  main() or triangulate()   Gosh, do everything.
//
//  The sequence is roughly as follows. Many of these steps can be skipped,
//  depending on the command line switches.
//
//  - Initialize constants and parse the command line.
//  - Read the vertices from a file and either
//    - triangulate them (no -r), or
//    - read an old mesh from files and reconstruct it (-r).
//  - Insert the PSLG segments (-p), and possibly segments on the convex
//      hull (-c).
//  - Read the holes (-p), regional attributes (-pA), and regional area
//      constraints (-pa). Carve the holes and concavities, and spread the
//      regional attributes and area constraints.
//  - Enforce the constraints on minimum angle (-q) and maximum area (-a).
//      Also enforce the conforming Delaunay property (-q and -a).
//  - Compute the number of edges in the resulting mesh.
//  - Promote the mesh's linear triangles to higher order elements (-o).
//  - Write the output files and print the statistics.
//  - Check the consistency and Delaunay property of the mesh (-C).
//
//***************************************************************************
#ifdef TRILIBRARY
void triangulate(char *triswitches, TriangulateIO *in, TriangulateIO *out, TriangulateIO *vorout)
#else // not TRILIBRARY
int triangleMain(int argc, char **argv)
#endif // not TRILIBRARY
{
  Behavior b;
  Mesh     m(b);
  REAL    *holearray   = NULL;   // Array of holes.
  REAL    *regionarray = NULL;   // Array of regional attributes and area constraints.
#ifndef TRILIBRARY
  FILE    *polyfile;
#endif // not TRILIBRARY
#ifndef NO_TIMER
  // Variables for timing the performance of Triangle. The types are
  // defined in sys/time.h.
  struct timeval tv0, tv1, tv2, tv3, tv4, tv5, tv6;
  struct timezone tz;
#endif // not NO_TIMER

#ifndef NO_TIMER
  gettimeofday(&tv0, &tz);
#endif // not NO_TIMER

#ifdef TRILIBRARY
  b.parsecommandline(1, &triswitches);
#else // not TRILIBRARY
  b.parsecommandline(argc, argv);
#endif // not TRILIBRARY
  m.steinerleft = b.steiner;

#ifdef TRILIBRARY
  m.transfernodes(in->pointlist
                 ,in->pointattributelist
                 ,in->pointmarkerlist
                 ,in->numberofpoints
                 ,in->numberofpointattributes);
#else // not TRILIBRARY
  m.readnodes(b.innodefilename, b.inpolyfilename, &polyfile);
#endif // not TRILIBRARY

#ifndef NO_TIMER
  if (!b.quiet) {
    gettimeofday(&tv1, &tz);
  }
#endif // not NO_TIMER

#ifdef CDT_ONLY
  m.hullsize = m.delaunay();                // Triangulate the vertices.
#else // not CDT_ONLY
  if (b.refine) {                           // Read and reconstruct a mesh.

#ifdef TRILIBRARY
    m.hullsize = m.reconstruct(in->trianglelist
                              ,in->triangleattributelist
                              ,in->trianglearealist
                              ,in->numberoftriangles, in->numberofcorners
                              ,in->numberoftriangleattributes
                              ,in->segmentlist
                              ,in->segmentmarkerlist
                              ,in->numberofsegments);
#else // not TRILIBRARY
    m.hullsize = m.reconstruct(b.inelefilename
                              ,b.areafilename
                              ,b.inpolyfilename
                              ,polyfile);
#endif // not TRILIBRARY
  } else {
    m.hullsize = m.delaunay();              // Triangulate the vertices.
  }
#endif // not CDT_ONLY

#ifndef NO_TIMER
  if (!b.quiet) {
    gettimeofday(&tv2, &tz);
    if (b.refine) {
      printf("Mesh reconstruction");
    } else {
      printf("Delaunay");
    }
    printf(" milliseconds:  %ld\n", 1000l * (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec) / 1000l);
  }
#endif // not NO_TIMER

  // Ensure that no Vertex can be mistaken for a triangular bounding
  // box Vertex in insertvertex().
  m.infvertex1 = NULL;
  m.infvertex2 = NULL;
  m.infvertex3 = NULL;

  if (b.usesegments) {
    m.checksegments = 1;                // Segments will be introduced next.
    if (!b.refine) {
      // Insert PSLG segments and/or convex hull segments.
#ifdef TRILIBRARY
      m.formskeleton(in->segmentlist, in->segmentmarkerlist, in->numberofsegments);
#else // not TRILIBRARY
      m.formskeleton(polyfile, b.inpolyfilename);
#endif // not TRILIBRARY
    }
  }

#ifndef NO_TIMER
  if (!b.quiet) {
    gettimeofday(&tv3, &tz);
    if (b.usesegments && !b.refine) {
      printf("Segment milliseconds:  %ld\n",
             1000l * (tv3.tv_sec - tv2.tv_sec) + (tv3.tv_usec - tv2.tv_usec) / 1000l);
    }
  }
#endif // not NO_TIMER

  if (b.poly && (m.triangles.getItems() > 0)) {
#ifdef TRILIBRARY
    holearray   = in->holelist;
    m.holes     = in->numberofholes;
    regionarray = in->regionlist;
    m.regions   = in->numberofregions;
#else // not TRILIBRARY
    m.readholes(polyfile
               ,b.inpolyfilename
               ,&holearray
               ,&m.holes
               ,&regionarray
               ,&m.regions);
#endif // not TRILIBRARY
    if (!b.refine) {
      // Carve out holes and concavities.
      m.carveholes(holearray, m.holes, regionarray, m.regions);
    }
  } else {
    // Without a PSLG, there can be no holes or regional attributes
    // or area constraints. The following are set to zero to avoid
    // an accidental free() later.
    m.holes   = 0;
    m.regions = 0;
  }

#ifndef NO_TIMER
  if (!b.quiet) {
    gettimeofday(&tv4, &tz);
    if (b.poly && !b.refine) {
      printf("Hole milliseconds:  %ld\n"
            , 1000l * (tv4.tv_sec - tv3.tv_sec) + (tv4.tv_usec - tv3.tv_usec) / 1000l);
    }
  }
#endif // not NO_TIMER

#ifndef CDT_ONLY
  if (b.quality && (m.triangles.getItems() > 0)) {
    m.enforcequality();           // Enforce angle and area constraints.
  }
#endif // not CDT_ONLY

#ifndef NO_TIMER
  if (!b.quiet) {
    gettimeofday(&tv5, &tz);
#ifndef CDT_ONLY
    if (b.quality) {
      printf("Quality milliseconds:  %ld\n",
             1000l * (tv5.tv_sec - tv4.tv_sec) + (tv5.tv_usec - tv4.tv_usec) / 1000l);
    }
#endif // not CDT_ONLY
  }
#endif // not NO_TIMER

  // Calculate the number of edges.
  m.edges = (3l * m.triangles.getItems() + m.hullsize) / 2l;

  if (b.order > 1) {
    m.highorder();       // Promote elements to higher polynomial order.
  }
  if (!b.quiet) {
    printf("\n");
  }

#ifdef TRILIBRARY
  if (b.jettison) {
    out->numberofpoints = m.vertices.getItems() - m.undeads;
  } else {
    out->numberofpoints = m.vertices.getItems();
  }
  out->numberofpointattributes = m.nextras;
  out->numberoftriangles = m.triangles.getItems();
  out->numberofcorners = (b.order + 1) * (b.order + 2) / 2;
  out->numberoftriangleattributes = m.eextras;
  out->numberofedges = m.edges;
  if (b.usesegments) {
    out->numberofsegments = m.subsegs.getItems();
  } else {
    out->numberofsegments = m.hullsize;
  }
  if (vorout != NULL) {
    vorout->numberofpoints          = m.triangles.getItems();
    vorout->numberofpointattributes = m.nextras;
    vorout->numberofedges           = m.edges;
  }
#endif // TRILIBRARY
  // If not using iteration numbers, don't write a .node file if one was
  // read, because the original one would be overwritten!
  if (b.nonodewritten || (b.noiterationnum && m.readnodefile)) {
    if (!b.quiet) {
#ifdef TRILIBRARY
      printf("NOT writing vertices.\n");
#else // not TRILIBRARY
      printf("NOT writing a .node file.\n");
#endif // not TRILIBRARY
    }
    m.numbernodes();         // We must remember to number the vertices.
  } else {
    // writenodes() numbers the vertices too.
#ifdef TRILIBRARY
    m.writenodes(&out->pointlist, &out->pointattributelist, &out->pointmarkerlist);
#else // not TRILIBRARY
    m.writenodes(b.outnodefilename, argc, argv);
#endif // TRILIBRARY
  }
  if (b.noelewritten) {
    if (!b.quiet) {
#ifdef TRILIBRARY
      printf("NOT writing triangles.\n");
#else // not TRILIBRARY
      printf("NOT writing an .ele file.\n");
#endif // not TRILIBRARY
    }
  } else {
#ifdef TRILIBRARY
    m.writeelements(&out->trianglelist, &out->triangleattributelist);
#else // not TRILIBRARY
    m.writeelements(b.outelefilename, argc, argv);
#endif // not TRILIBRARY
  }
  // The -c switch (convex switch) causes a PSLG to be written
  // even if none was read.
  if (b.poly || b.convex) {
    // If not using iteration numbers, don't overwrite the .poly file.
    if (b.nopolywritten || b.noiterationnum) {
      if (!b.quiet) {
#ifdef TRILIBRARY
        printf("NOT writing segments.\n");
#else // not TRILIBRARY
        printf("NOT writing a .poly file.\n");
#endif // not TRILIBRARY
      }
    } else {
#ifdef TRILIBRARY
      m.writepoly(&out->segmentlist, &out->segmentmarkerlist);
      out->numberofholes   = m.holes;
      out->numberofregions = m.regions;
      if (b.poly) {
        out->holelist   = in->holelist;
        out->regionlist = in->regionlist;
      } else {
        out->holelist   = NULL;
        out->regionlist = NULL;
      }
#else // not TRILIBRARY
      m.writepoly(b.outpolyfilename
                 ,holearray
                 ,m.holes
                 ,regionarray
                 ,m.regions
                 ,argc, argv);
#endif // not TRILIBRARY
    }
  }
#ifndef TRILIBRARY
#ifndef CDT_ONLY
  if (m.regions > 0) {
    trifree((VOID*)regionarray);
  }
#endif // not CDT_ONLY
  if (m.holes > 0) {
    trifree((VOID *)holearray);
  }
  if (b.geomview) {
    m.writeoff(b.offfilename, argc, argv);
  }
#endif // not TRILIBRARY
  if (b.edgesout) {
#ifdef TRILIBRARY
    m.writeedges(&out->edgelist, &out->edgemarkerlist);
#else // not TRILIBRARY
    m.writeedges(b.edgefilename, argc, argv);
#endif // not TRILIBRARY
  }
  if (b.voronoi) {
#ifdef TRILIBRARY
    m.writevoronoi(&vorout->pointlist
                  ,&vorout->pointattributelist
                  ,&vorout->pointmarkerlist
                  ,&vorout->edgelist
                  ,&vorout->edgemarkerlist
                  ,&vorout->normlist);
#else // not TRILIBRARY
    m.writevoronoi(b.vnodefilename, b.vedgefilename, argc, argv);
#endif // not TRILIBRARY
  }
  if (b.neighbors) {
#ifdef TRILIBRARY
    m.writeneighbors(&out->neighborlist);
#else // not TRILIBRARY
    m.writeneighbors(b.neighborfilename, argc, argv);
#endif // not TRILIBRARY
  }

  if (!b.quiet) {
#ifndef NO_TIMER
    gettimeofday(&tv6, &tz);
    printf("\nOutput milliseconds:  %ld\n",
           1000l * (tv6.tv_sec - tv5.tv_sec) + (tv6.tv_usec - tv5.tv_usec) / 1000l);
    printf("Total running milliseconds:  %ld\n",
           1000l * (tv6.tv_sec - tv0.tv_sec) + (tv6.tv_usec - tv0.tv_usec) / 1000l);
#endif // not NO_TIMER

    _tprintf(_T("%s"), m.getStatistics().cstr());
  }

#ifndef REDUCED
  if (b.docheck) {
    m.checkmesh();
    m.checkdelaunay();
  }
#endif // not REDUCED

#ifndef TRILIBRARY
  return 0;
#endif // not TRILIBRARY
}
