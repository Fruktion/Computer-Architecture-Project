#include <gtest/gtest.h>
#include "BigNumber.h"

TEST(BigNumberTest, StringConstructor) {
    BigNumber a("123.45", 2);
    ASSERT_EQ(a.toString(), "123.45");
    
    BigNumber b("-789.123", 3);
    ASSERT_EQ(b.toString(), "-789.123");
    
    BigNumber c("12345", 2);
    ASSERT_EQ(c.toString(), "12345.00");
}

TEST(BigNumberTest, PlusOperator) {
    BigNumber a("123.45", 2);
    BigNumber b("789.123", 3);
    
    BigNumber result = a + b;
    ASSERT_EQ(result.toString(), "912.573");
}

TEST(BigNumberTest, MinusOperator) {
    BigNumber a("123.45", 2);
    BigNumber b("789.123", 3);
    
    BigNumber result = a - b;
    ASSERT_EQ(result.toString(), "-665.673");
}

TEST(BigNumberTest, MultiplyOperator) {
    BigNumber a("123.45", 2);
    BigNumber b("789.123", 3);
    
    BigNumber result = a * b;
    ASSERT_EQ(result.toString(), "97432.69585");
}

TEST(BigNumberTest, EqualOperator) {
    BigNumber a("123.45", 2);
    BigNumber b("123.45", 2);
    
    ASSERT_TRUE(a == b);
    
    BigNumber c("789.123", 3);
    
    ASSERT_FALSE(a == c);
}

TEST(BigNumberTest, LessThanOperator) {
    BigNumber a("123.45", 2);
    BigNumber b("789.123", 3);
    
    ASSERT_TRUE(a < b);
    
    BigNumber c("122.45", 2);
    
    ASSERT_TRUE(c < a);
}

TEST(BigNumberTest, GreaterThanOrEqualOperator) {
    BigNumber a("123.45", 2);
    BigNumber b("789.123", 3);
    
    ASSERT_FALSE(a >= b);
    
    BigNumber c("122.45", 2);
    
    ASSERT_TRUE(a >= c);
}
