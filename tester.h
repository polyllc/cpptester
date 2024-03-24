#include <vector>
#include <iostream>
#include <any>
#include <functional>

namespace TesterLib {

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

    class Result {
    private:
        std::string message;
        bool state;
    public:
        Result(std::string m, bool s) {
            this->message = m;
            this->state = s;
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
        double upperLimit;
        double lowerLimit;
    public:

        TestFloat(T data, T expected, double range) : Test<T, U>(data, expected) {
            upperLimit = range;
            lowerLimit = -range;
        }

        TestFloat(T data, T expected, double lowerLimit, double upperLimit) : Test<T, U>(data, expected) {
            this->lowerLimit = lowerLimit;
            this->upperLimit = upperLimit;
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
    public:
        TestRange(int from, int to) {
            this->from = from;
            this->to = to;
        }
        TestRange(int from, int to, std::vector<U> expected) {
            this->from = from;
            this->to = to;
            this->expected = expected;
        }
        void UpdateTest(int from, int to) {
            this->from = from;
            this->to = to;
        }

        void UpdateTest(int from, int to, std::vector<U> expected) {
            this->from = from;
            this->to = to;
            this->expected = expected;
        }

        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable& method, Args... args) {
            std::vector<Result> results;
            int index = 0;
            for(int i = from; i <= to; i++) {
                if(expected.size() <= 0) {
                    bool state = false;
                    std::string result;
                    try {
                        result = std::invoke(method , i, args...); // for the time being, make it so that the developer has to make a toString wrapper class -> method in order to get output
                    }
                    catch(std::exception &e) {
                        result = "No toString Method defined...";
                    }
                    results.emplace_back(result, state);
                }
                else {
                    bool state = std::invoke(method, i, args...) == expected[index];
                    results.emplace_back(state ? "Success" : "Failure", state);
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
        std::vector<Result> RunAll(); //todo, return not bool, but a class with more description
        TestType(TestType<T, U> const &type) {
            expectedData = type.expectedData;
            actualData = type.actualData;
        }
    };

    // the actual tester container, this will contain all tests of all types
    template<class T, class U>
    class Tester {
    private:
        std::vector<TestType<T, U>> tests;

    public:
        Tester() {

        }
        ~Tester() {
        }

        void Add(T data, U expected) {
            // TestType.T == T && TestType.U == U
            for(int i = 0; i < tests.size(); i++) {
                if(typeid(tests.at(i)) == typeid(TestType<T, U>)){
                    tests.at(i).Add(data, expected);
                    return;
                }
            }
            // did not find one
            TestType<T, U> newTests;
            newTests.Add(data, expected);
            tests.push_back(static_cast<TestType<T, U>>(newTests));
        }

        void RunAll() {
            for(int i = 0; i < tests.size(); i++) {
                tests.at(i).RunAll();
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


    template<typename T, typename U> Result testOne(T data, U actual) {
        try {
            bool state = data == actual;
            return {state ? "Success" : "Failure", state};
        }
        catch (std::exception &exception) {
            return {std::string("Exception thrown: ") + exception.what(), false };
        }
    }

}


