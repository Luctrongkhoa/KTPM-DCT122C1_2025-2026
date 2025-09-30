#!/usr/bin/env python3
"""
divide.py - Simple division function + unit tests for CI/CD.

Usage:
  # Run a single division
  python divide.py --a 10 --b 2

  # Run tests (exit code 0 on success, 1 on failure)
  python divide.py --test
"""
from __future__ import annotations
import argparse
import sys
import unittest

def divide(a, b) -> float:
    """
    Return a / b as float.
    Raises:
      ValueError: if inputs cannot be converted to float
      ZeroDivisionError: if b == 0
    """
    try:
        a_f = float(a)
        b_f = float(b)
    except (TypeError, ValueError):
        raise ValueError("Inputs must be numbers") from None

    if b_f == 0.0:
        raise ZeroDivisionError("division by zero")

    return a_f / b_f


# -----------------------
# Unit tests
# -----------------------
class TestDivide(unittest.TestCase):
    def test_integer_division(self):
        self.assertEqual(divide(6, 3), 2.0)

    def test_float_division(self):
        self.assertAlmostEqual(divide(1, 3), 0.3333333333333, places=7)

    def test_negative(self):
        self.assertEqual(divide(-6, 3), -2.0)

    def test_zero_division(self):
        with self.assertRaises(ZeroDivisionError):
            divide(1, 0)

    def test_invalid_input(self):
        with self.assertRaises(ValueError):
            divide("not-a-number", 2)


# -----------------------
# CLI & test runner
# -----------------------
def run_tests_and_exit():
    # run unittest programmatically and set process exit code accordingly
    test_prog = unittest.main(argv=[sys.argv[0]], exit=False)
    # test_prog.result is populated after run() completes
    if not getattr(test_prog, "result", None) or not test_prog.result.wasSuccessful():
        sys.exit(1)
    sys.exit(0)


def main():
    parser = argparse.ArgumentParser(description="Simple divide utility (with tests).")
    parser.add_argument("--a", type=float, help="Numerator (a)")
    parser.add_argument("--b", type=float, help="Denominator (b)")
    parser.add_argument("--test", action="store_true", help="Run unit tests")
    args = parser.parse_args()

    if args.test:
        run_tests_and_exit()

    if args.a is None or args.b is None:
        parser.print_help()
        sys.exit(2)

    try:
        result = divide(args.a, args.b)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

    print(result)
    sys.exit(0)


if __name__ == "__main__":
    main()
