/*
 * (C) Copyright 1996-2014 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

/// @author Willem Deconinck
/// @date Jan 2014

#ifndef atlas_grids_rgg_OctahedralRGG_h
#define atlas_grids_rgg_OctahedralRGG_h

#include "eckit/memory/Builder.h"
#include "eckit/value/Params.h"
#include "atlas/grids/ReducedGaussianGrid.h"

namespace atlas {
namespace grids {
namespace rgg {

//------------------------------------------------------------------------------------------------------

class OctahedralRGG : public ReducedGaussianGrid {
public:

  static std::string className() { return "atlas.grids.rgg.OctahedralRGG"; }

  static std::string grid_type_str() { return "rgg4"; }

  OctahedralRGG(const int N);

  OctahedralRGG(Grid::ARG1 arg1);

private:

  void construct(const int N);

  void set_typeinfo();

};

//------------------------------------------------------------------------------------------------------

} // namespace rgg
} // namespace grids
} // namespace atlas

#endif // atlas_grids_rgg_OctahedralRGG_h
