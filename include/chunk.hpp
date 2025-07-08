// In bytecode format, one bytecode represents an operation. This can be lookup, add, subtract, etc.
// The operation is represented using an operation code.

#include "common.hpp"

typedef enum {
    OP_RETURN
} OpCode;
