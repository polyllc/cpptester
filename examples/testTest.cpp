#include "../tester.h"

using namespace TesterLib;

void testNumbers(Tester &tester);
void testTruthy(Tester &tester);
void testStringsAreEqual(Tester &tester, std::string someExtraArgs);

int main() {
    Tester tester;

    tester.test("test numbers...", tester, testNumbers);
    tester.test("test truthy, or whatever javascript calls this", tester, testTruthy);
    tester.test("test string is equal to passed in argument", tester, testStringsAreEqual, "an extra arg...");

    tester.printResults();
}

void testNumbers(Tester &tester) {
    tester.testOne(2, 3);
    tester.testFloat(2.4, 2.5, 0.2);
}

void testTruthy(Tester &tester) {
    tester.testTrue(2 > 3);
    tester.testFalse(2 > 3);
    tester.setStatus(TestResultStatus::FAILURE); // 2 > 3???!?!
}

void testStringsAreEqual(Tester &tester, std::string someExtraArgs) {
    tester.testOne("not the same", someExtraArgs);
    tester.testOne("an extra arg...", someExtraArgs);
}

