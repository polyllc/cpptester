/*
 * Tests the testOne tester function in a variety of different ways.
 */
#include <iostream>
#include "../tester.h"

using namespace TesterLib;

int main() {
    // create a tester object
    // required for all testing!
    // you may, if you'd like, create many different tester objects for different tests groups
    // although the tester object will differentiate between tests group, subgroups and subtests
    // you may want to read upon that in reference.md
    Tester tester;

    // this will run a test and compare 1 and 2, and return a result object with the information
    // it will save the result in the default test group
    Result result = tester.testOne(1, 2);
    // result.state -> false
    // result.testNum -> 0
    // ... plus some other information

    // now we can print it by either:
    // using std::cout
    std::cout << result.getMessage();

    // or using tester.printResults()
    tester.printResults();

    // the latter will print something like this!
    /**
     *  (default) | 0/1 passed | Status: SUCCESS
        ----------------------------------------------------------
        |- Group 1 | Test 1 | Result: false | Test #1 Failure
        |       expected:  2      was:  1
        |       expected type: int       was: int
        |       at: /cpptester/examples/testOne.cpp:19
        |       called in: int main()
        |       as: testOne(int actual = 1, int expected = 2, std::string message = "")
        |
     */

    // there are other aliases for testOne
    tester.testTrue(1 > 2);
    tester.testFalse(2 < 2);
    // they just replace the true and false and expect a bool
    tester.printResults();
    return 0;
}