#pragma once

#include "atlas/grid/detail/grid/Structured.h"

namespace atlas {
namespace grid {
namespace detail {
namespace grid {
namespace reduced {

class ReducedGaussian: public Structured {

public:

    static std::string grid_type_str();

    static std::string className();

    virtual std::string shortName() const;
    virtual std::string gridType() const { return "reduced_gaussian"; }

    ReducedGaussian( const Config& );
    ReducedGaussian( const int N, const long pl[] );

    virtual Spec spec() const;

protected:

    ReducedGaussian();

    void setup(const size_t N, const long pl[]);

};


}  // namespace reduced
}  // namespace grid
}  // namespace detail
}  // namespace grid
}  // namespace atlas
