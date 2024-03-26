#include "tester.h"

using namespace TesterLib;

int add(int val1, int val2) {
    return val1 + val2;
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
    list3.emplace_back(tests);
    list3.emplace_back(tests2);
   // allTests.Add(1, 2);
    TestType<int, int> allTestType;
    allTestType.Add(1, 2);


    allTestType.RunAll();

   std::vector<Result> results = tests2.RunAll();
   for(const auto & result : results) {
      // std::cout << result << std::endl;
   }

   std::vector<int> expected{2,3,4,5,6,7,8,9,10,11};
   TestRange<int, int> range(1, 10, "thing");

   results = range.RunAll(add, 1);

   for(const auto &result : results) {
       std::cout << result << std::endl;
   }

   Tester tester;

   tester.testOne("string", "string");
   tester.testOne("string", "uh oh");
   tester.testOne(1, 2);

   tester.testRange<int, int>(1, 20, "no", add, 1);

   tester.printResults();



    return 0;
}
