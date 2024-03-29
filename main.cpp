#include "tester.h"

using namespace TesterLib;

int add(int val1, int val2) {
    return val1 + val2;
}

int ad(int v) {
    throw std::out_of_range("2");
}

int add2(int val1) {
    return val1 + 1;
}

int main() {

    std::vector<int> list1;

    TestType<int, int> tests(list1, list1);

    tests.Add(1, 2);
    tests.Add(2, 2);
    tests.Add(3, 2);
    tests.Add(2, 2);

    TestType<std::string, std::string> tests2{};

   tests2.Add("wow", "wow");
    tests2.Add("wow", "wow");
    tests2.Add("wow2", "wow");



    tests.RunAll();
    tests2.RunAll();

    std::vector<Test<double, double>> list2;
    TestFloat<double, double> value(1.0, 1.0, 0.1);
    //std::cout << value.Run();
    list2.push_back(value);

    std::vector<std::any> list3;
  //  list3.emplace_back(tests);
  //  list3.emplace_back(tests2);
    // allTests.Add(1, 2);
    //TestType<int, int> allTestType;
   // allTestType.Add(1, 2);


   // allTestType.RunAll();

    std::vector<Result> results = tests2.RunAll();
    for (const auto &result: results) {
         std::cout << result << std::endl;
    }

    std::vector<int> expected{2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    TestRange<int> range(1, 10, expected,"thing", {"asdf", "b", "2", "a", "b", "q"});

    //results = range.RunAll(add, 1);
    //results = range.RunAll(add2);

    for (const auto &result: results) {
     //   std::cout << result << std::endl;
    }


    Tester tester;

    //tester.testOne("string", "string");
    //tester.testOne("string", "uh oh");
    //tester.testOne(1, 2);

    //tester.testRange(1, 2, "hi", {}, add, 2);

    tester.testException("2", "", ad, 122);

    std::vector<int> expected2{3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    std::vector<int> input{2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    tester.testType(expected2, input);
    tester.testTwoVectorMethod(input, expected2,  add, 1);
    tester.testTwoVectorMethod(input, add, 1);
    tester.testTwoVectorMethod(input, add2);
    tester.testTwoVectorMethod(input, add, 1);
    tester.testTwoVectorMethod(input, expected2,  add, 1);
    tester.testTwoVectorMethod(input, expected2, "", {"hi mom", "hey mom", "wow!"}, add, 1);
    tester.testTwoVectorMethod(input, expected2, "asdf", {"hi mom, ", "wow!"}, add, 1);


    tester.printResults();

    tester.printResults(true);
    tester.printResults(false);

    tester.printTest(1);

    tester.printGroup(-5);


    return 0;
}
