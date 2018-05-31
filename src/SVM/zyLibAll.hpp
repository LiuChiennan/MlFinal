#pragma once
#include <iostream>			// for std::cin & std::cout
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>			// for check memory leak
#endif
#include <cassert>			// for assert()
#include <type_traits>		// for std::enable_if & more type traits
#include <utility>			// for std::move etc.
// #include <atomic>			// for std::atomic_uint32_t
#include <mutex>			// for std::lock_guard and std::mutex
// #include <set>				// for std::set
// #include <string>			// for std::string
#include <iosfwd>			// for std::ostream & std::istream
// #include <stdexcept>		// for std::logic_error
#include <initializer_list>	// for {...} 
//////////////////////////////////////////////////////////////////////////
namespace zyLib
{
#ifdef _WIN32
#define checkMemoryLeaks _CrtDumpMemoryLeaks
#else
#define checkMemoryLeaks()
#endif

	inline std::ostream & zyOutput()
	{
		return std::cout;
	}
	template<typename type, typename ... types>
	inline std::ostream & zyOutput(const type & content1, types && ... contents)
	{
		std::cout << content1;
		return zyOutput(std::forward<types>(contents)...);
	}
	template<typename ... types>
	inline std::ostream & zyOutputLine(types && ... contents)
	{
		zyOutput(std::forward<types>(contents)...);
		return std::cout << std::endl;
	}
	template<typename ... types>
	inline std::ostream & zyMsgOutput(const char * const title, types && ... contents)
	{
		std::cout << title << ": ";
		return zyOutput(std::forward<types>(contents)...);
	}
	template<typename ... types>
	inline std::ostream & zyMsgOutputLine(const char * const title, types && ... contents)
	{
		zyMsgOutput(title, std::forward<types>(contents)...);
		return std::cout << std::endl;
	}

#define zyFormatOutput(...) zyMsgOutput(#__VA_ARGS__, __VA_ARGS__)
#define zyFormatOutputLine(...) zyMsgOutputLine(#__VA_ARGS__, __VA_ARGS__)
#ifdef _DEBUG
	template<typename ... types>
	inline std::ostream & zyDebugOut(types && ... contents)
	{
		return zyOutput(std::forward<types>(contents)...);
	}
	template<typename ... types>
	inline std::ostream & zyDebugOutLine(types && ... contents)
	{
		return zyOutputLine(std::forward<types>(contents)...);
	}
	template<typename ... types>
	inline std::ostream & zyDebugMsgOut(const char * title, types && ... contents)
	{
		return zyMsgOutput(title, std::forward<types>(contents)...);
	}
	template<typename ... types>
	inline std::ostream & zyDebugMsgOutLine(const char * title, types && ... contents)
	{
		return zyMsgOutputLine(title, std::forward<types>(contents)...);
	}
#define zyDebugFormatOut(...) zyDebugMsgOut(#__VA_ARGS__, __VA_ARGS__)
#ifdef _MSC_VER
#define zyDebugPause() std::system("pause")
#else
#define zyDebugPause()
#endif
#else
#define zyDebugOut(...)
#define zyDebugMsgOut(...)
#define zyDebugOutLine(...)
#define zyDebugMsgOutLine(...)
#endif
#define zySystemPause() std::system("pause")

	//////////////////////////////////////////////////////////////////////////
	template<typename type>
	using val = type;
	template<typename type>
	using crv = const type &;
	template<typename type>
	using lrv = type & ;
	template<typename type>
	using rrv = type && ;
	template<typename type>
	using in = crv<type>;
	template<typename type>
	using out = lrv<type>;
	template<typename type>
	using inout = lrv<type>;
	using Bool = uint8_t;
	template<typename _From, typename _To = _From>
	inline _To copy(const _From & from)
	{
		return from;
	}
#define zyDefaultFunctionsDecl(...)\
	__VA_ARGS__(zyLib::crv<__VA_ARGS__>);\
	__VA_ARGS__(zyLib::rrv<__VA_ARGS__>);\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::crv<__VA_ARGS__>);\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::rrv<__VA_ARGS__>);

#define zyDefaultFunctionsImpl(...)\
	__VA_ARGS__::__VA_ARGS__(zyLib::crv<__VA_ARGS__>) = default;\
	__VA_ARGS__::__VA_ARGS__(zyLib::rrv<__VA_ARGS__>) = default;\
	zyLib::lrv<__VA_ARGS__> __VA_ARGS__::operator=(zyLib::crv<__VA_ARGS__>) = default;\
	zyLib::lrv<__VA_ARGS__> __VA_ARGS__::operator=(zyLib::rrv<__VA_ARGS__>) = default;

#define zyDefaultFunctions(...)\
	__VA_ARGS__(zyLib::crv<__VA_ARGS__>) = default;\
	__VA_ARGS__(zyLib::rrv<__VA_ARGS__>) = default;\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::crv<__VA_ARGS__>) = default;\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::rrv<__VA_ARGS__>) = default;

#define zyDefaultMoveFunctionsDecl(...)\
	__VA_ARGS__(zyLib::rrv<__VA_ARGS__>);\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::rrv<__VA_ARGS__>);

#define zyDefaultMoveFunctionsImpl(...)\
	__VA_ARGS__::__VA_ARGS__(zyLib::rrv<__VA_ARGS__>) = default;\
	zyLib::lrv<__VA_ARGS__> __VA_ARGS__::operator=(zyLib::rrv<__VA_ARGS__>) = default;

#define zyDefaultMoveFunctions(...)\
	__VA_ARGS__(zyLib::rrv<__VA_ARGS__>) = default;\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::rrv<__VA_ARGS__>) = default;

#define zyDefaultMoveFunctionsDelete(...)\
__VA_ARGS__(zyLib::rrv<__VA_ARGS__>) = delete; \
zyLib::lrv<__VA_ARGS__> operator=(zyLib::rrv<__VA_ARGS__>) = delete;

#define zyDefaultCopyFunctionDecl(...)\
	__VA_ARGS__(zyLib::crv<__VA_ARGS__>);\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::crv<__VA_ARGS__>);

#define zyDefaultCopyFunctionsImpl(...)\
	__VA_ARGS__::__VA_ARGS__(zyLib::crv<__VA_ARGS__>) = default;\
	zyLib::lrv<__VA_ARGS__> __VA_ARGS__::operator=(zyLib::crv<__VA_ARGS__>) = default;

#define zyDefaultCopyFunctions(...)\
	__VA_ARGS__(zyLib::crv<__VA_ARGS__>) = default;\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::crv<__VA_ARGS__>) = default;

#define zyDefaultCopyFunctionsDelete(...)\
	__VA_ARGS__(zyLib::crv<__VA_ARGS__>) = delete;\
	zyLib::lrv<__VA_ARGS__> operator=(zyLib::crv<__VA_ARGS__>) = delete;


#define zyGetDecl(...)\
auto get_##__VA_ARGS__() -> zyLib::lrv<decltype(__VA_ARGS__);\
auto get_##__VA_ARGS__() const -> zyLib::crv<decltype(__VA_ARGS__)>;
#define zyGetImpl(type, member)\
auto type::get_##member() -> zyLib::lrv<decltype(member)\
{\
	return member;\
}\
auto type::get_##member() const -> zyLib::crv<decltype(member)>\
{\
	return member;\
}
#define zyGet(...) \
auto get_##__VA_ARGS__() -> zyLib::lrv<decltype(__VA_ARGS__)>\
{\
	return __VA_ARGS__;\
}\
auto get_##__VA_ARGS__() const -> zyLib::crv<decltype(__VA_ARGS__)>\
{\
	return __VA_ARGS__;\
}
#define zyGetInl zyGet

#define zyPimplGetDecl(...)\
auto get_##__VA_ARGS__() -> zyLib::lrv<decltype(this->decltype(*this)::impl->__VA_ARGS__)>;\
auto get_##__VA_ARGS__() const -> zyLib::crv<decltype(this->decltype(*this)::impl->__VA_ARGS__)>;
#define zyPimplGetImpl(type, member)\
auto type::get_##member() -> zyLib::lrv<decltype(this->decltype(*this)::impl->member)>\
{\
	return this->decltype(*this)::impl->member;\
}\
auto type::get_##member() const -> zyLib::crv<decltype(this->decltype(*this)::impl->member)>\
{\
	return this->decltype(*this)::impl->member;\
}

#define zyRefGetDecl(...)\
auto get_##__VA_ARGS__() -> zyLib::lrv<decltype(__VA_ARGS__);
#define zyRefGetImpl(type, member)\
auto type::get_##member() -> zyLib::lrv<decltype(member)\
{\
	return member;\
}
#define  zyRefGet(...)\
auto get_##__VA_ARGS__() -> zyLib::lrv<decltype(__VA_ARGS__)>\
{\
	return __VA_ARGS__;\
}
#define zyPimplRefGetDecl(...)\
auto get_##__VA_ARGS__() -> zyLib::lrv<decltype(this->decltype(*this)::impl->__VA_ARGS__)>;
#define zyPimplRefGetImpl(type, member)\
auto type::get_##member() -> zyLib::lrv<decltype(this->decltype(*this)::impl->member)>\
{\
	return this->decltype(*this)::impl->member;\
}
#define zyRefGetInl zyRefGet
#define zyConstGetDecl(...)\
auto get_##__VA_ARGS__() const -> zyLib::crv<decltype(__VA_ARGS__)>
#define zyConstGetImpl(type, member)\
auto type::get_##member() const -> zyLib::crv<decltype(member)>\
{\
	return member;\
}
#define zyPimplConstGetDecl(...)\
auto get_##__VA_ARGS__() const -> zyLib::crv<decltype(this->decltype(*this)::impl->__VA_ARGS__)>;
#define zyPimplConstGetImpl(...)\
auto type::get_##member() const -> zyLib::crv<decltype(this->decltype(*this)::impl->member)>\
{\
	return this->decltype(*this)::impl->member;\
}

#define zyConstGet(...)\
auto get_##__VA_ARGS__() const -> zyLib::crv<decltype(__VA_ARGS__)>\
{\
	return __VA_ARGS__;\
}
#define zyConstGetInl zyConstGet
#define zyCopyDecl(...)\
auto copy_##__VA_ARGS__() const -> decltype(__VA_ARGS__);
#define zyCopyImpl(type, member)\
auto type::copy_##member() const -> decltype(member)\
{\
	return zyLib::copy(member);\
}
#define zyPimplCopyDecl(...)\
auto copy_##__VA_ARGS__() const -> decltype(this->decltype(*this)::impl->__VA_ARGS__);
#define zyPimplCopyImpl(...)\
auto type::copy_##member() const -> decltype(this->decltype(*this)::impl->member)\
{\
	return zyLib::copy(this->decltype(*this)::impl->member);\
}

#define zyCopy(...)\
auto copy_##__VA_ARGS__() const -> decltype(__VA_ARGS__)\
{\
	return zyLib::copy(__VA_ARGS__);\
}
#define zyCopyInl zyCopy

#define zyPimplSetDecl(...)\
void set_##__VA_ARGS__(zyLib::crv<decltype(this->decltype(*this)::impl->__VA_ARGS__)> __VA_ARGS__);
#define zyPimplSetImpl(type, member)\
void type::set_##member(zyLib::crv<decltype(this->decltype(*this)::impl->member)> member)\
{\
	this->decltype(*this)::impl->member = member;\
}
#define zySetDecl(...)\
void set_##__VA_ARGS__(zyLib::crv<decltype(__VA_ARGS__)> __VA_ARGS__);
#define zySetImpl(type, member)\
void type::set_##member(zyLib::crv<decltype(member)> member)\
{\
	this->member = member;\
}

#define zySet(...)\
void set_##__VA_ARGS__(zyLib::crv<decltype(__VA_ARGS__)> __VA_ARGS__)\
{\
	this->__VA_ARGS__ = __VA_ARGS__;\
}
#define zySetInl zySet
#define zyModify(...)\
zyGet(__VA_ARGS__)\
zyCopy(__VA_ARGS__)\
zySet(__VA_ARGS__)
#define zyModifyInl zyModify
#define zyModifyDecl(...)\
zyGetDecl(__VA_ARGS__)\
zyCopyDecl(__VA_ARGS__)\
zySetDecl(__VA_ARGS__)
#define zyModifyImpl(type, member)\
zyGetImpl(type, member)\
zyCopyImpl(type, member)\
zySetImpl(type, member)
#define zyPimplModifyDecl(...)\
zyPimplGetDecl(__VA_ARGS__)\
zyPimplCopyDecl(__VA_ARGS__)\
zyPimplSetDecl(__VA_ARGS__)
#define zyPimplModifyImpl(type, member)\
zyPimplGetImpl(type, member)\
zyPimplCopyImpl(type, member)\
zyPimplSetImpl(type, member)
	//////////////////////////////////////////////////////////////////////////
// 用于包裹含逗号的整体作为宏的一个参数
#define zyUnity(...) __VA_ARGS__

// 对自我复制的两种处理方式，通过 handle 宏更改之。
#define zySelfAssignIf(rhs, ...) \
if (this != &rhs) {__VA_ARGS__}
#define zySelfAssignAssert(rhs, ...)\
assert(this != &rhs);\
{__VA_ARGS__}
#define zySelfAssignHandle(rhs, ...) zySelfAssignAssert(rhs, __VA_ARGS__)

// pimpl 中，由于指针指向的是不完整类型，所以必须显式定义含有指向不完整类型的指针的类型的所有默认函数，且移动函数必须是 default 的。
#define zyDefaultCtorDecl(...) __VA_ARGS__();
#define zyDefaultDtorDecl(...) ~__VA_ARGS__();
#define zyDefaultCtorImpl(...) __VA_ARGS__::__VA_ARGS__()= default;
#define zyDefaultDtorImpl(...) __VA_ARGS__::~__VA_ARGS__() = default;
#define zyDefaultVirtualDtorDecl(...) virtual zyDefaultDtorDecl(__VA_ARGS__)
#define zyDefaultCtor(...) __VA_ARGS__() = default;
#define zyDefaultDtor(...) ~__VA_ARGS__() = default;
#define zyDefaultVirtualDtor(...) virtual ~__VA_ARGS__() = default;
#define zyPimplDefaultFunctionsDecl(...)\
zyDefaultFunctionsDecl(__VA_ARGS__)\
zyDefaultDtorDecl(__VA_ARGS__)
#define zyPimplDefaultFunctionsImpl(...)\
zyDefaultFunctionsImpl(__VA_ARGS__)\
zyDefaultDtorImpl(__VA_ARGS__)
#define zyPimplDefaultFunctionsVirtualDecl(...)\
zyDefaultFunctionsDecl(__VA_ARGS__)\
zyDefaultVirtualDtorDecl(__VA_ARGS__)
#define zyPimplDefaultFunctionsVirtualImpl(...)\
zyDefaultFunctionsImpl(__VA_ARGS__)\
zyDefaultDtorImpl(__VA_ARGS__)

#define zyGetTypeName(...) typeid(__VA_ARGS__).name()
	template<bool _Test, typename _Ty = void>
	using EnableIf = std::enable_if_t<_Test, _Ty>;

	namespace type_traits_details
	{
		template<typename, template<typename ...> typename, typename ...>
		struct is_able : public std::false_type {};
		template<template<typename ...> typename op, typename ... args>
		struct is_able<std::void_t<op<args...>>, op, args...> : public std::true_type {};
	}
	template<template<typename ...> typename op, typename ...args>
	using is_able = type_traits_details::is_able<void, op, args...>;

	template<typename _Type, typename _IndexType>
	using index_result = decltype(std::declval<_Type>()[std::declval<_IndexType>()]);
	template<typename _Type, typename _IndexType>
	using is_able_index = is_able<index_result, _Type, _IndexType>;
	template<typename _Type, typename _IndexType>
	constexpr bool is_able_index_v = is_able_index<_Type, _IndexType>::value;

	template<typename _Type, typename ... _ParamTypes>
	using call_result = decltype(std::declval<_Type>()(std::forward<_ParamTypes>(std::declval<_ParamTypes>())...));
	template<typename _Type, typename ... _ParamTypes>
	using is_able_call = is_able<call_result, _Type, _ParamTypes...>;
	template<typename _Type, typename ... _ParamTypes>
	constexpr bool is_able_call_v = is_able_call<_Type, _ParamTypes...>;

	template<typename _Type>
	using not_result = decltype(std::declval<_Type>().operator!());
	template<typename _Type>
	using is_able_not = is_able<not_result, _Type>;
	template<typename _Type>
	constexpr bool is_able_not_v = is_able_not<_Type>;

#define bi_op_tmp(name, op)\
template<typename lhs, typename rhs>\
using name##_result = decltype(std::declval<lhs>() op std::declval<rhs>());\
template<typename lhs, typename rhs>\
using is_able_##name = is_able<name##_result, lhs, rhs>;\
template<typename lhs, typename rhs>\
constexpr bool is_able_##name##_v = is_able_##name<lhs, rhs>::value;
	bi_op_tmp(add, +)
		bi_op_tmp(sub, -)
		bi_op_tmp(mul, *)
		bi_op_tmp(div, / )
		bi_op_tmp(and, &&)
		bi_op_tmp(or , || )
		bi_op_tmp(mod, %)
		bi_op_tmp(bit_and, &)
		bi_op_tmp(bit_or, | )
		bi_op_tmp(bit_xor, ^)
		bi_op_tmp(comma, zyUnity(, ))
		bi_op_tmp(eq, =)
		bi_op_tmp(add_eq, +=)
		bi_op_tmp(sub_eq, -=)
		bi_op_tmp(mul_eq, *=)
		bi_op_tmp(div_eq, /=)
		bi_op_tmp(mod_eq, %=)
		bi_op_tmp(bit_and_eq, &=)
		bi_op_tmp(bit_or_eq, |=)
		bi_op_tmp(bit_xor_eq, ^=)
		bi_op_tmp(left_shift, << )
		bi_op_tmp(right_shift, >> )
		bi_op_tmp(left_shift_eq, <<=)
		bi_op_tmp(right_shift_eq, >>=)
		bi_op_tmp(equal, == )
		bi_op_tmp(not_equal, != )
		bi_op_tmp(less_than, < )
		bi_op_tmp(greater_than, > )
		bi_op_tmp(lt_or_eq, <= )
		bi_op_tmp(gt_or_eq, >= )
#undef bi_op_tmp
#define pre_step_op_tmp(name, op)\
template<typename type>\
using name##_result = decltype(op std::declval<type &>());\
template<typename type>\
using is_able_##name = is_able<name##_result, type>;\
template<typename type>\
constexpr bool is_able_##name##_v = is_able_##name<type>::value;
		pre_step_op_tmp(pre_inc, ++)
		pre_step_op_tmp(pre_dec, --)
		pre_step_op_tmp(neg, !)
#undef pre_step_op_tmp
#define post_step_op_tmp(name, op)\
template<typename type>\
using name##_result = decltype(std::declval<type>() op);\
template<typename type>\
using is_able_##name = is_able<name##_result, type>;\
template<typename type>\
constexpr bool is_able_##name##_v = is_able_##name<type>::value;
		post_step_op_tmp(post_inc, ++)
		post_step_op_tmp(post_dec, --)
#undef post_step_op_tmp
#define zyTypeTraitsMemberFunc(func_name, ...)\
template<typename type>\
using func_name##_result = decltype(std::declval<type>().func_name(__VA_ARGS__));\
template<typename type>\
using is_able_##func_name = is_able<func_name##_result, type>;\
template<typename type>\
constexpr bool is_able_##func_name##_v = is_able_##func_name<type>::value;
#define zyTypeTraitsConstMemberFunc(func_name, ...)\
template<typename type>\
using const_##func_name##_result = decltype(std::declval<const type>().func_name(__VA_ARGS__));\
template<typename type>\
using is_able_const_##func_name = is_able<const_##func_name##_result, type>;\
template<typename type>\
constexpr bool is_able_const_##func_name##_v = is_able_const_##func_name<type>::value;
#define zyTypeTraitsPtrMemberFunc(func_name, ...)\
template<typename type>\
using ptr_##func_name##_result = decltype(std::declval<type>()->func_name(__VA_ARGS__));\
template<typename type>\
using is_able_##ptr_##func_name = is_able<ptr_##func_name##_result, type>;\
template<typename type>\
constexpr bool is_able_##ptr_##func_name##_v = is_able_##ptr_##func_name<type>::value;
#define zyTypeTraitsFunc(func_name, ...)\
using func_name##_result = decltype(func_name(__VA_ARGS__));\
using is_able_##func_name = is_able<func_name##_result>;\
constexpr bool is_able_##func_name##_v = is_able_##func_name::value;

		zyTypeTraitsMemberFunc(begin)
		zyTypeTraitsMemberFunc(end)
		zyTypeTraitsConstMemberFunc(begin)
		zyTypeTraitsConstMemberFunc(end)
		zyTypeTraitsMemberFunc(cbegin)
		zyTypeTraitsMemberFunc(cend)


	// 复制构造时自动对内部指向地址执行复制的指针
	template<typename type>
	class AutoCopyPtr : public Object
	{
	private:
		type * ptr;
	public:
		AutoCopyPtr()
			: ptr{ new type } {}
		AutoCopyPtr(type * ptr) : ptr(ptr)
		{
			assert(ptr != nullptr);
		}
		AutoCopyPtr(crv<AutoCopyPtr> rhs)
			: ptr(new type(*(rhs.ptr)))
		{
			// zyDebugOut("copy ctor called");
		}
		AutoCopyPtr(rrv<AutoCopyPtr> rhs)
		{
			ptr = rhs.ptr;
			rhs.ptr = nullptr;
			// zyDebugOut("move ctor called");
		}
		~AutoCopyPtr()
		{
			delete ptr;
		}
		AutoCopyPtr & operator=(crv<AutoCopyPtr> rhs)
		{
			zySelfAssignHandle(rhs, { delete ptr; ptr = new type(*(rhs.ptr)); });
			// zyDebugOut("copy assign called");
			return *this;
		}
		AutoCopyPtr & operator=(rrv<AutoCopyPtr> rhs)
		{
			zySelfAssignHandle(rhs, { delete ptr; ptr = rhs.ptr; rhs.ptr = nullptr; });
			// zyDebugOut("move assign called");
			return *this;
		}
		bool operator==(crv<AutoCopyPtr> rhs) const
		{
			return ptr == rhs.ptr;
		}
		bool operator!=(crv<AutoCopyPtr> rhs) const
		{
			return ptr != rhs.ptr;
		}
		type * operator->()
		{
			return ptr;
		}
		const type * operator->() const
		{
			return ptr;
		}
		type & operator*()
		{
			return *ptr;
		}
		const type & operator*() const
		{
			return *ptr;
		}
		void swap(AutoCopyPtr & rhs)
		{
			std::swap(ptr, rhs.ptr);
		}
		bool isNull() const
		{
			return ptr == nullptr;
		}
		bool notNull() const
		{
			return ptr != nullptr;
		}
		void reset(type * new_ptr)
		{
			delete ptr;
			ptr = new_ptr;
		}
	};


}
#if !defined(ZY_FORCEINLINE)
#  if defined(_MSC_VER)
#    define ZY_FORCEINLINE __forceinline
#  elif defined(__GNUC__) && __GNUC__ > 3
// Clang also defines __GNUC__ (as 4)
#    define ZY_FORCEINLINE inline __attribute__ ((__always_inline__))
#  else
#    define ZY_FORCEINLINE inline
#  endif
#endif

#if !defined(ZY_NOINLINE)
#  if defined(_MSC_VER)
#    define ZY_NOINLINE __declspec(noinline)
#  elif defined(__GNUC__) && __GNUC__ > 3
// Clang also defines __GNUC__ (as 4)
#    if defined(__CUDACC__)
// nvcc doesn't always parse __noinline__,
// see: https://svn.boost.org/trac/boost/ticket/9392
#      define ZY_NOINLINE __attribute__ ((noinline))
#    else
#      define ZY_NOINLINE __attribute__ ((__noinline__))
#    endif
#  else
#    define ZY_NOINLINE
#  endif
#endif
namespace zyLib
{

#if (defined create) || (defined with) || (defined default_value)
	// nothing
#else
#define create std::make_shared<
#define with >
#define default_value ()
#endif

	// 写时复制智能指针
	template<typename type>
	// 发现必须要使用引用计数，不能单凭 copied_flag 判断。因为 自己类的 copied_flag 只能判断自己是否复制了，
	// 不能给其他对象以信号。所以其他对象不知道析构时是否要 delete 持有的指针。
	class COWSharedPtr : protected std::shared_ptr<type>
	{
	public:
		using Super = std::shared_ptr<type>;
		COWSharedPtr() : Super{ create type with default_value } {}
		COWSharedPtr(std::shared_ptr<type> && other)
			: Super{ std::move(other) } {}
		COWSharedPtr(const COWSharedPtr & other) = default;
		COWSharedPtr(COWSharedPtr &&) = default;
		COWSharedPtr & operator=(const COWSharedPtr & other) = default;
		COWSharedPtr & operator=(COWSharedPtr && other) = default;
		using Super::use_count;
		using Super::operator bool;
		ZY_FORCEINLINE auto read() const
		{
			return Super::get();
		}
		auto operator->() const
		{
			return read();;
		}
		auto operator->()
		{
			return write();
		}
		auto write()
		{
			copy();
			return Super::get();
		}
		void copy()
		{
			// 不是自己独有的，就执行复制。另外，可以确定绝对不会指向 nullptr。
			if (!copied())
				Super::operator=(create type with(*Super::get()));
		}
		ZY_FORCEINLINE auto copied() const
		{
			return use_count() == 1;
		}
	};
}
#define zyPimplDefaultFunctionsWithCopyDeleteDecl(...)\
zyDefaultCopyFunctionsDelete(__VA_ARGS__)\
zyDefaultMoveFunctionsDecl(__VA_ARGS__)\
zyDefaultDtorDecl(__VA_ARGS__)

#define zyPimplDefaultFunctionsWithCopyDeleteImpl(...)\
zyDefaultMoveFunctionsImpl(__VA_ARGS__)\
zyDefaultDtorImpl(__VA_ARGS__)

#define zyPimplDefaultFunctionsVirtualWithCopyDeleteDecl(...)\
zyDefaultCopyFunctionsDelete(__VA_ARGS__)\
zyDefaultMoveFunctionsDecl(__VA_ARGS__)\
zyDefaultVirtualDtorDecl(__VA_ARGS__)

#define zyPimplDefaultFunctionsVirtualWithCopyDeleteImpl(...)\
zyDefaultMoveFunctionsImpl(__VA_ARGS__)\
zyDefaultDtorImpl(__VA_ARGS__)


// 性能测试对比用代码

//auto begin2 = std::chrono::system_clock::now();
//std::shared_ptr<std::string> std_ptr;
//for (int i = 0; i < 1000000; ++i)
//{
//	std_ptr = std::make_shared<std::string>("123123123");
//	auto c = std_ptr->operator[](3);
//}
//auto end2 = std::chrono::system_clock::now();
//zyOutputLine((end2 - begin2).count());
//auto begin1 = std::chrono::system_clock::now();
//ptr<String> my_ptr;
//for (int i = 0; i < 1000000; ++i)
//{
//	my_ptr = "123123123";
//	auto c = my_ptr[3];
//}
//auto end1 = std::chrono::system_clock::now();
//zyOutputLine((end1 - begin1).count());
