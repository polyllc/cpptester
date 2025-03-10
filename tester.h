 #ifndef CPP_TESTER_H
#define CPP_TESTER_H

#include <utility>
#include <vector>
#include <iostream>
#include <any>
#include <functional>
#include <memory>
#include <sstream>
#include <fstream>
#include <source_location>
#include <numeric>
#include <chrono>
#include <concepts>
#include <map>
#include <regex>


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

    enum TestFilter {
        PASSING_ONLY,
        FAILING_ONLY,
        BOTH
    };

    enum TesterSettings {
        THROW_ON_FAIL, // throw exception (and then stop) on failure
        THROW_ON_ERROR, // throw exception (and then stop) on error
        THROW_ON_ALIAS, // throw exception (and then stop) when a == b and a is an alias of b (same address)
        PRINT_SYNC, // print tests as you go, this will give inaccurate information for test result headers
    };

    using signed_size_t = long long;


    /**
     * Function prototypes for global namespace functions
     */
    namespace CommonLib {
        template<typename T, typename Lambda>
        std::vector<T> filter(std::vector<T> &vector, Lambda func);

        template<typename T>
        std::vector<T> appendAllVectors(const std::vector<std::vector<T>> &vec);

        template<typename T, typename U>
        std::string getStringResultOnSuccess(T actual, U expected, const std::string &message, bool state, signed_size_t testNum = 1, const std::source_location loc = std::source_location::current(), const std::string &ogFunction = "(not specified)");

        template<typename T, typename U>
        bool isEqual(T actual, U expected, bool throwOnAlias = false);

        template<typename T>
        std::string toString(T from);

        template<class... T>
        constexpr std::string_view type_name();

        template<typename T>
        std::string vectorToString(std::vector<T> vec);

        std::string escapeString(const std::string& str);


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

        /**
         * Gets the string that is associated with a result
         * @tparam T actual value type
         * @tparam U expected value type
         * @param actual actual value
         * @param expected expected value
         * @param message message to append to end
         * @param state the state result
         * @param testNum the test number
         * @param loc source location of where it was called
         * @param ogFunction what test function was used
         * @return a string formatted with all of this info
         */
        template<typename T, typename U>
        std::string getStringResultOnSuccess(T actual, U expected, const std::string &message, bool state, signed_size_t testNum, const std::source_location loc, const std::string &ogFunction) {
            return "Test " + std::to_string(testNum) + (state ? " Success" : " Failure") +
                    " \n|\t\x1b[1mwas: \x1b[0m " + toString(actual) +
                    " \t\x1b[1mexpected: \x1b[0m " + toString(expected) +
                    " \n|\t\x1b[1mwas:\033[0m " + std::string(type_name<T>()).substr(22) + "\x1b[0m" +
                    "\t\x1b[1mexpected type:\x1b[0m " + std::string(type_name<U>()).substr(22) +

                   "\n|\t\x1b[1mat: " + loc.file_name() + ":" + std::to_string(loc.line()) +
                   "\x1b[0m\n|\t\x1b[1mcalled in: \x1b[0m" + loc.function_name() +
                   "\n|\t\x1b[1mas: \x1b[0m" + ogFunction +
                   (!message.empty() ? "\n|\t\033[1mmessage:\033[0m " + message : "") +
                   "\n|";
        }


        /**
         * Checks if lhv and rhv can be converted into a string and then compared
         * @tparam T lhv type
         * @tparam U rhv type
         */
        template<typename T, typename U>
        concept canBeStringCompared = requires(T& lhv, U& rhv) {
            {std::string(lhv)} -> std::convertible_to<std::string>;
            {std::string(rhv)} -> std::convertible_to<std::string>;
            {std::string(rhv) == std::string(lhv)} -> std::same_as<bool>; // although probably redundant
        };

        template<typename T, typename U>
        concept hasEquals = requires(T& obj, U& other) {
            {obj.equals(other)} -> std::same_as<bool>;
        };

        template<typename T, typename U>
        concept hasEqualsOperator = requires(const T& obj, const U& other) {
            {obj == other} -> std::same_as<bool>;
        };

        /**
         * @brief Checks if actual is equal to expected (in a variety of different ways)
         * @tparam T type of actual
         * @tparam U type of expected
         * @param actual actual value
         * @param expected expected value
         * @param throwOnAlias whether to throw on comparing addresses
         * @return true if equals, false otherwise
         */
        template<typename T, typename U>
        bool isEqual(T actual, U expected, bool throwOnAlias) {
            if constexpr (canBeStringCompared<T, U>) {
                return std::string(expected) == std::string(actual);
            }
            else if constexpr (hasEqualsOperator<T, U>) {
                return expected == actual;
            }
            else if constexpr (hasEquals<T, U>) {
                return actual.equals(expected);
            }
            else {
                bool res = CommonLib::toString(expected) == CommonLib::toString(actual);
                if (throwOnAlias && res) {
                    throw std::invalid_argument("\033[38;2;255;0;0mActual and expected refer to the same address and this is not allowed by THROW_ON_ALIAS (this means that there is no operator== nor equals method that is compatible with the types)\033[0m");
                }
                return res;
            }
        }

        /**
         * Checks if toAppend can be inserted into an output stream
         * @tparam T the type to check to insert
         */
        template<typename T>
        concept oStreamInsertionAbility = requires(std::ostream &os, T& toAppend) {
            {os << toAppend} -> std::same_as<std::ostream&>;
        };

        /**
         * Checks if there is a toString function already defined
         * @tparam T the type to check toString in
         */
        template<typename T>
        concept hasToString = requires(T obj) {
            {obj.toString()} -> std::same_as<std::string>;
        };

        template<typename T>
        std::string toString(T from) {
            std::ostringstream stream;
            if constexpr (oStreamInsertionAbility<T>) {
                stream << from;
            }
            else if constexpr (std::is_same<T, bool>::value) {
                stream << (from ? "true" : "false");
            }
            else if constexpr (hasToString<T>) {
                return from.toString();
            }
            else {
                stream << "*" << static_cast<const void*>(std::addressof(from));
            }
            return stream.str();
        }


        /**
         * Gets the type name of any type
         * @tparam T the type to get
         * @return a string_view containing the type's name
         */
        template <typename... T>
        constexpr std::string_view type_name() { // @howard-hinnant on stackoverflow :)
        #ifdef __clang__
            std::string_view p = __PRETTY_FUNCTION__;
            return {p.data() + 34, p.size() - 34 - 1};
        #elif defined(__GNUC__)
            std::string_view p = __PRETTY_FUNCTION__;
            return {p.data() + 50, p.find(';', 49) - 51};
        #elif defined(_MSC_VER)
            std::string_view p = __FUNCSIG__;
            return {p.data() + 84, p.size() - 84 - 7};
        #endif
        }



        // no need for anything fancy to get the enum name, it either requires a lot of random functions,
        // or another library, which would defeat the purpose of this project
        /**
         * Gets the enum TestResultStatus and makes it a string
         * @param status the enum to convert
         * @return a string with the enum name
         */
        const char* statusString(TestResultStatus status) {
            switch(status) {
                case SUCCESS: return "SUCCESS";
                case SUCCESS_EARLY: return "SUCCESS EARLY";
                case FAILURE: return "FAILURE";
                case FAILURE_EARLY: return "FAILURE EARLY";
                case DNF: return "DID NOT FINISH";
                default: return "none";
            }
        }

        std::string regexReplaceOne(std::pair<std::string, std::string> tokenReplace, std::string str) {
            return std::regex_replace(str, std::regex(tokenReplace.first), tokenReplace.second);
        }

        std::string escapeString(const std::string& str) {
            std::vector<std::pair<std::string, std::string>> escapes // not \0
            {{"\n", "\\n"}, {"\b", "\\b"}, {"\r", "\\r"},
             {"\e", "\\e"}, {"\"", "\\\""}, {"\a", "\\a"}, {"\f", "\\f"},
             {"\t", "\\t"}, {"\v", "\\v"}, {"\?", "\\?"}};
            std::regex slash(R"((\\|\S|\s))");
            std::string temp = std::regex_replace(str, slash, "");
            std::regex quote("\"");
            return std::regex_replace(temp, quote, "\\\"");
        }
    }

    /**
     * @brief Something that will be printed in the results.
     * Anything that extends this will be able to be eventually printed in the test results.
     */
    class Printable {
    public:
        /**
         * @brief Gets the message associated with this Printable.
         * This message may be of any form (as long as it is a string), the definition of the behaviour
         * depends on the inheriting class.
         * @return a string containing the message
         */
        [[nodiscard]] virtual std::string getMessage(bool collapse = false) const {
            return message;
        };

        /**
         * @brief Gets a well formatted JSON string representing the object that inherits from Printable.
         * @return JSON representing the object.
         */
        [[nodiscard]] virtual std::string getJSON() const {
            return R"({"type": "printable", "message": ")" + CommonLib::escapeString(getMessage()) + "\"}";
        }
    protected:
        std::string message;
        std::string partOf;
        size_t groupNum = 0;

        Printable(std::string m, std::string pOf, size_t group) {
            message = std::move(m);
            partOf = std::move(pOf);
            groupNum = group;
        }

    public:

        bool state = true;

        friend std::ostream &operator<<(std::ostream &os, const Printable &printable) {
            os << printable.message << '\n';
            return os;
        }
    };

    /**
     * A printable error that stores an error code.
     */
    class Error : public Printable {
    private:
        int errorCode = 0;
    public:
        Error(std::string m, int code, size_t group = 0, std::string pOf = "") : Printable(std::move(m), std::move(pOf), group) {
            errorCode = code;
        }

        [[nodiscard]] std::string getMessage(bool collapse = false) const override {
                return "\x1b[31m(Error code " + std::to_string(errorCode) + ") " + message + "\x1b[0m\n";
        }

        [[nodiscard]] std::string getJSON() const override {
            return R"({"type": "error", "errorCode": )" + std::to_string(errorCode) + R"(, "message": ")" + CommonLib::escapeString(getMessage()) + R"(", "groupNum": )" + std::to_string(groupNum) + R"(, "partOf": ")" + CommonLib::escapeString(partOf) + R"("})";
        }

        [[nodiscard]] int getErrorCode() const {
            return errorCode;
        }
    };

    template<typename CharType>
    class StringCompare : public Printable {
        friend class Tester;
    private:
        std::string expectedStr;
        std::string actualStr;

        std::string diffExpected;
        std::string diffActual;
        size_t diffs = 0;


        std::vector<std::string> detailedDiffs;

        bool detailedDiff = false;

        void calculateDiff() {
            diffExpected.reserve(4 * expectedStr.size() + 4); // for maximum number of escape characters
            diffActual.reserve(4 * actualStr.size() + 4);

            for (size_t i = 0; i < expectedStr.size(); i++) {
                if (actualStr.size() > i) {
                    if (expectedStr.at(i) == actualStr.at(i)) {
                        diffExpected += expectedStr.at(i);
                        diffActual += actualStr.at(i);
                    }
                    else {
                        diffs++;
                        diffExpected += std::string("\033[41m") + expectedStr.at(i) + "\033[0m";
                        diffActual += std::string("\033[41m") + actualStr.at(i) + "\033[0m";
                    }
                }
                else {
                    diffExpected += std::string("\033[43m") + expectedStr.at(i) + "\033[0m";
                    diffs++;
                }
            }
            if (expectedStr.size() < actualStr.size()) {
                diffs += (actualStr.size() - expectedStr.size());
                for (size_t i = expectedStr.size(); i < actualStr.size(); i++) {
                    diffActual += std::string("\033[43m") + actualStr.at(i) + "\033[0m";
                }
            }
        }

    public:

        StringCompare(std::string expected, std::string actual, size_t group = 0, std::string partOf = "") : Printable("", std::move(partOf), group),
        expectedStr(std::move(expected)),
        actualStr(std::move(actual)) {
            calculateDiff();
        }




        [[nodiscard]] std::string getMessage(bool collapse = false) const override {
            std::string res = " String Compare | Actual Size: " + std::to_string(actualStr.size()) +
                    ", Expected Size: " + std::to_string(expectedStr.size()) + " | # Diffs: " + std::to_string(diffs) +
                    "\n\t" + diffActual + "\n\t" + diffExpected;
            return res;
        }

    };

    /**
     *  @brief A class that holds the result of all tests.
     *  All fields are public for easy debugging
     *
     *  */
    class Result : public Printable {
        friend class Tester;
    private:
        void updateTimeTaken(std::chrono::duration<double> time) {
            timeTaken = time;
        }

        std::string wasValue;
        std::string expectedValue;
        std::string wasType;
        std::string expectedType;
        std::string calledIn;
        std::string calledAs;

    public:

        size_t testNum = 0;
        std::vector<Error> errors;
        std::chrono::duration<double> timeTaken {0.0};

        Result(std::string m, bool s, size_t group = 0, size_t test = 0, std::vector<Error> err = std::vector<Error>(), std::string pOf = "") : Printable(std::move(m), std::move(pOf), group) {
            state = s;
            testNum = test;
            errors = std::move(err);
        }

        Result() : Printable("", "", 0){}

        void updatePartOf(std::string newPart) {
            partOf = std::move(newPart);
        }


        std::string getPartOf() {
            return partOf;
        }


        template<typename T, typename U>
        void updateInternal(T actual, U expected, bool state, const std::source_location loc = std::source_location::current(), const std::string &ogFunction = "(not specified)") {
            wasValue = CommonLib::toString(actual);
            expectedValue = CommonLib::toString(expected);
            wasType = std::string(CommonLib::type_name<T>()).substr(22);
            expectedType = std::string(CommonLib::type_name<U>()).substr(22);
            calledIn = loc.function_name();
            calledAs = ogFunction;
        }

        [[nodiscard]] std::string getMessage(bool collapse = false) const override {
            return std::string("\x1b[35m\x1b[1mGroup " + std::to_string(groupNum) + "\x1b[0m | \x1b[36mTest " +
                               std::to_string(testNum)
                               + "\x1b[0m | Result: " + (state ? "\x1b[42mtrue\x1b[0m" : "\x1b[41mfalse\x1b[0m") +
                               " in " + std::to_string(timeTaken.count()) + "sec " +
                               (collapse ? "" : std::string(" | ") + message)
                               + std::accumulate(errors.begin(), errors.end(), std::string(),[collapse](const std::string& acc, const Error& err) {
                                   return acc + " | " + err.getMessage(collapse) + '\n';
                               }));
        }

        [[nodiscard]] std::string getJSON() const override {
            std::string errs = std::accumulate(errors.begin(), errors.end(), std::string(), [](const std::string& acc, const Error& err) -> std::string {
                return acc + ", " + err.getJSON();
            });
            return std::string(R"({"type": "result", "testNum": )" + std::to_string(testNum) + R"(, "errors": {)") + errs.substr((errors.empty() ? 0 : 1) , errs.size() - (errors.empty() ? 0 : 1)) +
            R"(}, "message": ")" + CommonLib::escapeString(getMessage()) + R"(", "state": )" + std::to_string(state) + R"(, "timeTaken": )" + std::to_string(timeTaken.count())
            + R"(, "groupNum": )" + std::to_string(groupNum) + R"(, "partOf": ")" + CommonLib::escapeString(partOf) + R"("})";
        }
    };

    /**
     * A message that can be added to tests
     */
    class TestMessage : public Printable {
    private:
        MessageType type;
    public:
        TestMessage(std::string m, std::string pOf, int group, MessageType messageType = MessageType::LOG) : Printable(std::move(m), std::move(pOf), group) {
            type = messageType;
        };

        [[nodiscard]] std::string getMessage(bool collapse = false) const override {
            std::string result;
            switch(type) {
                case LOG:
                    result += "\033[38;2;100;200;255mLOG: "; break;
                case WARNING:
                    result += "\033[38;2;250;250;25mWARNING: "; break;
                case SEVERE:
                    result += "\033[38;2;255;100;255mSEVERE: "; break;
                case FAIL:
                    result += "\033[38;2;255;0;0mFAIL: ";break;
            }
            result += message;
            result += "\033[0m";
            return result;
        }

        [[nodiscard]] std::string getJSON() const override {
            return R"({"type": "testMessage", "messageType": )" + std::to_string(type) + R"(, "message": ")" + CommonLib::escapeString(getMessage()) + R"(", "groupNum": )" + std::to_string(groupNum) + R"(, "partOf": ")" + CommonLib::escapeString(partOf) + R"("})";
        }
    };

    class TestException : public std::exception {
    private:
        std::string message;

    public:

        TestException(std::string m, std::unique_ptr<Printable> printable) {
            message = std::move(m);
            message += printable->getMessage(false);
        }

        explicit TestException(std::string m) {
            message = std::move(m);
        }

        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }
    };


    /**
     * Holds all results of a specified test(s)
     */
    class TestResult {
    private:
        std::vector<std::unique_ptr<Printable>> printables;
        std::string name;
        TestResultStatus status = TestResultStatus::SUCCESS;
        size_t numPassing = 0;
        size_t numTotal = 0;
        std::chrono::duration<double> timeTaken {0.0};

    public:

        explicit TestResult(std::string testName) {
            name = std::move(testName);
        }

        ~TestResult() = default;

        [[nodiscard]] std::string toString(bool collapseMessages = false, TestFilter filter = BOTH) {
            std::string result = "\x1b[92m\x1b[1m\x1b[4m" + name + "\033[0m\033[1m | " + std::to_string(numPassing) + "/"
                    + std::to_string(numTotal) + " passed | Status: " + CommonLib::statusString(status) + " in " + std::to_string(timeTaken.count()) + "sec\033[0m\n" +
                    "----------------------------------------------------------\n";
            for (const auto & printable : printables) { // todo, convert to filter lambda
                if (filter == BOTH) {
                    result += "|- " + printable->getMessage(collapseMessages);
                    result += "\n";
                }
                else if (filter == FAILING_ONLY) {
                    if (!printable->state) {
                        result += "|- " + printable->getMessage(collapseMessages);
                        result += "\n";
                    }
                }
                else {
                    if (printable->state) {
                        result += "|- " + printable->getMessage(collapseMessages);
                        result += "\n";
                    }
                }
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

        void setStatus(TestResultStatus resultStatus) {
            status = resultStatus;
        }

        void updateTime(std::chrono::duration<double> time) {
            timeTaken = time;
        }

        size_t getSize() {
            return printables.size();
        }

        std::string getPartOf() {
            return name;
        }


        std::string toJSON() {
            std::string acc = std::accumulate(printables.begin(), printables.end(), std::string(), [](const std::string& acc, const std::unique_ptr<Printable>& printable) {
                return acc + ", " + printable->getJSON();
            });
            std::string buffer = R"({"name": ")" + name + R"(", "status": ")" + CommonLib::statusString(status) + R"(", "numPassing": )"
                    + std::to_string(numPassing) + R"(, "numTotal": )" + std::to_string(numTotal) + R"(, "timeTaken": )" +
                    std::to_string(timeTaken.count()) + R"(, "printables": [)" + acc.substr((printables.empty() ? 0 : 1), acc.size() - (printables.empty() ? 0 : 1)) + "]}";
            return buffer;
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
        size_t groupNum;
    public:
        Test(T Data, T Expected, std::string Message, size_t group = 0) {
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
                state = (this->data - lowerLimit <= this->expected && this->data + upperLimit >= this->expected) || // todo, fix
                        CommonLib::isEqual(this->expected, this->data, false);
                result = CommonLib::getStringResultOnSuccess(this->data, this->expected, this->message, state, 1, loc, ogFunction);
            }
            catch (std::exception &e) {
                result = "Exception Thrown: " + std::string(e.what()) + " | " + this->message;
            }
            Result res(result, state, this->groupNum, 1);
            res.updateInternal(this->data, this->expected, state, loc, ogFunction);
            return res;
        }
    public:

        /**
         * @brief Range constructor where upperLimit and lowerLimit are range away from 0
         * @param data Actual data
         * @param expected Expected data
         * @param range Range of the limit from 0, + or -
         */
        TestFloat(T data, U expected, double range, std::string message = "", size_t group = 0) : Test<T, U>(data, expected, message, group) {
            upperLimit = range;
            lowerLimit = range;
        }

        /**
         * @brief Range constructor where upperLimit and lowerLimit are set
         * @param data Actual data
         * @param expected Expected data
         * @param lLimit Lower limit
         * @param uLimit Upper limit
         */
        TestFloat(T data, U expected, double lLimit, double uLimit, std::string message = "", size_t group = 0) : Test<T, U>(data, expected, message, group) {
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
        size_t groupNum;
        bool throwOnAlias = false;
    public:

        explicit VectorTest(std::vector<T> Expected, std::string Message = "", std::vector<std::string> Messages = {}, size_t group = 0, bool aliasThrow = false) {
            expected = Expected;
            messages = std::move(Messages);
            message = std::move(Message);
            groupNum = group;
            throwOnAlias = aliasThrow;
        }

        explicit VectorTest(std::string Message = "", std::vector<std::string> Messages = {}, size_t group = 0) {
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
        signed_size_t from;
        signed_size_t to;

    protected:
        template<typename Callable, typename... Args>
        std::vector<Result> RunAllArgs(const std::source_location loc, std::string ogFunction, Callable &method, Args... args) {
            int index = 0;
            for (signed_size_t i = from; i <= to; i++) {
                bool state = false;
                std::string result;
                U expected;
                try {

                    auto value = std::invoke(method, i, args...);
                    if (this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        result = CommonLib::getStringResultOnSuccess(CommonLib::toString(value),
                                                                     this->expected, this->message, true, i, loc, ogFunction);
                    } else {
                        // if expected is smaller than range, we just use the last value as expected
                        expected = this->expected.at(
                                std::min<unsigned long long>(this->expected.size() - 1, index));
                        state = CommonLib::isEqual(value, expected);
                        result = CommonLib::getStringResultOnSuccess(value, expected, this->message, state, i, loc, ogFunction);
                    }
                    this->results.emplace_back(this->message + " " + result +
                                               (index < this->messages.size() ? ", " + this->messages.at(index) : ""),
                                               state, this->groupNum, index + 1);
                    if (this->expected.empty()) {
                        this->results.at(this->results.size() - 1).updateInternal(value,
                                                                                  "(nothing)",
                                                                                  state,
                                                                                  loc,
                                                                                  ogFunction);
                    }
                    else {
                        this->results.at(this->results.size() - 1).updateInternal(value,
                                                                                  expected,
                                                                                  state,
                                                                                  loc,
                                                                                  ogFunction);
                    }
                }
                catch (std::exception &e) {
                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
                    this->results.emplace_back(this->message + " " + result +
                                               (index < this->messages.size() ? ", " + this->messages.at(index) : ""),
                                               state, this->groupNum, index + 1);
                    this->results.at(this->results.size() - 1).updateInternal("???",
                                                                              expected,
                                                                              this->expected.empty(),
                                                                              loc,
                                                                              ogFunction);
                }

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
        TestRange(signed_size_t From, signed_size_t To, std::string Message = "", std::vector<std::string> Messages = {}, size_t group = 0) : from(From), to(To), VectorTest<U>(Message, Messages, group) {}

        /**
         * @brief Expected constructor
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Expected an std::vector<U> that has the list of expected values for each test, in order
         * @param Message optional message that will print for every result
         * @param Messages optional message that will print for nth result
         */
        TestRange(int From, int To, std::vector<U> Expected, std::string Message = "", std::vector<std::string> Messages = {}, size_t group = 0) : from(From), to(To), VectorTest<U>(Expected, Message, Messages, group) {}

        ~TestRange() = default;

        /**
         * @brief Update the test
         * @param From the integer to start the range from
         * @param To the integer to end the range to
         * @param Message optional message that will print for every result
         * @param Messages optional messages that will print for nth result
         */
        void UpdateTest(signed_size_t From, signed_size_t To, const std::string &Message = "", const std::vector<std::string> &Messages = {}) {
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
        void UpdateTest(signed_size_t From, signed_size_t To, std::vector<U> Expected, const std::string &Message = "", const std::vector<std::string> &Messages = {}) {
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
            return RunAllArgs(std::source_location::current(), "", method, args...);
        }


//        // we have the exact same function here except due to template issues, this one does not have args.
//        // this **will** throw a compile time error if it is used with a function
//        /**
//         * @brief Run all the tests
//         * @param method A callable function, lambda or method
//         * @return A vector of Result with the results
//         *
//         * This will run through all the tests from `from` and to `to`, inclusive.
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
            return RunAllArgs(std::source_location::current(), "(not specified)", method, args...);
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
            return RunAllArgs(std::source_location::current(), "(not specified)", method);
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
        std::vector<Result> RunAll(std::source_location loc = std::source_location::current(), std::string ogFunction = "(not specified)", const std::string &message = "") {
            std::vector<Result> results;
            for (size_t i = 0; i < std::min(actualData.size(), this->expected.size()); i++) {
                try {
                    bool state = RunAt(i);
                    results.emplace_back(CommonLib::getStringResultOnSuccess(actualData.at(i),this->expected.at(i),this->message + (i < this->messages.size() ? ", " + this->messages.at(i) : ""),
                                                                             state, i, loc, ogFunction), state, this->groupNum, i + 1);
                }
                catch (std::exception &exception) {
                    results.emplace_back(message + " " + std::string("Exception thrown: ") + exception.what() + ", " +
                                         (i < this->messages.size() ? ", " + this->messages.at(i) : ""), false,
                                         this->groupNum, i + 1);
                }
                results.at(results.size() - 1).updateInternal(actualData.at(i), this->expected.at(i),
                                                              results.at(results.size() - 1).state,
                                                              loc,
                                                              ogFunction);
            }
            return results;
        }

    public:

        /**
         *
         * @param aData Actual Data
         * @param eData Expected Data
         */
        explicit TestType(std::vector<T> aData = {}, std::vector<U> eData = {}, std::string Message = "", std::vector<std::string> Messages = {}, int group = 0, bool aliasThrow = false) : actualData(aData), VectorTest<U>(eData, Message, Messages, group, aliasThrow) {}

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
        bool RunAt(size_t i) {
            if (i >= actualData.size() || i >= this->expected.size()) {
                return false;
            }
            return CommonLib::isEqual(actualData.at(i), this->expected.at(i), this->throwOnAlias);
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
        std::vector<Result> RunAllArgs(std::source_location loc, std::string ogFunction, Callable &method, Args... args) {
            for (size_t i = 0; i < actual.size(); i++) {
                bool state = false;
                std::string result;
                try {
                    const auto invokeResult = std::invoke(method, actual.at(i), args...);
                    if (this->expected.empty()) { // meaning that we are now only checking essentially if it throws an exception or not
                        result = CommonLib::getStringResultOnSuccess("No exception thrown", "(nothing)", this->message, true, i, loc);
                    } else {
                        state = CommonLib::isEqual(invokeResult, this->expected.at(std::min<size_t>(this->expected.size() - 1, i)));
                        result = CommonLib::getStringResultOnSuccess(actual.at(i), this->expected.at(i), this->message, state, i, loc);
                    }
                    this->results.emplace_back("For " + (i < this->messages.size() ? ", " + this->messages.at(i) : "") + ", " + result, state, this->groupNum, i + 1);
                    if (this->expected.empty()) {
                        this->results.at(this->results.size() - 1).updateInternal(invokeResult,
                                                                                   "(nothing)",
                                                                                   state,
                                                                                   loc,
                                                                                   ogFunction);
                    }
                    else {
                        this->results.at(this->results.size() - 1).updateInternal(invokeResult,
                                                                                  this->expected.at(this->expected.size() - 1),
                                                                                  state,
                                                                                  loc,
                                                                                  ogFunction);
                    }
                }
                catch (std::exception &e) {
                    result = "Exception Thrown: " + std::string(e.what()) + " on " + std::to_string(i);
                }

            }

            return this->results;
        }

    public:
        TestTwoVector(std::vector<T> Actual, std::vector<U> Expected, std::string Message = "", std::vector<std::string> Messages = {}, int group = 0, bool aliasThrow = false) : actual(Actual), VectorTest<U>(Expected, Message, Messages, group, aliasThrow) {}

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

        size_t groupNum = 1;

        std::mutex resultMutex; // lock for adding currentTestResult to results
        std::mutex testResultMutex; // lock for adding to current/defaultTestResult
        std::mutex settingsMutex; // lock for changing settings



        std::map<TesterSettings, bool> settingsMap;

        // gets the next group number for the calling function
        // every call is a new group
        size_t getNextGroupNum() {
            return groupNum++;
        }

        [[nodiscard]] std::chrono::duration<double> getDuration(const auto &start) const {
            const auto end{std::chrono::steady_clock::now()};
            const std::chrono::duration<double> taken{end - start};
            return taken;
        }

        template<typename T, typename U>
        std::vector<Result> testType(std::string ogFunction, std::vector<T> actual, std::vector<U> expected, std::string message = "", std::vector<std::string> messages = {}, const std::source_location loc = std::source_location::current()) {
            std::vector<Result> testResults = TestType(actual, expected, message, messages, getNextGroupNum()).RunAll(loc, ogFunction);
            for (const auto& result : testResults) {
                addResult(result);
            }
            return testResults;
        }

        template <typename... Args>
        constexpr bool isPackUsed() {
            return sizeof...(Args) > 0;
        }


        template<typename Callable, typename... Args>
        void testThreadSafe(const std::string& testName, Tester &tester, Callable &method, Args... args) {
            defaultTestResult = std::move(currentTestResult);
            currentTestResult = std::make_unique<TestResult>(testName);
            if (settingsMap[PRINT_SYNC]) {
                std::cout << currentTestResult->toString(false) << '\n';
            }
            const auto start{std::chrono::steady_clock::now()};
            try {
                std::invoke(method, tester, args...);
            }
            catch (std::exception &e) {
                tester.addMessage("Test ended prematurely, exception thrown: " + std::string(e.what()), FAIL);
                tester.setStatus(FAILURE_EARLY);
            }
            const auto end{std::chrono::steady_clock::now()};
            const std::chrono::duration<double> taken{end - start};
            currentTestResult->updateTime(taken);
            results.emplace_back(std::move(currentTestResult));
            currentTestResult = std::move(defaultTestResult);
        }

        void addResult(const Result &res) {
            std::lock_guard<std::mutex> resLock(testResultMutex);
            currentTestResult->addPrintable(std::make_unique<Result>(res));
            if (settingsMap[PRINT_SYNC]) {
                std::cout << res.getMessage(false) << '\n';
            }
            currentTestResult->giveResultsState(res.state);
        }

        std::string getNextEmptyLabel() const {
            return std::to_string(results.size() + 1);
        }

    public:

        Tester() = default;

        ~Tester() = default;


/**
         * @brief Tests one comparison using operator==. Will automatically put into results.
         * @tparam T The type of data that you are testing
         * @tparam U The type of data that you are expecting
         * @param actual The actual data
         * @param expected The expected data
         * @param message a message to append to the result
         * @return A Result object containing the results of the test
         */
        template<typename T, typename U>
        Result testOne(T actual, U expected, std::string message = "", const std::source_location loc = std::source_location::current()) {
            const auto start{std::chrono::steady_clock::now()};
            try {
                bool state = CommonLib::isEqual(actual, expected, settingsMap[THROW_ON_ALIAS]);

                std::chrono::duration<double> timeTaken = getDuration(start);
                std::string args = CommonLib::getStringResultOnSuccess(actual, expected, message, state, 1, loc,
                                                                       "testOne(" + std::string(CommonLib::type_name<T>()).substr(22) + " actual = " + CommonLib::toString(actual) + ", " +
                                                                                 std::string(CommonLib::type_name<U>()).substr(22) + " expected = " + CommonLib::toString(expected) +
                                                                                 ", std::string message = \"" + message + "\")");
                std::vector<Error> errors;
                if (CommonLib::toString(expected) == CommonLib::toString(actual) && !state) {
                    errors.emplace_back("\t\tNote this test ^ may show the same address due to compiler optimizations", 1);
                }
                Result res{args, state, getNextGroupNum(), 1, errors};
                res.updateTimeTaken(timeTaken);


                if (settingsMap[THROW_ON_FAIL] && !state) {
                    std::lock_guard<std::mutex> resLock(testResultMutex);
                    currentTestResult->setStatus(FAILURE_EARLY);
                    throw TestException("\033[38;2;255;0;0mTest failed when no fails were allowed\033[0m\n", std::make_unique<Result>(res));
                }

                addResult(res);
                return res;
            }
            catch (std::exception &exception) { // todo: handle exceptions better
                if (settingsMap[THROW_ON_FAIL] || settingsMap[THROW_ON_ALIAS] || settingsMap[THROW_ON_ERROR]) {
                    throw TestException(exception.what());
                }

                std::chrono::duration<double> timeTaken = getDuration(start);

                std::string args = "Test #" + std::to_string(1) + std::string("\nException thrown: ") + exception.what() +
                                   (!message.empty() ? " | Message: " + message : "");
                Result res{args, false, getNextGroupNum(), 1};

                res.updateTimeTaken(timeTaken);

                addResult(res);

                return {args, false, getNextGroupNum(), 1};
            }
        }

        /**
         * Tests whether actual is true.
         * @param actual the actual value
         * @param message the message to append
         * @return A Result object containing the results of the test
         */
        Result testTrue(bool actual, std::string message = "", const std::source_location loc = std::source_location::current()) {
            return testOne(actual, true, std::move(message), loc);
        }

        /**
         * Tests whether actual is false.
         * @param actual the actual value
         * @param message the message to append
         * @return A Result object containing the results of the test
         */
        Result testFalse(bool actual, std::string message = "", const std::source_location loc = std::source_location::current()) {
            return testOne(actual, false, std::move(message), loc);
        }


        /**
         * @brief Test floating point number with imprecision leniency
         * @tparam T A floating point number
         * @tparam U A floating point number
         * @param actual A floating point number that is the actual result
         * @param expected A floating point number to compare against the actual
         * @param range Range of the limit from 0 (equal upper and lower bound)
         * @param message A message appended to the result
         * @return A Result
         */
        template<typename T, typename U>
        Result testFloat(T actual, U expected, double range, std::string message = "", const std::source_location loc = std::source_location::current()) {
            const auto start{std::chrono::steady_clock::now()};
            Result res = TestFloat(actual, expected, range, message, getNextGroupNum()).Run(loc,
                         "testFloat(" + std::string(CommonLib::type_name<T>()).substr(22) + " actual = " + CommonLib::toString(actual) + ", " +
                         std::string(CommonLib::type_name<U>()).substr(22) + " expected = " + CommonLib::toString(expected) + ", double range = " +
                         std::to_string(range) + ", std::string message = \"" + message + "\")");
            const auto end{std::chrono::steady_clock::now()};
            const std::chrono::duration<double> taken{end - start};
            res.updateTimeTaken(taken);

            addResult(res);
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
            const auto start{std::chrono::steady_clock::now()};
            Result res = TestFloat(actual, expected, lowerBound, upperBound, message, getNextGroupNum()).Run(loc,
                         "testFloat(" + std::string(CommonLib::type_name<T>()).substr(22) + " actual = " + CommonLib::toString(actual) + ", " +
                         std::string(CommonLib::type_name<U>()).substr(22) + " expected = " + CommonLib::toString(expected) +
                         ", double lowerBound = " + std::to_string(lowerBound) +
                         ", double upperBound = " + std::to_string(upperBound) +
                         ", std::string message = \"" + message + "\")");
            const auto end{std::chrono::steady_clock::now()};
            const std::chrono::duration<double> taken{end - start};
            res.updateTimeTaken(taken);
            addResult(res);
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
            std::string allMessages;
            for (const auto& m : messages) {
                allMessages += m + ", ";
            }
            allMessages = allMessages.substr(0, 50) + "...";
            return testType("testType(" + std::string(CommonLib::type_name<T>()).substr(22) + " actual = " + CommonLib::toString(actual) + ", " +
                            std::string(CommonLib::type_name<U>()).substr(22) + " expected = " + CommonLib::toString(expected) +
                            ", std::string message = " + message +
                            ", std::vector<std::string> messages = {" + allMessages +
                            "})", actual, expected, message, messages, loc);
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
        std::vector<Result> testRange(long long from, long long to, std::vector<T> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            return testRange(std::source_location::current(), from, to, expected, message, messages, method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> testRange(long long from, long long to, Callable &method, Args... args) {
            return testRange(std::source_location::current(), from, to, std::vector<long long>{}, "", {}, method, args...);
        }

        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testRange(long long from, long long to, std::vector<T> expected, Callable &method, Args... args) {
            return testRange(std::source_location::current(), from, to, expected, "", {}, method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> testRange(long long from, long long to, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            return testRange(std::source_location::current(), from, to, std::vector<int>{}, message, messages, method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> testRange(std::source_location loc, long long from, long long to, Callable &method, Args... args) {
            return testRange(loc, from, to, std::vector<int>{}, "", {}, method, args...);
        }

        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testRange(std::source_location loc, long long from, long long to, std::vector<T> expected, Callable &method, Args... args) {
            return testRange(loc, from, to, expected, "", {}, method, args...);
        }

        template<typename Callable, typename... Args>
        std::vector<Result> testRange(std::source_location loc, long long from, long long to, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            return testRange(loc, from, to, std::vector<long long>{}, message, messages, method, args...);
        }

        template<typename T, typename Callable, typename... Args>
        std::vector<Result> testRange(std::source_location loc, long long from, long long to, std::vector<T> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            std::string allExpected;
            for (const auto& m : expected) {
                allExpected += CommonLib::toString(m) + ", ";
            }
            allExpected = allExpected.substr(0, 50) + "...";

            std::string allMessages;
            for (const auto& m : messages) {
                allMessages += m + ", ";
            }
            allMessages = allMessages.substr(0, 50) + "...";
            std::string ogFunction = "testRange(long long from = " + std::to_string(from) +
                                     ", long long to = " + std::to_string(to) +
                                     ", std::vector<" + std::string(CommonLib::type_name<T>()).substr(22) +
                                     "> expected = {" + allExpected +
                                     "}, std::string message = " + message +
                                     ", std::vector<std::string> messages = {" + allMessages +
                                     "}, Callable (" + std::string(CommonLib::type_name<Callable>()).substr(22) +
                                     ") = " + CommonLib::toString(method) +
                                     ", Args... (" + std::string(CommonLib::type_name<Args...>()).substr(22) +
                                     ") = ...)";
            std::vector<Result> testResults = TestRange<T>(from, to, expected, message, messages, getNextGroupNum()).RunAllArgs(loc, ogFunction, method, args...);
            for (const auto& result : testResults) {
                addResult(result);
            }
            return testResults;
        }



        /**
         * @brief Function version of the class {@link TestTwoVector}
         *
         * Tests a function where the nth element in the actual vector is used as the 1st argument for the
         * method that is passed through on the RunAll function call. Additional arguments may be passed in as well.
         * All results will then be checked with the nth element in the result vector, if provided. Otherwise,
         * check only for exceptions.
         *
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
            std::vector<Result> testResults = TestTwoVector<T, U>(inputs, expected, message, messages, static_cast<int>(results.size() + 1)).RunAll(loc, "", method, args...);
            for (const auto& result : testResults) {
                addResult(result);
            }
            return testResults;
        }

        template<typename T, typename U, typename Callable, typename... Args>
        std::vector<Result> testTwoVectorMethod(std::vector<T> inputs, std::vector<U> expected, std::string message, std::vector<std::string> messages, Callable &method, Args... args) {
            return testTwoVectorMethod(std::source_location::current(), inputs, expected, message, messages, method, args...);
        }
        // we have to do a lot of copy-pasting due to the fact that we can't simply have default parameters here
        // also we cannot use just messages or just message because then the compiler infers it as Callable, which we do not want
        // in the event that you just want message or messages, make message = std::string() or messages = {}
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
            const auto start{std::chrono::steady_clock::now()};
            try {
                std::invoke(method, args...);
                const auto end{std::chrono::steady_clock::now()};
                const std::chrono::duration<double> taken{end - start};
                Result res{"Did not throw exception.", false, results.size() + 1, 1};
                res.updateTimeTaken(taken);
                addResult(res);
                return res;
            }
            catch (std::exception &e) {
                if (e.what() == exception) {
                    const auto end{std::chrono::steady_clock::now()};
                    const std::chrono::duration<double> taken{end - start};
                    Result res{"Matched exception.", true, results.size() + 1, 1};
                    res.updateTimeTaken(taken);
                    addResult(res);
                    return res;
                }
                const auto end{std::chrono::steady_clock::now()};
                const std::chrono::duration<double> taken{end - start};
                Result res{"Did not match exception. Exception: " + std::string(e.what()), false,results.size() + 1, 1};
                res.updateTimeTaken(taken);
                addResult(res);
                return res;
            }
        }

//        template<typename Callable, typename... Args>
//        void test(Callable &method, Args... args) {
//            test(std::string("(no label ") + getNextEmptyLabel() + ")", method, args...);
//        }



        template<typename Callable, typename... Args>
        void test(const std::string& testName, Callable &method, Args... args) {
            Tester tempTester;
            tempTester.settingsMap = settingsMap;
            tempTester.testThreadSafe(testName, tempTester, method, args...);

            std::lock_guard<std::mutex> resultLock(resultMutex);
            for (auto & result : tempTester.results) {
                std::unique_ptr<TestResult> res = std::move(result);
                results.emplace_back(std::move(res));
            }
            tempTester.results.clear();
        }


        void setStatus(TestResultStatus status) {
            currentTestResult->setStatus(status);
        }

        void addMessage(const std::string& message, MessageType type = MessageType::LOG) {
            currentTestResult->addPrintable(std::make_unique<TestMessage>(message, currentTestResult->getPartOf(), results.size() + 1, type));
        }


        void updateSetting(TesterSettings setting, bool newSetting) {
            std::lock_guard<std::mutex> settingsLock(settingsMutex);
            settingsMap[setting] = newSetting;
        }

        bool getSetting(TesterSettings setting) {
            std::lock_guard<std::mutex> settingsLock(settingsMutex);
            return settingsMap[setting];
        }


        /**
         * @brief Prints the results of the vector results
         */
        void printResults(bool collapse = false, TestFilter filter = TestFilter::BOTH) {
            for (const auto& testRes : results) {
                std::cout << testRes->toString(collapse, filter) << "\n";
            }
            std::cout << currentTestResult->toString(collapse, filter) << std::endl;
        }


        std::string getJSON() {
            std::string acc = R"({ "testResults": [)";

            acc += currentTestResult->toJSON();

            if (!results.empty()) {
                acc += ", ";
                acc += std::accumulate(results.begin(), results.end(), std::string(results.at(0)->toJSON()), [](const std::string& acc, const std::unique_ptr<TestResult> &res) {
                    return acc + ", " + res->toJSON();
                });
            }

            acc += "]}";
            // todo add settings
            return acc;
        }


    };


}

#endif