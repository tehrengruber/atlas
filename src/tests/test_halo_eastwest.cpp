/*
 * (C) Copyright 1996-2014 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <sstream>
#include <algorithm>

#define BOOST_TEST_MODULE TestHaloEastWest
#define BOOST_UNIT_TEST_FRAMEWORK_HEADER_ONLY
#include "ecbuild/boost_test_framework.h"

#include "atlas/mpl/MPL.hpp"
#include "atlas/atlas_config.h"
#include "atlas/meshgen/RGG.hpp"
#include "atlas/meshgen/EqualAreaPartitioner.hpp"
#include "atlas/io/Gmsh.hpp"
#include "atlas/mesh/Mesh.hpp"
#include "atlas/mesh/FunctionSpace.hpp"
#include "atlas/mesh/Metadata.hpp"
#include "atlas/mesh/ArrayView.hpp"
#include "atlas/mesh/IndexView.hpp"
#include "atlas/actions/BuildHalo.hpp"
#include "atlas/mesh/Parameters.hpp"

using namespace atlas;
using namespace atlas::meshgen;

#define DISABLE if(0)
#define ENABLE if(1)

namespace atlas {
namespace test {

} // end namespace test
} // end namespace atlas

BOOST_AUTO_TEST_CASE( init ) { MPL::init(); }

BOOST_AUTO_TEST_CASE( test_halo_2parts )
{
  RGGMeshGenerator generate;
  Mesh* m = generate( T63() );

  actions::build_halo(*m,1);

  std::stringstream filename; filename << "T63_halo_EW.msh";
  Gmsh::write(*m,filename.str());
  delete m;
}

BOOST_AUTO_TEST_CASE( finalize ) { MPL::finalize(); }