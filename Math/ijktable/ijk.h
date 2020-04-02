/// \file ijk.txx
/// ijk templates defining general ijk objects, i.e. classes BOX and ERROR
/// Version 0.1.0

/*
  IJK: Isosurface Jeneration Kode
  Copyright (C) 2008 Rephael Wenger

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
#include <sstream>
#include <vector>
#include <utility>

/// General purpose ijk classes and routines.
/// Box, line segments, array and error classes.
namespace IJK {

  // **************************************************
  // TEMPLATE CLASS BOX
  // **************************************************

  /// Axis-parallel box data structure.  
  /// Represents box by minimum and maximum coordinates.
  template <class COORD_TYPE> class BOX {
  protected:
    int dimension;
    COORD_TYPE * min_coord;
    COORD_TYPE * max_coord;

    void Init();
    void FreeAll();
    
  public:
    BOX() {
      Init();
    }
    BOX(const int dimension);
    ~BOX() {
      FreeAll();
    }
    BOX(const BOX & box);
    const BOX & operator=(const BOX &);

    int Dimension() const {
      return dimension;
    }
    COORD_TYPE MinCoord(const int d) const {
      return min_coord[d];
    }
    COORD_TYPE MaxCoord(const int d) const {
      return max_coord[d];
    }

    void SetDimension(const int d);
    void SetMinCoord(const int d, const COORD_TYPE c) {
      min_coord[d] = c;
    }
    void SetMaxCoord(const int d, const COORD_TYPE c) {
      max_coord[d] = c;
    }
    void SetAllMinCoord(const COORD_TYPE c);  // set all min coord to c
    void SetAllMaxCoord(const COORD_TYPE c);  // set all max coord to c
  };

  // **************************************************
  // CLASS LINE_SEGMENT & ASSOCIATED FUNCTIONS
  // **************************************************

  /// Line segment between two grid vertices
  template <class VTYPE> class LINE_SEGMENT:public std::pair<VTYPE,VTYPE> {  
  public:
    LINE_SEGMENT() {}
    LINE_SEGMENT(const VTYPE iv0, const VTYPE iv1) {
      SetEnd(iv0, iv1);
    }

    /// Set line segment endpoints. 
    void SetEnd(const VTYPE iv0, const VTYPE iv1) {
      this->first = iv0;
      this->second = iv1;
      Order();
    }

    /// Order endpoints so that  V0() <= V1().
    void Order() {
      if(V0() > V1()) {
        std::swap(this->first, this->second);
      }
    }

    // get functions
    ///< Return endpoint 0.
    VTYPE V0() const {
      return this->first;
    }
    ///< Return endpoint 1.
    VTYPE V1() const {
      return this->second;
    }
    ///< Return endpoint i.
    template <class ITYPE> VTYPE V(const ITYPE i) const {
      if(i == 0) {
        return V0();
      } else {
        return V1();
      }
    }
  };

  /// Return true if (V0 < V1) for every pair (V0,V1)
  //    of line segment endpoints
  template <class VTYPE> bool is_ordered(const std::vector<LINE_SEGMENT<VTYPE> > &list) {
    typename std::vector<LINE_SEGMENT<VTYPE> >::const_iterator pos;
    for(pos = list.begin(); pos != list.end(); ++pos) {
      if(pos->V0() > pos->V1()) {
	    return false;
      }
    }
    return true;
  }

  // **************************************************
  // TEMPLATE CLASS ARRAY AND ARRAY_L
  // **************************************************

  /// Simple array class for creating static arrays
  /// Making this a class guarantees that when a program leaves
  /// the function where this class is declared, the array memory is freed.
  template <class ETYPE> class ARRAY {
  protected:
    ETYPE *element;

    template <class LTYPE> void Init(const LTYPE array_length) {
      element = new ETYPE[array_length];
    }

    template <class LTYPE> void Init(const LTYPE array_length, const ETYPE init_value) { 
      Init(array_length);
      for(LTYPE i = 0; i < array_length; i++) {
        element[i] = init_value;
      }
    }

  public:
    template <class LTYPE> ARRAY(const LTYPE array_length) {
      Init(array_length);
    }
    template <class LTYPE> ARRAY(const LTYPE array_length, const ETYPE init_value) {
      Init(array_length, init_value);
    }
    ~ARRAY();

    // get functions
    ETYPE *Ptr() {
      return element;
    }
    const ETYPE *PtrConst() const {
      return element;
    }
    template <class ITYPE> ETYPE & operator[](const ITYPE i) {
      return element[i];
    }
    template <class ITYPE> ETYPE operator[](const ITYPE i) const {
      return element[i];
    }
    void Free();
  };

  /// Class array with length stored
  template <class ETYPE, class LTYPE> class ARRAY_L:public ARRAY<ETYPE> {
  protected:
    LTYPE length;

    void Init(const LTYPE length) {
      this->length = length;
    }

  public:
    ARRAY_L(const LTYPE length):ARRAY<ETYPE>(length) {
      Init(length);
    }

    ARRAY_L(const LTYPE length, const ETYPE init_value) : ARRAY<ETYPE>(length, init_value) {
      Init(length);
    }

    LTYPE Length() const {
      return length;
    }
    ETYPE *End() {
      return this->element+length;
    }
  };

  // **************************************************
  // TEMPLATE CLASS CONSTANT
  // **************************************************

  /// Class CONSTANT always returns the same value
  template <class ITYPE, class CTYPE> class CONSTANT {
  protected:
    CTYPE c;           ///< The constant value.

  public:
    CONSTANT(const CTYPE c) {
      this->c = c;
    }

    CTYPE operator[](const ITYPE i) const {
      return c;
    }
    CTYPE operator()(const ITYPE i) const {
      return c;
    }
  };

  // **************************************************
  // INTEGER POWER FUNCTION
  // **************************************************

  /// Integer power function.
  /// Return (base)^p.
  template<class ITYPE> ITYPE int_power(const ITYPE base, const ITYPE p) {
    ITYPE result = 1;
    for(ITYPE k = 0; k < p; k++) {
      result *= base;
    }
    return result;
  }

  // **************************************************
  // ERROR CLASSES
  // **************************************************

  /// Compose string from multiple elements.
  /// Format string using output string stream.
  template<class T1, class T2, class T3> std::string compose_string(T1 a1, T2 a2, T3 a3) {
    std::ostringstream os0;
    os0 << a1 << a2 << a3;
    return os0.str();
  }

  /// Compose string from multiple elements.
  /// Format string using output string stream.
  template < class T1, class T2, class T3, class T4, class T5 > 
  std::string compose_string(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5)
  {
    std::ostringstream os0;
    os0 << a1 << a2 << a3 << a4 << a5;
    return os0.str();
  }

  /// Compose string from multiple elements.
  /// Format string using output string stream.
  template < class T1, class T2, class T3, class T4, class T5, class T6, class T7 > 
  std::string compose_string(T1 a1, T2 a2, T3 a3, T4 a4, T5 a5, T6 a6, T7 a7)
  {
    std::ostringstream os0;
    os0 << a1 << a2 << a3 << a4 << a5 << a6 << a7;
    return os0.str();
  }

  // **************************************************
  // TEMPLATE CLASS BOX MEMBER FUNCTIONS
  // **************************************************

  template <class T> void BOX<T>::Init() {
    dimension = 0;
    min_coord = 0;
    max_coord = 0;
  }

  template <class T> void BOX<T>::FreeAll() {
    dimension = 0;
    if (min_coord != NULL) { delete [] min_coord; }
    if (max_coord != NULL) { delete [] max_coord; }
    min_coord = NULL;
    max_coord = NULL;
  }

  template <class T> BOX<T>::BOX(const int dimension) {
    Init();
    SetDimension(dimension);
  }

  template <class COORD_TYPE> void BOX<COORD_TYPE>::SetDimension(const int d) {
    FreeAll();
    Init();
    if (d <= 0) return;
    min_coord = new COORD_TYPE[d];
    max_coord = new COORD_TYPE[d];
    dimension = d;
  }

  template <class COORD_TYPE> void BOX<COORD_TYPE>::SetAllMinCoord(const COORD_TYPE c) {
    for(int d = 0; d < dimension; d++) {
      SetMinCoord(d, c);
    }
  }

  template <class COORD_TYPE> void BOX<COORD_TYPE>::SetAllMaxCoord(const COORD_TYPE c) {
    for(int d = 0; d < dimension; d++) {
      SetMaxCoord(d, c);
    }
  }

  template <class COORD_TYPE> BOX<COORD_TYPE>::BOX(const BOX<COORD_TYPE> & box) {
    Init();
    SetDimension(box.Dimension());
    for(int d = 0; d < box.Dimension(); d++) {
      SetMinCoord(d, box.MinCoord(d));
      SetMaxCoord(d, box.MaxCoord(d));
    }
  }

  template <class COORD_TYPE> const BOX<COORD_TYPE> & BOX<COORD_TYPE>::operator=(const BOX<COORD_TYPE> & right)   {
    if(&right != this) {         // avoid self-assignment
      FreeAll();
      SetDimension(right.Dimension());
      for (int d = 0; d < right.Dimension(); d++) {
	    SetMinCoord(d, right.MinCoord(d));
	    SetMaxCoord(d, right.MaxCoord(d));
      }
    }
    return *this;
  }

  // **************************************************
  // TEMPLATE CLASS ARRAY MEMBER FUNCTIONS
  // **************************************************

  template <class ETYPE> ARRAY<ETYPE>::~ARRAY() {
    Free();
  }

  template <class ETYPE> void ARRAY<ETYPE>::Free() {
    delete [] element;
    element = NULL;
  }
}; // namespace IJK
