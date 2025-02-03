#include "tester.h"



using namespace TesterLib;

int add(int val1, int val2) {
    return val1 + val2;
}

int ad(int v) {
    throw std::out_of_range("2");
}
int adad() {
    throw std::out_of_range("2");
}

int exceptioner(int v) {
    if(v > 10) {
        throw std::out_of_range("too much!");
    }
    return v;
}


int add2(int val1) {
    return val1 + 1;
}

void testVectors(Tester &tester);
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



  //  tests.RunAll();
   // tests2.RunAll();

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

  //  std::vector<Result> results = tests2.RunAll();
  //  for (const auto &result: results) {
       //  std::cout << result << std::endl;
 //   }

    std::vector<int> expected{2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    TestRange<int> range(1, 10, expected,"thing", {"asdf", "b", "2", "a", "b", "q"});

    //results = range.RunAll(add, 1);
    //results = range.RunAll(add2);

  //  for (const auto &result: results) {
     //   std::cout << result << std::endl;
  //  }


    Tester tester;

    tester.testOne("string", "string");
    tester.testOne("string", "uh oh");
   tester.testOne(2, 2);


  //  tester.testRange(1, 20, std::vector<int>{3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22}, "hi", {}, add, 2);

//    tester.testException("too much!", "", exceptioner, 11);

    //testVectors(tester);
    tester.test("vector tests",tester, testVectors);

    //  tester.printGroup(-5);

    tester.printResults();

    TestResult result("test");
   // result.addPrintable(std::make_unique<Result>(CommonLib::getStringResultOnSuccess(1, 2, "wow", false, 1), false, 1, 1, 0, "test"));
    result.addPrintable(std::make_unique<TestMessage>("wow", "test", 1, MessageType::LOG));
    result.giveResultsState(false);
    std::cout << "\n" << result.toString() << "\n";

    std::cout << TestMessage("hi", "none", 0, LOG).getMessage() << '\n';
    std::cout << TestMessage("hi", "none", 0, WARNING).getMessage() << '\n';
    std::cout << TestMessage("hi", "none", 0, SEVERE).getMessage() << '\n';
    std::cout << TestMessage("hi", "none", 0, FAIL).getMessage() << '\n';

    return 0;
}

void testVectors(Tester &tester) {
    std::vector<int> expected2{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        std::vector<int> input{2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    tester.testType(expected2, input);
    tester.testTwoVectorMethod(std::source_location::current(),input, expected2,  add, 1);
    tester.testTwoVectorMethod(std::source_location::current(),input, add, 1);
    tester.testTwoVectorMethod(std::source_location::current(),input, add2);
    tester.testTwoVectorMethod(std::source_location::current(),input, add, 1);
     tester.testTwoVectorMethod(std::source_location::current(),input, expected2,  add, 1);
     tester.testTwoVectorMethod(std::source_location::current(),input, expected2, "", {"hi mom", "hey mom", "wow!"}, add, 1);
     tester.testTwoVectorMethod(std::source_location::current(),input, expected2, "asdf", {"hi mom, ", "wow!"}, add, 1);
     tester.addMessage("wow!!!!\n many lines!!", MessageType::LOG);

   tester.testRange(1, 3, std::vector<int>{2,4,4},  add, 2);
 tester.testType(std::vector<int>{1, 2, 3}, std::vector<int>{1, 3, 3});
    tester.testTwoVectorMethod(std::source_location::current(),std::vector<size_t>{2, 7, 100},
                               std::vector<int>{3, 4, 101}, "", {"hi mom", "hey mom", "wow!"}, add, 1);
    tester.testOne("hi mom", std::string("hi mom"));
    tester.addMessage("this one tests if the 2 new vectors are the same", MessageType::LOG);
    tester.testOne(std::vector<int>(), std::vector<int>());
    tester.testFloat(2.3, 2.5, 0.1);
//
}
