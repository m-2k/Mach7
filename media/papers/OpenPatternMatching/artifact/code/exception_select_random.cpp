///
/// \file
///
/// This file is a part of pattern matching testing suite.
///
/// \author Yuriy Solodkyy <yuriy.solodkyy@gmail.com>
///
/// This file is a part of Mach7 library (http://parasol.tamu.edu/mach7/).
/// Copyright (C) 2011-2012 Texas A&M University.
/// All rights reserved.
///

#include "testshape.hpp"
#include "testutils.hpp"
#include "match.hpp"

//------------------------------------------------------------------------------

template <size_t N>
struct shape_kind : OtherBase, Shape
{
    void accept(ShapeVisitor&) const;
    void raise() const { throw *this; }
};

//------------------------------------------------------------------------------

struct ShapeVisitor
{
    #define FOR_EACH_MAX NUMBER_OF_DERIVED-1
    #define FOR_EACH_N(N) virtual void visit(const shape_kind<N>&) {}
    #include "loop_over_numbers.hpp"
    #undef  FOR_EACH_N
    #undef  FOR_EACH_MAX
};

//------------------------------------------------------------------------------

template <size_t N> void shape_kind<N>::accept(ShapeVisitor& v) const { v.visit(*this); }

//------------------------------------------------------------------------------

namespace mch ///< Mach7 library namespace
{
    template <> struct bindings<Shape> { RS(Shape::raise); };
} // of namespace mch

//------------------------------------------------------------------------------

#if 1
XTL_TIMED_FUNC_BEGIN
size_t do_match(const Shape& s, size_t)
{
    /// \note Unlike the rest of our Match statements, MatchE does not allow {} 
    ///       around the case clauses.
    MatchE(s)
    {
        #define FOR_EACH_MAX  NUMBER_OF_DERIVED-1
        #define FOR_EACH_N(N) CaseE(shape_kind<N>) return N;
        #include "loop_over_numbers.hpp"
        #undef  FOR_EACH_N
        #undef  FOR_EACH_MAX
    }
    EndMatchE

    return invalid;
}
XTL_TIMED_FUNC_END
#else
#error No preprocessed version provided
#endif

//------------------------------------------------------------------------------

XTL_TIMED_FUNC_BEGIN
size_t do_visit(const Shape& s, size_t)
{
    struct Visitor : ShapeVisitor
    {
        #define FOR_EACH_MAX  NUMBER_OF_DERIVED-1
        #define FOR_EACH_N(N) virtual void visit(const shape_kind<N>&) { result = N; }
        #include "loop_over_numbers.hpp"
        #undef  FOR_EACH_N
        #undef  FOR_EACH_MAX
        size_t result;
    };

    Visitor v;
    v.result = invalid;
    s.accept(v);
    return v.result;
}
XTL_TIMED_FUNC_END

//------------------------------------------------------------------------------

Shape* make_shape(size_t i)
{
    switch (i % NUMBER_OF_DERIVED)
    {
        #define FOR_EACH_MAX  NUMBER_OF_DERIVED-1
        #define FOR_EACH_N(N) case N: return new shape_kind<N>;
        #include "loop_over_numbers.hpp"
        #undef  FOR_EACH_N
        #undef  FOR_EACH_MAX
    }
    return 0;
}

//------------------------------------------------------------------------------

#include "testvismat1.hpp"    // Utilities for timing tests

//------------------------------------------------------------------------------

int main()
{
    using namespace mch; // Mach7's library namespace

    verdict pp = test_repetitive();
//    verdict ps = test_sequential();
//    verdict pr = test_randomized();
    std::cout << "OVERALL: "
              << "Repetitive: " << pp << "; "
//              << "Sequential: " << ps << "; "
//              << "Random: "     << pr 
              << std::endl; 
}

//------------------------------------------------------------------------------