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
    _self_(destructor quit) : _super_(quit) {} \
    public: \
    _self_() : _super_(_quit__), __VA_ARGS__ {_init_();} \
    _self_(_self_&& rhs) : _super_(_quit__), __VA_ARGS__ {_move_(OPT_Move(rhs)); rhs._init_();} \
    _self_(const _self_& rhs) : _super_(_quit__), __VA_ARGS__ {_copy_(rhs);} \
    ~CLASS() {} \
    _self_& operator=(_self_&& rhs) {_quit_(); _move_(OPT_Move(rhs)); rhs._init_(); return *this;} \
    _self_& operator=(const _self_& rhs) {_quit_(); _copy_(rhs); return *this;} \
    private: \
    static void _quit__(Opt* opt) {((_self_*) opt)->_quit_();} \
    protected

#define opt_init_override(...) \
    public: \
    _self_(__VA_ARGS__) : _super_(_quit__)

class OptBase : public Opt
{
opt_member(OptBase, Opt):
    void _init_() {}
    void _quit_() {}
    void _move_(OptBase&& rhs) {}
    void _copy_(const OptBase& rhs) {OPT_Assert(false);}
};
