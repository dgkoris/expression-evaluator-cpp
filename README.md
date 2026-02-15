# expression-evaluator-cpp
Arithmetic expression evaluator in C++.

## Parser Overview

The expression evaluator parses and evaluates integer arithmetic expressions (left to right using + - * / and no precedence).

### Key Components

- **skipWhitespace()**
  Skips whitespace.

- **parseInteger()**
  Reads an integer value and detects overflow safely.

- **parseValue()**
  Parses the smallest unit of an expression:
  - integer values
  - parenthesised expressions
  - unary + and -

- **parseExpression()**
  Evaluates expressions left to right using + - * / (no precedence).

- **parseOperator()**
  Reads the next operator if present.

- **evaluate()**
  Entry point that validates and evaluates the full expression.
  