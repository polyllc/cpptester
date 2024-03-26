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

    /**
     *  @brief A class that holds the result of all tests.
     *  All fields are public in order for easy debugging
     *
     *  */
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

    /**
     *  @brief A class that is the parent of all Tests except Tester
     *
     *  @tparam T The type of the data that you are testing the value for
     *  @tparam U the type of the data that you are comparing against for a truth value
     *  */
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

        virtual bool Run() {
            return false;
        }
    };


    /**
     *  @brief A class that can return floating point comparison with a range for imprecision.
     *  @class Inherits from TesterLib::Test
     *
     *  @tparam T The type of the data that you are testing the value for, expected to be either **float** or **double**
     *  @tparam U the type of the data that you are comparing against for a truth value, expected to be either **float** or **double**
     *  */
    template<class T, class U>
    class TestFloat : public Test<T, U> {
    private:
        double upperLimit = 0;
        double lowerLimit = 0;
    public:

        /**
         * @brief Range constructor where upperLimit and lowerLimit are range away from 0
         * @param data Actual data
         * @param expected Expected data
         * @param range Range of the limit from 0, + or -
         */
        TestFloat(T data, T expected, double range) : Test<T, U>(data, expected) {
            upperLimit = range;
            lowerLimit = -range;
        }

        /**
         * @brief Range constructor where upperLimit and lowerLimit are set
         * @param data Actual data
         * @param expected Expected data
         * @param lLimit Lower limit
         * @param uLimit Upper limit
         */
        TestFloat(T data, T expected, double lLimit, double uLimit) : Test<T, U>(data, expected) {
            lowerLimit = lLimit;
            upperLimit = uLimit;
        }

        /**
         * @brief Runs the test with lower and upper limit of leniency for floating point imprecision
         * @return The state of the test
         */
        bool Run() {
            return Test<T, U>::data - lowerLimit >= Test<T, U>::expected || Test<T, U>::data + upperLimit <= Test<T, U>::expected;
            //i'm not sure why this inherited class doesn't want to take the fields of the protected variables that it has access to... i feel like it's due to template issues
        }
    };

    /**
      *  @brief A class that can return a list of Results that input a range of integers into a function as a first parameter
      *  Range is inclusive
      *  @class Inherits from TesterLib::Test
      *
      *  @tparam U The return type that the function/lambda will return
      *  */
    template<class U>
    class TestRange { // although it doesn't really matter what types they are
    private:
        int from;
        int to;
        std::vector<U> expected;
        std::string message;
    public:
        /**
         * @brief No expected constructor
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Message optional message that will print for every result
         */
        TestRange(int From, int To, std::string Message = "") : from(From), to(To), message(std::move(Message)) {}

        /**
         * @brief Expected constructor
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Expected an std::vector<U> that has the list of expected values for each test, in order
         * @param Message optional message that will print for every result
         */
        TestRange(int From, int To, std::vector<U> Expected, std::string Message = "") : from(From), to(To), expected(Expected), message(std::move(Message)) {}

        ~TestRange() = default;

        /**
         * @brief Update the test
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Message optional message that will print for every result
         */
        void UpdateTest(int From, int To, std::string Message = "") {
            from = From;
            to = To;
            message = std::move(Message);
        }

        /**
         * @brief Update the test
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Expected an std::vector<U> that has the list of expected values for each test, in order
         * @param Message optional message that will print for every result
         */
        void UpdateTest(int From, int To, std::vector<U> Expected, std::string Message = "") {
            from = From;
            to = To;
            message = std::move(Message);
            expected = Expected;
        }

        /**
         * @brief Run all of the tests
         * @param method A callable function, lambda or method
         * @param args The list of argument to be passed onto the Callable
         * @return A vector of Result with the results
         *
         * This will run through all of the tests from `from` and to `to`, inclusive.
         * It will input the current value of the range as the first parameter into the function.
         * All other arguments will then be passed through.
         * If supplied, it will check against the expected vector.
         *
         */
        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable& method, Args... args) {
            std::vector<Result> results;
            int index = 0;
            for(int i = from; i <= to; i++) {
                bool state = false;
                std::string result;
                try {
                    if(expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        std::invoke(method, i, args...);
                        result = std::string("Passed: ") + std::to_string(i);
                    }
                    else {
                        state = std::invoke(method, i, args...) == expected[index];
                        result = std::string(state ? "Passed: " : "Failed: ") + std::to_string(i);
                    }
                }
                catch(std::exception &e) {
                    result = "Exception Thrown" + std::string(e.what()) + " on " + std::to_string(i);
                }
                results.emplace_back(message + " " + result, state);
                index++;
            }
            return results;
        }


    };

    /**
     *  @brief A class that can hold multiple tests of one type
     *
     *  @tparam T The type of the data that you are testing the value for
     *  @tparam U the type of the data that you are comparing against for a truth value
     *  */
    template<class T, class U>
    class TestType {
    private:
        std::vector<T> actualData;
        std::vector<U> expectedData;
    public:
        TestType() = default;

        /**
         *
         * @param aData Actual Data
         * @param eData Expected Data
         */
        TestType(std::vector<T> aData, std::vector<U> eData) : actualData(aData), expectedData(eData) {}

        ~TestType() = default;

        /**
         * @brief Adds a new test to the end of the vector
         * @param data Actual data
         * @param expected Expected data
         */
        void Add(T data, U expected) {
            actualData.push_back(data);
            expectedData.push_back(expected);
        }

        /**
         * @brief Removes the last most test
         */
        void Remove() {
            if (!actualData.empty() && !expectedData.empty()) {
                actualData.pop_back();
                expectedData.pop_back();
            }
        }

        /**
         * @brief Run test at index i
         * @param i The index to run at
         * @return The state of the test
         */
        bool RunAt(int i) {
            if (i >= actualData.size()) {
                return false;
            }
            std::any actual = actualData.at(i);
            return actualData.at(i) == expectedData.at(i);
        }

        /**
         * @brief Runs all of the tests
         * @return A vector of the results
         */
        std::vector<Result> RunAll() {
            std::vector<Result> results;
            for (int i = 0; i < actualData.size(); i++) {
                try {
                    bool state = RunAt(i);
                    results.emplace_back(state ? "Success" : "Failure", state);
                }
                catch (std::exception &exception) {
                    results.emplace_back(std::string("Exception thrown: ") + exception.what(), false);
                }
            }
            return results;
        }
    };

   /**
    * @brief A tester container that stores information about ran tests
    *
    * Allows the testing of various methods and then those results to be stored into one
    * vector of results, rather than having multiple different tests that might make testing
    * simpler objects harder.
    */
    class Tester {
    private:
        std::vector<Result> results;

    public:
        Tester() = default;
        ~Tester() = default;

        /**
         * @brief Tests one comparison using operator==. Will automatically put into results.
         * @tparam T1 The type of data that you are testing
         * @tparam U2 The type of data that you are expecting
         * @param data The actual data
         * @param actual The expected data
         * @return A Result object containing the results of the test
         */
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

        /**
         * @brief Function version of the class TestRange
         * @tparam T1 The return type of the Callable
         * @tparam Callable Any function, method or lambda that can be called upon
         * @tparam Args The arguments for Callable
         * @param from Starting range (inclusive)
         * @param to Ending range (inclusive)
         * @param message A message to append to all results
         * @param method A Callable
         * @param args An Args for method's arguments
         * @return A vector of Results
         */
        template<typename T1, typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::string message, Callable &method, Args... args) {
            std::vector<Result> testResults = TestRange<T1>(from, to, message).RunAll(method, args...);
            std::copy(testResults.begin(), testResults.end(), std::back_inserter(results));
            return testResults;
        }

        /**
         * @brief Prints the results of the vector results
         */
        void printResults() {
            unsigned long long success = filter(results, [](const Result& res) { return res.state; }).size();
            std::cout << std::endl << "Test Results: (" << success << "/" << results.size() << ") passed." << std::endl;
            for(Result result : results) {
                std::cout << '\t' << result << std::endl;
            }
        }
    };

}


