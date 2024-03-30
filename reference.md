## `testOne(T actual, U expected, string message = "")`
Tests actual against expected using the `==` operator. Returns a `Result`.
Message appends to the result, but is optional.
```c++
tester.testOne(1, 2);
// --> Result("Test #1 Failure", false, 0, 1)
```

## `testFloat(T actual, U expected, double range, string message = "")`
Tests actual against expected using > and < for floating point numbers. Range is the leniency of floating
point precision error to have. Message appends to the result, but is optional.
```c++
tester.testFloat(0.001, 0.002, 0.001);
// --> Result("Passed", true, 0, 1)
```

## `testFloat(T actual, U expected, double lowerBound, double upperBound, string message = "")`
Same as testFloat above, but with specified lower and upper bound. 
```c++
tester.testFloat(0.001, 0.002, -0.001, 0);
// --> Result("Failed", false, 0, 1)
```

## `testType(vector<T> actual, vector<U> expected, string message, vector<string> messages)`
Tests two vectors of type T and type U, and runs a test for each vector. `Message` appends to all tests, and nth element
of `Messages` appends to nth test.
```c++
tester.testType(vector<int>{1, 2, 3}, vector<int>{2, 2, 3});
// --> vector{
//     Result("Failure", false, 1, 1)
//     Result("Success", true, 1, 2)
//     Result("Success", true, 1, 3)
//     }
```

## `testRange(int from, int to, vector<T> expected, string message, vector<string> messages, Callable method, Args... args)`
**Overloaded variants**

`testRange(int from, int to, Callable method, Args... args)`

`testRange(int from, int to, vector<T> expected, Callable method, Args... args`

`testRange(int from, int to, string message, vector<string> messages, Callable method, Args.. args`

`testRange(int from, int to, vector<T> expected, Callable method, string message = "", vector<string> messages = {})`

Goes from `from` to `to` in a loop and calls the function `method` that is put in, with the current index put in as the 
first argument for that function. If there is no overload that you want, you may want to put in `""` and `{}` for an empty
`message` and empty `messages`, respectively. You also do not need to put extra arguments, see the last overload. If you do not put in an expected,
the method will check if it does or doesn't throw an exception, noting it in the results. 
```c++
int add(int val1, int val2) {
    return val1 + val2;
}
tester.testRange(1, 3, vector<int>{2,4,4}, add, 2);
// --> vector{
//     Result("Failed: 1", false, 1, 1)
//     Result("Passed: 2", true, 1, 2)
//     Result("Failed: 3", false, 1, 3)
//     }
```
*Common Problems:* Sometimes template issues may arise when using this function. Make sure that the number of arguments match both the type
and the number of arguments in the passed-in Callable. Remember that this function will already put in an `int` for the first argument, so 
make sure that your Callable will have a parameter of type `int` for the first argument.

## `testTwoVectorMethod(vector<T> inputs, vector<U> expected, string message, vector<string> messages, Callable method, Args... args)`
**Overloaded variants**

`testTwoVectorMethod(vector<T> inputs, vector<U> expected, Callable method, Args... args)`

`testTwoVectorMethod(vector<T> inputs, Callable method, Args... args)`

`testTwoVectorMethod(vector<T> inputs, Callable method, vector<U> expected = {}, string message = "", vector<string> messages = {})`

Goes through the inputs list and puts it in as the first argument in the `method`. If supplied, it will then check with an `expected` vector to
check if the output is what is desired. If not, then it will check if it throws an exception or not. If there is no overload you want, you may
want to put in `""` and `{}` for an empty `message` and empty `messages`, respectively. You also do not need to put extra arguments, see the last
overload. 
```c++
int add(int val1, int val2) {
    return val1 + val2;
}
tester.testTwoVectorMethod(vector<int>{2, 7, 100}, 
                           vector<int>{3, 4, 101}, "", {"hi mom", "hey mom", "wow!"}, add, 1);
// --> vector{
//     Result("Passed: 0, hi mom", true, 1, 1)
//     Result("Failed: 1, hey mom", false, 1, 2)
//     Result("Passed: 2, wow!", true, 1, 3)
//     }
```
*Common Problems:* Sometimes template issues may arise when using this function. Make sure that the number of arguments match both the type
and the number of arguments in the passed-in Callable. Remember that this function will already put in an `typename T` for the first argument, so
make sure that your Callable will have a parameter of type `typename T` for the first argument, meaning that the type of the vector of inputs
must match the first type of the parameter of the Callable that you are passing in.

## `testException(string exception, string message, Callable method, Args... args)`
Tests if the string value of an exception thrown by a `Callable` (with optional arguments supplied) will throw the same exception as supplied.
If no exception is thrown or the exception does not match the string, then it will fail. It does *not* check by exception type as `std::exception`
does not have `operator==` overloaded. 
```c++
    int exceptioner(int v) {
        if(v > 10) {
            throw std::out_of_range("too much!");
        }
        return v;
    }
    tester.testException("too much!", "", exceptioner, 11);
// --> Result("Matched exception.", true, 1, 1)
```

## `printResults()`
Prints all results of a `Tester` object.
```shell
// Example output after some tests:
// tester.printResults();
Test Results: (3/4) passed.
(1)  Group 1, Test 1    Result:  true  | Message: Matched exception.
(2)  Group 2, Test 1    Result:  true  | Message:  Passed: 0, hi mom
(3)  Group 2, Test 2    Result:  false  | Message:  Failed: 1, hey mom
(4)  Group 2, Test 3    Result:  true  | Message:  Passed: 2, wow!
```

## `printResults(bool showPassing)`
Prints results that are passing (true) or failing (false).
```shell
// Same examples as above
// tester.printResults(true);
Test Results: (3/4) passed.                                           
(1)  Group 1, Test 1    Result:  true  | Message: Matched exception.  
(2)  Group 2, Test 1    Result:  true  | Message:  Passed: 0, hi mom  
(4)  Group 2, Test 3    Result:  true  | Message:  Passed: 2, wow!    
```

## `printTest(int testNumber)`
Prints just the test number (which is labeled on the left by a number in parentheses).
```shell
// Same examples as above
// tester.printTest(2);
Test Results: (3/4) passed. Showing only Test #2                             
(2)  Group 2, Test 1    Result:  true  | Message:  Passed: 0, hi mom  
```

## `printGroup(int groupNumber)`
Prints just the group number (which is labeled by Group # for every test).
```shell
// Same examples as above
// tester.printGroup(2);
Test Results: (2/3) passed. Showing only Group #2                            
(2)  Group 2, Test 1    Result:  true  | Message:  Passed: 0, hi mom  
(3)  Group 2, Test 2    Result:  false  | Message:  Failed: 1, hey mom
(4)  Group 2, Test 3    Result:  true  | Message:  Passed: 2, wow!    
```

## `getResults()`
Returns the result vector vector (std::vector<std::vector<Result>>)