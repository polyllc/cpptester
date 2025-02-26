#include "../tester.h"
#include <fstream>
using namespace TesterLib;



int main() {
    Tester t;

   // t.updateSetting(PRINT_SYNC, true);

    Error e1("error", 1, 1, "part");
    Error e2("", 100, 1, "part");

    Result r1("result", false, 1, 2, {}, "part");
    Result r2("result2", true, 12, 22, {e1, e2}, "part");


    //std::cout << t.testOne(e1.getJSON(), "{\"type\": \"error\", \"errorCode\": 1, \"message\": \"\x1b[31m(Error code 1) error\x1b[0m\n\", \"groupNum\": 1, \"partOf\": \"part\"}").getJSON();
    t.testOne(e2.getJSON(), R"({"type": "error", "errorCode": 100, "message": "(Error code 100) ", "groupNum": 1, "partOf": "part"})");
    t.testOne(r1.getJSON(), R"("type": "result", "testNum": 2, "errors": {}, "message": "Group 1 | Test 2 | Result: false in 0.000000sec  | result", "state": 0, "timeTaken": 0.000000", "groupNum": 1, "partOf": "part"})");
    t.testOne(r2.getJSON(), "");

    std::cout << t.getJSON();

    std::fstream s{"file.txt", std::fstream::binary | std::fstream::trunc | std::fstream::in | std::fstream::out};
    s << t.getJSON();


    t.printResults();
    s.close();




    return 0;
}