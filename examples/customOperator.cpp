#include "../tester.h"

using namespace TesterLib;

template<class T>
class SomeListAbstraction {
private:
    std::vector<T> vec;
public:
    explicit SomeListAbstraction(std::vector<T> &v) {
        vec = v;
    }
    // do whatever with this class...

    // ... and then create a toString (Java style)
    std::string toString() {
        return "a list of some type and size " + std::to_string(vec.size()) + "\n";
    }

    // ... or create an insertion operator overload for ostreams (C++ style)
    friend std::ostream& operator<< (std::ostream& stream, SomeListAbstraction<T> &item) {
        stream << "A list of some type and size " << item.vec.size() << "\n";
        return stream;
    }
    // use whatever you feel like!
    // the tester library will use either one, with priority for ostream& operator<<
    // if your class has none of the above, it will simply print the address of your object,
    //   which often isn't very helpful

    // we also need some sort of way to compare this class with something else...
    // we don't care about the result, so we'll keep it false, but make sure you care about the result!
    bool operator==(const SomeListAbstraction<T> &other) {
        return false;
    }
};

int main() {

    Tester tester;

    tester.updateSetting(THROW_ON_FAIL, false);

    std::vector<int> myVec1 = {1, 2, 3, 4, 5};
    std::vector<std::string> myVec2 = {"str1", "str2", "str3..."};



    SomeListAbstraction<int> list1(myVec1);
    SomeListAbstraction<std::string> list2(myVec2);


    tester.testOne(list1, list2);

    tester.printResults();
}