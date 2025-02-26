#include "../tester.h"

using namespace TesterLib;

int main() {
    // create a tester object
    // required for all testing!
    // you may, if you'd like, create many different tester objects for different tests groups
    // although the tester object will differentiate between tests group, subgroups and subtests
    // you may want to read upon that in reference.md
    Tester tester;

    // This tests 2.0 and 2.1 with a range of +/- 0.1
    tester.testFloat(2.0, 2.1, 0.1);

    // you may also want to explicitly set your upper and lower bound
    // where lowerBound is how much lower expected can be from actual
    // and upperBound is how much higher it can be
    // so expected is in between actual - lowerBound and actual + upperBound
    tester.testFloat(2.1, 0.1, 2.1, 0);

    tester.testFloat(2.0, 1.9, 0.15);

    tester.printResults();

    return 0;
}