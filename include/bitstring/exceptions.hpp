#ifndef header_bitstring_exceptions_hpp
#define header_bitstring_exceptions_hpp

#include <stdexcept>

namespace bitstring {

class parse_error : public std::runtime_error {
  using runtime_error::runtime_error;
};

}

#endif