/*! \file semantic_error.hpp
Define a typed exception to indicate semantic errors during evaluation.
 */

#ifndef SEMANTIC_ERROR_HPP
#define SEMANTIC_ERROR_HPP

#include <exception>
#include <stdexcept>

/*! \class SemanticError
\brief Exception subclass to indicate semantic errors during evaluation
 */
class SemanticError: public std::runtime_error {
public:
  /// Construct an exeption with a given message
  SemanticError(const std::string& message): std::runtime_error(message){};
};

#endif
