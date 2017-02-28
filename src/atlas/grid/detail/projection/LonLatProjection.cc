#include "atlas/grid/detail/projection/LonLatProjection.h"

namespace atlas {
namespace grid {
namespace projection {

template <typename Rotation>
LonLatProjectionT<Rotation>::LonLatProjectionT(const eckit::Parametrisation& params) :
  Projection(),
  rotation_(params) {
}

// copy constructor
template <typename Rotation>
LonLatProjectionT<Rotation>::LonLatProjectionT( const LonLatProjectionT& rhs ) :
  Projection(rhs),
  rotation_(rhs.rotation_) {
}

// clone method
template <typename Rotation>
Projection* LonLatProjectionT<Rotation>::clone() const  {
  return new LonLatProjectionT<Rotation>(*this);
}

// specification
template <typename Rotation>
eckit::Properties LonLatProjectionT<Rotation>::spec() const {
  eckit::Properties proj_spec;
  proj_spec.set("projectionType",static_type());
  rotation_.spec(proj_spec);
  return proj_spec;
}

register_BuilderT1(Projection,LonLatProjection,LonLatProjection::static_type());
register_BuilderT1(Projection,RotatedLonLatProjection,RotatedLonLatProjection::static_type());

}  // namespace projection
}  // namespace grid
}  // namespace atlas

