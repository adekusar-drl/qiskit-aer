/**
 * This code is part of Qiskit.
 *
 * (C) Copyright IBM Corp. 2017 and later.
 *
 * This code is licensed under the Apache License, Version 2.0. You may
 * obtain a copy of this license in the LICENSE.txt file in the root directory
 * of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Any modifications or derivative works of this code must retain this
 * copyright notice, and modified files need to carry a notice indicating
 * that they have been altered from the originals.
 */



#ifndef _qv_qubit_vector_hpp_
#define _qv_qubit_vector_hpp_

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "framework/json.hpp"

namespace QV {

// Type aliases
using uint_t = uint64_t;
using int_t = int64_t;
using reg_t = std::vector<uint_t>;
using indexes_t = std::unique_ptr<uint_t[]>;
using complex_t = std::complex<double>;
using cvector_t = std::vector<complex_t>;
using rvector_t = std::vector<double>;
template <size_t N> using areg_t = std::array<uint_t, N>;

//============================================================================
// BIT MASKS and indexing
//============================================================================

/*
# Auto generate these values with following python snippet
import json

def cpp_init_list(int_lst):
    ret = json.dumps([str(i) + 'ULL' for i in int_lst])
    return ret.replace('"', '').replace('[','{{').replace(']','}};')

print('const std::array<uint_t, 64> BITS = ' + cpp_init_list([(1 << i) for i in range(64)]) + '\n')
print('const std::array<uint_t, 64> MASKS = ' + cpp_init_list([(1 << i) - 1 for i in range(64)]))
*/

const std::array<uint_t, 64> BITS {{
  1ULL, 2ULL, 4ULL, 8ULL,
  16ULL, 32ULL, 64ULL, 128ULL,
  256ULL, 512ULL, 1024ULL, 2048ULL,
  4096ULL, 8192ULL, 16384ULL, 32768ULL,
  65536ULL, 131072ULL, 262144ULL, 524288ULL,
  1048576ULL, 2097152ULL, 4194304ULL, 8388608ULL,
  16777216ULL, 33554432ULL, 67108864ULL, 134217728ULL,
  268435456ULL, 536870912ULL, 1073741824ULL, 2147483648ULL,
  4294967296ULL, 8589934592ULL, 17179869184ULL, 34359738368ULL, 
  68719476736ULL, 137438953472ULL, 274877906944ULL, 549755813888ULL,
  1099511627776ULL, 2199023255552ULL, 4398046511104ULL, 8796093022208ULL,
  17592186044416ULL, 35184372088832ULL, 70368744177664ULL, 140737488355328ULL, 
  281474976710656ULL, 562949953421312ULL, 1125899906842624ULL, 2251799813685248ULL,
  4503599627370496ULL, 9007199254740992ULL, 18014398509481984ULL, 36028797018963968ULL,
  72057594037927936ULL, 144115188075855872ULL, 288230376151711744ULL, 576460752303423488ULL,
  1152921504606846976ULL, 2305843009213693952ULL, 4611686018427387904ULL, 9223372036854775808ULL
}};


const std::array<uint_t, 64> MASKS {{
  0ULL, 1ULL, 3ULL, 7ULL,
  15ULL, 31ULL, 63ULL, 127ULL,
  255ULL, 511ULL, 1023ULL, 2047ULL,
  4095ULL, 8191ULL, 16383ULL, 32767ULL,
  65535ULL, 131071ULL, 262143ULL, 524287ULL,
  1048575ULL, 2097151ULL, 4194303ULL, 8388607ULL,
  16777215ULL, 33554431ULL, 67108863ULL, 134217727ULL,
  268435455ULL, 536870911ULL, 1073741823ULL, 2147483647ULL,
  4294967295ULL, 8589934591ULL, 17179869183ULL, 34359738367ULL,
  68719476735ULL, 137438953471ULL, 274877906943ULL, 549755813887ULL,
  1099511627775ULL, 2199023255551ULL, 4398046511103ULL, 8796093022207ULL,
  17592186044415ULL, 35184372088831ULL, 70368744177663ULL, 140737488355327ULL,
  281474976710655ULL, 562949953421311ULL, 1125899906842623ULL, 2251799813685247ULL,
  4503599627370495ULL, 9007199254740991ULL, 18014398509481983ULL, 36028797018963967ULL,
  72057594037927935ULL, 144115188075855871ULL, 288230376151711743ULL, 576460752303423487ULL,
  1152921504606846975ULL, 2305843009213693951ULL, 4611686018427387903ULL, 9223372036854775807ULL
}};


//============================================================================
// QubitVector class
//============================================================================

// Template class for qubit vector.
// The arguement of the template must have an operator[] access method.
// The following methods may also need to be template specialized:
//   * set_num_qubits(size_t)
//   * initialize()
//   * initialize_from_vector(cvector_t)
// If the template argument does not have these methods then template
// specialization must be used to override the default implementations.

template <typename data_t = complex_t*>
class QubitVector {

public:

  //-----------------------------------------------------------------------
  // Constructors and Destructor
  //-----------------------------------------------------------------------

  QubitVector();
  explicit QubitVector(size_t num_qubits);
  virtual ~QubitVector();
  QubitVector(const QubitVector& obj) = delete;
  QubitVector &operator=(const QubitVector& obj) = delete;

  //-----------------------------------------------------------------------
  // Data access
  //-----------------------------------------------------------------------

  // Element access
  complex_t &operator[](uint_t element);
  complex_t operator[](uint_t element) const;

  // Returns a reference to the underlying data_t data class
  data_t &data() {return data_;}

  // Returns a copy of the underlying data_t data class
  data_t data() const {return data_;}

  //-----------------------------------------------------------------------
  // Utility functions
  //-----------------------------------------------------------------------

  // Set the size of the vector in terms of qubit number
  virtual void set_num_qubits(size_t num_qubits);

  // Returns the number of qubits for the current vector
  uint_t num_qubits() const {return num_qubits_;}

  // Returns the size of the underlying n-qubit vector
  uint_t size() const {return data_size_;}

  // Returns a copy of the underlying data_t data as a complex vector
  cvector_t vector() const;

  // Return JSON serialization of QubitVector;
  json_t json() const;

  // Set all entries in the vector to 0.
  void zero();

  // index0 returns the integer representation of a number of bits set
  // to zero inserted into an arbitrary bit string.
  // Eg: for qubits 0,2 in a state k = ba ( ba = 00 => k=0, etc).
  // indexes0([1], k) -> int(b0a)
  // indexes0([1,3], k) -> int(0b0a)
  // Example: k = 77  = 1001101 , qubits_sorted = [1,4]
  // ==> output = 297 = 100101001 (with 0's put into places 1 and 4).
  template<typename list_t>
  uint_t index0(const list_t &qubits_sorted, const uint_t k) const;

  // Return a std::unique_ptr to an array of of 2^N in ints
  // each int corresponds to an N qubit bitstring for M-N qubit bits in state k,
  // and the specified N qubits in states [0, ..., 2^N - 1]
  // qubits_sorted must be sorted lowest to highest. Eg. {0, 1}.
  // qubits specifies the location of the qubits in the returned strings.
  // NOTE: since the return is a unique_ptr it cannot be copied.
  // indexes returns the array of all bit values for the specified qubits
  // (Eg: for qubits 0,2 in a state k = ba:
  // indexes([1], [1], k) = [int(b0a), int(b1a)],
  // if it were two qubits inserted say at 1,3 it would be:
  // indexes([1,3], [1,3], k) -> [int(0b0a), int(0b1a), int(1b0a), (1b1a)]
  // If the qubits were passed in reverse order it would swap qubit position in the list:
  // indexes([3,1], [1,3], k) -> [int(0b0a), int(1b0a), int(0b1a), (1b1a)]
  // Example: k=77, qubits=qubits_sorted=[1,4] ==> output=[297,299,313,315]
  // input: k = 77  = 1001101
  // output[0]: 297 = 100101001 (with 0's put into places 1 and 4).
  // output[1]: 299 = 100101011 (with 0 put into place 1, and 1 put into place 4).
  // output[2]: 313 = 100111001 (with 1 put into place 1, and 0 put into place 4).
  // output[3]: 313 = 100111011 (with 1's put into places 1 and 4).
  indexes_t indexes(const reg_t &qubits, const reg_t &qubits_sorted, const uint_t k) const;

  // As above but returns a fixed sized array of of 2^N in ints
  template<size_t N>
  areg_t<1ULL << N> indexes(const areg_t<N> &qs, const areg_t<N> &qubits_sorted, const uint_t k) const;

  // State initialization of a component
  // Initialize the specified qubits to a desired statevector
  // (leaving the other qubits in their current state)
  // assuming the qubits being initialized have already been reset to the zero state
  // (using apply_reset)
  void initialize_component(const reg_t &qubits, const cvector_t &state);

  //-----------------------------------------------------------------------
  // Check point operations
  //-----------------------------------------------------------------------

  // Create a checkpoint of the current state
  void checkpoint();

  // Revert to the checkpoint
  void revert(bool keep);

  // Compute the inner product of current state with checkpoint state
  complex_t inner_product() const;

  //-----------------------------------------------------------------------
  // Initialization
  //-----------------------------------------------------------------------

  // Initializes the current vector so that all qubits are in the |0> state.
  void initialize();

  // Initializes the vector to a custom initial state.
  // If the length of the data vector does not match the number of qubits
  // an exception is raised.
  void initialize_from_vector(const cvector_t &data);

  // Initializes the vector to a custom initial state.
  // If num_states does not match the number of qubits an exception is raised.
  void initialize_from_data(const data_t &data, const size_t num_states);

  //-----------------------------------------------------------------------
  // Apply Matrices
  //-----------------------------------------------------------------------

  // Apply a 1-qubit matrix to the state vector.
  // The matrix is input as vector of the column-major vectorized 1-qubit matrix.
  void apply_matrix(const uint_t qubit, const cvector_t &mat);

  // Apply a N-qubit matrix to the state vector.
  // The matrix is input as vector of the column-major vectorized N-qubit matrix.
  void apply_matrix(const reg_t &qubits, const cvector_t &mat);

  // Apply a N-qubit matrix constructed from composition of 1 and 2 qubit matrices.
  // The sets of qubits and matrices are passed as vectors, where each individual matrix
  // is input as a column-major vectorized matrix.
  void apply_matrix_sequence(const std::vector<reg_t> &regs, const std::vector<cvector_t> &mats);

  // Apply a 1-qubit diagonal matrix to the state vector.
  // The matrix is input as vector of the matrix diagonal.
  void apply_diagonal_matrix(const uint_t qubit, const cvector_t &mat);

  // Apply a N-qubit diagonal matrix to the state vector.
  // The matrix is input as vector of the matrix diagonal.
  void apply_diagonal_matrix(const reg_t &qubits, const cvector_t &mat);
  
  // Swap pairs of indicies in the underlying vector
  void apply_permutation_matrix(const reg_t &qubits,
                                const std::vector<std::pair<uint_t, uint_t>> &pairs);

  //-----------------------------------------------------------------------
  // Apply Specialized Gates
  //-----------------------------------------------------------------------

  // Apply a general N-qubit multi-controlled X-gate
  // If N=1 this implements an optimized X gate
  // If N=2 this implements an optimized CX gate
  // If N=3 this implements an optimized Toffoli gate
  void apply_mcx(const reg_t &qubits);

  // Apply a general multi-controlled Y-gate
  // If N=1 this implements an optimized Y gate
  // If N=2 this implements an optimized CY gate
  // If N=3 this implements an optimized CCY gate
  void apply_mcy(const reg_t &qubits);

  // Apply a general multi-controlled Z-gate
  // If N=1 this implements an optimized Z gate
  // If N=2 this implements an optimized CZ gate
  // If N=3 this implements an optimized CCZ gate
  void apply_mcz(const reg_t &qubits);
  
  // Apply a general multi-controlled single-qubit unitary gate
  // If N=1 this implements an optimized single-qubit gate
  // If N=2 this implements an optimized CU gate
  // If N=3 this implements an optimized CCU gate
  void apply_mcu(const reg_t &qubits, const cvector_t &mat);

  // Apply a general multi-controlled SWAP gate
  // If N=2 this implements an optimized SWAP  gate
  // If N=3 this implements an optimized Fredkin gate
  void apply_mcswap(const reg_t &qubits);

  //-----------------------------------------------------------------------
  // Z-measurement outcome probabilities
  //-----------------------------------------------------------------------

  // Return the Z-basis measurement outcome probability P(outcome) for
  // outcome in [0, 2^num_qubits - 1]
  double probability(const uint_t outcome) const;

  // Return the probabilities for all measurement outcomes in the current vector
  // This is equivalent to returning a new vector with  new[i]=|orig[i]|^2.
  // Eg. For 2-qubits this is [P(00), P(01), P(010), P(11)]
  rvector_t probabilities() const;

  // Return the Z-basis measurement outcome probabilities [P(0), P(1)]
  // for measurement of specified qubit
  rvector_t probabilities(const uint_t qubit) const;

  // Return the Z-basis measurement outcome probabilities [P(0), ..., P(2^N-1)]
  // for measurement of N-qubits.
  rvector_t probabilities(const reg_t &qubits) const;

  // Return M sampled outcomes for Z-basis measurement of all qubits
  // The input is a length M list of random reals between [0, 1) used for
  // generating samples.
  std::vector<uint_t> sample_measure(const std::vector<double> &rnds) const;

  //-----------------------------------------------------------------------
  // Norms
  //-----------------------------------------------------------------------
  
  // Returns the norm of the current vector
  double norm() const;

  // These functions return the norm <psi|A^dagger.A|psi> obtained by
  // applying a matrix A to the vector. It is equivalent to returning the
  // expectation value of A^\dagger A, and could probably be removed because
  // of this.

  // Return the norm for of the vector obtained after apply the 1-qubit
  // matrix mat to the vector.
  // The matrix is input as vector of the column-major vectorized 1-qubit matrix.
  double norm(const uint_t qubit, const cvector_t &mat) const;

  // Return the norm for of the vector obtained after apply the N-qubit
  // matrix mat to the vector.
  // The matrix is input as vector of the column-major vectorized N-qubit matrix.
  double norm(const reg_t &qubits, const cvector_t &mat) const;

  // Return the norm for of the vector obtained after apply the 1-qubit
  // diagonal matrix mat to the vector.
  // The matrix is input as vector of the matrix diagonal.
  double norm_diagonal(const uint_t qubit, const cvector_t &mat) const;

  // Return the norm for of the vector obtained after apply the N-qubit
  // diagonal matrix mat to the vector.
  // The matrix is input as vector of the matrix diagonal.
  double norm_diagonal(const reg_t &qubits, const cvector_t &mat) const;

  //-----------------------------------------------------------------------
  // JSON configuration settings
  //-----------------------------------------------------------------------

  // Set the threshold for chopping values to 0 in JSON
  void set_json_chop_threshold(double threshold);

  // Set the threshold for chopping values to 0 in JSON
  double get_json_chop_threshold() {return json_chop_threshold_;}

  //-----------------------------------------------------------------------
  // OpenMP configuration settings
  //-----------------------------------------------------------------------

  // Set the maximum number of OpenMP thread for operations.
  void set_omp_threads(int n);

  // Get the maximum number of OpenMP thread for operations.
  uint_t get_omp_threads() {return omp_threads_;}

  // Set the qubit threshold for activating OpenMP.
  // If self.qubits() > threshold OpenMP will be activated.
  void set_omp_threshold(int n);

  // Get the qubit threshold for activating OpenMP.
  uint_t get_omp_threshold() {return omp_threshold_;}

  //-----------------------------------------------------------------------
  // Optimization configuration settings
  //-----------------------------------------------------------------------

  // Set the sample_measure index size
  void set_sample_measure_index_size(int n) {sample_measure_index_size_ = n;}

  // Get the sample_measure index size
  int get_sample_measure_index_size() {return sample_measure_index_size_;}

protected:

  //-----------------------------------------------------------------------
  // Protected data members
  //-----------------------------------------------------------------------
  size_t num_qubits_;
  size_t data_size_;
  data_t data_;
  data_t checkpoint_;

  //-----------------------------------------------------------------------
  // Config settings
  //----------------------------------------------------------------------- 
  uint_t omp_threads_ = 1;     // Disable multithreading by default
  uint_t omp_threshold_ = 14;  // Qubit threshold for multithreading when enabled
  int sample_measure_index_size_ = 10; // Sample measure indexing qubit size
  double json_chop_threshold_ = 0;  // Threshold for choping small values
                                    // in JSON serialization

  //-----------------------------------------------------------------------
  // Error Messages
  //-----------------------------------------------------------------------

  void check_qubit(const uint_t qubit) const;
  void check_vector(const cvector_t &diag, uint_t nqubits) const;
  void check_matrix(const cvector_t &mat, uint_t nqubits) const;
  void check_dimension(const QubitVector &qv) const;
  void check_checkpoint() const;

  //-----------------------------------------------------------------------
  // Statevector update with Lambda function
  //-----------------------------------------------------------------------
  // Apply a lambda function to all entries of the statevector.
  // The function signature should be:
  //
  // [&](const int_t k)->void
  //
  // where k is the index of the vector
  template <typename Lambda>
  void apply_lambda(Lambda&& func);

  //-----------------------------------------------------------------------
  // Statevector block update with Lambda function
  //-----------------------------------------------------------------------
  // These functions loop through the indexes of the qubitvector data and
  // apply a lambda function to each block specified by the qubits argument.
  //
  // NOTE: The lambda functions can use the dynamic or static indexes
  // signature however if N is known at compile time the static case should
  // be preferred as it is significantly faster.

  // Apply a N-qubit lambda function to all blocks of the statevector
  // for the given qubits. The function signature should be either:
  //
  // (Static): [&](const areg_t<1ULL<<N> &inds)->void
  // (Dynamic): [&](const indexes_t &inds)->void
  //
  // where `inds` are the 2 ** N indexes for each N-qubit block returned by
  // the `indexes` function.
  template <typename Lambda, typename list_t>
  void apply_lambda(Lambda&& func, const list_t &qubits);

  // Apply an N-qubit parameterized lambda function to all blocks of the
  // statevector for the given qubits. The function signature should be:
  //
  // (Static): [&](const areg_t<1ULL<<N> &inds, const param_t &params)->void
  // (Dynamic): [&](const indexes_t &inds, const param_t &params)->void
  //
  // where `inds` are the 2 ** N indexes for each N-qubit block returned by
  // the `indexes` function and `param` is a templated parameter class.
  // (typically a complex vector).
  template <typename Lambda, typename list_t, typename param_t>
  void apply_lambda(Lambda&& func, const list_t &qubits, const param_t &par);

  //-----------------------------------------------------------------------
  // State reduction with Lambda functions
  //-----------------------------------------------------------------------
  // Apply a complex reduction lambda function to all entries of the
  // statevector and return the complex result.
  // The function signature should be:
  //
  // [&](const int_t k, double &val_re, double &val_im)->void
  //
  // where k is the index of the vector, val_re and val_im are the doubles
  // to store the reduction.
  // Returns complex_t(val_re, val_im)
  template <typename Lambda>
  complex_t apply_reduction_lambda(Lambda&& func) const;

  //-----------------------------------------------------------------------
  // Statevector block reduction with Lambda function
  //-----------------------------------------------------------------------
  // These functions loop through the indexes of the qubitvector data and
  // apply a reduction lambda function to each block specified by the qubits
  // argument. The reduction lambda stores the reduction in two doubles
  // (val_re, val_im) and returns the complex result complex_t(val_re, val_im)
  //
  // NOTE: The lambda functions can use the dynamic or static indexes
  // signature however if N is known at compile time the static case should
  // be preferred as it is significantly faster.

  // Apply a N-qubit complex matrix reduction lambda function to all blocks
  // of the statevector for the given qubits.
  // The lambda function signature should be:
  //
  // (Static): [&](const areg_t<1ULL<<N> &inds, const param_t &mat,
  //               double &val_re, double &val_im)->void
  // (Dynamic): [&](const indexes_t &inds, const param_t &mat,
  //                double &val_re, double &val_im)->void
  //
  // where `inds` are the 2 ** N indexes for each N-qubit block returned by
  // the `indexes` function, `val_re` and `val_im` are the doubles to
  // store the reduction returned as complex_t(val_re, val_im).
  template <typename Lambda, typename list_t>
  complex_t apply_reduction_lambda(Lambda&& func,
                                   const list_t &qubits) const;

  // Apply a N-qubit complex matrix reduction lambda function to all blocks
  // of the statevector for the given qubits.
  // The lambda function signature should be:
  //
  // (Static): [&](const areg_t<1ULL<<N> &inds, const param_t &parms,
  //               double &val_re, double &val_im)->void
  // (Dynamic): [&](const indexes_t &inds, const param_t &params,
  //                double &val_re, double &val_im)->void
  //
  // where `inds` are the 2 ** N indexes for each N-qubit block returned by
  // the `indexe`s function, `params` is a templated parameter class
  // (typically a complex vector), `val_re` and `val_im` are the doubles to
  // store the reduction returned as complex_t(val_re, val_im).
  template <typename Lambda, typename list_t, typename param_t>
  complex_t apply_reduction_lambda(Lambda&& func,
                                   const list_t &qubits,
                                   const param_t &params) const;

  // Permute an N-qubit vectorized matrix to match a reordering of qubits
  cvector_t sort_matrix(const reg_t &src,
                        const reg_t &sorted,
                        const cvector_t &mat) const;

  // Swap cols and rows of vectorized matrix
  void swap_cols_and_rows(const uint_t idx1, const uint_t idx2,
                          cvector_t &mat, uint_t dim) const;

  // Extend a matrix for qubits (src_qubits) to a matrix for more qubits (dst_sorted_qubits)
  // qubits in dst_sorted_qubits must contains all the qubits in src_qubits
  cvector_t expand_matrix(const reg_t& src_qubits,
                          const reg_t& dst_sorted_qubits,
                          const cvector_t& vmat) const;
};

/*******************************************************************************
 *
 * Implementations
 *
 ******************************************************************************/

//------------------------------------------------------------------------------
// JSON Serialization
//------------------------------------------------------------------------------

template <typename data_t>
inline void to_json(json_t &js, const QubitVector<data_t> &qv) {
  js = qv.json();
}

template <typename data_t>
json_t QubitVector<data_t>::json() const {
  const int_t END = data_size_;
  const json_t ZERO = complex_t(0.0, 0.0);
  json_t js = json_t(data_size_, ZERO);
  
  if (json_chop_threshold_ > 0) {
    #pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
    for (int_t j=0; j < END; j++) {
      if (std::abs(data_[j].real()) > json_chop_threshold_)
        js[j][0] = data_[j].real();
      if (std::abs(data_[j].imag()) > json_chop_threshold_)
        js[j][1] = data_[j].imag();
    }
  } else {
    #pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
    for (int_t j=0; j < END; j++) {
      js[j][0] = data_[j].real();
      js[j][1] = data_[j].imag();
    }
  }
  return js;
}

//------------------------------------------------------------------------------
// Error Handling
//------------------------------------------------------------------------------

template <typename data_t>
void QubitVector<data_t>::check_qubit(const uint_t qubit) const {
  if (qubit + 1 > num_qubits_) {
    std::string error = "QubitVector: qubit index " + std::to_string(qubit) +
                        " > " + std::to_string(num_qubits_);
    throw std::runtime_error(error);
  }
}

template <typename data_t>
void QubitVector<data_t>::check_matrix(const cvector_t &vec, uint_t nqubits) const {
  const size_t DIM = BITS[nqubits];
  const auto SIZE = vec.size();
  if (SIZE != DIM * DIM) {
    std::string error = "QubitVector: vector size is " + std::to_string(SIZE) +
                        " != " + std::to_string(DIM * DIM);
    throw std::runtime_error(error);
  }
}

template <typename data_t>
void QubitVector<data_t>::check_vector(const cvector_t &vec, uint_t nqubits) const {
  const size_t DIM = BITS[nqubits];
  const auto SIZE = vec.size();
  if (SIZE != DIM) {
    std::string error = "QubitVector: vector size is " + std::to_string(SIZE) +
                        " != " + std::to_string(DIM);
    throw std::runtime_error(error);
  }
}

template <typename data_t>
void QubitVector<data_t>::check_dimension(const QubitVector &qv) const {
  if (data_size_ != qv.size_) {
    std::string error = "QubitVector: vectors are different shape " +
                         std::to_string(data_size_) + " != " +
                         std::to_string(qv.num_states_);
    throw std::runtime_error(error);
  }
}

template <typename data_t>
void QubitVector<data_t>::check_checkpoint() const {
  if (!checkpoint_) {
    throw std::runtime_error("QubitVector: checkpoint must exist for inner_product() or revert()");
  }
}

//------------------------------------------------------------------------------
// Constructors & Destructor
//------------------------------------------------------------------------------

template <typename data_t>
QubitVector<data_t>::QubitVector(size_t num_qubits) : num_qubits_(0), data_(nullptr), checkpoint_(0){
  set_num_qubits(num_qubits);
}

template <typename data_t>
QubitVector<data_t>::QubitVector() : QubitVector(0) {}

template <typename data_t>
QubitVector<data_t>::~QubitVector() {
  if (data_)
    free(data_);

  if (checkpoint_)
    free(checkpoint_);
}

//------------------------------------------------------------------------------
// Element access operators
//------------------------------------------------------------------------------

template <typename data_t>
complex_t &QubitVector<data_t>::operator[](uint_t element) {
  // Error checking
  #ifdef DEBUG
  if (element > data_size_) {
    std::string error = "QubitVector: vector index " + std::to_string(element) +
                        " > " + std::to_string(data_size_);
    throw std::runtime_error(error);
  }
  #endif
  return data_[element];
}

template <typename data_t>
complex_t QubitVector<data_t>::operator[](uint_t element) const {
  // Error checking
  #ifdef DEBUG
  if (element > data_size_) {
    std::string error = "QubitVector: vector index " + std::to_string(element) +
                        " > " + std::to_string(data_size_);
    throw std::runtime_error(error);
  }
  #endif
  return data_[element];
}

template <typename data_t>
cvector_t QubitVector<data_t>::vector() const {
  cvector_t ret(data_size_, 0.);
  const int_t END = data_size_;
  #pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  for (int_t j=0; j < END; j++) {
    ret[j] = data_[j];
  }
  return ret;
}

//------------------------------------------------------------------------------
// Indexing
//------------------------------------------------------------------------------

template <typename data_t>
template <typename list_t>
uint_t QubitVector<data_t>::index0(const list_t &qubits_sorted, const uint_t k) const {
  uint_t lowbits, retval = k;
  for (size_t j = 0; j < qubits_sorted.size(); j++) {
    lowbits = retval & MASKS[qubits_sorted[j]];
    retval >>= qubits_sorted[j];
    retval <<= qubits_sorted[j] + 1;
    retval |= lowbits;
  }
  return retval;
}

template <typename data_t>
template <size_t N>
areg_t<1ULL << N> QubitVector<data_t>::indexes(const areg_t<N> &qs,
                                               const areg_t<N> &qubits_sorted,
                                               const uint_t k) const {
  areg_t<1ULL << N> ret;
  ret[0] = index0(qubits_sorted, k);
  for (size_t i = 0; i < N; i++) {
    const auto n = BITS[i];
    const auto bit = BITS[qs[i]];
    for (size_t j = 0; j < n; j++)
      ret[n + j] = ret[j] | bit;
  }
  return ret;
}

template <typename data_t>
indexes_t QubitVector<data_t>::indexes(const reg_t& qubits,
                                       const reg_t& qubits_sorted,
                                       const uint_t k) const {
  const auto N = qubits_sorted.size();
  indexes_t ret(new uint_t[BITS[N]]);
  // Get index0
  ret[0] = index0(qubits_sorted, k);
  for (size_t i = 0; i < N; i++) {
    const auto n = BITS[i];
    const auto bit = BITS[qubits[i]];
    for (size_t j = 0; j < n; j++)
      ret[n + j] = ret[j] | bit;
  }
  return ret;
}

//------------------------------------------------------------------------------
// State initialize component
//------------------------------------------------------------------------------
template <typename data_t>
void QubitVector<data_t>::initialize_component(const reg_t &qubits, const cvector_t &state) {

  // Lambda function for initializing component
  const size_t N = qubits.size();
  auto lambda = [&](const indexes_t &inds, const cvector_t &_state)->void {
    const uint_t DIM = 1ULL << N;
    complex_t cache = data_[inds[0]];  // the k-th component of non-initialized vector
    for (size_t i = 0; i < DIM; i++) {
      data_[inds[i]] = cache * _state[i];  // set component to psi[k] * state[i]
    }    // (where psi is is the post-reset state of the non-initialized qubits)
   };
  // Use the lambda function
  apply_lambda(lambda, qubits, state);
}

//------------------------------------------------------------------------------
// Utility
//------------------------------------------------------------------------------

template <typename data_t>
void QubitVector<data_t>::zero() {
  const int_t END = data_size_;    // end for k loop

#pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  for (int_t k = 0; k < END; ++k) {
    data_[k] = 0.0;
  }
}

template <typename data_t>
void QubitVector<data_t>::set_num_qubits(size_t num_qubits) {

  size_t prev_num_qubits = num_qubits_;
  num_qubits_ = num_qubits;
  data_size_ = BITS[num_qubits];

  if (checkpoint_) {
    free(checkpoint_);
    checkpoint_ = nullptr;
  }

  // Free any currently assigned memory
  if (data_) {
    if (prev_num_qubits != num_qubits_) {
      free(data_);
      data_ = nullptr;
    }
  }

  // Allocate memory for new vector
  if (data_ == nullptr)
    data_ = reinterpret_cast<complex_t*>(malloc(sizeof(complex_t) * data_size_));
}


template <typename data_t>
void QubitVector<data_t>::checkpoint() {
  if (!checkpoint_)
    checkpoint_ = reinterpret_cast<complex_t*>(malloc(sizeof(complex_t) * data_size_));

  const int_t END = data_size_;    // end for k loop
#pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  for (int_t k = 0; k < END; ++k)
    checkpoint_[k] = data_[k];
}


template <typename data_t>
void QubitVector<data_t>::revert(bool keep) {

  #ifdef DEBUG
  check_checkpoint();
  #endif

  const int_t END = data_size_;    // end for k loop
#pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  for (int_t k = 0; k < END; ++k)
    data_[k] = checkpoint_[k];

  if (!keep) {
    free(checkpoint_);
    checkpoint_ = nullptr;
  }
}

template <typename data_t>
complex_t QubitVector<data_t>::inner_product() const {

  #ifdef DEBUG
  check_checkpoint();
  #endif
  // Lambda function for inner product with checkpoint state
  auto lambda = [&](int_t k, double &val_re, double &val_im)->void {
    const complex_t z = data_[k] * std::conj(checkpoint_[k]);
    val_re += std::real(z);
    val_im += std::imag(z);
  };
  return apply_reduction_lambda(lambda);
}

//------------------------------------------------------------------------------
// Initialization
//------------------------------------------------------------------------------

template <typename data_t>
void QubitVector<data_t>::initialize() {
  zero();
  data_[0] = 1.;
}

template <typename data_t>
void QubitVector<data_t>::initialize_from_vector(const cvector_t &statevec) {
  if (data_size_ != statevec.size()) {
    std::string error = "QubitVector::initialize input vector is incorrect length (" + 
                        std::to_string(data_size_) + "!=" +
                        std::to_string(statevec.size()) + ")";
    throw std::runtime_error(error);
  }

  const int_t END = data_size_;    // end for k loop

#pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  for (int_t k = 0; k < END; ++k)
    data_[k] = statevec[k];
}

template <typename data_t>
void QubitVector<data_t>::initialize_from_data(const data_t &statevec, const size_t num_states) {
  if (data_size_ != num_states) {
    std::string error = "QubitVector::initialize input vector is incorrect length (" +
                        std::to_string(data_size_) + "!=" + std::to_string(num_states) + ")";
    throw std::runtime_error(error);
  }

  const int_t END = data_size_;    // end for k loop

#pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  for (int_t k = 0; k < END; ++k)
    data_[k] = statevec[k];
}


/*******************************************************************************
 *
 * CONFIG SETTINGS
 *
 ******************************************************************************/

template <typename data_t>
void QubitVector<data_t>::set_omp_threads(int n) {
  if (n > 0)
    omp_threads_ = n;
}

template <typename data_t>
void QubitVector<data_t>::set_omp_threshold(int n) {
  if (n > 0)
    omp_threshold_ = n;
}

template <typename data_t>
void QubitVector<data_t>::set_json_chop_threshold(double threshold) {
  json_chop_threshold_ = threshold;
}

/*******************************************************************************
 *
 * LAMBDA FUNCTION TEMPLATES
 *
 ******************************************************************************/


//------------------------------------------------------------------------------
// State update
//------------------------------------------------------------------------------

template <typename data_t>
template<typename Lambda>
void QubitVector<data_t>::apply_lambda(Lambda&& func) {
  const int_t END = data_size_;
#pragma omp parallel if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  {
#pragma omp for
    for (int_t k = 0; k < END; k++) {
      std::forward<Lambda>(func)(k);
    }
  }
}

template <typename data_t>
template<typename Lambda, typename list_t>
void QubitVector<data_t>::apply_lambda(Lambda&& func, const list_t &qubits) {

  // Error checking
  #ifdef DEBUG
  for (const auto &qubit : qubits)
    check_qubit(qubit);
  #endif

  const auto NUM_QUBITS = qubits.size();
  const int_t END = data_size_ >> NUM_QUBITS;
  auto qubits_sorted = qubits;
  std::sort(qubits_sorted.begin(), qubits_sorted.end());
#pragma omp parallel if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  {
#pragma omp for
    for (int_t k = 0; k < END; k++) {
      // store entries touched by U
      const auto inds = indexes(qubits, qubits_sorted, k);
      std::forward<Lambda>(func)(inds);
    }
  }
}

template <typename data_t>
template<typename Lambda, typename list_t, typename param_t>
void QubitVector<data_t>::apply_lambda(Lambda&& func,
                                       const list_t &qubits,
                                       const param_t &params) {

  // Error checking
  #ifdef DEBUG
  for (const auto &qubit : qubits)
    check_qubit(qubit);
  #endif

  const auto NUM_QUBITS = qubits.size();
  const int_t END = data_size_ >> NUM_QUBITS;
  auto qubits_sorted = qubits;
  std::sort(qubits_sorted.begin(), qubits_sorted.end());

#pragma omp parallel if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  {
#pragma omp for
    for (int_t k = 0; k < END; k++) {
      const auto inds = indexes(qubits, qubits_sorted, k);
      std::forward<Lambda>(func)(inds, params);
    }
  }
}


//------------------------------------------------------------------------------
// Reduction Lambda
//------------------------------------------------------------------------------

template <typename data_t>
template<typename Lambda>
complex_t QubitVector<data_t>::apply_reduction_lambda(Lambda &&func) const {
  // Reduction variables
  double val_re = 0.;
  double val_im = 0.;
  const int_t END = data_size_;
#pragma omp parallel reduction(+:val_re, val_im) if (num_qubits_ > omp_threshold_ && omp_threads_ > 1)         \
                                               num_threads(omp_threads_)
  {
#pragma omp for
    for (int_t k = 0; k < END; k++) {
        std::forward<Lambda>(func)(k, val_re, val_im);
      }
  } // end omp parallel
  return complex_t(val_re, val_im);
}


template <typename data_t>
template<typename Lambda, typename list_t>
complex_t QubitVector<data_t>::apply_reduction_lambda(Lambda&& func,
                                                      const list_t &qubits) const {

  // Error checking
  #ifdef DEBUG
  for (const auto &qubit : qubits)
    check_qubit(qubit);
  #endif

  const size_t NUM_QUBITS =  qubits.size();
  const int_t END = data_size_ >> NUM_QUBITS;
  auto qubits_sorted = qubits;
  std::sort(qubits_sorted.begin(), qubits_sorted.end());

  // Reduction variables
  double val_re = 0.;
  double val_im = 0.;
#pragma omp parallel reduction(+:val_re, val_im) if (num_qubits_ > omp_threshold_ && omp_threads_ > 1)         \
                                               num_threads(omp_threads_)
  {
#pragma omp for
    for (int_t k = 0; k < END; k++) {
      const auto inds = indexes(qubits, qubits_sorted, k);
      std::forward<Lambda>(func)(inds, val_re, val_im);
    }
  } // end omp parallel
  return complex_t(val_re, val_im);
}


template <typename data_t>
template<typename Lambda, typename list_t, typename param_t>
complex_t QubitVector<data_t>::apply_reduction_lambda(Lambda&& func,
                                                      const list_t &qubits,
                                                      const param_t &params) const {

  const auto NUM_QUBITS = qubits.size();
  // Error checking
  #ifdef DEBUG
  for (const auto &qubit : qubits)
    check_qubit(qubit);
  #endif

  const int_t END = data_size_ >> NUM_QUBITS;
  auto qubits_sorted = qubits;
  std::sort(qubits_sorted.begin(), qubits_sorted.end());

  // Reduction variables
  double val_re = 0.;
  double val_im = 0.;
#pragma omp parallel reduction(+:val_re, val_im) if (num_qubits_ > omp_threshold_ && omp_threads_ > 1)         \
                                               num_threads(omp_threads_)
  {
#pragma omp for
    for (int_t k = 0; k < END; k++) {
      const auto inds = indexes(qubits, qubits_sorted, k);
      std::forward<Lambda>(func)(inds, params, val_re, val_im);
    }
  } // end omp parallel
  return complex_t(val_re, val_im);
}


/*******************************************************************************
 *
 * MATRIX MULTIPLICATION
 *
 ******************************************************************************/
template <typename data_t>
void QubitVector<data_t>::apply_matrix(const reg_t &qubits,
                                       const cvector_t &mat) {
  
  const size_t N = qubits.size();
  // Error checking
  #ifdef DEBUG
  check_vector(mat, 2 * N);
  #endif

  // Static array optimized lambda functions
  switch (N) {
    case 1:
      apply_matrix(qubits[0], mat);
      return;
    case 2: {
      // Lambda function for 2-qubit matrix multiplication
      auto lambda = [&](const areg_t<4> &inds, const cvector_t &_mat)->void {
        std::array<complex_t, 4> cache;
        for (size_t i = 0; i < 4; i++) {
          const auto ii = inds[i];
          cache[i] = data_[ii];
          data_[ii] = 0.;
        }
        // update state vector
        for (size_t i = 0; i < 4; i++)
          for (size_t j = 0; j < 4; j++)
            data_[inds[i]] += _mat[i + 4 * j] * cache[j];
      };
      apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}), mat);
      return;
    }
    case 3: {
      // Lambda function for 3-qubit matrix multiplication
      auto lambda = [&](const areg_t<8> &inds, const cvector_t &_mat)->void {
        std::array<complex_t, 8> cache;
        for (size_t i = 0; i < 8; i++) {
          const auto ii = inds[i];
          cache[i] = data_[ii];
          data_[ii] = 0.;
        }
        // update state vector
        for (size_t i = 0; i < 8; i++)
          for (size_t j = 0; j < 8; j++)
            data_[inds[i]] += _mat[i + 8 * j] * cache[j];
      };
      apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}), mat);
      return;
    }
    case 4: {
      // Lambda function for 4-qubit matrix multiplication
      auto lambda = [&](const areg_t<16> &inds, const cvector_t &_mat)->void {
        std::array<complex_t, 16> cache;
        for (size_t i = 0; i < 16; i++) {
          const auto ii = inds[i];
          cache[i] = data_[ii];
          data_[ii] = 0.;
        }
        // update state vector
        for (size_t i = 0; i < 16; i++)
          for (size_t j = 0; j < 16; j++)
            data_[inds[i]] += _mat[i + 16 * j] * cache[j];
      };
      apply_lambda(lambda, areg_t<4>({{qubits[0], qubits[1], qubits[2], qubits[3]}}), mat);
      return;
    }
    default: {
      const uint_t DIM = BITS[N];
      // Lambda function for N-qubit matrix multiplication
      auto lambda = [&](const indexes_t &inds, const cvector_t &_mat)->void {
        auto cache = std::make_unique<complex_t[]>(DIM);
        for (size_t i = 0; i < DIM; i++) {
          const auto ii = inds[i];
          cache[i] = data_[ii];
          data_[ii] = 0.;
        }
        // update state vector
        for (size_t i = 0; i < DIM; i++)
          for (size_t j = 0; j < DIM; j++)
            data_[inds[i]] += _mat[i + DIM * j] * cache[j];
      };
      apply_lambda(lambda, qubits, mat);
    }
  } // end switch
}

template <typename data_t>
void QubitVector<data_t>::apply_matrix_sequence(const std::vector<reg_t> &regs,
                                         const std::vector<cvector_t> &mats) {
  if (mats.size() == 0)
    return;


#ifdef DEBUG
  if (regs.size() != mats.size());
    throw std::runtime_error("QubitVector<data_t>::apply_matrix_sequence allows same size of qubitss and mats.");
#endif

  bool at_most_two = true;
  // check 1 or 2 qubits
  for (const reg_t& reg: regs) {
    if (reg.size() > 2) {
      at_most_two = false;
      break;
    }
  }

  if (!at_most_two) {
    for (size_t i = 0; i < regs.size(); ++i)
      apply_matrix(regs[i], mats[i]);
    return;
  }


  reg_t sorted_qubits;
  for (const reg_t& reg: regs)
    for (const uint_t qubit: reg)
      if (std::find(sorted_qubits.begin(), sorted_qubits.end(), qubit) == sorted_qubits.end())
        sorted_qubits.push_back(qubit);

  std::sort(sorted_qubits.begin(), sorted_qubits.end());

  std::vector<cvector_t> sorted_mats;

  for (size_t i = 0; i < regs.size(); ++i) {
    const reg_t& reg = regs[i];
    const cvector_t& mat = mats[i];
    sorted_mats.push_back(expand_matrix(reg, sorted_qubits, mat));
  }

  auto U = sorted_mats[0];
  const auto dim = BITS[sorted_qubits.size()];

  for (size_t m = 1; m < sorted_mats.size(); m++) {

    cvector_t u_tmp(U.size(), (0., 0.));
    const cvector_t& u = sorted_mats[m];

    for (size_t i = 0; i < dim; ++i)
      for (size_t j = 0; j < dim; ++j)
        for (size_t k = 0; k < dim; ++k)
          u_tmp[i + j * dim] += u[i + k * dim] * U[k + j * dim];

    U = u_tmp;
  }

  apply_matrix(sorted_qubits, U);
}

template <typename data_t>
void QubitVector<data_t>::apply_diagonal_matrix(const reg_t &qubits,
                                                const cvector_t &diag) {
  const size_t N = qubits.size();
  // Error checking
  #ifdef DEBUG
  check_vector(diag, N);
  #endif

  switch (N) {
    case 1:
      apply_diagonal_matrix(qubits[0], diag);
      return;
    case 2: {
      // Lambda function for 2-qubit diagional matrix multiplication
      auto lambda = [&](const areg_t<4> &inds,
                        const cvector_t &_mat)->void {
        for (size_t i = 0; i < 4; i++) {
          data_[inds[i]] *= _mat[i];
        }
      };
      apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}), diag);
      return;
    }
    case 3: {
      // Lambda function for 3-qubit diagional matrix multiplication
      auto lambda = [&](const areg_t<8> &inds,
                        const cvector_t &_mat)->void {
        for (size_t i = 0; i < 8; i++) {
          data_[inds[i]] *= _mat[i];
        }
      };
      apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}), diag);
      return;
    }
    case 4: {
      // Lambda function for 4-qubit diagional matrix multiplication
      auto lambda = [&](const areg_t<16> &inds,
                        const cvector_t &_mat)->void {
        for (size_t i = 0; i < 16; i++) {
          data_[inds[i]] *= _mat[i];
        }
      };
      apply_lambda(lambda, areg_t<4>({{qubits[0], qubits[1], qubits[2], qubits[3]}}), diag);
      return;
    }
    default: {
      const uint_t DIM = BITS[N];
      // Lambda function for N-qubit diagional  matrix multiplication
      auto lambda = [&](const indexes_t &inds,
                        const cvector_t &_mat)->void {
        for (size_t i = 0; i < DIM; i++) {
          data_[inds[i]] *= _mat[i];
        }
      };
      apply_lambda(lambda, qubits, diag);
    }
  } // end switch
}

template <typename data_t>
void QubitVector<data_t>::apply_permutation_matrix(const reg_t& qubits,
                                                   const std::vector<std::pair<uint_t, uint_t>> &pairs) {
  const size_t N = qubits.size();

  // Error checking
  #ifdef DEBUG
  check_vector(diag, N);
  #endif

  switch (N) {
    case 1: {
      // Lambda function for permutation matrix
      auto lambda = [&](const areg_t<2> &inds)->void {
        for (const auto& p : pairs) {
          std::swap(data_[inds[p.first]], data_[inds[p.second]]);
        }
      };
      apply_lambda(lambda, areg_t<1>({{qubits[0]}}));
      return;
    }
    case 2: {
      // Lambda function for permutation matrix
      auto lambda = [&](const areg_t<4> &inds)->void {
        for (const auto& p : pairs) {
          std::swap(data_[inds[p.first]], data_[inds[p.second]]);
        }
      };
      apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}));
      return;
    }
    case 3: {
      // Lambda function for permutation matrix
      auto lambda = [&](const areg_t<8> &inds)->void {
        for (const auto& p : pairs) {
          std::swap(data_[inds[p.first]], data_[inds[p.second]]);
        }
      };
      apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}));
      return;
    }
    case 4: {
      // Lambda function for permutation matrix
      auto lambda = [&](const areg_t<16> &inds)->void {
        for (const auto& p : pairs) {
          std::swap(data_[inds[p.first]], data_[inds[p.second]]);
        }
      };
      apply_lambda(lambda, areg_t<4>({{qubits[0], qubits[1], qubits[2], qubits[3]}}));
      return;
    }
    default: {
      // Lambda function for permutation matrix
      auto lambda = [&](const indexes_t &inds)->void {
        for (const auto& p : pairs) {
          std::swap(data_[inds[p.first]], data_[inds[p.second]]);
        }
      };
      // Use the lambda function
      apply_lambda(lambda, qubits);
    }
  } // end switch
}


/*******************************************************************************
 *
 * APPLY OPTIMIZED GATES
 *
 ******************************************************************************/

//------------------------------------------------------------------------------
// Multi-controlled gates
//------------------------------------------------------------------------------

template <typename data_t>
void QubitVector<data_t>::apply_mcx(const reg_t &qubits) {
  // Calculate the permutation positions for the last qubit.
  const size_t N = qubits.size();
  const size_t pos0 = MASKS[N - 1];
  const size_t pos1 = MASKS[N];

  switch (N) {
    case 1: {
      // Lambda function for X gate
      auto lambda = [&](const areg_t<2> &inds)->void {
        std::swap(data_[inds[pos0]], data_[inds[pos1]]);
      };
      apply_lambda(lambda, areg_t<1>({{qubits[0]}}));
      return;
    }
    case 2: {
      // Lambda function for CX gate
      auto lambda = [&](const areg_t<4> &inds)->void {
        std::swap(data_[inds[pos0]], data_[inds[pos1]]);
      };
      apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}));
      return;
    }
    case 3: {
      // Lambda function for Toffli gate
      auto lambda = [&](const areg_t<8> &inds)->void {
        std::swap(data_[inds[pos0]], data_[inds[pos1]]);
      };
      apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}));
      return;
    }
    default: {
      // Lambda function for general multi-controlled X gate
      auto lambda = [&](const indexes_t &inds)->void {
        std::swap(data_[inds[pos0]], data_[inds[pos1]]);
      };
      apply_lambda(lambda, qubits);
    }
  } // end switch
}

template <typename data_t>
void QubitVector<data_t>::apply_mcy(const reg_t &qubits) {
  // Calculate the permutation positions for the last qubit.
  const size_t N = qubits.size();
  const size_t pos0 = MASKS[N - 1];
  const size_t pos1 = MASKS[N];
  const complex_t I(0., 1.);

  switch (N) {
    case 1: {
      // Lambda function for Y gate
      auto lambda = [&](const areg_t<2> &inds)->void {
        const complex_t cache = data_[inds[pos0]];
        data_[inds[pos0]] = -I * data_[inds[pos1]];
        data_[inds[pos1]] = I * cache;
      };
      apply_lambda(lambda, areg_t<1>({{qubits[0]}}));
      return;
    }
    case 2: {
      // Lambda function for CY gate
      auto lambda = [&](const areg_t<4> &inds)->void {
        const complex_t cache = data_[inds[pos0]];
        data_[inds[pos0]] = -I * data_[inds[pos1]];
        data_[inds[pos1]] = I * cache;
      };
      apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}));
      return;
    }
    case 3: {
      // Lambda function for CCY gate
      auto lambda = [&](const areg_t<8> &inds)->void {
        const complex_t cache = data_[inds[pos0]];
        data_[inds[pos0]] = -I * data_[inds[pos1]];
        data_[inds[pos1]] = I * cache;
      };
      apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}));
      return;
    }
    default: {
      // Lambda function for general multi-controlled Y gate
      auto lambda = [&](const indexes_t &inds)->void {
        const complex_t cache = data_[inds[pos0]];
        data_[inds[pos0]] = -I * data_[inds[pos1]];
        data_[inds[pos1]] = I * cache;
      };
      apply_lambda(lambda, qubits);
    }
  } // end switch
}

template <typename data_t>
void QubitVector<data_t>::apply_mcz(const reg_t &qubits) {
  const size_t N = qubits.size();

  switch (N) {
    case 1: {
      // Lambda function for Z gate
      auto lambda = [&](const areg_t<2> &inds)->void {
        data_[inds[1]] *= -1.;
      };
      apply_lambda(lambda, areg_t<1>({{qubits[0]}}));
      return;
    }
    case 2: {
      // Lambda function for CZ gate
      auto lambda = [&](const areg_t<4> &inds)->void {
        data_[inds[3]] *= -1.;
      };
      apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}));
      return;
    }
    case 3: {
      // Lambda function for CCZ gate
      auto lambda = [&](const areg_t<8> &inds)->void {
         data_[inds[7]] *= -1.;
      };
      apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}));
      return;
    }
    default: {
      // Lambda function for general multi-controlled X gate
      auto lambda = [&](const indexes_t &inds)->void {
         data_[inds[MASKS[N]]] *= -1.;
      };
      apply_lambda(lambda, qubits);
    }
  } // end switch
}


template <typename data_t>
void QubitVector<data_t>::apply_mcswap(const reg_t &qubits) {
  // Calculate the swap positions for the last two qubits.
  // If N = 2 this is just a regular SWAP gate rather than a controlled-SWAP gate.
  const size_t N = qubits.size();
  const size_t pos0 = MASKS[N - 1];
  const size_t pos1 = pos0 + BITS[N - 2];

  switch (N) {
    case 2: {
      // Lambda function for SWAP gate
      auto lambda = [&](const areg_t<4> &inds)->void {
        std::swap(data_[inds[pos0]], data_[inds[pos1]]);
      };
      apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}));
      return;
    }
    case 3: {
      // Lambda function for C-SWAP gate
      auto lambda = [&](const areg_t<8> &inds)->void {
        std::swap(data_[inds[pos0]], data_[inds[pos1]]);
      };
      apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}));
      return;
    }
    default: {
      // Lambda function for general multi-controlled SWAP gate
      auto lambda = [&](const indexes_t &inds)->void {
        std::swap(data_[inds[pos0]], data_[inds[pos1]]);
      };
      apply_lambda(lambda, qubits);
    }
  } // end switch
}

template <typename data_t>
void QubitVector<data_t>::apply_mcu(const reg_t &qubits,
                                    const cvector_t &mat) {
  // Calculate the permutation positions for the last qubit.
  const size_t N = qubits.size();
  const size_t pos0 = MASKS[N - 1];
  const size_t pos1 = MASKS[N];

  // Check if matrix is actually diagonal and if so use 
  // diagonal matrix lambda function
  // TODO: this should be changed to not check doubles with ==
  if (mat[1] == 0.0 && mat[2] == 0.0) {
    const cvector_t diag = {{mat[0], mat[3]}};
    // Diagonal version
    switch (N) {
      case 1: {
        // If N=1 this is just a single-qubit matrix
        apply_diagonal_matrix(qubits[0], diag);
        return;
      }
      case 2: {
        // Lambda function for CU gate
        auto lambda = [&](const areg_t<4> &inds,
                          const cvector_t &_diag)->void {
          data_[pos0] = _diag[0] * data_[pos0];
          data_[pos1] = _diag[1] * data_[pos1];
        };
        apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}), diag);
        return;
      }
      case 3: {
        // Lambda function for CCU gate
        auto lambda = [&](const areg_t<8> &inds,
                          const cvector_t &_diag)->void {
          data_[pos0] = _diag[0] * data_[pos0];
          data_[pos1] = _diag[1] * data_[pos1];
        };
        apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}), diag);
        return;
      }
      default: {
        // Lambda function for general multi-controlled U gate
        auto lambda = [&](const indexes_t &inds,
                          const cvector_t &_diag)->void {
          data_[pos0] = _diag[0] * data_[pos0];
          data_[pos1] = _diag[1] * data_[pos1];
        };
        apply_lambda(lambda, qubits, diag);
        return;
      }
    } // end switch
  }

  // Non-diagonal version
  switch (N) {
    case 1: {
      // If N=1 this is just a single-qubit matrix
      apply_matrix(qubits[0], mat);
      return;
    }
    case 2: {
      // Lambda function for CU gate
      auto lambda = [&](const areg_t<4> &inds,
                        const cvector_t &_mat)->void {
      const auto cache = data_[pos0];
      data_[pos0] = _mat[0] * data_[pos0] + _mat[2] * data_[pos1];
      data_[pos1] = _mat[1] * cache + _mat[3] * data_[pos1];
      };
      apply_lambda(lambda, areg_t<2>({{qubits[0], qubits[1]}}), mat);
      return;
    }
    case 3: {
      // Lambda function for CCU gate
      auto lambda = [&](const areg_t<8> &inds,
                        const cvector_t &_mat)->void {
      const auto cache = data_[pos0];
      data_[pos0] = _mat[0] * data_[pos0] + _mat[2] * data_[pos1];
      data_[pos1] = _mat[1] * cache + _mat[3] * data_[pos1];
      };
      apply_lambda(lambda, areg_t<3>({{qubits[0], qubits[1], qubits[2]}}), mat);
      return;
    }
    default: {
      // Lambda function for general multi-controlled U gate
      auto lambda = [&](const indexes_t &inds,
                        const cvector_t &_mat)->void {
      const auto cache = data_[pos0];
      data_[pos0] = _mat[0] * data_[pos0] + _mat[2] * data_[pos1];
      data_[pos1] = _mat[1] * cache + _mat[3] * data_[pos1];
      };
      apply_lambda(lambda, qubits, mat);
      return;
    }
  } // end switch
}

//------------------------------------------------------------------------------
// Single-qubit matrices
//------------------------------------------------------------------------------

template <typename data_t>
void QubitVector<data_t>::apply_matrix(const uint_t qubit,
                                       const cvector_t& mat) {
  // Check if matrix is actually diagonal and if so use 
  // apply_diagonal_matrix
  // TODO: this should be changed to not check doubles with ==
  if (mat[1] == 0.0 && mat[2] == 0.0) {
    const cvector_t diag = {{mat[0], mat[3]}};
    apply_diagonal_matrix(qubit, diag);
    return;
  }

  // Lambda function for single-qubit matrix multiplication
  auto lambda = [&](const areg_t<2> &inds,
                    const cvector_t &_mat)->void {
    const auto pos0 = inds[0];
    const auto pos1 = inds[1];
    const auto cache = data_[pos0];
    data_[pos0] = _mat[0] * data_[pos0] + _mat[2] * data_[pos1];
    data_[pos1] = _mat[1] * cache + _mat[3] * data_[pos1];
  };
  apply_lambda(lambda, areg_t<1>({{qubit}}), mat);
}

template <typename data_t>
void QubitVector<data_t>::apply_diagonal_matrix(const uint_t qubit,
                                                const cvector_t& diag) {
  // TODO: This should be changed so it isn't checking doubles with ==
  if (diag[0] == 1.0) {  // [[1, 0], [0, z]] matrix
    if (diag[1] == 1.0)
      return; // Identity

    if (diag[1] == complex_t(0., -1.)) { // [[1, 0], [0, -i]]
      auto lambda = [&](const areg_t<2> &inds,
                        const cvector_t &_mat)->void {
        const auto k = inds[1];
        double cache = data_[k].imag();
        data_[k].imag(data_[k].real() * -1.);
        data_[k].real(cache);
      };
      apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
      return;
    }
    if (diag[1] == complex_t(0., 1.)) {
      // [[1, 0], [0, i]]
      auto lambda = [&](const areg_t<2> &inds,
                        const cvector_t &_mat)->void {
        const auto k = inds[1];
        double cache = data_[k].imag();
        data_[k].imag(data_[k].real());
        data_[k].real(cache * -1.);
      };
      apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
      return;
    } 
    if (diag[0] == 0.0) {
      // [[1, 0], [0, 0]]
      auto lambda = [&](const areg_t<2> &inds,
                        const cvector_t &_mat)->void {
        data_[inds[1]] = 0.0;
      };
      apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
      return;
    } 
    // general [[1, 0], [0, z]]
    auto lambda = [&](const areg_t<2> &inds,
                      const cvector_t &_mat)->void {
      const auto k = inds[1];
      data_[k] *= _mat[1];
    };
    apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
    return;
  } else if (diag[1] == 1.0) {
    // [[z, 0], [0, 1]] matrix
    if (diag[0] == complex_t(0., -1.)) {
      // [[-i, 0], [0, 1]]
      auto lambda = [&](const areg_t<2> &inds,
                        const cvector_t &_mat)->void {
        const auto k = inds[1];
        double cache = data_[k].imag();
        data_[k].imag(data_[k].real() * -1.);
        data_[k].real(cache);
      };
      apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
      return;
    } 
    if (diag[0] == complex_t(0., 1.)) {
      // [[i, 0], [0, 1]]
      auto lambda = [&](const areg_t<2> &inds,
                        const cvector_t &_mat)->void {
        const auto k = inds[1];
        double cache = data_[k].imag();
        data_[k].imag(data_[k].real());
        data_[k].real(cache * -1.);
      };
      apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
      return;
    } 
    if (diag[0] == 0.0) {
      // [[0, 0], [0, 1]]
      auto lambda = [&](const areg_t<2> &inds,
                        const cvector_t &_mat)->void {
        data_[inds[0]] = 0.0;
      };
      apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
      return;
    } 
    // general [[z, 0], [0, 1]]
    auto lambda = [&](const areg_t<2> &inds,
                      const cvector_t &_mat)->void {
      const auto k = inds[0];
      data_[k] *= _mat[0];
    };
    apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
    return;
  } else {
    // Lambda function for diagonal matrix multiplication
    auto lambda = [&](const areg_t<2> &inds,
                      const cvector_t &_mat)->void {
      const auto k0 = inds[0];
      const auto k1 = inds[1];
      data_[k0] *= _mat[0];
      data_[k1] *= _mat[1];
    };
    apply_lambda(lambda, areg_t<1>({{qubit}}), diag);
  }
}

//------------------------------------------------------------------------------
// Gate-swap optimized helper functions
//------------------------------------------------------------------------------
template <typename data_t>
void QubitVector<data_t>::swap_cols_and_rows(const uint_t idx1,
                                             const uint_t idx2,
                                             cvector_t &mat,
                                             uint_t dim) const {

  uint_t mask1 = BITS[idx1];
  uint_t mask2 = BITS[idx2];

  for (uint_t first = 0; first < dim; ++first) {
    if ((first & mask1) && !(first & mask2)) {
      uint_t second = (first ^ mask1) | mask2;

      for (uint_t i = 0; i < dim; ++i) {
        complex_t cache = mat[first * dim + i];
        mat[first * dim + i] = mat[second * dim +  i];
        mat[second * dim +  i] = cache;
      }
      for (uint_t i = 0; i < dim; ++i) {
        complex_t cache = mat[i * dim + first];
        mat[i * dim + first] = mat[i * dim + second];
        mat[i * dim + second] = cache;
      }
    }
  }
}

template <typename data_t>
cvector_t QubitVector<data_t>::sort_matrix(const reg_t& src,
                                           const reg_t& sorted,
                                           const cvector_t &mat) const {

  const uint_t N = src.size();
  const uint_t DIM = BITS[N];
  auto ret = mat;
  auto current = src;

  while (current != sorted) {
    uint_t from;
    uint_t to;
    for (from = 0; from < current.size(); ++from)
      if (current[from] != sorted[from])
        break;
    if (from == current.size())
      break;
    for (to = from + 1; to < current.size(); ++to)
      if (current[from] == sorted[to])
        break;
    if (to == current.size()) {
      throw std::runtime_error("QubitVector<data_t>::sort_matrix we should not reach here");
    }
    swap_cols_and_rows(from, to, ret, DIM);
    std::swap(current[from], current[to]);
  }

  return ret;
}

template <typename data_t>
cvector_t QubitVector<data_t>::expand_matrix(const reg_t& src_qubits, const reg_t& dst_sorted_qubits, const cvector_t& vmat) const {

  const auto dst_dim = BITS[dst_sorted_qubits.size()];
  const auto dst_vmat_size = dst_dim * dst_dim;
  const auto src_dim = BITS[src_qubits.size()];

  // generate a matrix for op
  cvector_t u(dst_vmat_size, (.0, .0));
  std::vector<bool> filled(dst_dim, false);

  if (src_qubits.size() == 1) { //1-qubit operation
    // 1. identify delta
    const auto index = std::distance(dst_sorted_qubits.begin(),
                               std::find(dst_sorted_qubits.begin(), dst_sorted_qubits.end(), src_qubits[0]));

    const auto delta = BITS[index];

    // 2. find vmat(0, 0) position in U
    for (uint_t i = 0; i < dst_dim; ++i) {

      if (filled[i])
        continue;

      //  3. allocate op.u to u based on u(i, i) and delta
      u[i           + (i + 0)     * dst_dim] = vmat[0 + 0  * src_dim];
      u[i           + (i + delta) * dst_dim] = vmat[0 + 1  * src_dim];
      u[(i + delta) + (i + 0)     * dst_dim] = vmat[1 + 0  * src_dim];
      u[(i + delta) + (i + delta) * dst_dim] = vmat[1 + 1  * src_dim];
      filled[i] = filled[i + delta] = true;
    }
  } else if (src_qubits.size() == 2) { //2-qubit operation

    reg_t sorted_src_qubits = src_qubits;
    std::sort(sorted_src_qubits.begin(), sorted_src_qubits.end());
    const cvector_t sorted_vmat = sort_matrix(src_qubits, sorted_src_qubits, vmat);

    // 1. identify low and high delta
    auto low = std::distance(dst_sorted_qubits.begin(),
                               std::find(dst_sorted_qubits.begin(), dst_sorted_qubits.end(), src_qubits[0]));

    auto high = std::distance(dst_sorted_qubits.begin(),
                                std::find(dst_sorted_qubits.begin(), dst_sorted_qubits.end(), src_qubits[1]));

    auto low_delta = BITS[low];
    auto high_delta = BITS[high];

    // 2. find op.u(0, 0) position in U
    for (uint_t i = 0; i < dst_dim; ++i) {
      if (filled[i])
        continue;

      //  3. allocate vmat to u based on u(i, i) and delta
      u[i                             + (i + 0)                      * dst_dim] = sorted_vmat[0 + 0 * src_dim];
      u[i                             + (i + low_delta)              * dst_dim] = sorted_vmat[0 + 1 * src_dim];
      u[i                             + (i + high_delta)             * dst_dim] = sorted_vmat[0 + 2 * src_dim];
      u[i                             + (i + low_delta + high_delta) * dst_dim] = sorted_vmat[0 + 3 * src_dim];
      u[(i + low_delta)               + (i + 0)                      * dst_dim] = sorted_vmat[1 + 0 * src_dim];
      u[(i + low_delta)               + (i + low_delta)              * dst_dim] = sorted_vmat[1 + 1 * src_dim];
      u[(i + low_delta)               + (i + high_delta)             * dst_dim] = sorted_vmat[1 + 2 * src_dim];
      u[(i + low_delta)               + (i + low_delta + high_delta) * dst_dim] = sorted_vmat[1 + 3 * src_dim];
      u[(i + high_delta)              + (i + 0)                      * dst_dim] = sorted_vmat[2 + 0 * src_dim];
      u[(i + high_delta)              + (i + low_delta)              * dst_dim] = sorted_vmat[2 + 1 * src_dim];
      u[(i + high_delta)              + (i + high_delta)             * dst_dim] = sorted_vmat[2 + 2 * src_dim];
      u[(i + high_delta)              + (i + low_delta + high_delta) * dst_dim] = sorted_vmat[2 + 3 * src_dim];
      u[(i + low_delta + high_delta)  + (i + 0)                      * dst_dim] = sorted_vmat[3 + 0 * src_dim];
      u[(i + low_delta + high_delta)  + (i + low_delta)              * dst_dim] = sorted_vmat[3 + 1 * src_dim];
      u[(i + low_delta + high_delta)  + (i + high_delta)             * dst_dim] = sorted_vmat[3 + 2 * src_dim];
      u[(i + low_delta + high_delta)  + (i + low_delta + high_delta) * dst_dim] = sorted_vmat[3 + 3 * src_dim];

      filled[i] = true;
      filled[i + low_delta] = true;
      filled[i + high_delta] = true;
      filled[i + low_delta + high_delta] = true;
    }
    //TODO: } else if (src_qubits.size() == 3) {
  } else {
    std::stringstream ss;
    ss << "Fusion::illegal qubit number:" << src_qubits.size();
    throw std::runtime_error(ss.str());
  }

  return u;
}
/*******************************************************************************
 *
 * NORMS
 *
 ******************************************************************************/
template <typename data_t>
double QubitVector<data_t>::norm() const {
  // Lambda function for norm
  auto lambda = [&](int_t k, double &val_re, double &val_im)->void {
    (void)val_im; // unused
    val_re += std::real(data_[k] * std::conj(data_[k]));
  };
  return std::real(apply_reduction_lambda(lambda));
}

template <typename data_t>
double QubitVector<data_t>::norm(const reg_t &qubits, const cvector_t &mat) const {

  const uint_t N = qubits.size();

  // Error checking
  #ifdef DEBUG
  check_vector(mat, 2 * N);
  #endif

  // Static array optimized lambda functions
  switch (N) {
    case 1:
      return norm(qubits[0], mat);
    case 2: {
      // Lambda function for 2-qubit matrix norm
      auto lambda = [&](const areg_t<4> &inds, const cvector_t &_mat, 
                        double &val_re, double &val_im)->void {
        (void)val_im; // unused
        for (size_t i = 0; i < 4; i++) {
          complex_t vi = 0;
          for (size_t j = 0; j < 4; j++)
            vi += _mat[i + 4 * j] * data_[inds[j]];
          val_re += std::real(vi * std::conj(vi));
        }
      };
      areg_t<2> qubits_arr = {{qubits[0], qubits[1]}};
      return std::real(apply_reduction_lambda(lambda, qubits_arr, mat));
    }
    case 3: {
      // Lambda function for 3-qubit matrix norm
      auto lambda = [&](const areg_t<8> &inds, const cvector_t &_mat, 
                        double &val_re, double &val_im)->void {
        (void)val_im; // unused
        for (size_t i = 0; i < 8; i++) {
          complex_t vi = 0;
          for (size_t j = 0; j < 8; j++)
            vi += _mat[i + 8 * j] * data_[inds[j]];
          val_re += std::real(vi * std::conj(vi));
        }
      };
      areg_t<3> qubits_arr = {{qubits[0], qubits[1], qubits[2]}};
      return std::real(apply_reduction_lambda(lambda, qubits_arr, mat));
    }
    case 4: {
      // Lambda function for 4-qubit matrix norm
      auto lambda = [&](const areg_t<16> &inds, const cvector_t &_mat, 
                        double &val_re, double &val_im)->void {
        (void)val_im; // unused
        for (size_t i = 0; i < 16; i++) {
          complex_t vi = 0;
          for (size_t j = 0; j < 16; j++)
            vi += _mat[i + 16 * j] * data_[inds[j]];
          val_re += std::real(vi * std::conj(vi));
        }
      };
      areg_t<4> qubits_arr = {{qubits[0], qubits[1], qubits[2], qubits[3]}};
      return std::real(apply_reduction_lambda(lambda, qubits_arr, mat));
    }
    default: {
      // Lambda function for N-qubit matrix norm
      const uint_t DIM = BITS[N];
      auto lambda = [&](const indexes_t &inds, const cvector_t &_mat, 
                        double &val_re, double &val_im)->void {
        (void)val_im; // unused
        for (size_t i = 0; i < DIM; i++) {
          complex_t vi = 0;
          for (size_t j = 0; j < DIM; j++)
            vi += _mat[i + DIM * j] * data_[inds[j]];
          val_re += std::real(vi * std::conj(vi));
        }
      };
      // Use the lambda function
      return std::real(apply_reduction_lambda(lambda, qubits, mat));
    }
  } // end switch
}

template <typename data_t>
double QubitVector<data_t>::norm_diagonal(const reg_t &qubits, const cvector_t &mat) const {

  const uint_t N = qubits.size();

  // Error checking
  #ifdef DEBUG
  check_vector(mat, N);
  #endif

  // Static array optimized lambda functions
  switch (N) {
    case 1:
      return norm_diagonal(qubits[0], mat);
    case 2: {
      // Lambda function for 2-qubit matrix norm
      auto lambda = [&](const areg_t<4> &inds, const cvector_t &_mat, 
                        double &val_re, double &val_im)->void {
        (void)val_im; // unused
        for (size_t i = 0; i < 4; i++) {
          const auto vi = _mat[i] * data_[inds[i]];
          val_re += std::real(vi * std::conj(vi));
        }
      };
      areg_t<2> qubits_arr = {{qubits[0], qubits[1]}};
      return std::real(apply_reduction_lambda(lambda, qubits_arr, mat));
    }
    case 3: {
      // Lambda function for 3-qubit matrix norm
      auto lambda = [&](const areg_t<8> &inds, const cvector_t &_mat, 
                        double &val_re, double &val_im)->void {
        (void)val_im; // unused
        for (size_t i = 0; i < 8; i++) {
          const auto vi = _mat[i] * data_[inds[i]];
          val_re += std::real(vi * std::conj(vi));
        }
      };
      areg_t<3> qubits_arr = {{qubits[0], qubits[1], qubits[2]}};
      return std::real(apply_reduction_lambda(lambda, qubits_arr, mat));
    }
    case 4: {
      // Lambda function for 4-qubit matrix norm
      auto lambda = [&](const areg_t<16> &inds, const cvector_t &_mat, 
                        double &val_re, double &val_im)->void {
        (void)val_im; // unused
        for (size_t i = 0; i < 16; i++) {
          const auto vi = _mat[i] * data_[inds[i]];
          val_re += std::real(vi * std::conj(vi));
        }
      };
      areg_t<4> qubits_arr = {{qubits[0], qubits[1], qubits[2], qubits[3]}};
      return std::real(apply_reduction_lambda(lambda, qubits_arr, mat));
    }
    default: {
      // Lambda function for N-qubit matrix norm
      const uint_t DIM = BITS[N];
      auto lambda = [&](const indexes_t &inds, const cvector_t &_mat,
                        double &val_re, double &val_im)->void {
        (void)val_im; // unused
        for (size_t i = 0; i < DIM; i++) {
          const auto vi = _mat[i] * data_[inds[i]];
          val_re += std::real(vi * std::conj(vi));
        }
      };
      // Use the lambda function
      return std::real(apply_reduction_lambda(lambda, qubits, mat));
    }
  } // end switch
}

//------------------------------------------------------------------------------
// Single-qubit specialization
//------------------------------------------------------------------------------
template <typename data_t>
double QubitVector<data_t>::norm(const uint_t qubit, const cvector_t &mat) const {
  // Error handling
  #ifdef DEBUG
  check_vector(mat, 2);
  #endif

  // Check if input matrix is diagonal, and if so use diagonal function.
  if (mat[1] == 0.0 && mat[2] == 0.0) {
    const cvector_t diag = {{mat[0], mat[3]}};
    return norm_diagonal(qubit, diag);
  }

  // Lambda function for norm reduction to real value.
  auto lambda = [&](const areg_t<2> &inds,
                    const cvector_t &_mat,
                    double &val_re,
                    double &val_im)->void {
    (void)val_im; // unused
    const auto v0 = _mat[0] * data_[inds[0]] + _mat[2] * data_[inds[1]];
    const auto v1 = _mat[1] * data_[inds[0]] + _mat[3] * data_[inds[1]];
    val_re += std::real(v0 * std::conj(v0)) + std::real(v1 * std::conj(v1));
  };
  return std::real(apply_reduction_lambda(lambda, areg_t<1>({{qubit}}), mat));
}

template <typename data_t>
double QubitVector<data_t>::norm_diagonal(const uint_t qubit, const cvector_t &mat) const {
  // Error handling
  #ifdef DEBUG
  check_vector(mat, 1);
  #endif
  // Lambda function for norm reduction to real value.
  auto lambda = [&](const areg_t<2> &inds,
                    const cvector_t &_mat,
                    double &val_re,
                    double &val_im)->void {
    (void)val_im; // unused
    const auto v0 = _mat[0] * data_[inds[0]];
    const auto v1 = _mat[1] * data_[inds[1]];
    val_re += std::real(v0 * std::conj(v0)) + std::real(v1 * std::conj(v1));
  };
  return std::real(apply_reduction_lambda(lambda, areg_t<1>({{qubit}}), mat));
}


/*******************************************************************************
 *
 * Probabilities
 *
 ******************************************************************************/

template <typename data_t>
double QubitVector<data_t>::probability(const uint_t outcome) const {
  const auto v = data_[outcome];
  return std::real(v * std::conj(v));
}

template <typename data_t>
rvector_t QubitVector<data_t>::probabilities() const {
  rvector_t probs(data_size_);
  const int_t END = data_size_;
  probs.assign(data_size_, 0.);

#pragma omp parallel for if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  for (int_t j=0; j < END; j++) {
    probs[j] = probability(j);
  }
  return probs;
}

template <typename data_t>
rvector_t QubitVector<data_t>::probabilities(const reg_t &qubits) const {

  const size_t N = qubits.size();

  if (N == 1)
    return probabilities(qubits[0]);

  const uint_t DIM = BITS[N];
  const uint_t END = BITS[num_qubits_ - N];

  // Error checking
  #ifdef DEBUG
  for (const auto &qubit : qubits)
    check_qubit(qubit);
  #endif

  if (N == 0)
    return rvector_t({norm()});

  auto qubits_sorted = qubits;
  std::sort(qubits_sorted.begin(), qubits_sorted.end());
  if ((N == num_qubits_) && (qubits == qubits_sorted))
    return probabilities();

  rvector_t probs(DIM, 0.);
  #pragma omp parallel if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
  {
    rvector_t probs_private(DIM, 0.);
    #pragma omp for
      for (int_t k = 0; k < END; k++) {
        auto idx = indexes(qubits, qubits_sorted, k);
        for (int_t m = 0; m < DIM; ++m) {
          probs_private[m] += probability(idx[m]);
        }
      }
    #pragma omp critical
    for (int_t m = 0; m < DIM; ++m) {
      probs[m] += probs_private[m];
    }
  }
  
  return probs;
}

//------------------------------------------------------------------------------
// Single-qubit specialization
//------------------------------------------------------------------------------

template <typename data_t>
rvector_t QubitVector<data_t>::probabilities(const uint_t qubit) const {

  // Error handling
  #ifdef DEBUG
  check_qubit(qubit);
  #endif

  // Lambda function for single qubit probs as reduction
  // p(0) stored as real part p(1) as imag part
  auto lambda = [&](const areg_t<2> &inds,
                    double &val_p0,
                    double &val_p1)->void {
    val_p0 += probability(inds[0]);
    val_p1 += probability(inds[1]);
  };
  auto p0p1 = apply_reduction_lambda(lambda, areg_t<1>({{qubit}}));
  return rvector_t({std::real(p0p1), std::imag(p0p1)});
}


//------------------------------------------------------------------------------
// Sample measure outcomes
//------------------------------------------------------------------------------
template <typename data_t>
reg_t QubitVector<data_t>::sample_measure(const std::vector<double> &rnds) const {

  const int_t END = data_size_;
  const int_t SHOTS = rnds.size();
  reg_t samples;
  samples.assign(SHOTS, 0);

  const int INDEX_SIZE = sample_measure_index_size_;
  const int_t INDEX_END = BITS[INDEX_SIZE];
  // Qubit number is below index size, loop over shots
  if (END < INDEX_END) {
    #pragma omp parallel if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
    {
      #pragma omp for
      for (int_t i = 0; i < SHOTS; ++i) {
        double rnd = rnds[i];
        double p = .0;
        int_t sample;
        for (sample = 0; sample < END - 1; ++sample) {
          p += std::real(std::conj(data_[sample]) * data_[sample]);
          if (rnd < p)
            break;
        }
        samples[i] = sample;
      }
    } // end omp parallel
  }
  // Qubit number is above index size, loop over index blocks
  else {
    // Initialize indexes
    std::vector<double> idxs;
    idxs.assign(INDEX_END, 0.0);
    uint_t loop = (END >> INDEX_SIZE);
    #pragma omp parallel if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
    {
      #pragma omp for
      for (int_t i = 0; i < INDEX_END; ++i) {
        uint_t base = loop * i;
        double total = .0;
        double p = .0;
        for (uint_t j = 0; j < loop; ++j) {
          uint_t k = base | j;
          p = std::real(std::conj(data_[k]) * data_[k]);
          total += p;
        }
        idxs[i] = total;
      }
    } // end omp parallel

    #pragma omp parallel if (num_qubits_ > omp_threshold_ && omp_threads_ > 1) num_threads(omp_threads_)
    {
      #pragma omp for
      for (int_t i = 0; i < SHOTS; ++i) {
        double rnd = rnds[i];
        double p = .0;
        int_t sample = 0;
        for (uint_t j = 0; j < idxs.size(); ++j) {
          if (rnd < (p + idxs[j])) {
            break;
          }
          p += idxs[j];
          sample += loop;
        }

        for (; sample < END - 1; ++sample) {
          p += std::real(std::conj(data_[sample]) * data_[sample]);
          if (rnd < p){
            break;
          }
        }
        samples[i] = sample;
      }
    } // end omp parallel
  }
  return samples;
}

//------------------------------------------------------------------------------
} // end namespace QV
//------------------------------------------------------------------------------

// ostream overload for templated qubitvector
template <typename data_t>
inline std::ostream &operator<<(std::ostream &out, const QV::QubitVector<data_t>&qv) {

  out << "[";
  size_t last = qv.size() - 1;
  for (size_t i = 0; i < qv.size(); ++i) {
    out << qv[i];
    if (i != last)
      out << ", ";
  }
  out << "]";
  return out;
}

//------------------------------------------------------------------------------
#endif // end module
