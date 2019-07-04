// opt.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <iostream>

class A : public OptBase
{
public:
    void Print()
    {
        std::cout << mText.c_str();
    }

opt_member(A, OptBase):
    void _init_(InitType type)
    {
        if(type == InitType::Create)
            mText = "Hello World!-1\n";
    }
    void _quit_()
    {
        std::cout << "Bye!\n";
    }
    void _move_(A&& rhs)
    {
        mText = OPT_Move(rhs.mText);
    }
    void _copy_(const A& rhs)
    {
        mText = rhs.mText;
    }
    opt_init(const char* text)
    {
        mText = text;
    }
    std::string mText;
};

class B : public OptBase
{
public:
    void Print()
    {
        std::cout << mText.c_str();
    }

opt_member(B, OptBase, mText("Hello World!-3\n")):
    const std::string mText;
};

int main()
{
    A a;
    a.Print();

    A b("Hello World!-2\n");
    b.Print();

    B c;
    c.Print();

    a = b;
    a.Print();
}
