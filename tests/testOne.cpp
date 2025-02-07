//
// Created by poly on 2025/02/06.
//
#include "../tester.h"

using namespace TesterLib;


void testNumberValue(Tester &t);
void testStringValue(Tester &t);

int main() {

    Tester tester;
    tester.test("test number values", tester, testNumberValue);
    tester.test("test string values", tester, testStringValue);
    tester.printResults(true);
    return 0;
}

void testNumberValue(Tester &t) {
    t.testOne(1, 2); // should be false
    t.testOne(1, 1); // should be true
    t.testOne(2.0, 2.0); // should be true
    t.testOne(2.1, 2.0); // should be false
}

void testStringValue(Tester &t) {
    t.testOne("string", ""); // should be false
    t.testOne(std::string("str"), "str"); // should be true
    const char array[] = "str";
    t.testOne(std::string("str"), array); // should be true
    t.testOne("str", array); // should be true
    t.testOne(std::string("str").c_str(), "str");
    t.testOne(std::string("str").c_str(), array);
    t.testOne(std::string("str").c_str(), std::string("str"));
}