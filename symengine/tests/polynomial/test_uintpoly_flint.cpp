#include "catch.hpp"
#include <chrono>
#include <iostream>

#include <symengine/mul.h>
#include <symengine/polys/uintpoly_flint.h>
#include <symengine/pow.h>
#include <symengine/dict.h>
#include <symengine/add.h>

using SymEngine::UIntPolyFlint;
using SymEngine::Symbol;
using SymEngine::add;
using SymEngine::symbol;
using SymEngine::Pow;
using SymEngine::RCP;
using SymEngine::make_rcp;
using SymEngine::print_stack_on_segfault;
using SymEngine::map_uint_mpz;
using SymEngine::Basic;
using SymEngine::one;
using SymEngine::zero;
using SymEngine::integer;
using SymEngine::vec_basic_eq_perm;
using SymEngine::integer_class;
using SymEngine::gcd_upoly;
using SymEngine::lcm_upoly;
using SymEngine::divides_upoly;
using SymEngine::pow_upoly;
using SymEngine::vec_integer_class;

using namespace SymEngine::literals;

TEST_CASE("Constructor of UIntPolyFlint", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> P
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    REQUIRE(P->__str__() == "x**2 + 2*x + 1");

    RCP<const UIntPolyFlint> Q
        = UIntPolyFlint::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(Q->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPolyFlint> R
        = UIntPolyFlint::from_vec(x, {1_z, 0_z, 2_z, 1_z});
    REQUIRE(R->__str__() == "x**3 + 2*x**2 + 1");

    RCP<const UIntPolyFlint> S = UIntPolyFlint::from_dict(x, {{0, 2_z}});
    REQUIRE(S->__str__() == "2");

    RCP<const UIntPolyFlint> T = UIntPolyFlint::from_dict(x, {});
    REQUIRE(T->__str__() == "0");

    RCP<const UIntPolyFlint> U
        = UIntPolyFlint::from_dict(x, {{0, 2_z}, {1, 0_z}, {2, 0_z}});
    REQUIRE(U->__str__() == "2");
}

TEST_CASE("Adding two UIntPolyFlint", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPolyFlint> a
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyFlint> b
        = UIntPolyFlint::from_dict(x, {{0, 2_z}, {1, 3_z}, {2, 4_z}});

    RCP<const Basic> c = add_upoly(*a, *b);
    REQUIRE(c->__str__() == "5*x**2 + 5*x + 3");

    RCP<const UIntPolyFlint> d = UIntPolyFlint::from_dict(x, {{0, 1_z}});
    RCP<const Basic> e = add_upoly(*a, *d);
    RCP<const Basic> f = add_upoly(*d, *a);
    REQUIRE(e->__str__() == "x**2 + 2*x + 2");
    REQUIRE(f->__str__() == "x**2 + 2*x + 2");

    RCP<const UIntPolyFlint> g
        = UIntPolyFlint::from_dict(y, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    CHECK_THROWS_AS(add_upoly(*a, *g), std::runtime_error);
}

TEST_CASE("Negative of a UIntPolyFlint", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> a
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyFlint> b = UIntPolyFlint::from_dict(x, {});

    RCP<const UIntPolyFlint> c = neg_upoly(*a);
    RCP<const UIntPolyFlint> d = neg_upoly(*b);
    REQUIRE(c->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(d->__str__() == "0");
}

TEST_CASE("Subtracting two UIntPolyFlint", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPolyFlint> a
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyFlint> b
        = UIntPolyFlint::from_dict(x, {{0, 2_z}, {1, 3_z}, {2, 4_z}});
    RCP<const UIntPolyFlint> c = UIntPolyFlint::from_dict(x, {{0, 2_z}});
    RCP<const UIntPolyFlint> f = UIntPolyFlint::from_dict(y, {{0, 2_z}});

    RCP<const Basic> d = sub_upoly(*b, *a);
    REQUIRE(d->__str__() == "3*x**2 + x + 1");
    d = sub_upoly(*c, *a);
    REQUIRE(d->__str__() == "-x**2 - 2*x + 1");
    d = sub_upoly(*a, *c);
    REQUIRE(d->__str__() == "x**2 + 2*x - 1");
    CHECK_THROWS_AS(sub_upoly(*a, *f), std::runtime_error);
}

TEST_CASE("Multiplication of two UIntPolyFlint", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const Symbol> y = symbol("y");

    RCP<const UIntPolyFlint> a
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyFlint> b
        = UIntPolyFlint::from_dict(x, {{0, -1_z}, {1, -2_z}, {2, -1_z}});
    RCP<const UIntPolyFlint> e
        = UIntPolyFlint::from_dict(x, {{0, 5_z}, {1, -2_z}, {2, -1_z}});
    RCP<const UIntPolyFlint> f
        = UIntPolyFlint::from_dict(x, {{0, 6_z}, {1, -2_z}, {2, 3_z}});
    RCP<const UIntPolyFlint> k
        = UIntPolyFlint::from_dict(x, {{0, -1_z}, {1, -2_z}, {2, -100_z}});

    RCP<const UIntPolyFlint> c = mul_upoly(*a, *a);
    RCP<const UIntPolyFlint> d = mul_upoly(*a, *b);
    RCP<const UIntPolyFlint> g = mul_upoly(*e, *e);
    RCP<const UIntPolyFlint> h = mul_upoly(*e, *f);
    RCP<const UIntPolyFlint> i = mul_upoly(*f, *f);
    RCP<const UIntPolyFlint> l = mul_upoly(*k, *f);
    RCP<const UIntPolyFlint> m = mul_upoly(*k, *k);

    REQUIRE(c->__str__() == "x**4 + 4*x**3 + 6*x**2 + 4*x + 1");
    REQUIRE(d->__str__() == "-x**4 - 4*x**3 - 6*x**2 - 4*x - 1");
    REQUIRE(g->__str__() == "x**4 + 4*x**3 - 6*x**2 - 20*x + 25");
    REQUIRE(h->__str__() == "-3*x**4 - 4*x**3 + 13*x**2 - 22*x + 30");
    REQUIRE(i->__str__() == "9*x**4 - 12*x**3 + 40*x**2 - 24*x + 36");
    REQUIRE(l->__str__() == "-300*x**4 + 194*x**3 - 599*x**2 - 10*x - 6");
    REQUIRE(m->__str__() == "10000*x**4 + 400*x**3 + 204*x**2 + 4*x + 1");

    c = UIntPolyFlint::from_dict(x, {{0, -1_z}});
    REQUIRE(mul_upoly(*a, *c)->__str__() == "-x**2 - 2*x - 1");
    REQUIRE(mul_upoly(*c, *a)->__str__() == "-x**2 - 2*x - 1");

    c = UIntPolyFlint::from_dict(y, {{0, -1_z}});
    CHECK_THROWS_AS(mul_upoly(*a, *c), std::runtime_error);
}

TEST_CASE("Evaluation of UIntPolyFlint", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> a
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});
    RCP<const UIntPolyFlint> b
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 0_z}, {2, -1_z}});

    REQUIRE(a->eval(2_z) == 9);
    REQUIRE(a->eval(10_z) == 121);
    REQUIRE(b->eval(-1_z) == 0);
    REQUIRE(b->eval(0_z) == 1);

    vec_integer_class resa = {9_z, 121_z, 0_z, 1_z};
    vec_integer_class resb = {-3_z, -99_z, 0_z, 1_z};
    REQUIRE(a->multieval({2_z, 10_z, -1_z, 0_z}) == resa);
    REQUIRE(b->multieval({2_z, 10_z, -1_z, 0_z}) == resb);
}

TEST_CASE("UIntPolyFlint as_symbolic", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> a
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 2_z}, {2, 1_z}});

    REQUIRE(eq(*a->as_symbolic(),
               *add({one, mul(integer(2), x), pow(x, integer(2))})));
    REQUIRE(not eq(*a->as_symbolic(),
                   *add({one, mul(integer(3), x), pow(x, integer(2))})));

    RCP<const UIntPolyFlint> b
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 1_z}, {2, 2_z}});
    REQUIRE(eq(*b->as_symbolic(),
               *add({one, x, mul(integer(2), pow(x, integer(2)))})));

    RCP<const UIntPolyFlint> c = UIntPolyFlint::from_dict(x, map_uint_mpz{});
    REQUIRE(eq(*c->as_symbolic(), *zero));
}

TEST_CASE("UIntPolyFlint gcd", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> a = UIntPolyFlint::from_dict(x, {{2, 2_z}});
    RCP<const UIntPolyFlint> b = UIntPolyFlint::from_dict(x, {{1, 3_z}});
    RCP<const UIntPolyFlint> c
        = UIntPolyFlint::from_dict(x, {{0, 6_z}, {1, 8_z}, {2, 2_z}});
    RCP<const UIntPolyFlint> d
        = UIntPolyFlint::from_dict(x, {{1, 4_z}, {2, 4_z}});

    RCP<const UIntPolyFlint> ab = gcd_upoly(*a, *b);
    RCP<const UIntPolyFlint> cd = gcd_upoly(*c, *d);
    RCP<const UIntPolyFlint> ad = gcd_upoly(*a, *d);
    RCP<const UIntPolyFlint> bc = gcd_upoly(*b, *c);

    REQUIRE(ab->__str__() == "x");
    REQUIRE(cd->__str__() == "2*x + 2");
    REQUIRE(ad->__str__() == "2*x");
    REQUIRE(bc->__str__() == "1");
}

TEST_CASE("UIntPolyFlint lcm", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> a = UIntPolyFlint::from_dict(x, {{2, 6_z}});
    RCP<const UIntPolyFlint> b = UIntPolyFlint::from_dict(x, {{1, 8_z}});
    RCP<const UIntPolyFlint> c
        = UIntPolyFlint::from_dict(x, {{0, 8_z}, {1, 8_z}});

    RCP<const UIntPolyFlint> ab = lcm_upoly(*a, *b);
    RCP<const UIntPolyFlint> bc = lcm_upoly(*b, *c);
    RCP<const UIntPolyFlint> ac = lcm_upoly(*a, *c);

    REQUIRE(ab->__str__() == "24*x**2");
    REQUIRE(bc->__str__() == "8*x**2 + 8*x");
    REQUIRE(ac->__str__() == "24*x**3 + 24*x**2");
}

TEST_CASE("UIntPolyFlint divides", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> a
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 1_z}});
    RCP<const UIntPolyFlint> b = UIntPolyFlint::from_dict(x, {{0, 4_z}});
    RCP<const UIntPolyFlint> c
        = UIntPolyFlint::from_dict(x, {{0, 8_z}, {1, 8_z}});

    std::pair<bool, RCP<const UIntPolyFlint>> ac = divides_upoly(*a, *c);
    std::pair<bool, RCP<const UIntPolyFlint>> bc = divides_upoly(*b, *c);
    std::pair<bool, RCP<const UIntPolyFlint>> ba = divides_upoly(*b, *a);

    REQUIRE(ac.first);
    REQUIRE(ac.second->__str__() == "8");
    REQUIRE(bc.first);
    REQUIRE(bc.second->__str__() == "2*x + 2");
    REQUIRE(!ba.first);
}

TEST_CASE("UIntPolyFlint pow", "[UIntPolyFlint]")
{
    RCP<const Symbol> x = symbol("x");
    RCP<const UIntPolyFlint> a
        = UIntPolyFlint::from_dict(x, {{0, 1_z}, {1, 1_z}});
    RCP<const UIntPolyFlint> b
        = UIntPolyFlint::from_dict(x, {{0, 3_z}, {2, 1_z}});

    RCP<const UIntPolyFlint> aaa = pow_upoly(*a, 3);
    RCP<const UIntPolyFlint> bb = pow_upoly(*b, 2);

    REQUIRE(aaa->__str__() == "x**3 + 3*x**2 + 3*x + 1");
    REQUIRE(bb->__str__() == "x**4 + 6*x**2 + 9");
}
