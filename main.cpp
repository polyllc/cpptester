#include "tester.h"

using namespace TesterLib;

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
    std::cout << value.Run();
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
       std::cout << result << std::endl;
   }




    return 0;
}
