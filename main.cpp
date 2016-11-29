#include <iostream>

using namespace std;

class A{
public:
    virtual string gen_key(){
        return "i am A";
    }
    void print(){
        cout<< gen_key() << endl;
    }
};

class B : public A{
public:
    virtual string gen_key(){
        return "i am B";
    }
};

int main(){
    B b;
    b.print();

    return 0;
}