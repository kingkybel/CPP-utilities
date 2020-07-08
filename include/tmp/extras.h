
#ifndef EXTRAS_H
#define EXTRAS_H


void doSomething(char c, int64_t i)
{
   cout << "non-template c: " << ((int)c) << " i=" << i << endl;
}

template <typename T_>
void doSomething(T_ v, T_ v2)
{
   cout << "Same type v=" << v << " v2="<< v2 << endl;
}

template <typename T_,typename T2_>
void doSomething(T_ v, T2_ v2)
{
   cout << "Different types v=" << v << " v2="<< v2 << endl;
}

template <typename T_,typename T2_>
void doSomething(T_ &v, T2_ &v2)
{
   cout << "Different types v=" << v << " REFERENCE v2="<< v2 << endl;
}

template <typename T_,typename T2_, typename T3_>
void doSomething(T_ v, T2_ v2, T3_ v3)
{
   cout << "Syntactically correct, but never instantiated v=" << v << " v2=" << v2 
         <<  " v3=" << v3.val() << endl;  // @NOTE : assumption about v3!
}
/////////////////////// type-tests /////////////////////////////////////////////

struct MyType {};
// Default
template<typename>
struct has_my_desired_property : std::false_type {};
// Partial specialization for types that have my property
template<>
struct has_my_desired_property<MyType> : std::true_type {};


// @NOTE : cheating 
namespace std 
{
   template<>
   struct is_integral<MyType> : std::true_type {};
};

// @NOTE: Implicit that T_ needs to be integral
template<typename ValType_, typename T_ = int32_t, T_ n = 3, T_ m = n>
struct matrix
{
   static constexpr T_ n_ = n;
   static constexpr T_ m_ = m;
   ValType_ val_[n_][m_]; // @NOTE: implicit n/m need to >= 0
};

template<typename Matrix_>
struct user_of_matrix
{
   typedef decltype(Matrix_::n_) MatrixIndexType;
   constexpr static MatrixIndexType maxIndex = std::max(Matrix_::n_,Matrix_::m_);
};
void execute()
{
   SHOW0("======================= EXTRAS =====================================");
   SHOW0("------------ type deduction -------------------");
   doSomething('a', int64_t(5));
   doSomething('a', 'b');
   doSomething(int64_t('a'), int64_t(5));
   doSomething('a', 5);
   doSomething(5L, 5);
   
   char const c = 'A';
   int64_t const ci = 666LL;
   doSomething(c,ci);
   int64_t nci = 666LL;
   doSomething(c,nci);
   int32_t nci32 = 42;
   char c2 = 'x';
#ifdef SHOW_COMPILE_ERROR
   doSomething(c,nci32); // ambiguous
   doSomething<char&,int32_t&>(c2,nci32);
   doSomething<char&,int32_t&>((char&)c2,(int32_t&)nci32);
#endif
   doSomething('a',nci32);

   SHOW0("------------ type properties -------------------");
   SHOW(has_my_desired_property<MyType>::value,"My type has the desired property");
   SHOW(has_my_desired_property<long>::value,"long doesn't have the desired property");
   SHOW(std::is_integral<MyType>::value,"woops: I made MyType integral - MyType could be a 1024 bit integer!");
   
   matrix<double> m1;
   SHOW(m1.m_,"");
   matrix<double,int,4> m2;
   SHOW(m2.m_,"");
   matrix<double,int,4,5> m3;
   SHOW(m3.m_,"");
   matrix<double, short, 6, 7> m6;
   
   SHOW0("------------ type propagation with decltype() -------------------");
   user_of_matrix<matrix<double, char, 3, 4>> u_o_m1;
   user_of_matrix<matrix<double, short, 6, 7>> u_o_m2;
   
   SHOW(u_o_m1.maxIndex,"");
   SHOW(typeid(u_o_m1.maxIndex).name(),"");
   SHOW(u_o_m2.maxIndex,"");
   SHOW(typeid(u_o_m2.maxIndex).name(),"");
   
#ifdef SHOW_COMPILE_ERROR
   matrix<double,int,-4,5> m4;
#endif
  
}

#endif // EXTRAS_H

