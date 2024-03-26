#include <vector>
#include <iostream>
#include <any>
#include <functional>

namespace TesterLib {

    // filters out any element in the vector that does not pass the lambda
    template<class T, typename Lambda>
    std::vector<T> filter(std::vector<T> &vector, Lambda func) {
        std::vector<T> copier;
        std::copy_if(vector.begin(), vector.end(), std::back_inserter(copier), func);
        return copier;
    }

    std::string AnyPrint(const std::any &value)
    {
        std::cout << size_t(&value) << ", " << value.type().name() << " ";
        if (auto x = std::any_cast<int>(&value)) {
            return "int(" + std::to_string(*x) + ")";
        }
        if (auto x = std::any_cast<float>(&value)) {
            return "float(" + std::to_string(*x) + ")";
        }
        if (auto x = std::any_cast<double>(&value)) {
            return "double(" + std::to_string(*x) + ")";
        }
        if (auto x = std::any_cast<std::string>(&value)) {
            return "string(\"" + (*x) + "\")";
        }
        if (auto x = std::any_cast<const char*>(&value)) {
            return *x;
        }
        return "other";
    }

    // a class that holds the result of all tests
    // all fields are public in order for easy debugging
    class Result {
    public:
        std::string message;
        bool state;
        Result(std::string m, bool s) {
            message = m;
            state = s;
        }

        void print() {
            std::cout << "Result: " << state << " Reason: " << message << std::endl;
        }

        friend std::ostream& operator << (std::ostream& os, Result result){
            os << std::string("Result: ") + (result.state ? "true" : "false") + std::string(" Reason: ") + result.message;
            return os;
        }
    };

    // test class that is the parent for all testers
    template<class T, class U>
    class Test {
    protected:
        T data;
        U expected;
    public:
        Test(T Data, T Expected) {
            data = Data;
            expected = Expected;
        }
        ~Test() {}

        virtual bool Run();
        //friend TestDouble;
    };

    template<class T, class U>
    // run the test, a failsafe virtual definition for the Test parent class
    bool Test<T, U>::Run() {
        return false;
    }

    // Test -> TestDouble, is a test that is made for floating point imprecision
    template<class T, class U>
    class TestFloat : public Test<T, U> {
    private:
        double upperLimit = 0;
        double lowerLimit = 0;
    public:

        TestFloat(T data, T expected, double range) : Test<T, U>(data, expected) {
            upperLimit = range;
            lowerLimit = -range;
        }

        TestFloat(T data, T expected, double lLimit, double uLimit) : Test<T, U>(data, expected) {
            lowerLimit = lLimit;
            upperLimit = uLimit;
        }

        // runs the test, checking if data is +- upper and lower limit of expected
        bool Run() {
            return Test<T, U>::data - lowerLimit >= Test<T, U>::expected || Test<T, U>::data + upperLimit <= Test<T, U>::expected;
            //i'm not sure why this inherited class doesn't want to take the fields of the protected variables that it has access to... i feel like it's due to template issues
        }
    };

    // Test -> TestRange, is a test that tests a range of values on a method
    // the range is inclusive
    template<class T, class U>
    class TestRange { // although it doesn't really matter what types they are
    private:
        int from;
        int to;
        //Callable& methodToInvoke;
        std::vector<U> expected;
        std::string message;
    public:
        TestRange(int From, int To, std::string Message = "") {
            from = From;
            to = To;
            message = Message;
        }
        TestRange(int From, int To, std::vector<U> Expected, std::string Message = "") {
            from = From;
            to = To;
            message = Message;
            expected = Expected;
        }
        void UpdateTest(int From, int To, std::string Message = "") {
            from = From;
            to = To;
            message = Message;
        }

        void UpdateTest(int From, int To, std::vector<U> Expected, std::string Message = "") {
            from = From;
            to = To;
            message = Message;
            expected = Expected;
        }

        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable& method, Args... args) {
            std::vector<Result> results;
            int index = 0;
            for(int i = from; i <= to; i++) {
                if(expected.empty()) {
                    bool state = false;
                    std::string result;
                    try {
                        std::invoke(method , i, args...);
                        result = std::string("Passed: ") + std::to_string(i);
                    }
                    catch(std::exception &e) {
                        result = "Exception Thrown" + std::string(e.what()) + " on " + std::to_string(i);
                    }
                    results.emplace_back(message + " " + result, state);
                }
                else {
                    bool state = std::invoke(method, i, args...) == expected[index];
                    results.emplace_back(std::string(state ? "Success" : "Failure") + ", " + message, state);
                }
                index++;
            }
            return results;
        }


    };

    // a container class that holds all tests of one type (the first type that is)
    template<class T, class U>
    class TestType {
    private:
        std::vector<T> actualData;
        std::vector<U> expectedData; //std:any to hold any type
    public:
        TestType() {

        }
        TestType(std::vector<T> aData, std::vector<U> eData) {
            // we then can use a template here to make U of any type, and we don't have to declare
            actualData = aData;
            expectedData = eData;
        }

        ~TestType() {}

        void Add(T data, U expected);
        void Remove();
        bool RunFirst();
        bool RunAt(int i);
        std::vector<Result> RunAll();
        TestType(TestType<T, U> const &type) {
            expectedData = type.expectedData;
            actualData = type.actualData;
        }
    };

    // the actual tester container, this will contain all tests of all types
    class Tester {
    private:
        std::vector<Result> results;

    public:
        Tester() {

        }
        ~Tester() {
        }

        template<typename T1, typename U2>
        Result testOne(T1 data, U2 actual) {
            try {
                bool state = data == actual;
                std::string args = "Test #" + std::to_string(results.size() + 1) + (state ? "Success" : "Failure");
                results.emplace_back(args, state);
                return {args, state};
            }
            catch (std::exception &exception) {
                std::string args = "Test #" + std::to_string(results.size() + 1) + std::string("Exception thrown: ") + exception.what();
                results.emplace_back(args, false);
                return {args, false };
            }
        }

        template<typename T1, typename U2, typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::string message, Callable &method, Args... args) {
            std::vector<Result> testResults = TestRange<T1, U2>(from, to, message).RunAll(method, args...);
            std::copy(testResults.begin(), testResults.end(), std::back_inserter(results));
            return testResults;
        }

        void printResults() {
            unsigned long long success = filter(results, [](const Result& res) { return res.state; }).size();
            std::cout << std::endl << "Test Results: (" << success << "/" << results.size() << ") passed." << std::endl;
            for(Result result : results) {
                std::cout << '\t' << result << std::endl;
            }
        }
    };

    template <class T, class U>
    void TestType<T, U>::Add(T data, U expected) {
        if(typeid(data) != typeid(T) || typeid(expected) != typeid(U)) {
            return;
        }
        actualData.push_back(data);
        expectedData.push_back(expected);
    }

    template <class T, class U>
    void TestType<T, U>::Remove() {
        if (!actualData.empty() && !expectedData.empty()) {
            actualData.pop_back();
            expectedData.pop_back();
        }
    }

    template <class T, class U>
    bool TestType<T, U>::RunFirst() {
        return actualData.at(actualData.size() - 1) == expectedData.at(expectedData.size() - 1);
    }

    template <class T, class U>
    bool TestType<T, U>::RunAt(int i) {
        if (i >= actualData.size()) {
            return false;
        }
        std::any actual = actualData.at(i);
        return actualData.at(i) == expectedData.at(i);
       // return false;
       // return std::any_cast<actual.type()>(actualData.at(i)) == std::any_cast<U>(expectedData.at(i));
        // the entire point of this any_cast is to make sure that actual and expected are the same type
        // and if they aren't that the class that it is can input a different type
    }

    template <class T, class U>
    std::vector<Result> TestType<T, U>::RunAll() {
        std::vector<Result> results;
        for (int i = 0; i < actualData.size(); i++) {
            try {
                bool state = RunAt(i);
                results.emplace_back(state ? "Success" : "Failure", state);
            }
            catch (std::exception &exception) {
                results.emplace_back(std::string("Exception thrown: ") + exception.what(), false );
            }
        }
        return results;
    }
}


