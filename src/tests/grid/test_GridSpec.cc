/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <string>
#include <iostream>

#define BOOST_TEST_MODULE TestGridSpec
#include "ecbuild/boost_test_framework.h"

#include "eckit/filesystem/PathName.h"
#include "eckit/filesystem/LocalPathName.h"

#include "eckit/grib/GribHandle.h"
#include "eckit/grib/GribAccessor.h"
#include "eckit/types/FloatCompare.h"

#include "atlas/io/Grib.h"
#include "atlas/GridSpec.h"
#include "atlas/grids/grids.h"

using namespace std;
using namespace eckit;
using namespace eckit::grib;
using namespace atlas;
using namespace atlas::io;
using namespace atlas::grids;

/// Test for Grid* derivatives
/// This test uses the grib samples directory, this grib_api/1.13.0 move to test data server
/// We open each file and attempt to create the Grid* class.

static void test_grib_file(const std::string& file);
static void comparePointList(const std::vector<Grid::Point>& grib_pntlist, const std::vector<Grid::Point>& points, double epsilon, eckit::grib::GribHandle& gh);


BOOST_AUTO_TEST_SUITE( TestGridSpec )

struct ArgsFixture {
   ArgsFixture(): argc(boost::framework::master_test_suite().argc),
           argv(boost::framework::master_test_suite().argv){}
   int argc;
   char **argv;
};

BOOST_FIXTURE_TEST_CASE ( test_grib_to_grid_to_gridspec, ArgsFixture ) {
    cout << "Grid:: ...test_grib_to_grid_to_gridspec argc = " << argc << " ";
    if (argc == 2) cout << argv[1];
    cout << "\n";
    BOOST_REQUIRE_MESSAGE( argc == 2, "You missed one argument" );

    test_grib_file(argv[1]);
}

//BOOST_AUTO_TEST_CASE( test_rotated_grids )
//{
//   cout << "Grid:: ...test_rotated_grids \n";
//
//   // Note: we need to wait till grib iterator, rotates the points.
//   // At the moment(grib 13.1) it just, return the points, in regular lat long fashion.
//   std::string path = "/scratch/ma/ma0/wind_rotated_latlon.grb";
//   test_grib_file( path );
//}

BOOST_AUTO_TEST_SUITE_END()


static void test_grib_file(const std::string& fpath)
{
   std::cout << "   Opening GRIB file " << fpath << std::endl;

   LocalPathName path(fpath);
   eckit::grib::GribHandle gh(path);
   std::string gridType = gh.gridType();
   std::cout << "   Create Grid derivatives " << gridType << std::endl;

   if ( gridType == "polar_stereographic" || gridType == "sh" || gridType.empty())
   {
      // The GRIB samples for polar stereographic are corrupt in terms of the GRID section, hence IGNORE
      std::cout << " ** Ignoring grid types [ polar_stereographic | sh ] " << std::endl;
      return;
   }

   // Create Grid derivatives from the GRIB file
   Grid::Ptr grid_created_from_grib = Grib::create_grid(gh);
   BOOST_CHECK_MESSAGE(grid_created_from_grib,"Grib::create_grid  failed for file " << fpath);
   if (!grid_created_from_grib) return;


   // The Grid produced, has a GRID spec, the grid spec can be used to, make sure the grid types match
   GridSpec g_spec = grid_created_from_grib->spec();
   std::cout << "   " << g_spec << std::endl;
   BOOST_CHECK_MESSAGE(grid_created_from_grib->grid_type() == gridType,"gridType(" << gridType << ") did not match Grid constructor(" << grid_created_from_grib->grid_type() << ") for file " << fpath);
   BOOST_CHECK_MESSAGE(g_spec.grid_type() == gridType,"gridType(" << gridType << ") did not match GridSpec constructor(" << g_spec.grid_type() << ") for file " << fpath);


   // From the Spec, create another Grid, we should get back the same Grid
   Grid::Ptr grid_created_from_spec = Grid::create(g_spec);
   BOOST_CHECK_MESSAGE(grid_created_from_spec,"Failed to create GRID from GridSpec");
   bool grid_compare = grid_created_from_grib->same(*grid_created_from_spec);
   BOOST_CHECK_MESSAGE(grid_compare,"The grids are different");


   // For reduced Guassian Grid, check the no of pts per latitude read from grib, matches the computed values
   if (gridType == "reduced_gg") {

      ReducedGG* read_from_grib = dynamic_cast<ReducedGG*>(grid_created_from_grib.get());
      BOOST_CHECK_MESSAGE(read_from_grib,"Downcast to ReducedGG failed ?");

      if (read_from_grib) {
         // Create on the fly, this will compute no of pts per latitude on the fly
         ReducedGG reducedgg(read_from_grib->N());
         BOOST_CHECK_MESSAGE(read_from_grib->npts_per_lat() == reducedgg.npts_per_lat(),"Pts per latitide read from grib, different to computed pts per latitude");
      }
   }

   // epsilon varies depending on the edition number
   long editionNumber = GribAccessor<long>("editionNumber")(gh);
   BOOST_CHECK_MESSAGE(editionNumber ==  gh.edition(),"Edition numbers dont match");

   double epsilon = (editionNumber == 1) ? 1e-3 : 1e-6;

   if (gridType == "reduced_gg" || gridType == "regular_gg") {

      // ---------------------------------------------------------------------------------------
      // Old Grib samples file have errors in the EXPECTED_longitudeOfLastGridPointInDegrees
      // This can then effect, comparison of the points
      // ----------------------------------------------------------------------------------------
      double guass = GribAccessor<long>("numberOfParallelsBetweenAPoleAndTheEquator")(gh);
      Grid::BoundBox bbox = grid_created_from_grib->bounding_box();

      double EXPECTED_longitudeOfLastGridPointInDegrees = 360.0 - (90.0/guass);
      std::cout << "   EXPECTED longitudeOfLastGridPointInDegrees     " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << EXPECTED_longitudeOfLastGridPointInDegrees << std::endl;
      std::cout << "   east                                           " << std::setprecision(std::numeric_limits<double>::digits10 + 1) << bbox.east() << std::endl;
      BOOST_CHECK_CLOSE(bbox.east(),EXPECTED_longitudeOfLastGridPointInDegrees,epsilon);
   }


   // =============================================================================================
   // Compare GRIB points with GRID points,
   // get GRIB points, caution grib iterator depends on scanning mode, Grid is always left ->right, top -> bottom
   std::vector<Grid::Point> grib_pntlist;
   gh.getLonLatPoints( grib_pntlist );
   BOOST_CHECK_MESSAGE( grid_created_from_grib->npts() == grib_pntlist.size(),"GRIB pt list size " << grib_pntlist.size() << " different to GRID " << grid_created_from_grib->npts());

   // get the GRID points
   std::vector<Grid::Point> grid_points; grid_points.resize( grid_created_from_grib->npts());
   grid_created_from_grib->lonlat(grid_points);

   comparePointList(grib_pntlist,grid_points,epsilon,gh);


   // ===================================================================================================
   // Inverse check given a GridSpec find corresponding GRIB sample file
   // However we need to take into account that the GRIB samples, file are *NOT* unique in their GRID definition.
   // The sample file name produced does not have '.tmpl' extension
   //
   // Comment this section out if we do not want to depend on Grib::grib_sample_file !
   //
   // This test will fail, if grib_api module has not been loaded. Guard against this.
   if ( getenv("GRIB_SAMPLES_PATH") || getenv("GRIB_API_PATH") ) {

      std::string generated_sample_file_name = Grib::grib_sample_file( g_spec , gh.edition());
      BOOST_CHECK_MESSAGE( !generated_sample_file_name.empty(),"   Could *not* find sample file for grid_spec " << g_spec );

      LocalPathName base_name = path.baseName(false);
      std::string grib_sample_file = base_name.localPath();
      BOOST_WARN_MESSAGE( generated_sample_file_name == grib_sample_file, "\n   Could not match samples expected '"
                          << grib_sample_file << "' but found('"
                          << generated_sample_file_name
                          << "') for grid spec "
                          << g_spec );
   }
}

void comparePointList(const std::vector<Grid::Point>& grib_pntlist, const std::vector<Grid::Point>& points, double epsilon, eckit::grib::GribHandle& gh)
{
   BOOST_CHECK_MESSAGE(  points.size() == grib_pntlist.size(),"\n  **GRIB pt list size " << grib_pntlist.size() << " different to GRID " << points.size() );

   RealCompare<double> isEqual(epsilon);

   int print_point_list = 0;
   std::streamsize old_precision = cout.precision();
   for(size_t i =0;  i < points.size() && i < grib_pntlist.size(); i++) {
      if (!isEqual(points[i].lat(),grib_pntlist[i].lat()) ||
          !isEqual(points[i].lon(),grib_pntlist[i].lon()))
      {
         if (print_point_list == 0) {
            Log::info() << " Point list DIFFER, show first 10, epsilon(" << epsilon << ")" << std::endl;
            Log::info() << setw(40) << left << "     GRID   " << setw(40) << left << "         GRIB"<< std::endl;
         }
         Log::info() << setw(3) << i << " :"
                  << setw(20) << std::setprecision(std::numeric_limits<double>::digits10 + 1) << points[i].lat() << ", "
                  << setw(20) << std::setprecision(std::numeric_limits<double>::digits10 + 1) << points[i].lon() << "  "
                  << setw(20) << std::setprecision(std::numeric_limits<double>::digits10 + 1) << grib_pntlist[i].lat() << ", "
                  << setw(20) << std::setprecision(std::numeric_limits<double>::digits10 + 1) << grib_pntlist[i].lon()
                  << std::endl;
         print_point_list++;
         if (print_point_list > 10) break;
      }
   }

   // reset precision
   Log::info() << std::setprecision(old_precision);
}

