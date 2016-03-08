// TL399

#include "atlas/grid/predefined/rgg/rgg.h"

namespace atlas {
namespace grid {
namespace predefined {
namespace rgg {

eckit::ConcreteBuilderT1<Grid,N200> deprecated_builder_N200 (N200::grid_type_str());
eckit::ConcreteBuilderT1<Grid,N200> builder_N200 ("N200");

void N200::construct()
{
  int N=200;
  long lon[] = {
      18,
      25,
      36,
      40,
      45,
      50,
      60,
      64,
      72,
      72,
      75,
      81,
      90,
      96,
     100,
     108,
     120,
     125,
     128,
     135,
     144,
     150,
     160,
     160,
     180,
     180,
     180,
     192,
     192,
     200,
     216,
     216,
     225,
     225,
     240,
     240,
     243,
     250,
     256,
     270,
     270,
     288,
     288,
     288,
     300,
     300,
     320,
     320,
     320,
     320,
     360,
     360,
     360,
     360,
     360,
     360,
     375,
     375,
     375,
     384,
     400,
     400,
     400,
     400,
     432,
     432,
     432,
     432,
     432,
     450,
     450,
     450,
     480,
     480,
     480,
     480,
     480,
     480,
     486,
     500,
     500,
     500,
     512,
     512,
     512,
     540,
     540,
     540,
     540,
     540,
     576,
     576,
     576,
     576,
     576,
     576,
     576,
     576,
     600,
     600,
     600,
     600,
     600,
     640,
     640,
     640,
     640,
     640,
     640,
     640,
     640,
     640,
     640,
     648,
     648,
     675,
     675,
     675,
     675,
     675,
     675,
     675,
     720,
     720,
     720,
     720,
     720,
     720,
     720,
     720,
     720,
     720,
     720,
     720,
     720,
     720,
     729,
     729,
     729,
     750,
     750,
     750,
     750,
     750,
     750,
     750,
     750,
     768,
     768,
     768,
     768,
     768,
     768,
     768,
     768,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800,
     800
  };
  std::vector<double> lats(N);
  gaussian_latitudes_npole_equator(N,lats.data());
  setup_lat_hemisphere(N,lats.data(),lon);
}

} // namespace rgg
} // namespace predefined
} // namespace grid
} // namespace atlas
