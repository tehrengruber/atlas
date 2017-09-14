/*
 * (C) Copyright 1996-2017 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "eckit/utils/MD5.h"
#include "eckit/os/BackTrace.h"
#include "atlas/parallel/mpi/mpi.h"

#include "atlas/mesh/Mesh.h"
#include "atlas/field/FieldSet.h"
#include "atlas/field/Options.h"
#include "atlas/functionspace/Spectral.h"
#include "atlas/runtime/ErrorHandling.h"
#include "atlas/runtime/Log.h"
#include "atlas/array/MakeView.h"

#ifdef ATLAS_HAVE_TRANS
#include "atlas/trans/Trans.h"
#endif

namespace atlas {
namespace functionspace {
namespace detail {

void Spectral::set_field_metadata(const eckit::Configuration& config, Field& field) const
{
  field.set_functionspace(this);

  bool global(false);
  if( config.get("global",global) )
  {
    if( global )
    {
      size_t owner(0);
      config.get("owner",owner);
      field.metadata().set("owner",owner);
    }
  }
  field.metadata().set("global",global);
  
  field.set_levels( config_levels(config) );
  field.set_variables(0);
}


size_t Spectral::config_size(const eckit::Configuration& config) const
{
  size_t size = nb_spectral_coefficients();
  bool global(false);
  if( config.get("global",global) )
  {
    if( global )
    {
      size_t owner(0);
      config.get("owner",owner);
      size = (parallel::mpi::comm().rank() == owner ? nb_spectral_coefficients_global() : 0);
    }
  }
  return size;
}

// ----------------------------------------------------------------------

Spectral::Spectral(const size_t truncation) :
    truncation_(truncation),
    nb_levels_(0),
    trans_(0)
{
}

Spectral::Spectral(trans::Trans& trans) :
#ifdef ATLAS_HAVE_TRANS
    truncation_(trans.nsmax()),
    trans_(&trans),
#else
    truncation_(0),
    trans_(0),
#endif
    nb_levels_(0)
{
}

Spectral::~Spectral()
{
}

size_t Spectral::footprint() const {
  size_t size = sizeof(*this);
  // TODO
  return size;
}

size_t Spectral::nb_spectral_coefficients() const {
#ifdef ATLAS_HAVE_TRANS
  if( trans_ ) return trans_->nspec2();
#endif
  return (truncation_+1)*(truncation_+2);
}

size_t Spectral::nb_spectral_coefficients_global() const {
#ifdef ATLAS_HAVE_TRANS
  if( trans_ ) return trans_->nspec2g();
#endif
  return (truncation_+1)*(truncation_+2);
}


array::DataType Spectral::config_datatype(const eckit::Configuration& config) const
{
  array::DataType::kind_t kind;
  if( ! config.get("datatype",kind) ) throw eckit::AssertionFailed("datatype missing",Here());
  return array::DataType(kind);
}

std::string Spectral::config_name(const eckit::Configuration& config) const
{
  std::string name;
  config.get("name",name);
  return name;
}

size_t Spectral::config_levels(const eckit::Configuration& config) const
{
  size_t levels(nb_levels_);
  config.get("levels",levels);
  return levels;
}

Field Spectral::createField(const eckit::Configuration& options) const {
  array::ArrayShape array_shape;

  size_t nb_spec_coeffs = config_size(options);
  array_shape.push_back(nb_spec_coeffs);

  size_t levels = config_levels(options);
  if( levels ) array_shape.push_back(levels);

  ATLAS_DEBUG_VAR(array_shape);
  Field field = Field(config_name(options), config_datatype(options), array_shape );

  set_field_metadata(options,field);
  return field;
}

template <>
Field Spectral::createField<float>(const eckit::Configuration& options) const {
  return createField( util::Config(options) | field::datatypeT<float>() );
}

template <>
Field Spectral::createField<double>(const eckit::Configuration& options) const {
  return createField( util::Config(options) | field::datatypeT<double>() );
}

void Spectral::gather( const FieldSet& local_fieldset, FieldSet& global_fieldset ) const
{
  ASSERT( trans_ );
  ASSERT(local_fieldset.size() == global_fieldset.size());

  for( size_t f=0; f<local_fieldset.size(); ++f ) {

    const Field& loc = local_fieldset[f];
    if( loc.datatype() != array::DataType::str<double>() )
    {
      std::stringstream err;
      err << "Cannot gather spectral field " << loc.name() << " of datatype " << loc.datatype().str() << ".";
      err << "Only " << array::DataType::str<double>() << " supported.";
      throw eckit::BadValue(err.str());
    }

#ifdef ATLAS_HAVE_TRANS
    Field& glb = global_fieldset[f];
    size_t root=0;
    glb.metadata().get("owner",root);
    ASSERT( loc.shape(0) == nb_spectral_coefficients() );
    if( parallel::mpi::comm().rank() == root )
      ASSERT( glb.shape(0) == nb_spectral_coefficients_global() );
    std::vector<int> nto(1,root+1);
    if( loc.rank() > 1 ) {
      nto.resize(loc.stride(0));
      for( size_t i=0; i<nto.size(); ++i ) nto[i] = root+1;
    }
    trans_->gathspec(nto.size(),nto.data(),
                     array::make_storageview<double>(loc).data(),
                     array::make_storageview<double>(glb).data());
#else

    throw eckit::Exception("Cannot gather spectral fields because Atlas has not been compiled with TRANS support.");
#endif
  }
}
void Spectral::gather( const Field& local, Field& global ) const
{
  FieldSet local_fields;
  FieldSet global_fields;
  local_fields.add(local);
  global_fields.add(global);
  gather(local_fields,global_fields);
}

void Spectral::scatter( const FieldSet& global_fieldset, FieldSet& local_fieldset ) const
{
  ASSERT( trans_ );
  ASSERT(local_fieldset.size() == global_fieldset.size());

  for( size_t f=0; f<local_fieldset.size(); ++f ) {

    const Field& glb = global_fieldset[f];
    Field& loc = local_fieldset[f];
    if( loc.datatype() != array::DataType::str<double>() )
    {
      std::stringstream err;
      err << "Cannot scatter spectral field " << glb.name() << " of datatype " << glb.datatype().str() << ".";
      err << "Only " << array::DataType::str<double>() << " supported.";
      throw eckit::BadValue(err.str());
    }

#ifdef ATLAS_HAVE_TRANS
    size_t root=0;
    glb.metadata().get("owner",root);
    ASSERT( loc.shape(0) == nb_spectral_coefficients() );
    if( parallel::mpi::comm().rank() == root )
      ASSERT( glb.shape(0) == nb_spectral_coefficients_global() );
    std::vector<int> nfrom(1,root+1);
    if( loc.rank() > 1 ) {
      nfrom.resize(loc.stride(0));
      for( size_t i=0; i<nfrom.size(); ++i ) nfrom[i] = root+1;
    }
    trans_->distspec(nfrom.size(),nfrom.data(),
                     array::make_storageview<double>(glb).data(),
                     array::make_storageview<double>(loc).data());
    glb.metadata().broadcast(loc.metadata(),root);
    loc.metadata().set("global",false);
#else
    throw eckit::Exception("Cannot scatter spectral fields because Atlas has not been compiled with TRANS support.");
#endif

  }
}
void Spectral::scatter( const Field& global, Field& local ) const
{
  FieldSet global_fields;
  FieldSet local_fields;
  global_fields.add(global);
  local_fields.add(local);
  scatter(global_fields,local_fields);
}

std::string Spectral::checksum( const FieldSet& fieldset ) const {
  eckit::MD5 md5;
  NOTIMP;
  return md5;
}
std::string Spectral::checksum( const Field& field ) const {
  FieldSet fieldset;
  fieldset.add(field);
  return checksum(fieldset);
}

void Spectral::norm( const Field& field, double& norm, int rank ) const {
#ifdef ATLAS_HAVE_TRANS
  ASSERT( std::max<int>(1,field.levels()) == 1 );
  trans_->specnorm(1,field.data<double>(), &norm, rank);
#else
  throw eckit::Exception("Cannot compute spectral norms because Atlas has not been compiled with TRANS support.");
#endif
}
void Spectral::norm( const Field& field, double norm_per_level[], int rank ) const {
#ifdef ATLAS_HAVE_TRANS
  trans_->specnorm( std::max<int>(1,field.levels()),field.data<double>(), norm_per_level, rank);
#else
  throw eckit::Exception("Cannot compute spectral norms because Atlas has not been compiled with TRANS support.");
#endif
}
void Spectral::norm( const Field& field, std::vector<double>& norm_per_level, int rank ) const {
#ifdef ATLAS_HAVE_TRANS
  norm_per_level.resize( std::max<int>(1,field.levels()) );
  trans_->specnorm(norm_per_level.size(),field.data<double>(), norm_per_level.data(), rank);
#else
  throw eckit::Exception("Cannot compute spectral norms because Atlas has not been compiled with TRANS support.");
#endif
}

} // namespace detail

// ----------------------------------------------------------------------

Spectral::Spectral( const FunctionSpace& functionspace ) :
  FunctionSpace(functionspace),
  functionspace_( dynamic_cast< const detail::Spectral* >( get() ) ) {
}

Spectral::Spectral( const size_t truncation ) :
  FunctionSpace( new detail::Spectral(truncation) ),
  functionspace_( dynamic_cast< const detail::Spectral* >( get() ) ) {
}

Spectral::Spectral( trans::Trans& trans ) :
  FunctionSpace( new detail::Spectral(trans) ),
  functionspace_( dynamic_cast< const detail::Spectral* >( get() ) ) {
}

size_t Spectral::nb_spectral_coefficients() const {
  return functionspace_->nb_spectral_coefficients();
}

size_t Spectral::nb_spectral_coefficients_global() const {
  return functionspace_->nb_spectral_coefficients_global();
}

template <>
Field Spectral::createField<float>(const eckit::Configuration& options) const {
  return functionspace_->createField<float>(options);
}

template <>
Field Spectral::createField<double>(const eckit::Configuration& options) const {
  return functionspace_->createField<double>(options);
}

void Spectral::gather( const FieldSet& local_fieldset, FieldSet& global_fieldset ) const {
  functionspace_->gather(local_fieldset,global_fieldset);
}

void Spectral::gather( const Field& local, Field& global ) const {
  functionspace_->gather(local,global);
}

void Spectral::scatter( const FieldSet& global_fieldset, FieldSet& local_fieldset ) const {
  functionspace_->scatter(global_fieldset,local_fieldset);
}

void Spectral::scatter( const Field& global, Field& local ) const {
  functionspace_->scatter(global,local);
}

std::string Spectral::checksum( const FieldSet& fieldset ) const {
  return functionspace_->checksum(fieldset);
}

std::string Spectral::checksum( const Field& field ) const {
  return functionspace_->checksum(field);
}

void Spectral::norm( const Field& field, double& norm, int rank ) const {
  functionspace_->norm(field,norm,rank);
}

void Spectral::norm( const Field& field, double norm_per_level[], int rank ) const {
  functionspace_->norm(field,norm_per_level,rank);
}

void Spectral::norm( const Field& field, std::vector<double>& norm_per_level, int rank ) const {
  functionspace_->norm(field,norm_per_level,rank);
}


// ----------------------------------------------------------------------


extern "C"
{
const detail::Spectral* atlas__SpectralFunctionSpace__new__truncation (int truncation)
{
  ATLAS_ERROR_HANDLING(
    return new detail::Spectral(truncation);
  );
  return 0;
}

const detail::Spectral* atlas__SpectralFunctionSpace__new__trans (trans::Trans* trans)
{
  ATLAS_ERROR_HANDLING(
    return new detail::Spectral(*trans);
  );
  return 0;
}

void atlas__SpectralFunctionSpace__delete ( detail::Spectral* This )
{
  ATLAS_ERROR_HANDLING(
    ASSERT(This);
    delete This;
  );
}

field::FieldImpl* atlas__fs__Spectral__create_field (const detail::Spectral* This, const eckit::Configuration* options)
{
  ATLAS_ERROR_HANDLING(
    ASSERT(This);
    ASSERT(options);
    field::FieldImpl* field;
    {
      Field f = This->createField( *options );
      field = f.get();
      field->attach();
    }
    field->detach();
    return field;
  );
  return 0;
}


void atlas__SpectralFunctionSpace__gather (const detail::Spectral* This, const field::FieldImpl* local, field::FieldImpl* global)
{
  ATLAS_ERROR_HANDLING(
    ASSERT(This);
    ASSERT(global);
    ASSERT(local);
    const Field l(local);
    Field g(global);
    This->gather(l,g);
  );
}

void atlas__SpectralFunctionSpace__scatter (const detail::Spectral* This, const field::FieldImpl* global, field::FieldImpl* local)
{
  ATLAS_ERROR_HANDLING(
    ASSERT(This);
    ASSERT(global);
    ASSERT(local);
    const Field g(global);
    Field l(local);
    This->scatter(g,l);
  );
}

void atlas__SpectralFunctionSpace__gather_fieldset (const detail::Spectral* This, const field::FieldSetImpl* local, field::FieldSetImpl* global)
{
  ATLAS_ERROR_HANDLING(
    ASSERT(This);
    ASSERT(global);
    ASSERT(local);
    const FieldSet l(local);
    FieldSet g(global);
    This->gather(l,g);
  );
}

void atlas__SpectralFunctionSpace__scatter_fieldset (const detail::Spectral* This, const field::FieldSetImpl* global, field::FieldSetImpl* local)
{
  ATLAS_ERROR_HANDLING(
    ASSERT(This);
    ASSERT(global);
    ASSERT(local);
    const FieldSet g(global);
    FieldSet l(local);
    This->scatter(g,l);
  );
}

void atlas__SpectralFunctionSpace__norm(const detail::Spectral* This, const field::FieldImpl* field, double norm[], int rank)
{
  ATLAS_ERROR_HANDLING(
    ASSERT(This);
    ASSERT(field);
    ASSERT(norm);
    This->norm(field,norm,rank);
  );
}



}

} // namespace functionspace
} // namespace atlas

