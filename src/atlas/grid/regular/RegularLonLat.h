#ifndef atlas_grid_regular_RegularLonLat_h
#define atlas_grid_regular_RegularLonLat_h

#include "atlas/grid/regular/Regular.h"
#include "atlas/grid/domain/Domain.h"
#include "atlas/grid/spacing/Spacing.h"
#include "atlas/grid/projection/Projection.h"

namespace atlas {
namespace grid {
namespace regular {

class RegularLonLat: public Regular {

  public:

    static std::string grid_type_str();

    static std::string className();

    RegularLonLat();
    RegularLonLat(const util::Config& params);

    eckit::Properties spec() const;
    
  protected:

    void setup(long nlon, long nlat);

    //virtual void set_typeinfo() = 0;
    //static eckit::Value domain_spec(const Domain& dom);
    
    bool shiftLon_;
    bool shiftLat_;

};


}  // namespace regular
}  // namespace grid
}  // namespace atlas


#endif
