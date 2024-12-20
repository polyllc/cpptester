#include "../tester.h"

using namespace TesterLib;

int addOne(long long num, int two, int three, double four, std::string five) {
    return ++num;
}

int main() {
    // create a tester object
    // required for all testing!
    // you may, if you'd like, create many different tester objects for different tests groups
    // although the tester object will differentiate between tests group, subgroups and subtests
    // you may want to read upon that in reference.md
    Tester tester;

    // our expected values, if it is too small, it will use the last value
    std::vector<int> expected {2, 3, 4, 5, 6, 7};
    // in this case we will use a smaller vector to demonstrate that

    // the messages appended to the nth test, if it is too small, a blank string will be appended
    std::vector<std::string> messages {"message1", "message2" , "etc.etc"};

    // then we run it with our arguments!
    tester.testRange(1, 10, expected, std::string("hi"), messages, addOne, 2, 3, 4.0, "");
    // what this does is:
    // - from 1 to 10 -> i
    //  - call addOne(i, 2, 3, 4.0, "")
    // - check the result with your expected
    // in the results, after the colored test number and group number, it'll say
    // Test # Success/Failure
    // # represents what number was put in as the first parameter

    // you may want to generate expected and messages with a helper function

    // when you run the function above, it will display the wrong "at" and "called in"
    // this is because this delegates a function call for a many overrides of this function
    // you can simply add std::source_location::current() as the first parameter of the function
    // it'll look like this
    tester.testRange(std::source_location::current(), 1, 10, expected, std::string("hi"), messages, addOne, 2, 3, 4.0, "");
    // it will now point to this line for "at" and int main() for "called in"

    // if you don't put an expected vector, it will just check for exceptions
    tester.testRange(1, 10, addOne, 2, 3, 4.0, "hi");

    // of course, a source_location overload is available
    tester.testRange(std::source_location::current(), 1, 10, addOne, 2, 3, 4.0, "");


    // common issues:
    // when you're writing a testRange, make sure that if you want a message,
    // put in an std::string() as the compiler may think that it is a const char[]
    // like this tester.testRange(std::source_location::current(), 1, 2, expected, "hi", messages, addOne, ...); // wrong
    //           tester.testRange(std::source_location::current(), 1, 2, expected, std::string("hi"), messages, addOne, ...); // right

    tester.printResults();
}