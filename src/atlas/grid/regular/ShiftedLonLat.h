#ifndef atlas_grid_regular_ShiftedLonLat_h
#define atlas_grid_regular_ShiftedLonLat_h

#include "atlas/grid/regular/GlobalLonLat.h"
#include "atlas/grid/domain/Domain.h"
#include "atlas/grid/spacing/Spacing.h"
#include "atlas/grid/projection/Projection.h"

namespace atlas {
namespace grid {
namespace regular {

class ShiftedLonLat: public GlobalLonLat {

  public:

    static std::string grid_type_str();

    static std::string className();

    ShiftedLonLat(const util::Config& params);

    eckit::Properties spec() const;
    
  protected:

    void setup();

    //virtual void set_typeinfo() = 0;
    //static eckit::Value domain_spec(const Domain& dom);

};


}  // namespace regular
}  // namespace grid
}  // namespace atlas


#endif
