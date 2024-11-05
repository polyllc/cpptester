#include <utility>
#include <vector>
#include <iostream>
#include <any>
#include <functional>
#include <memory>
#include <sstream>
#include <fstream>
#include <source_location>

/* Simple C++ Tester Library
 * This code is available for use according the MIT license.
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
     * Function prototypes for global namespace functions
     */
    namespace CommonLib {
        template<typename T, typename Lambda>
        std::vector<T> filter(std::vector<T> &vector, Lambda func);

        template<typename T>
        std::vector<T> appendAllVectors(const std::vector<std::vector<T>> &vec);

        template<typename T, typename U>
        std::string
        getStringResultOnSuccess(T actual, U expected, const std::string &message, bool state, int testNum = 1, const std::source_location loc = std::source_location::current(), std::string ogFunction = "(not specified)");

        template<typename T, typename U>
        bool isEqual(T actual, U expected);

        template<typename T>
        std::string toString(T from);

        template <class T>
        constexpr std::string_view type_name();


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
        std::vector<T> appendAllVectors(const std::vector<std::vector<T>> &vec) {
            std::vector<T> newVec;
            for (const std::vector<T> &v: vec) {
                newVec.reserve(v.size());
                newVec.insert(newVec.end(), v.begin(), v.end());
            }
            return newVec;
        }

        template<typename T, typename U>
        std::string getStringResultOnSuccess(T actual, U expected, const std::string &message, bool state, int testNum, const std::source_location loc, std::string ogFunction) {
            return "Test #" + std::to_string(testNum) + (state ? " Success" : " Failure") +
                   " \n|\t\x1b[1mexpected: \x1b[0m " +
                   toString(expected) + " \t \x1b[1m was: \x1b[0m " + toString(actual) +
                   "\n|\t\x1b[1mexpected type:\x1b[0m " + std::string(type_name<U>()).substr(22) + " \t\x1b[1m was:\033[0m " + std::string(type_name<T>()).substr(22) + "\x1b[0m\n" +
                   "|\t\x1b[1mat: " + loc.file_name() + ":" + std::to_string(loc.line()) +
                   "\x1b[0m\n|\t\x1b[1mcalled in: \x1b[0m" + loc.function_name() +
                   "\n|\t\x1b[1mas: \x1b[0m" + ogFunction +
                   (!message.empty() ? "\n\t\033[1mmessage:\033[0m " + message : "") +
                   "\n|";
        }

        /**
         * @brief Checks if actual is equal to expected (in a variety of different ways)
         * @tparam T type of actual
         * @tparam U type of expected
         * @param actual actual value
         * @param expected expected value
         * @return true if equals, false otherwise
         */
        template<typename T, typename U>
        bool isEqual(T actual, U expected) {
            if constexpr ((std::is_same<T, const char *>::value || std::is_same<T, char *>::value ||
                           std::is_same<T, std::string>::value || std::is_same<T, char[]>::value) &&
                          (std::is_same<U, const char *>::value || std::is_same<U, char *>::value ||
                           std::is_same<U, std::string>::value || std::is_same<U, char[]>::value)) {
                return std::string(expected) == std::string(actual);
            } else {
                return expected == actual;
            }
        }

        template<typename T>
        std::string toString(T from) {
            if constexpr (std::is_same<T, int>::value ||
                          std::is_same<T, long long>::value ||
                          std::is_same<T, unsigned>::value ||
                          std::is_same<T, unsigned long>::value ||
                          std::is_same<T, unsigned long long>::value ||
                          std::is_same<T, float>::value ||
                          std::is_same<T, double>::value ||
                          std::is_same<T, long double>::value ||
                          std::is_same<T, std::string>::value ||
                          std::is_same<T, std::wstring>::value ||
                          std::is_same<T, const char *>::value ||
                          std::is_same<T, char *>::value ||
                          std::is_same<T, long double>::value) {
                std::ostringstream stream;
                stream << from;
                return stream.str();
            } else {
                std::ostringstream stream;
                stream << "*" << std::addressof(from);
                return stream.str();
            }
        }


        template <typename T>
        constexpr std::string_view type_name() { // @howard-hinnant on stackoverflow :)
        #ifdef __clang__
            std::string_view p = __PRETTY_FUNCTION__;
            return {p.data() + 34, p.size() - 34 - 1};
        #elif defined(__GNUC__)
            std::string_view p = __PRETTY_FUNCTION__;
        #  if __cplusplus < 201402
            return {p.data() + 36, p.size() - 36 - 1};
        #  else
            return {p.data() + 49, p.find(';', 49) - 49};
        #  endif
        #elif defined(_MSC_VER)
            std::string_view p = __FUNCSIG__;
            return {p.data() + 84, p.size() - 84 - 7};
        #endif
        }
    }

    enum MessageType {
        LOG,
        WARNING,
        SEVERE,
        FAIL
    };

    enum TestResultStatus {
        SUCCESS,
        FAILURE,
        DNF,
        SUCCESS_EARLY,
        FAILURE_EARLY
    };

    /**
     * @brief Something that will be printed in the results.
     * Anything that extends this will be able to be eventually printed in the test results.
     */
    class Printable {
    public:
        /**
         * @brief Gets the message associated with this Printable
         * This message may be of any form (as long as it is a string), the definition of the behaviour
         * depends on the inheriting class.
         * @return
         */
        [[nodiscard]] virtual std::string getMessage() const {
            return message;
        };
    protected:
        std::string message;
        std::string partOf;
        int groupNum = 0;
        Printable(std::string m, std::string pOf, int group) {
            message = std::move(m);
            partOf = std::move(pOf);
            groupNum = group;
        }
    };

    /**
     *  @brief A class that holds the result of all tests.
     *  All fields are public for easy debugging
     *
     *  */
    class Result : public Printable {
    public:
        bool state;
        int testNum = 0;
        int error = 0;

        Result(std::string m, bool s, int group = 0, int test = 0, int err = 0, std::string pOf = "") : Printable(std::move(m), std::move(pOf), group) {
            state = s;
            testNum = test;
            error = err;
        }

//        friend std::ostream &operator<<(std::ostream &os, const Result &result) {
//            os << " \x1b[35m Group " << result.groupNum << "\x1b[0m,\x1b[36m Test " << result.testNum
//               << "\x1b[0m\tResult: "
//               << (result.state ? "\x1b[42m true \x1b[0m" : "\x1b[41m false \x1b[0m") + std::string(" | ") +
//                  result.message;
//            return os;
//        }

        void updatePartOf(std::string newPart) {
            partOf = std::move(newPart);
        }

        std::string getPartOf() {
            return partOf;
        }

        [[nodiscard]] std::string getMessage() const override {
            return std::string("\x1b[35m\x1b[1mGroup " + std::to_string(groupNum) + "\x1b[0m | \x1b[36mTest " +
                               std::to_string(testNum)
                               + "\x1b[0m | Result: " + (state ? "\x1b[42m true \x1b[0m" : "\x1b[41m false \x1b[0m") +
                               std::string(" | ") + message
                               + (error == 1
                                  ? "\n\t\tNote this test ^ may show the same address due to compiler optimizations"
                                  : ""));
        }
    };

    class TestMessage : public Printable {
    private:
        MessageType type;
    public:
        TestMessage(std::string m, std::string pOf, int group, MessageType messageType = MessageType::LOG) : Printable(std::move(m), std::move(pOf), group) {
            type = messageType;
        };

        [[nodiscard]] std::string getMessage() const override {
            std::string result;
            switch(type) {
                case LOG:
                    result += "\033[38;2;100;200;255m"; break;
                case WARNING:
                    result += "\033[38;2;250;250;25m"; break;
                case SEVERE:
                    result += "\033[38;2;255;100;255m"; break;
                case FAIL:
                    result += "\033[38;2;255;0;0m";break;
            }
            result += message;
            result += "\033[0m";
            return result;
        }
    };


    class TestResult {
    private:
        std::vector<std::unique_ptr<Printable>> printables;
        std::string name;
        TestResultStatus status = TestResultStatus::DNF;
        int numPassing = 0;
        int numTotal = 0;

    public:

        explicit TestResult(std::string testName) {
            name = std::move(testName);
        }

        ~TestResult() = default;

        [[nodiscard]] std::string toString() const {
            std::string result = "\x1b[92m\x1b[1m\x1b[4m" + name + "\033[0m\033[1m | " + std::to_string(numPassing) + "/"
                    + std::to_string(numTotal) + " passed\033[0m\n" +
                    "----------------------------------------------------------\n";
            for (const auto & printable : printables) {
                result += "|- " + printable->getMessage();
                result += "\n";
            }
            return result;
        }

        void giveResultsState(bool passing) {
            numPassing += passing ? 1 : 0;
            numTotal++;
        }

        void addPrintable(std::unique_ptr<Printable> printable) {
            printables.emplace_back(std::move(printable));
        }

        std::string getPartOf() {
            return name;
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
        friend class Tester;
    protected:
        T data;
        U expected;
        std::string message;
        int groupNum;
    public:
        Test(T Data, T Expected, std::string Message, int group = 0) {
            data = Data;
            expected = Expected;
            message = std::move(Message);
            groupNum = group;
        }

        ~Test() = default;

        virtual Result Run() {
            return Result{"", false};
        };
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
        friend class Tester;
    private:
        double upperLimit = 0;
        double lowerLimit = 0;
        Result Run(const std::source_location loc = std::source_location::current(), std::string ogFunction = "(not specified)") {
            bool state = false;
            std::string result;
            try {
                state = (this->data + lowerLimit <= this->expected && this->data + upperLimit >= this->expected) ||
                        CommonLib::isEqual(this->expected, this->data);
                result = CommonLib::getStringResultOnSuccess(this->expected, this->data, this->message, state, 1, loc, ogFunction);
            }
            catch (std::exception &e) {
                result = "Exception Thrown: " + std::string(e.what()) + " | " + this->message;
            }
            return {result, state, this->groupNum, 1};
        }
    public:

        /**
         * @brief Range constructor where upperLimit and lowerLimit are range away from 0
         * @param data Actual data
         * @param expected Expected data
         * @param range Range of the limit from 0, + or -
         */
        TestFloat(T data, U expected, double range, std::string message = "", int group = 0) : Test<T, U>(data, expected, message, group) {
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
        TestFloat(T data, U expected, double lLimit, double uLimit, std::string message = "", int group = 0) : Test<T, U>(data, expected, message, group) {
            lowerLimit = lLimit;
            upperLimit = uLimit;
        }



        /**
         * @brief Runs the test with lower and upper limit of leniency for floating point imprecision
         * @return The Result of the test
         */
        Result Run() {
            return Run(std::source_location::current());
        }
    };


    /**
     * @brief A class that is the parent of all Tests that use a vector for results
     * @tparam T The type of the expected vector
     */
    template<class T>
    class VectorTest {
        friend class Tester;
    protected:
        std::vector<Result> results;
        std::string message; // something appended to all tests
        std::vector<std::string> messages; // something appended to nth test
        std::vector<T> expected;
        int groupNum;
    public:

        explicit VectorTest(std::vector<T> Expected, std::string Message = "", std::vector<std::string> Messages = {}, int group = 0) {
            expected = Expected;
            messages = std::move(Messages);
            message = std::move(Message);
            groupNum = group;
        }

        explicit VectorTest(std::string Message = "", std::vector<std::string> Messages = {}, int group = 0) {
            message = std::move(Message);
            messages = std::move(Messages);
            groupNum = group;
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
        friend class Tester;
    private:
        int from;
        int to;

    protected:
        template<typename Callable, typename... Args>
        std::vector<Result> RunAllArgs(const std::source_location loc, Callable &method, Args... args) {
            int index = 0;
            for (int i = from; i <= to; i++) {
                bool state = false;
                std::string result;
                try {
                    if (this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        std::invoke(method, i, args...);
                        result = CommonLib::getStringResultOnSuccess(this->actual, this->expected, this->message, true, i);
                    } else {
                        // if expected is smaller than range, we just use the last value as expected
                        state = CommonLib::isEqual(std::invoke(method, i, args...), this->expected.at(
                                std::min<unsigned long long>(this->expected.size() - 1, index)));
                        result = CommonLib::getStringResultOnSuccess(this->actual, this->expected, this->message, state, i);
                    }
                }
                catch (std::exception &e) {
                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back(this->message + " " + result +
                                           (index < this->messages.size() ? ", " + this->messages.at(index) : ""),
                                           state, this->groupNum, index + 1);
                index++;
            }
            return this->results;
        }


    public:
        /**
         * @brief No expected constructor
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Message optional message that will print for every result
         * @param Messages optional message that will print for nth result
         */
        TestRange(int From, int To, std::string Message = "", std::vector<std::string> Messages = {}, int group = 0) : from(From), to(To), VectorTest<U>(Message, Messages, group) {}

        /**
         * @brief Expected constructor
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Expected an std::vector<U> that has the list of expected values for each test, in order
         * @param Message optional message that will print for every result
         * @param Messages optional message that will print for nth result
         */
        TestRange(int From, int To, std::vector<U> Expected, std::string Message = "", std::vector<std::string> Messages = {}, int group = 0) : from(From), to(To), VectorTest<U>(Expected, Message, Messages, group) {}

        ~TestRange() = default;

        /**
         * @brief Update the test
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Message optional message that will print for every result
         * @param Messages optional messages that will print for nth result
         */
        void UpdateTest(int From, int To, const std::string &Message = "", const std::vector<std::string> &Messages = {}) {
            from = From;
            to = To;
            this->message = Message;
            this->messages = Messages;
        }

        /**
         * @brief Update the test
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Expected an std::vector<U> that has the list of expected values for each test, in order
         * @param Message optional message that will print for every result
         * @param Messages optional messages that will print for nth result
         */
        void UpdateTest(int From, int To, std::vector<U> Expected, const std::string &Message = "", const std::vector<std::string> &Messages = {}) {
            from = From;
            to = To;
            this->message = Message;
            this->expected = Expected;
            this->messages = Messages;
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
        std::vector<Result> RunAllArgs(Callable &method, Args... args) {
            return RunAllArgs(std::source_location::current(), method, args...);
        }


//        // we have the exact same function here except due to template issues, this one does not have args.
//        // this **will** throw a compile time error if it is used with a function
//        /**
//         * @brief Run all of the tests
//         * @param method A callable function, lambda or method
//         * @return A vector of Result with the results
//         *
//         * This will run through all of the tests from `from` and to `to`, inclusive.
//         * It will input the current value of the range as the first parameter into the function.
//         * This is designed for functions that specifically take in *1* parameter of type int
//         * If supplied, it will check against the expected vector.
//         *
//         */
//        template<typename Callable>
//        std::vector<Result> RunAllNoArgs(Callable& method) {
//            int index = 0;
//            for(int i = from; i <= to; i++) {
//                bool state = false;
//                std::string result;
//                try {
//                    if(this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
//                        std::invoke(method, i);
//                        result = std::string("Passed: ") + std::to_string(i);
//                    }
//                    else {
//                        // if expected is smaller than range, we just use the last value as expected
//                        state = CommonLib::isEqual(std::invoke(method, i), this->expected.at(std::min<unsigned long long>(this->expected.size() - 1, index)));
//                        result = std::string(state ? "Passed: " : "Failed: ") + std::to_string(i);
//                    }
//                }
//                catch(std::exception &e) {
//                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
//                }
//                this->results.emplace_back(this->message + " " + result + (index < this->messages.size() ? ", " + this->messages.at(index) : ""), state, this->groupNum, index + 1);
//                index++;
//            }
//            return this->results;
//        }

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
        std::vector<Result> RunAll(Callable &method, Args... args) {
            return RunAllArgs(std::source_location::current(), method, args...);
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
        std::vector<Result> RunAll(Callable &method) {
            return RunAllArgs(std::source_location::current(), method);
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
        friend class Tester;
    private:
        std::vector<T> actualData;

    protected:
        std::vector<Result> RunAll(std::source_location loc, const std::string &message = "") {
            std::vector<Result> results;
            for (int i = 0; i < std::min(actualData.size(), this->expected.size()); i++) {
                try {
                    bool state = RunAt(i);
                    results.emplace_back(CommonLib::getStringResultOnSuccess(actualData.at(i),this->expected.at(i),this->message + (i < this->messages.size() ? ", " + this->messages.at(i) : ""),
                                                                             state, i), state, this->groupNum, i + 1);
                }
                catch (std::exception &exception) {
                    results.emplace_back(message + " " + std::string("Exception thrown: ") + exception.what() + ", " +
                                         (i < this->messages.size() ? ", " + this->messages.at(i) : ""), false,
                                         this->groupNum, i + 1);
                }
            }
            return results;
        }

    public:

        /**
         *
         * @param aData Actual Data
         * @param eData Expected Data
         */
        explicit TestType(std::vector<T> aData = {}, std::vector<U> eData = {}, std::string Message = "", std::vector<std::string> Messages = {}, int group = 0) : actualData(aData), VectorTest<U>(eData, Message, Messages, group) {}

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
                if (!this->messages.empty()) {
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
            if (i >= actualData.size() || i >= this->expected.size()) {
                return false;
            }
            return CommonLib::isEqual(actualData.at(i), this->expected.at(i));
        }

        /**
         * @brief Runs all of the tests
         * @return A vector of the results
         */
        std::vector<Result> RunAll(const std::string &message = "") {
            return RunAll(std::source_location::current(), message);
        }

        friend class Tester;
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
        friend class Tester;
    private:
        std::vector<T> actual;

    protected:
        template<typename Callable, typename... Args>
        std::vector<Result> RunAllArgs(std::source_location loc, Callable &method, Args... args) {
            for (int i = 0; i < actual.size(); i++) {
                bool state = false;
                std::string result;
                try {
                    if (this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        std::invoke(method, actual.at(i), args...);
                        result = CommonLib::getStringResultOnSuccess("No exception thrown", "(nothing)", this->message, true, i, loc);
                    } else {
                        state = CommonLib::isEqual(std::invoke(method, actual.at(i), args...), this->expected.at(std::min<size_t>(this->expected.size() - 1, i)));
                        result = CommonLib::getStringResultOnSuccess(actual.at(i), this->expected.at(i), this->message, state, i, loc);
                    }
                }
                catch (std::exception &e) {
                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
                }
                this->results.emplace_back("For " + (i < this->messages.size() ? ", " + this->messages.at(i) : "") + ", " + result, state, this->groupNum, i + 1);
            }

            return this->results;
        }

    public:
        TestTwoVector(std::vector<T> Actual, std::vector<U> Expected, std::string Message = "", std::vector<std::string> Messages = {}, int group = 0) : actual(Actual), VectorTest<U>(Expected, Message, Messages, group) {}

        explicit TestTwoVector(std::vector<T> Actual, std::string Message = "", std::vector<std::string> Messages = {}, int group = 0) : actual(Actual), VectorTest<U>(Message, Messages, group) {}

        void UpdateTest(std::vector<T> Actual, std::vector<U> Expected, const std::string& Message = "") {
            actual = Actual;
            this->expected = Expected;
            this->message = Message;
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
//            template<typename Callable>
//            std::vector<Result> RunAllNoArgs(Callable& method) {
//                for(int i = 0; i < actual.size(); i++) {
//                    bool state = false;
//                    std::string result;
//                    try {
//                        if(this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
//                            std::invoke(method, actual[i]);
//                            result = std::string("Passed: ") + std::to_string(i);
//                        }
//                        else {
//                            state = std::invoke(method, actual[i]) == this->expected.at(std::min<unsigned long long int>(this->expected.size() - 1, i));
//                            result = std::string(state ? "Passed: " : "Failed: ") + std::to_string(i);
//                        }
//                    }
//                    catch(std::exception &e) {
//                        result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
//                    }
//                    this->results.emplace_back(this->message + " " + result + (i < this->messages.size() ? ", " + this->messages.at(i) : ""), state, this->groupNum, i + 1);
//                }
//
//                return this->results;
//            }

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
        std::vector<Result> RunAllArgs(Callable &method, Args... args) {
            return RunAllArgs(std::source_location::current(), method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable &method, Args... args) {
            return RunAllArgs(method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> RunAll(Callable &method) {
            return RunAllArgs(method);
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
        std::vector<std::unique_ptr<TestResult>> results;
        std::unique_ptr<TestResult> defaultTestResult = std::make_unique<TestResult>("(default)");
        std::unique_ptr<TestResult> currentTestResult = std::move(defaultTestResult);



    public:
        Tester() = default;

        ~Tester() = default;

        /**
         * @brief Tests one comparison using operator==. Will automatically put into results.
         * @tparam T The type of data that you are testing
         * @tparam U The type of data that you are expecting
         * @param actual The actual data
         * @param expected The expected data
         * @return A Result object containing the results of the test
         */
        template<typename T, typename U>
        Result testOne(T actual, U expected, std::string message = "", const std::source_location loc = std::source_location::current()) {
            try {
                bool state = CommonLib::isEqual(actual, expected);
                std::string args = CommonLib::getStringResultOnSuccess(actual, expected, message, state, 1, loc,
                                                                       "testOne(" + std::string(CommonLib::type_name<T>()).substr(22) + " actual = " + CommonLib::toString(actual) + ", " +
                                                                                 std::string(CommonLib::type_name<U>()).substr(22) + " expected = " + CommonLib::toString(expected) +
                                                                                 ", std::string message = \"" + message + "\")");
                currentTestResult->addPrintable(std::make_unique<Result>(Result{args, state, static_cast<int>(results.size() + 1), 1, CommonLib::toString(expected) == CommonLib::toString(actual) && !state ? 1 : 0}));
                currentTestResult->giveResultsState(state);
                return {args, state};
            }
            catch (std::exception &exception) {
                std::string args = "Test #" + std::to_string(1) + std::string("Exception thrown: ") + exception.what() +
                                   (!message.empty() ? " | Message: " + message : "");
                currentTestResult->addPrintable(std::make_unique<Result>(Result{args, false, static_cast<int>(results.size() + 1), 1}));
                currentTestResult->giveResultsState(false);
                return {args, false};
            }
        }


        /**
         * @brief Test floating point number with imprecision leniency
         * @tparam T A floating point number
         * @tparam U A floating point number
         * @param actual A floating point number that is the actual result
         * @param expected A floating point number to compare against the actual
         * @param range Range of the limit from 0, + or -
         * @param message A message appended to the result
         * @return A Result
         */
        template<typename T, typename U>
        Result testFloat(T actual, U expected, double range, std::string message = "", const std::source_location loc = std::source_location::current()) {
            Result res = TestFloat(actual, expected, range, message, static_cast<int>(results.size() + 1)).Run(loc,
                                                                                                               "testFloat(" + std::string(CommonLib::type_name<T>()).substr(22) + " actual = " + CommonLib::toString(actual) + ", " +
                                                                                                               std::string(CommonLib::type_name<U>()).substr(22) + " expected = " + CommonLib::toString(expected) + ", range = " + std::to_string(range) + ", std::string message = " + message + ")");
            currentTestResult->addPrintable(std::make_unique<Result>(res));
            currentTestResult->giveResultsState(res.state);
            return res;
        }

        /**
         * @brief Test floating point number with imprecision leniency
         * @tparam T A floating point number
         * @tparam U A floating point number
         * @param actual A floating point number that is the actual result
         * @param expected A floating point number to compare against the actual
         * @param lowerBound The lower bound of the imprecision
         * @param upperBound The upper bound of the imprecision
         * @param message A message appended to the result
         * @return A Result
         */
        template<typename T, typename U>
        Result testFloat(T actual, U expected, double lowerBound, double upperBound, std::string message = "", const std::source_location loc = std::source_location::current()) {
            Result res = TestFloat(actual, expected, lowerBound, upperBound, message, static_cast<int>(results.size() + 1)).Run(loc);
            currentTestResult->addPrintable(std::make_unique<Result>(res));
            currentTestResult->giveResultsState(res.state);
            return res;
        }


        /**
         * @brief Test multiple tests of one pair of types
         * @tparam T Type of actual
         * @tparam U Type of expected
         * @param actual Actual data that you are testing against
         * @param expected Expected data that you are comparing with
         * @param message A message to append to all results
         * @param messages A message to append to nth result
         * @return
         */
        template<typename T, typename U>
        std::vector<Result> testType(std::vector<T> actual, std::vector<U> expected, std::string message = "", std::vector<std::string> messages = {}, const std::source_location loc = std::source_location::current()) {
            std::vector<Result> testResults = TestType(actual, expected, message, messages, static_cast<int>(results.size() + 1)).RunAll(loc);
            for (const auto& result : testResults) {
                currentTestResult->addPrintable(std::make_unique<Result>(result)); // todo, this would be quite slow for large tests, make a addPrintableBulk method
                currentTestResult->giveResultsState(result.state);
            }
            return testResults;
        }


        /**
         * @brief Function version of the class TestRange
         * @tparam T The return type of the Callable
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
        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::vector<T> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            return testRange(std::source_location::current(), from, to, expected, message, messages, method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, Callable &method, Args... args) {
            return testRange(std::source_location::current(), from, to, std::vector<int>{}, "", {}, method, args...);
        }

        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::vector<T> expected, Callable &method, Args... args) {
            return testRange(std::source_location::current(), from, to, expected, "", {}, method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            return testRange(std::source_location::current(), from, to, std::vector<int>{}, message, messages, method, args...);
        }

        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testRange(std::source_location loc, int from, int to, std::vector<T> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            std::vector<Result> testResults = TestRange<T>(from, to, expected, message, messages, static_cast<int>(results.size() + 1)).RunAll(loc, method, args...);
            for (const auto& result : testResults) {
                currentTestResult->addPrintable(std::make_unique<Result>(result)); // todo, this would be quite slow for large tests, make a addPrintableBulk method
                currentTestResult->giveResultsState(result.state);
            }
            return testResults;
        }

        template<typename Callable, typename... Args>
        std::vector<Result> testRange(std::source_location loc, int from, int to, Callable &method, Args... args) {
            return testRange(loc, from, to, std::vector<int>{}, "", {}, method, args...);
        }

        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testRange(std::source_location loc, int from, int to, std::vector<T> expected, Callable &method, Args... args) {
            return testRange(loc, from, to, expected, "", {}, method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> testRange(std::source_location loc, int from, int to, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            return testRange(loc, from, to, std::vector<int>{}, message, messages, method, args...);
        }

        /**
         * @brief Function version of the class TestRange
         * @tparam T The return type of the Callable
         * @tparam Callable Any function, method or lambda that can be called upon
         * @tparam Args The arguments for Callable
         * @param from Starting range (inclusive)
         * @param to Ending range (inclusive)
         * @param method A Callable
         * @param message A message to append to all results
         * @param messages A message to append to nth result
         * @return A vector of Results
         */
        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testRange(int from, int to, std::vector<T> expected, Callable &method, std::string message = "", std::vector<std::string> messages = {}) {
            std::vector<Result> testResults = TestRange<T>(from, to, expected, message, messages, static_cast<int>(results.size() + 1)).RunAll(method);
            for (const auto& result : testResults) {
                currentTestResult->addPrintable(std::make_unique<Result>(result)); // todo, this would be quite slow for large tests, make a addPrintableBulk method
                currentTestResult->giveResultsState(result.state);
            }
            return testResults;
        }


        /**
         * @brief Function version of the class TestTwoVector
         * @tparam T Type of the inputs
         * @tparam U Type of the expected
         * @tparam Callable Any function, method or lambda tha can be called upon
         * @tparam Args The arguments for Callable
         * @param inputs Inputs for each test
         * @param expected Expected output for each input test
         * @param message A message appended to all results
         * @param method A Callable
         * @param args An Args for method's arguments
         * @return A vector of Results
         */
        template<typename T, typename U, typename Callable, typename... Args>
        std::vector<Result> testTwoVectorMethod(std::source_location loc, std::vector<T> inputs, std::vector<U> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            std::vector<Result> testResults = TestTwoVector<T, U>(inputs, expected, message, messages, static_cast<int>(results.size() + 1)).RunAll(loc, method, args...);
            for (const auto& result : testResults) {
                currentTestResult->addPrintable(std::make_unique<Result>(result)); // todo, this would be quite slow for large tests, make a addPrintableBulk method
                currentTestResult->giveResultsState(result.state);
            }
            return testResults;
        }

        template<typename T, typename U, typename Callable, typename... Args>
        std::vector<Result> testTwoVectorMethod(std::vector<T> inputs, std::vector<U> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            return testTwoVectorMethod(std::source_location::current(), inputs, expected, message, messages, method, args...);
        }
        // we have to do a lot of copy-pasting due to the fact that we can't simply have default parameters here
        // also we cannot use just messages or just message because then the compiler infers it as Callable, which we do not want
        // in the event that you just want message or messages, make message = "" or messages = {}
        template<typename T, typename U, typename Callable, typename... Args>
        std::vector<Result> testTwoVectorMethod(std::vector<T> inputs, std::vector<U> expected, Callable &method, Args... args) {
            return testTwoVectorMethod(std::source_location::current(), inputs, expected, "", {}, method, args...);
        }

        template<typename T, typename U, typename Callable, typename... Args>
        std::vector<Result> testTwoVectorMethod(std::source_location loc, std::vector<T> inputs, std::vector<U> expected, Callable &method, Args... args) {
            return testTwoVectorMethod(loc, inputs, expected, "", {}, method, args...);
        }

        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testTwoVectorMethod(std::vector<T> inputs, Callable &method, Args... args) {
            return testTwoVectorMethod(std::source_location::current(), inputs, std::vector<T>{}, "", {}, method, args...);
        }

        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testTwoVectorMethod(std::source_location loc, std::vector<T> inputs, Callable &method, Args... args) {
            return testTwoVectorMethod(loc, inputs, std::vector<T>{}, "", {}, method, args...);
        }

        // now we have to make the same thing but except this time for no arguments
        // but this time due to the lack of a packed argument, we can use default arguments!
        /**
         * @brief Function version of the class TestTwoVector
         * @tparam T Type of the inputs
         * @tparam U Type of the expected
         * @tparam Callable Any function, method or lambda tha can be called upon
         * @param inputs Inputs for each test
         * @param method A Callable
         * @param expected *optional* Expected output for each input test
         * @param message *optional* A message appended to all results
         * @param messages *optional* A message appended to nth result
         * @return A vector of Results
         */
        template<typename T, typename U, typename Callable>
        std::vector<Result> testTwoVectorMethod(std::source_location loc, std::vector<T> inputs, Callable &method, std::vector<U> expected = {}, std::string message = "", std::vector<std::string> messages = {}) {
            return testTwoVectorMethod(loc, inputs, expected, message, messages, method);
        }

        template<typename T, typename U, typename Callable>
        std::vector<Result> testTwoVectorMethod(std::vector<T> inputs, Callable &method, std::vector<U> expected = {}, std::string message = "", std::vector<std::string> messages = {}) {
            return testTwoVectorMethod(std::source_location::current(), inputs, method, expected, message, messages);
        }

        /**
         * @brief Checks if a Callable throws the same exception as specified
         * @tparam Callable Any function, method or lambda that can be called upon
         * @tparam Args The arguments for callable
         * @param exception The exception to look for
         * @param message A message to append to the test
         * @param method A Callable
         * @param args An Args
         * @return A Result with the details on the test
         *
         * This will only check the value of the exception through a string.
         * It will not check by type because std::exception is a class that
         * is the parent class for all exceptions, and does not have a virtual
         * method that allows me to know for all exceptions what type they are.
         */
        template<typename Callable, typename... Args>
        Result testException(const std::string &exception, const std::string &message, Callable &method, Args... args) {
            try {
                std::invoke(method, args...);
                Result res{"Did not throw exception.", false, static_cast<int>(results.size() + 1), 1};
                currentTestResult->addPrintable(std::make_unique<Result>(res));
                currentTestResult->giveResultsState(res.state);
                return res;
            }
            catch (std::exception &e) {
                if (e.what() == exception) {
                    Result res{"Matched exception.", true, static_cast<int>(results.size() + 1), 1};
                    currentTestResult->addPrintable(std::make_unique<Result>(res));
                    currentTestResult->giveResultsState(res.state);
                    return res;
                }
                Result res{"Did not match exception. Exception: " + std::string(e.what()), false,static_cast<int>(results.size() + 1), 1};
                currentTestResult->addPrintable(std::make_unique<Result>(res));
                currentTestResult->giveResultsState(res.state);
                return res;
            }
        }

        template<typename Callable, typename... Args>
        void test(Tester &tester, Callable &method, Args... args) {
            test(std::string("(no label ") + std::to_string(results.size() + 1) + ")", method, tester, args...);
        }

        template<typename Callable, typename... Args>
        void test(const std::string& testName, Tester &tester, Callable &method, Args... args) {
            defaultTestResult = std::move(currentTestResult);
            currentTestResult = std::make_unique<TestResult>(testName);
            std::invoke(method, tester, args...);
            results.emplace_back(std::move(currentTestResult));
            currentTestResult = std::move(defaultTestResult);
        }


        void addMessage(std::string message, MessageType type = MessageType::LOG) {
            currentTestResult->addPrintable(std::make_unique<TestMessage>(message, currentTestResult->getPartOf(), results.size() + 1, type));
        }

        /**
         * @brief Prints the results of the vector results
         */
        void printResults() {
            std::cout.flush();
            for (const auto& testRes : results) {
                std::cout << testRes->toString() << "\n";
            }
            std::cout << currentTestResult->toString() << std::endl;
        }

//        /**
//         * @brief Prints the results of the vector results, true for results that passed, false for failed
//         * @param showPassing  true for results that passed, false for failed
//         */
//        void printResults(bool showPassing) {
//            std::cout.flush();
//            std::vector<Result> appended = CommonLib::appendAllVectors(results);
//            std::vector<Result> success = CommonLib::filter(appended,
//                                                 [showPassing](const Result &res) { return showPassing == res.state; });
//            std::cout << std::endl << "Test Results: ("
//                      << (showPassing ? success.size() : appended.size() - success.size()) << "/" << appended.size()
//                      << ") passed." << std::endl;
//            int total = 0;
//            std::string printedResult;
//            for (Result r: appended) { // we use appended to get the test number
//                if (r.state == showPassing) {
//                    printedResult += (!r.getPartOf().empty() ? "\x1b[92m \x1b[1m \x1b[4m " + r.getPartOf() + " \x1b[0m" : "");
//                    printedResult += "(" + std::to_string(total + 1) + ")" + r.getMessage() + '\n';
//                }
//                total++;
//            }
//            std::cout << printedResult << std::endl;
//            appended.clear();
//        }
//
//        /**
//         * @brief Print the selected test number result
//         * @param testNumber The test number
//         */
//        void printTest(int testNumber) {
//            std::cout.flush();
//            std::vector<Result> appended = CommonLib::appendAllVectors(results);
//            unsigned long long success = CommonLib::filter(appended, [](const Result &res) { return res.state; }).size();
//            std::cout << std::endl << "Test Results: (" << success << "/" << appended.size()
//                      << ") passed. Showing only Test #" << testNumber << std::endl;
//            if (testNumber <= appended.size() && testNumber >= 1) {
//                testNumber--;
//                Result r = appended.at(testNumber);
//                std::cout << (!r.getPartOf().empty() ? "\x1b[92m \x1b[1m \x1b[4m " + r.getPartOf() + " \x1b[0m" : "") << "("
//                          << testNumber + 1 << ")" << r.getMessage() << std::endl;
//            } else {
//                std::cout << "No results.";
//            }
//            appended.clear();
//        }
//
//        /**
//         * @brief Prints out the entire group test results
//         * @param groupNumber The group to print out
//         */
//        void printGroup(int groupNumber) {
//            std::cout.flush();
//            if (groupNumber <= results.size() && groupNumber >= 1) {
//                std::vector<Result> &group = results.at(groupNumber);
//                unsigned long long success = CommonLib::filter(group, [](const Result &res) { return res.state; }).size();
//                std::cout << std::endl << "Test Results: (" << success << "/" << group.size()
//                          << ") passed. Showing only Group #" << groupNumber << std::endl;
//                std::string printedResult;
//                groupNumber--;
//                int i = 1;
//                for (Result r: group) {
//                    printedResult += (!r.getPartOf().empty() ? "\x1b[92m \x1b[1m \x1b[4m " + r.getPartOf() + " \x1b[0m" : "");
//                    printedResult += "(" + std::to_string(i) + ")" + r.getMessage() + '\n';
//                    i++;
//                }
//                std::cout << printedResult << std::endl;
//            } else {
//                std::cout << "No results.";
//            }
//        }

//        /**
//         * @brief Get results
//         */
//        std::vector<TestResult> getResults() {
//            return {results};
//        }
    };


}