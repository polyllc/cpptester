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
            return this->data - lowerLimit >= this->expected || this->data + upperLimit <= this->expected;
        }
    };


    /**
     * @brief A class that is the parent of all Tests that use a vector for results
     * @tparam T The type of the expected vector
     */
    template<class T>
    class VectorTest {
    protected:
        std::vector<Result> results;
        std::string message; // something appended to all tests
        std::vector<std::string> messages; // something appended to nth test
        std::vector<T> expected;
    public:
        VectorTest(std::vector<T> Expected, std::string Message) {
            expected = Expected;
            message = std::move(Message);
        }

        VectorTest(std::vector<T> Expected, std::vector<std::string> Messages) {
            expected = Expected;
            messages = std::move(Messages);
        }

        VectorTest(std::vector<T> Expected, std::string Message, std::vector<std::string> Messages) {
            expected = Expected;
            message = std::move(Message);
            messages = std::move(Messages);
        }

        VectorTest(std::string Message, std::vector<std::string> Messages) {
            message = std::move(Message);
            messages = std::move(Messages);
        }

        explicit VectorTest(std::string Message) {
            message = std::move(Message);
        }

        explicit VectorTest(std::vector<std::string> Messages) {
            messages = std::move(Messages);
        }

        ~VectorTest() = default;
    };

    /**
      *  @brief A class that can return a list of Results that input a range of integers into a function as a first parameter
      *  Range is inclusive
      *
      *  @tparam U The return type that the function/lambda will return
      *  */
    template<class U>
    class TestRange : public VectorTest<U> {
    private:
        int from;
        int to;
    public:
        /**
         * @brief No expected constructor
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Message optional message that will print for every result
         */
        TestRange(int From, int To, std::string Message = "", std::vector<std::string> Messages = {}) : from(From), to(To), VectorTest<U>(Message, Messages) {}

        /**
         * @brief Expected constructor
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Expected an std::vector<U> that has the list of expected values for each test, in order
         * @param Message optional message that will print for every result
         */
        TestRange(int From, int To, std::vector<U> Expected, std::string Message = "", std::vector<std::string> Messages = {}) : from(From), to(To), VectorTest<U>(Expected, Message, Messages) {}

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
            this->message = std::move(Message);
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
            this->message = std::move(Message);
            this->expected = Expected;
        }

        /**
         * @brief Run all of the tests
         * @param method A callable function, lambda or method
         * @param args The list of arguments to be passed onto the Callable
         * @return A vector of Result with the results
         *
         * This will run through all of the tests from `from` and to `to`, inclusive.
         * It will input the current value of the range as the first parameter into the function.
         * This MUST be the first parameter of the function or else it WILL throw a template error
         * All other arguments will then be passed through.
         * If supplied, it will check against the expected vector.
         *
         */
        template<typename Callable, typename... Args>
        std::vector<Result> RunAllArgs(Callable& method, Args... args) {
            int index = 0;
            for(int i = from; i <= to; i++) {
                bool state = false;
                std::string result;
                try {
                    if(this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        std::invoke(method, i, args...);
                        result = std::string("Passed: ") + std::to_string(i);
                    }
                    else {
                        // if expected is smaller than range, we just use the last value as expected
                        state = std::invoke(method, i, args...) == this->expected.at(std::min<unsigned long long>(this->expected.size() - 1, index));
                        result = std::string(state ? "Passed: " : "Failed: ") + std::to_string(i);
                    }
                }
                catch(std::exception &e) {
                    result = "Exception Thrown" + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back(this->message + " " + result + (index < this->messages.size() ? ", " + this->messages.get(index) : ""), state);
                index++;
            }
            return this->results;
        }

        // we have the exact same function here except due to template issues, this one does not have args.
        // this **will** throw a compile time error if it is used with a function
        /**
         * @brief Run all of the tests
         * @param method A callable function, lambda or method
         * @return A vector of Result with the results
         *
         * This will run through all of the tests from `from` and to `to`, inclusive.
         * It will input the current value of the range as the first parameter into the function.
         * This is designed for functions that specifically take in *1* parameter of type int
         * If supplied, it will check against the expected vector.
         *
         */
        template<typename Callable>
        std::vector<Result> RunAllNoArgs(Callable& method) {
            int index = 0;
            for(int i = from; i <= to; i++) {
                bool state = false;
                std::string result;
                try {
                    if(this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        std::invoke(method, i);
                        result = std::string("Passed: ") + std::to_string(i);
                    }
                    else {
                        // if expected is smaller than range, we just use the last value as expected
                         state = std::invoke(method, i) == this->expected.at(std::min<unsigned long long>(this->expected.size() - 1, index));
                        result = std::string(state ? "Passed: " : "Failed: ") + std::to_string(i);
                    }
                }
                catch(std::exception &e) {
                    result = "Exception Thrown" + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back(this->message + " " + result + (index < this->messages.size() ? ", " + this->messages.get(index) : ""), state);
                index++;
            }
            return this->results;
        }

        /**
         * @brief Run all of the tests
         * @param method A callable function, lambda or method
         * @param args The list of arguments to be passed onto the Callable
         * @return A vector of Result with the results
         *
         * This will run through all of the tests from `from` and to `to`, inclusive.
         * It will input the current value of the range as the first parameter into the function.
         * This MUST be the first parameter of the function or else it WILL throw a template error
         * All other arguments will then be passed through.
         * If supplied, it will check against the expected vector.
         *
         */
        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable& method, Args... args) {
            return RunAllArgs(method, args...);
        }

        /**
         * @brief Run all of the tests
         * @param method A callable function, lambda or method
         * @return A vector of Result with the results
         *
         * This will run through all of the tests from `from` and to `to`, inclusive.
         * It will input the current value of the range as the first parameter into the function.
         * This is designed for functions that specifically take in *1* parameter of type int
         * If supplied, it will check against the expected vector.
         *
         */
        template<typename Callable>
        std::vector<Result> RunAll(Callable& method) {
            return RunAllNoArgs( method); // use 0 as default argument, but won't be used
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
        std::vector<Result> RunAll(const std::string& message = "") {
            std::vector<Result> results;
            for (int i = 0; i < actualData.size(); i++) {
                try {
                    bool state = RunAt(i);
                    results.emplace_back(std::string(state ? "Success" : "Failure") + ", " + message, state);
                }
                catch (std::exception &exception) {
                    results.emplace_back(std::string("Exception thrown: ") + exception.what() + ", " + message, false);
                }
            }
            return results;
        }
    };

    /**
     * @brief Test a function where the nth element in the actual vector is used as the 1st argument
     * @tparam T Type for actual
     * @tparam U Type for expected
     *
     * Tests a function where the nth element in the actual vector is used as the 1st argument for the
     * method that is passed through on the RunAll function call. Additional arguments may be passed in as well.
     * All results will then be checked with the nth element in the result vector, if provided. Otherwise,
     * check only for exceptions.
     */
    template<class T, class U>
    class TestTwoVector : public VectorTest<U> {
    private:
        std::vector<T> actual;
    public:
        TestTwoVector(std::vector<T> Actual, std::vector<U> Expected, std::string Message = "") : actual(Actual), VectorTest<U>(Expected, Message) {}

        TestTwoVector(std::vector<T> Actual, std::string Message = "") : actual(Actual), VectorTest<U>(Message) {}

        void UpdateTest(std::vector<T> Actual, std::vector<U> Expected, std::string Message = "") {
            actual = Actual;
            this->expected = Expected;
            this->message = Message;
        }

        /**
          * @brief Run all of the tests
          * @param method A callable function, lambda or method
          * @param args The list of extra arguments to be passed onto the Callable
          * @param useArgs Use the list of args or not
          * @return A vector of Result with the results
          *
          * Tests a function where the nth element in the actual vector is used as the 1st argument for the
          * method that is passed through on the RunAll function call. Additional arguments may be passed in as well.
          * All results will then be checked with the nth element in the result vector, if provided. Otherwise,
          * check only for exceptions.
          *
          */
        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(bool useArgs, Callable& method, Args... args) {
            for(int i = 0; i <= actual.size(); i++) {
                bool state = false;
                std::string result;
                try {
                    if(this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        if(useArgs) {
                          //  std::invoke(method, actual[i], args...);
                        }
                        else {
                          //  std::invoke(method, actual[i]);
                        }
                        result = std::string("Passed: ") + std::to_string(i);
                    }
                    else {
                        // if expected is smaller than range, we just use the last value as expected
                       // state = (useArgs ? std::invoke(method, actual[i], args...)
                       //                  : std::invoke(method, actual[i])) == this->expected[std::min<unsigned long long int>(this->expected.size() - 1, i)];
                        result = std::string(state ? "Passed: " : "Failed: ") + std::to_string(i);
                    }
                }
                catch(std::exception &e) {
                    result = "Exception Thrown" + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back(this->message + " " + result, state);
            }

            return this->results;
        }

        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable& method, Args... args) {
            return RunAll(true, method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable& method) {
            return RunAll(false, method, 0); // use 0 as default argument, but won't be used
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
         * @brief Function version of the class TestTwoVector
         * @tparam T1 Type of the inputs
         * @tparam U2 Type of the expected
         * @tparam Callable Any function, method or lambda tha can be called upon
         * @tparam Args The arguments for Callable
         * @param inputs Inputs for each test
         * @param expected Expected output for each input test
         * @param message A message appended to all results
         * @param method A Callable
         * @param args An Args for method's arguments
         * @return A vector of Results
         */
        template<typename T1, typename U2, typename Callable, typename... Args>
        std::vector<Result> testTwoVectorMethod(std::vector<T1> inputs, std::vector<U2> expected, std::string message, bool useArgs, Callable &method, Args... args) {
            std::vector<Result> testResults = TestTwoVector<T1, U2>(inputs, expected, message).RunAll(method, args..., useArgs);
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


