#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>
using namespace std;

int solveQuartic(double a, double b, double c, double x[]) {
    if (a == 0 && b == 0 && c == 0) {
        return -1;
    }
    if (a == 0 && b == 0) {
        return 0;
    }
    if (a == 0) {
        double y = -c / b;
        if (y < 0) return 0;
        x[0] = sqrt(y);
        x[1] = -sqrt(y);
        return 2;
    }
    double delta = b * b - 4 * a * c;
    if (delta < 0) return 0;
    double y1 = (-b + sqrt(delta)) / (2 * a);
    double y2 = (-b - sqrt(delta)) / (2 * a);
    int count = 0;
    if (y1 >= 0) {
        x[count++] = sqrt(y1);
        x[count++] = -sqrt(y1);
    }
    if (y2 >= 0 && y2 != y1) {
        x[count++] = sqrt(y2);
        x[count++] = -sqrt(y2);
    }
    return count;
}

// helper to evaluate polynomial a*x^4 + b*x^2 + c
double poly(double a, double b, double c, double x) {
    return ((a * x * x) * (x * x)) + (b * x * x) + c;
}

// Normalize and compare solutions (order independent)
bool check_solutions(double a, double b, double c, int reported_n, double reported_x[], int expected_n) {
    // If expected is -1 (infinite) then reported must be -1
    if (expected_n == -1) return reported_n == -1;
    if (reported_n != expected_n) return false;
    // For each reported root, check that it satisfies polynomial within tolerance
    double eps = 1e-6;
    for (int i = 0; i < reported_n; ++i) {
        double val = poly(a,b,c,reported_x[i]);
        if (!(fabs(val) <= 1e-5)) {
            return false;
        }
    }
    return true;
}

int main() {
    struct Test { double a,b,c; int expected_n; string note; };
    vector<Test> tests = {
        {0,0,0, -1, "all zero -> infinite"},
        {0,0,1, 0, "a=0,b=0,c!=0 -> no solution"},
        {0,1,-4, 2, "linear in y -> y=4 -> x=+-2"},
        {0,1,4, 0, "linear in y -> y=-4 -> no real x"},
        {1,0,1, 0, "delta < 0 in y -> no real x"},
        {1,-5,4, 4, "y roots 4 and 1 -> x=+-2,+-1"},
        {1,-2,1, 2, "double y root 1 -> x=+-1"},
        {1,0,-1, 2, "y roots 1 and -1 -> only y=1 -> +-1"},
        {1,0,0, 1, "y double root 0 -> mathematically x=0 unique (code returns two zeros) - test expects 1 to detect duplicate issue"}
    };

    cout << fixed << setprecision(8);
    int pass = 0;
    for (size_t i = 0; i < tests.size(); ++i) {
        double x[4] = {0,0,0,0};
        int n = solveQuartic(tests[i].a, tests[i].b, tests[i].c, x);
        // For the last case (1,0,0) the provided code returns 2 (0 and -0) but mathematically unique root is 1 (x=0).
        int expected_n = tests[i].expected_n;
        // For the special test expecting 1 (unique zero root) we will treat it as expected mismatch to flag the issue.
        bool ok = check_solutions(tests[i].a, tests[i].b, tests[i].c, n, x, (expected_n==-1? -1 : tests[i].expected_n));
        // However for case where expected_n==1 but code returns 2 (two zeros), check_solutions will pass since both zeros satisfy polynomial.
        // To detect duplicate-root issue we additionally flag if expected_n==1 but reported n>1 and values equal (duplicates).
        bool duplicate_zero_issue = false;
        if (tests[i].a == 1 && tests[i].b == 0 && tests[i].c == 0) {
            if (n == 2 && fabs(x[0]) < 1e-12 && fabs(x[1]) < 1e-12) duplicate_zero_issue = true;
        }

        cout << "Test " << i+1 << ": a=" << tests[i].a << " b=" << tests[i].b << " c=" << tests[i].c
             << " | expected_n=" << tests[i].expected_n << " | reported_n=" << n << " | note: " << tests[i].note << "\n";
        cout << "   reported roots: ";
        for (int k=0;k<n;k++) cout << x[k] << " ";
        cout << "\n";
        if (ok) {
            if (duplicate_zero_issue) {
                cout << "   -> WARNING: Duplicate zero roots returned (logical issue). MARK TEST AS FAIL to highlight bug.\n";
            } else {
                cout << "   -> OK (satisfies polynomial and expected count)\n";
                pass++;
            }
        } else {
            cout << "   -> FAIL (mismatch or root does not satisfy polynomial)\n";
        }
        cout << "-----------------------------------------------------------\n";
    }
    cout << "Summary: passed " << pass << " / " << tests.size() << " tests (some tests intentionally highlight issues).\n";
    return 0;
}
