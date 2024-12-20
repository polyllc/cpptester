#include "../tester.h"

using namespace TesterLib;

int main() {
    // create a tester object
    // required for all testing!
    // you may, if you'd like, create many different tester objects for different tests groups
    // although the tester object will differentiate between tests group, subgroups and subtests
    // you may want to read upon that in reference.md
    Tester tester;

    // We want to set up our actual and expected, ints and doubles respectively
    std::vector<int> ints {1, 2, 3, 4};
    std::vector<double> doubles {3.0, 2.0, 1.4, 4.0};

    // This will then go through the nth value in each vector and then
    // check for equality!
    tester.testType(ints, doubles);

    // you may also want to add a message or messages
    tester.testType(ints, doubles, "hi",
                    {"1 and 3.0",
                     "2 and 2.0",
                     "3 and 1.4",
                     "4 and 4.0"});

    // you may safely ignore std::source_location, it will fill in with a default argument that is the calling line

    tester.printResults();

    return 0;
}