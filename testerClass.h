
#ifndef CPPTESTER_TESTERCLASS_H
#define CPPTESTER_TESTERCLASS_H
#include "tester.h"

namespace TesterLib {
    class TestSuite : public Tester {
    protected:

        /**
         * Setup function as to be defined by the inheriting class.
         * Run once before every runTest call.
         */
        virtual void setup() {};


        /**
         * In your run override, call runTest for every method you have created in your suite.
         * @tparam ObjRef A pointer type of the object instance of the non static methods you want to call. Put `this`.
         * @tparam Callable The function you want to call.
         * @tparam Args Arguments after Tester...
         * @param testName The name of the test
         * @param ref A pointer to the object instance, just put `this`.
         * @param callable The function you want to call. Put `&ClassName::methodName`
         * @param args Arguments that come after the Tester argument.
         */
        template<typename ObjRef, typename Callable, typename ...Args>
        void runTest(const std::string& testName, ObjRef ref, Callable callable, Args... args) {
            setup();
            testWithObject(testName, ref, callable, args...);
        }

    public:

        /**
         * This is where you will "register" all your test methods.
         * Call runTest for every method you have in your inheriting class.
         * Something like this
         * `runTest("Test for Object Equality", this, &MyClass::testEquality)`
         */
        virtual void run() {};


    };
}

#endif //CPPTESTER_TESTERCLASS_H
