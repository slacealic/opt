#pragma once

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || \
    defined(__ppc64__) || defined(_WIN64) || defined(__LP64__) || defined(_LP64)
    #define OPT_PtrToUint64(PTR)  (*((const unsigned long long*) &(PTR)))
#else
    #define OPT_PtrToUint64(PTR)  ((const unsigned long long) *((const unsigned int*) &(PTR)))
#endif

#if 1
    #define OPT_Assert(QUERY) do{if((QUERY) == 0) __debugbreak();} while(0)
#else
    #define OPT_Assert(QUERY) do{} while(0)
#endif

template<class TYPE>
struct _OPT_Move {typedef TYPE type;};
template<class TYPE>
struct _OPT_Move<TYPE&> {typedef TYPE type;};
template<class TYPE>
struct _OPT_Move<TYPE&&> {typedef TYPE type;};

template<typename TYPE>
inline typename _OPT_Move<TYPE>::type&& OPT_Move(TYPE&& param)
{return static_cast<typename _OPT_Move<TYPE>::type&&>(param);}

class Opt
{
protected:
    typedef void (*destructor)(Opt*);
protected:
    Opt(destructor q) {quit__ = q;}
    ~Opt() {quit__(this);}
private:
    destructor quit__;
};

#define opt_member(CLASS, PARENT, ...) \
    private: \
    typedef CLASS _self_; \
    typedef PARENT _super_; \
    protected: \
    _self_(destructor quit) : _super_(quit), __VA_ARGS__ {} \
    public: \
    _self_() : _super_(_quit__), __VA_ARGS__ {_init_(InitType::Create);} \
    _self_(_self_&& rhs) : _super_(_quit__), __VA_ARGS__ {_move_(OPT_Move(rhs)); rhs._init_(InitType::MoveAfter);} \
    _self_(const _self_& rhs) : _super_(_quit__), __VA_ARGS__ {_copy_(rhs);} \
    ~CLASS() {} \
    _self_& operator=(_self_&& rhs) {_quit_(); _move_(OPT_Move(rhs)); rhs._init_(InitType::MoveAfter); return *this;} \
    _self_& operator=(const _self_& rhs) {_quit_(); _copy_(rhs); return *this;} \
    private: \
    static void _quit__(Opt* opt) {((_self_*) opt)->_quit_();} \
    protected

class OptBase : public Opt
{
protected:
    enum class InitType {Create, MoveAfter};
opt_member(OptBase, Opt):
    void _init_(InitType type) {} // 모든 멤버의 생성자 호출직후에 굳이 따로 처리해야 하는 일
    void _quit_() {} // 모든 멤버의 소멸자 호출직후에 굳이 따로 처리해야 하는 일
    void _move_(_self_&& rhs) {} // _move_() 직후에 rhs._init_(true)가 호출, 직전에 _quit_()가 선택적 호출
    void _copy_(const _self_& rhs) {OPT_Assert(false);} // _copy_() 직전에 _quit_()가 선택적 호출
};

// 특수생성자1 : 부모는 일반생성자
// → opt_special_init(A a, B b)
// → opt_special_init(A a, B b), c(a)
#define opt_special_init(...) \
    public: \
    _self_(__VA_ARGS__, destructor quit = _quit__) : _super_(quit)

// 특수생성자2 : 부모도 특수생성자
// → opt_special_init_(A a, B b)_with_super(a, b)
// → opt_special_init_(A a, B b)_with_super(a, b), c(a)
#define opt_special_init_(...) \
    public: \
    _self_(__VA_ARGS__, destructor quit = _quit__) : _super_
#define _with_super(...) \
    (__VA_ARGS__, quit)
