#ifndef atlas_FocusSpacing_H
#define atlas_FocusSpacing_H

#include "atlas/grid/detail/spacing/Spacing.h"

namespace atlas {
namespace grid {
namespace spacing {

class FocusSpacing: public Spacing {

public:

    // constructor
    FocusSpacing(const eckit::Parametrisation& p);

    // class name
    static std::string static_type() {return "focus";}
    virtual std::string type() const {return static_type();}

private:

    double focus_factor_;
};

}  // namespace spacing
}  // namespace grid
}  // namespace atlas


#endif
