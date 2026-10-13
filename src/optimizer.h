#pragma once
#include <memory>
#include "ast.h"

// Constant folding + dead code elimination.
// Takes ownership of a subtree and returns the optimized replacement.
std::unique_ptr<Node> optimize(std::unique_ptr<Node> n);