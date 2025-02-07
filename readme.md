## Simple C++ Tester Library
This library allows you to easily test C++ code in a variety of different ways. 

To get started, simply add `#include "tester.h"` into your code, and either `using namespace TesterLib` or by explicitly
writing `TesterLib::` for every function, you can start to use all of what the tester library has to offer. 

**Make sure to use C++23 or above when compiling!
The library is designed to use modern C++ to reduce code 
complexity and encourage adoption of newer standards :).**

## Documentation
Instead of reading this readme, you can also read all the examples in `examples/`. You should start with testOne.cpp, it'll teach
you the basics of using this library. 

## How to use
There are two main ways to start using the tester library. You can either create an object of every different class test
that you would like to use, or simply create a new `TesterLib::Tester` object, which allows you to access all the 
other classes' functionality with methods that also allow you to print out the results in a neat format. 

To get started, create a `TesterLib::Tester` object.
```c++
TesterLib::Tester tester;
```
With that, you will have access to all the testing capabilities that it has to offer. Let's try to compare two values for now.
```c++
TesterLib::Tester tester;
tester.testOne(1, 2); 
tester.printResults();
```
Great! `tester.testOne(1, 2)` will return a new `Result` object that can be used on its own, but it will also be added
to the list of Results that `tester` has. Calling `tester.printResults()` will display:
```shell
(default) | 0/1 passed | Status: SUCCESS in 0.000000sec
----------------------------------------------------------
|- Group 1 | Test 1 | Result: false in 0.000005sec  | Test 1 Failure
|       was:  1         expected:  2
|       was: int        expected type: int
|       at: C:\Users\poly\cpptester\main.cpp:21
|       called in: int main()
|       as: testOne(int actual = 1, int expected = 2, std::string message = "")
|
```
The group number here means the nth method call that you called upon the `tester` object. Meaning that everytime you
call a method on `tester`, group number will increase. The test number indicates which test number of the group it is 
displaying. Many functions can run multiple tests at once, so they'll show multiple tests per group, like:
```c++
tester.testType(std::vector<int>{1, 2, 3}, std::vector<int>{1, 3, 3});
// testType will check the nth element in the first parameter and check if
// it is equal to the nth element in the second parameter
// If you are putting in the constructor right in the method parameter
// make sure to specify the type so that C++ can infer the type for the template.
// In this case, we simply add std::vector<int>, where int is the type we want to test.
```
Which then calling `tester.printResults()`
```shell
(default) | 2/3 passed | Status: SUCCESS in 0.000000sec
----------------------------------------------------------
|- Group 1 | Test 1 | Result: true in 0.000000sec  | Test 0 Success
|       was:  1         expected:  1
|       was: int        expected type: int
|       at: C:\Users\poly\cpptester\main.cpp:21
|       called in: int main()
|       as: testType(int actual = *0x5ae21ff430, int expected = *0x5ae21ff310, std::string message = , std::vector<std::string> messages = {...})
|
|- Group 1 | Test 2 | Result: false in 0.000000sec  | Test 1 Failure
|       was:  2         expected:  3
|       was: int        expected type: int
|       at: C:\Users\poly\cpptester\main.cpp:21
|       called in: int main()
|       as: testType(int actual = *0x5ae21ff430, int expected = *0x5ae21ff310, std::string message = , std::vector<std::string> messages = {...})
|
|- Group 1 | Test 3 | Result: true in 0.000000sec  | Test 2 Success
|       was:  3         expected:  3
|       was: int        expected type: int
|       at: C:\Users\poly\cpptester\main.cpp:21
|       called in: int main()
|       as: testType(int actual = *0x5ae21ff430, int expected = *0x5ae21ff310, std::string message = , std::vector<std::string> messages = {...})
|

```
Now you can see that since `tester.testType` was one method call, it counts as one group number, while the nth element
or test counts as the test number, hence why we have 3.

That is pretty much how the entire library works!
All the functions and classes have detailed descriptions on what they do, along with the template arguments,
parameters and return values are written in the code.

If you want to see a detailed use of the library, see `reference.md`