#include "../tester.h"

using namespace TesterLib;

int addOne(int num, int two, int three, double four, std::string five) {
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

    tester.testRange(std::source_location::current(), 1, 10, addOne, 2, 3, 4, "");
    tester.testRange( std::source_location::current(), 1, 10, expected, std::string(),
                      messages,  addOne, 2, 3, 4, "");


    // common issues:
    // when you're writing a testRange, make sure that if you want a blank message,
    // put in a blank std::string() as the compiler may think that it is a const char[]
    // like this tester.testRange(std::source_location::current(), 1, 2, expected, "", messages, addOne, ...); // wrong
    //           tester.testRange(std::source_location::current(), 1, 2, expected, std::string(), messages, addOne, ...); // right

    tester.printResults();
}