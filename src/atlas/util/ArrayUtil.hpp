/*
 * (C) Copyright 1996-2014 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */



#ifndef atlas_ArrayUtil_hpp
#define atlas_ArrayUtil_hpp

#include <vector>

//------------------------------------------------------------------------------------------------------

namespace atlas {

template<typename T> struct remove_const          { typedef T type; };
template<typename T> struct remove_const<T const> { typedef T type; };

template<typename T> struct add_const          { typedef const typename remove_const<T>::type type; };
template<typename T> struct add_const<T const> { typedef const T type; };

typedef std::vector<int> ArrayExtents;
typedef std::vector<int> ArrayStrides;
typedef std::vector<int> ArrayIdx;

inline ArrayExtents make_extents(int size1) { return std::vector<int>(1,size1); }
inline ArrayExtents make_extents(int size1, int size2) { std::vector<int> v(2); v[0]=size1; v[1]=size2; return v; }
inline ArrayExtents make_extents(int size1, int size2, int size3) { std::vector<int> v(3); v[0]=size1; v[1]=size2; v[2]=size3; return v; }
inline ArrayExtents make_extents(int size1, int size2, int size3, int size4) { std::vector<int> v(4); v[0]=size1; v[1]=size2; v[2]=size3; v[3]=size4; return v; }

inline ArrayStrides make_strides(int size1) { return std::vector<int>(1,size1); }
inline ArrayStrides make_strides(int size1, int size2) { std::vector<int> v(2); v[0]=size1; v[1]=size2; return v; }
inline ArrayStrides make_strides(int size1, int size2, int size3) { std::vector<int> v(3); v[0]=size1; v[1]=size2; v[2]=size3; return v; }
inline ArrayStrides make_strides(int size1, int size2, int size3, int size4) { std::vector<int> v(4); v[0]=size1; v[1]=size2; v[2]=size3; v[3]=size4; return v; }

inline ArrayIdx make_idx(int size1) { return std::vector<int>(1,size1); }
inline ArrayIdx make_idx(int size1, int size2) { std::vector<int> v(2); v[0]=size1; v[1]=size2; return v; }
inline ArrayIdx make_idx(int size1, int size2, int size3) { std::vector<int> v(3); v[0]=size1; v[1]=size2; v[2]=size3; return v; }
inline ArrayIdx make_idx(int size1, int size2, int size3, int size4) { std::vector<int> v(4); v[0]=size1; v[1]=size2; v[2]=size3; v[3]=size4; return v; }

//------------------------------------------------------------------------------------------------------

// Legacy for compatibility
inline ArrayExtents Extents(int size1) { return std::vector<int>(1,size1); }
inline ArrayExtents Extents(int size1, int size2) { std::vector<int> v(2); v[0]=size1; v[1]=size2; return v; }
inline ArrayExtents Extents(int size1, int size2, int size3) { std::vector<int> v(3); v[0]=size1; v[1]=size2; v[2]=size3; return v; }
inline ArrayExtents Extents(int size1, int size2, int size3, int size4) { std::vector<int> v(4); v[0]=size1; v[1]=size2; v[2]=size3; v[3]=size4; return v; }

} // namespace atlas

#endif