/*
 * (C) Copyright 1996-2016 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#ifndef atlas_interpolation_method_KNearestNeighbours_h
#define atlas_interpolation_method_KNearestNeighbours_h

#include "atlas/interpolation/method/KNearestNeighboursBase.h"


namespace atlas {
namespace interpolation {
namespace method {


class KNearestNeighbours : public KNearestNeighboursBase {
public:

    KNearestNeighbours(const Config& config);
    virtual ~KNearestNeighbours() {}

protected:

    /**
     * @brief Create an interpolant sparse matrix relating two (pre-partitioned) meshes,
     * using the k-nearest neighbours method
     * @param meshSource mesh containing source elements
     * @param meshTarget mesh containing target points
     */
    void setup(mesh::Mesh& meshSource, mesh::Mesh& meshTarget);

    size_t k_;

};


}  // method
}  // interpolation
}  // atlas


#endif
