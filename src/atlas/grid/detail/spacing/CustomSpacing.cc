#include <algorithm>
#include "atlas/grid/detail/spacing/CustomSpacing.h"
#include "eckit/config/Parametrisation.h"
#include "eckit/exception/Exceptions.h"

namespace atlas {
namespace grid {
namespace spacing {

CustomSpacing::CustomSpacing(long N, const double values[], const Interval& interval) {

  x_.assign(values,values+N);
  min_ = std::min(interval[0],interval[1]);
  max_ = std::max(interval[0],interval[1]);
}

CustomSpacing::CustomSpacing(const eckit::Parametrisation& params) {

  params.get("values",x_);

  long N;
  if( params.get("N",N) ) {
    ASSERT( x_.size() == N );
  }
  N = x_.size();

  std::vector<double> interval;
  if( params.get("interval",interval) ) {

    min_ = std::min(interval[0],interval[1]);
    max_ = std::max(interval[0],interval[1]);

  } else {

    min_ = x_.front();
    max_ = x_.front();
    for( size_t j=1; j<N; ++j ) {
      min_ = std::min(min_, x_[j]);
      max_ = std::max(max_, x_[j]);
    }
  }
}

register_BuilderT1(Spacing,CustomSpacing,CustomSpacing::static_type());

}  // namespace spacing
}  // namespace grid
}  // namespace atlas

