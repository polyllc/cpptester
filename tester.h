#include <utility>
#include <vector>
#include <iostream>
#include <any>
#include <functional>

/* Simple C++ Tester Library
 * This code is available for use according the GPLv3 license.
 * Original code by @polyllc on GitHub
 *
 * Look around the library, most, if not all, functions and classes have a description saying what they do, as well
 * as arguments and return type.
 *
 * If you would like to contribute, great! Make sure all of your code aligns to:
 *  - proper indentation (4 spaces)
 *  - proper naming, as in relatively intuitive naming
 *  - proper template etiquette, make sure that the template works for all cases! If not, make sure
 *    to have the template be somehow called only upon types that can be accessed by one function
 *    and then have multiple overrides of the function pointing to the correct template implementation.
 *    (see TestRange's RunAll)
 *  - proper comments! Make sure all functions and classes have the block comment as seen below
 */

namespace TesterLib {

    /**
     * @brief Filter out elements that return false for predicate
     * @tparam T Type of data for the vector
     * @tparam Lambda A Callable function, lambda or method
     * @param vector The vector to perform the filtering on
     * @param func The Lambda to be the predicate for filtering
     * @return The filtered vector
     */
    template<typename T, typename Lambda>
    std::vector<T> filter(std::vector<T> &vector, Lambda func) {
        std::vector<T> copier;
        std::copy_if(vector.begin(), vector.end(), std::back_inserter(copier), func);
        return copier;
    }

    /**
     * @brief Appends a list of vectors into one vector
     * @tparam T type of the vector
     * @param vec The vector of vectors
     * @return A vector of type T
     */
    template<typename T>
    std::vector<T> appendAllVectors(std::vector<std::vector<T>> vec) {
        std::vector<T> newVec;
        for(std::vector<T> v : vec) {
            newVec.reserve(v.size());
            newVec.insert(newVec.end(), v.begin(), v.end());
        }
        return newVec;
    }


    /**
     *  @brief A class that holds the result of all tests.
     *  All fields are public for easy debugging
     *
     *  */
    class Result {
    public:
        std::string message;
        bool state;
        Result(std::string m, bool s) {
            message = std::move(m);
            state = s;
        }

        void print() const {
            std::cout << "Result: " << state << " | Reason: " << message << std::endl;
        }

        friend std::ostream& operator << (std::ostream& os, const Result& result){
            os << std::string("Result: ") + (result.state ? "\x1b[42m true \x1b[0m" : "\x1b[41m false \x1b[0m") + std::string(" | Reason: ") + result.message;
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
        std::string message;
    public:
        Test(T Data, T Expected, std::string Message) {
            data = Data;
            expected = Expected;
            message = Message;
        }
        ~Test() = default;

        virtual Result Run() {
            return {"", false};
        }
    };


    /**
     *  @brief A class that can return floating point comparison with a range for imprecision.
     *  @class Inherits from TesterLib::Test
     *
     *  @tparam T The type of the data that you are testing the value for, expected to be either **float** or **double**
     *  @tparam U the type of the data that you are comparing against for a truth value, expected to be either **float** or **double**
     *  */
    template<class T, class U> // todo, update for result object
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
        TestFloat(T data, T expected, double range, std::string message = "") : Test<T, U>(data, expected, message) {
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
        TestFloat(T data, T expected, double lLimit, double uLimit, std::string message = "") : Test<T, U>(data, expected, message) {
            lowerLimit = lLimit;
            upperLimit = uLimit;
        }

        /**
         * @brief Runs the test with lower and upper limit of leniency for floating point imprecision
         * @return The Result of the test
         */
        Result Run() {
            bool state = false;
            std::string result;
            try {
                    state = this->data - lowerLimit >= this->expected || this->data + upperLimit <= this->expected;
                    result = std::string(state ? "Passed" : "Failed");
            }
            catch(std::exception &e) {
                result = "Exception Thrown: " + std::string(e.what());
            }
            return {this->message + " " + result, state};
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

        explicit VectorTest(std::vector<T> Expected, std::string Message = "", std::vector<std::string> Messages = {}) {
            expected = Expected;
            messages = std::move(Messages);
            message = std::move(Message);
        }

        explicit VectorTest(std::string Message = "", std::vector<std::string> Messages = {}) {
            message = std::move(Message);
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
         * @param Messages optional message that will print for nth result
         */
        TestRange(int From, int To, std::string Message = "", std::vector<std::string> Messages = {}) : from(From), to(To), VectorTest<U>(Message, Messages) {}

        /**
         * @brief Expected constructor
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Expected an std::vector<U> that has the list of expected values for each test, in order
         * @param Message optional message that will print for every result
         * @param Messages optional message that will print for nth result
         */
        TestRange(int From, int To, std::vector<U> Expected, std::string Message = "", std::vector<std::string> Messages = {}) : from(From), to(To), VectorTest<U>(Expected, Message, Messages) {}

        ~TestRange() = default;

        /**
         * @brief Update the test
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Message optional message that will print for every result
         * @param Messages optional messages that will print for nth result
         */
        void UpdateTest(int From, int To, std::string Message = "", std::vector<std::string> Messages = {}) {
            from = From;
            to = To;
            this->message = std::move(Message);
            this->messages = std::move(Messages);
        }

        /**
         * @brief Update the test
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Expected an std::vector<U> that has the list of expected values for each test, in order
         * @param Message optional message that will print for every result
         * @param Messages optional messages that will print for nth result
         */
        void UpdateTest(int From, int To, std::vector<U> Expected, std::string Message = "", std::vector<std::string> Messages = {}) {
            from = From;
            to = To;
            this->message = std::move(Message);
            this->expected = Expected;
            this->messages = std::move(Messages);
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
                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back(this->message + " " + result + (index < this->messages.size() ? ", " + this->messages.at(index) : ""), state);
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
                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back(this->message + " " + result + (index < this->messages.size() ? ", " + this->messages.at(index) : ""), state);
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
    class TestType : public VectorTest<U> {
    private:
        std::vector<T> actualData;
    public:

        /**
         *
         * @param aData Actual Data
         * @param eData Expected Data
         */
        explicit TestType(std::vector<T> aData = {}, std::vector<U> eData = {}, std::string Message = "", std::vector<std::string> Messages = {}) : actualData(aData), VectorTest<U>(eData, Message, Messages) {}

        ~TestType() = default;

        /**
         * @brief Adds a new test to the end of the vector
         * @param data Actual data
         * @param expected Expected data
         */
        void Add(T data, U expected, std::string Message = "") {
            actualData.push_back(data);
            this->expected.push_back(expected);
            this->messages.push_back(Message);
        }

        /**
         * @brief Removes the last most test
         */
        void Remove() {
            if (!actualData.empty() && !this->expected.empty()) {
                actualData.pop_back();
                this->expected.pop_back();
                if(!this->messages.empty()) {
                    this->messages.pop_back();
                }
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
            T actual = actualData.at(i);
            return actualData.at(i) == this->expected.at(i);
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
                    results.emplace_back(this->message + " " + std::string(state ? "Success" : "Failure") + (i < this->messages.size() ? ", " + this->messages.at(i) : ""), state);
                }
                catch (std::exception &exception) {
                    results.emplace_back(message + " " + std::string("Exception thrown: ") + exception.what() + ", " + (i < this->messages.size() ? ", " + this->messages.at(i) : ""), false);
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
        TestTwoVector(std::vector<T> Actual, std::vector<U> Expected, std::string Message = "", std::vector<std::string> Messages = {}) : actual(Actual), VectorTest<U>(Expected, Message, Messages) {}

        explicit TestTwoVector(std::vector<T> Actual, std::string Message = "", std::vector<std::string> Messages = {}) : actual(Actual), VectorTest<U>(Message, Messages) {}

        void UpdateTest(std::vector<T> Actual, std::vector<U> Expected, std::string Message = "") {
            actual = Actual;
            this->expected = Expected;
            this->message = Message;
        }

        /**
          * @brief Run all of the tests
          * @param method A callable function, lambda or method
          * @param args The list of extra arguments to be passed onto the Callable
          * @return A vector of Result with the results
          *
          * Tests a function where the nth element in the actual vector is used as the 1st argument for the
          * method that is passed through on the RunAll function call. Additional arguments may be passed in as well.
          * All results will then be checked with the nth element in the result vector, if provided. Otherwise,
          * check only for exceptions.
          *
          */
        template<typename Callable, typename... Args>
        std::vector<Result> RunAllArgs(Callable& method, Args... args) {
            for(int i = 0; i < actual.size(); i++) {
                bool state = false;
                std::string result;
                try {
                    if(this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        std::invoke(method, actual.at(i), args...);
                        result = std::string("Passed: ") + std::to_string(i);
                    }
                    else {
                        state = std::invoke(method, actual.at(i), args...) == this->expected.at(std::min<unsigned long long int>(this->expected.size() - 1, i));
                        result = std::string(state ? "Passed: " : "Failed: ") + std::to_string(i);
                    }
                }
                catch(std::exception &e) {
                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back(this->message + " " + result + (i < this->messages.size() ? ", " + this->messages.at(i) : ""), state);
            }

            return this->results;
        }

        /**
          * @brief Run all of the tests
          * @param method A callable function, lambda or method
          * @return A vector of Result with the results
          *
          * Tests a function where the nth element in the actual vector is used as the 1st argument for the
          * method that is passed through on the RunAll function call.
          * All results will then be checked with the nth element in the result vector, if provided. Otherwise,
          * check only for exceptions.
          *
          */
        template<typename Callable>
        std::vector<Result> RunAllNoArgs(Callable& method) {
            for(int i = 0; i < actual.size(); i++) {
                bool state = false;
                std::string result;
                try {
                    if(this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        std::invoke(method, actual[i]);
                        result = std::string("Passed: ") + std::to_string(i);
                    }
                    else {
                        state = std::invoke(method, actual[i]) == this->expected.at(std::min<unsigned long long int>(this->expected.size() - 1, i));
                        result = std::string(state ? "Passed: " : "Failed: ") + std::to_string(i);
                    }
                }
                catch(std::exception &e) {
                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back(this->message + " " + result + (i < this->messages.size() ? ", " + this->messages.at(i) : ""), state);
            }

            return this->results;
        }

        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable& method, Args... args) {
            return RunAllArgs(method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable& method) {
            return RunAllNoArgs(method);
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
        std::vector<std::vector<Result>> results;

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
                std::string args = "Test #" + std::to_string(results.size() + 1) + (state ? " Success" : " Failure");
                results.emplace_back(std::vector<Result>{Result{args, state}});
                return {args, state};
            }
            catch (std::exception &exception) {
                std::string args = "Test #" + std::to_string(results.size() + 1) + std::string("Exception thrown: ") + exception.what();
                results.emplace_back(std::vector<Result>{Result{args, false}});
                return {args, false };
            }
        }

        /**
         * @brief Test floating point number with imprecision leniency
         * @tparam T1 A floating point number
         * @tparam U2 A floating point number
         * @param data A floating point number that is the actual result
         * @param actual A floating point number to compare against the actual
         * @param range Range of the limit from 0, + or -
         * @param message A message appended to the result
         * @return A Result
         */
        template<typename T1, typename U2>
        Result testFloat(T1 data, U2 actual, double range, std::string message = "") {
            return testFloat(data, actual, range, message).Run();
        }

        /**
         * @brief Test floating point number with imprecision leniency
         * @tparam T1 A floating point number
         * @tparam U2 A floating point number
         * @param data A floating point number that is the actual result
         * @param actual A floating point number to compare against the actual
         * @param lowerBound The lower bound of the imprecision
         * @param upperBound The upper bound of the imprecision
         * @param message A message appended to the result
         * @return A Result
         */
        template<typename T1, typename U2>
        Result testFloat(T1 data, U2 actual, double lowerBound, double upperBound, std::string message = "") {
            return testFloat(data, actual, lowerBound, upperBound, message).Run();
        }


        /**
         * @brief Test multiple tests of one pair of types
         * @tparam T1 Type of actual
         * @tparam U2 Type of expected
         * @param actual Actual data that you are testing against
         * @param expected Expected data that you are comparing with
         * @param message A message to append to all results
         * @param messages A message to append to nth result
         * @return
         */
        template<typename T1, typename U2>
        std::vector<Result> testType(std::vector<T1> actual, std::vector<U2> expected, std::string message = "", std::vector<std::string> messages = {}) {
            std::vector<Result> testResults = TestType(actual, expected, message, messages).RunAll();
            results.emplace_back(testResults);
            return testResults;
        }


        /**
         * @brief Function version of the class TestRange
         * @tparam T1 The return type of the Callable
         * @tparam Callable Any function, method or lambda that can be called upon
         * @tparam Args The arguments for Callable
         * @param from Starting range (inclusive)
         * @param to Ending range (inclusive)
         * @param message A message to append to all results
         * @param messages A message to append to nth result
         * @param method A Callable
         * @param args An Args for method's arguments
         * @return A vector of Results
         */
        template<typename T1, typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::vector<T1> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            std::vector<Result> testResults = TestRange<T1>(from, to, expected, message, messages).RunAll(method, args...);
            results.emplace_back(testResults);
            return testResults;
        }
        template<typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, Callable &method, Args... args) {
            return testRange(from, to, std::vector<int>{}, "", {}, method, args...);
        }
        template<typename T1, typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::vector<T1> expected, Callable &method, Args... args) {
            return testRange(from, to, expected, "", {}, method, args...);
        }
        template<typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::string message, std::vector<std::string> messages,  Callable &method, Args... args) {
            return testRange(from, to, std::vector<int>{}, message, messages, method, args...);
        }

        /**
         * @brief Function version of the class TestRange
         * @tparam T1 The return type of the Callable
         * @tparam Callable Any function, method or lambda that can be called upon
         * @tparam Args The arguments for Callable
         * @param from Starting range (inclusive)
         * @param to Ending range (inclusive)
         * @param method A Callable
         * @param message A message to append to all results
         * @param messages A message to append to nth result
         * @return A vector of Results
         */
        template<typename T1, typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::vector<T1> expected, Callable &method, std::string message = "", std::vector<std::string> messages = {}) {
            std::vector<Result> testResults = TestRange<T1>(from, to, expected, message, messages).RunAll(method);
            results.emplace_back(testResults);
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
        std::vector<Result> testTwoVectorMethod(std::vector<T1> inputs, std::vector<U2> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            std::vector<Result> testResults = TestTwoVector<T1, U2>(inputs, expected, message, messages).RunAll(method, args...);
            results.emplace_back(testResults);
            return testResults;
        }
        // we have to do a lot of copy-pasting due to the fact that we can't simply have default parameters here
        // also we cannot use just messages or just message because then the compiler infers it as Callable, which we do not want
        // in the event that you just want message or messages, make message = "" or messages = {}
        template<typename T1, typename U2, typename Callable, typename... Args> // no message, no messages
        std::vector<Result> testTwoVectorMethod(std::vector<T1> inputs, std::vector<U2> expected, Callable &method, Args... args) {
            return testTwoVectorMethod(inputs, expected, "", {}, method, args...);
        }
        template<typename T1, typename Callable, typename... Args> // no message, no messages, no expected
        std::vector<Result> testTwoVectorMethod(std::vector<T1> inputs, Callable &method, Args... args) {
            return testTwoVectorMethod(inputs, std::vector<T1>{}, "", {}, method, args...);
        }

        // now we have to make the same thing but except this time for no arguments
        // but this time due to the lack of a packed argument, we can use default arguments!
        /**
         * @brief Function version of the class TestTwoVector
         * @tparam T1 Type of the inputs
         * @tparam U2 Type of the expected
         * @tparam Callable Any function, method or lambda tha can be called upon
         * @param inputs Inputs for each test
         * @param method A Callable
         * @param expected *optional* Expected output for each input test
         * @param message *optional* A message appended to all results
         * @param messages *optional* A message appended to nth result
         * @return A vector of Results
         */
        template<typename T1, typename U2, typename Callable>
        std::vector<Result> testTwoVectorMethod(std::vector<T1> inputs, Callable &method, std::vector<U2> expected = {}, std::string message = "", std::vector<std::string> messages = {}) {
            std::vector<Result> testResults = TestTwoVector<T1, U2>(inputs, expected, message, messages).RunAll(method);
            results.emplace_back(testResults);
            return testResults;
        }

        /**
         * @brief Prints the results of the vector results
         */
        void printResults() {
            std::vector<Result> appended = appendAllVectors(results);
            unsigned long long success = filter(appended, [](const Result& res) { return res.state; }).size();
            std::cout << std::endl << "Test Results: (" << success << "/" << appended.size() << ") passed." << std::endl;
            int total = 1;
            int index = 1;
            int testNum = 1;
            for(const std::vector<Result>& result : results) {
                for(const Result& r : result) {
                    std::cout << "(" << total << ") \x1b[35m Group " << testNum << "\x1b[0m,\x1b[36m Test " << index << "\x1b[0m\t" << r << std::endl;
                    total++;
                    index++;
                }
                testNum++;
                index = 1;
            }
        }
    };

}


