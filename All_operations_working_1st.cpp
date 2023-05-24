#include <vector>

#include <stdexcept>

#include <algorithm>

#include <string>

#include <sstream>

#include <iostream>

class BigNumber {

private:

    std::vector<int_least8_t> digits;

    int decimalPoint;

    bool isNegative;

public:

    BigNumber(std::string str, int maxDecimalPlaces = 0) {

        isNegative = false;

        if (str[0] == '-') {

            isNegative = true;

            str = str.substr(1);

        }

        int pointPos = str.find('.');

        if (pointPos == std::string::npos) {

            decimalPoint = 0;

        } else {

            decimalPoint = str.length() - pointPos - 1;

            str.erase(str.find('.'), 1);

        }

        for (char c : str) {

            if (c != '.') {

                digits.push_back(c - '0');

            }

        }

        // Fill in trailing zeros

        while (digits.size() < decimalPoint + maxDecimalPlaces) {

            digits.push_back(0);

        }

        std::reverse(digits.begin(), digits.end());

    }

    BigNumber operator+(const BigNumber& other) const {

        if (isNegative != other.isNegative) {

            // If the signs are different, perform subtraction

            return isNegative ? other.subtract(*this) : subtract(other);

        }

        BigNumber result(*this);

        result.digits.resize(std::max(digits.size(), other.digits.size()) + 1, 0);

        int carry = 0;

        for (int i = 0; i < result.digits.size(); ++i) {

            int sum = getDigit(i) + other.getDigit(i) + carry;

            result.setDigit(i, sum % 10);

            carry = sum / 10;

        }

        result.removeLeadingZeros();

        return result;

    }

    BigNumber operator-(const BigNumber& other) const {

        if (isNegative != other.isNegative) {

            // If the signs are different, perform addition

            return operator+(-other);

        }

        if (isNegative && other.isNegative) {

            // Subtracting a negative number is equivalent to adding the absolute values

            return other.subtract(*this);

        }

        if (isNegative && !other.isNegative) {

            // Subtracting a positive number from a negative number

            return operator+(-other);

        }

        if (!isNegative && other.isNegative) {

            // Subtracting a negative number from a positive number

            return operator+(-other);

        }

        return subtract(other);

    }

    BigNumber operator-() const {

        BigNumber result(*this);

        result.isNegative = !isNegative;

        return result;

    }

    bool operator<(const BigNumber& other) const {

        if (isNegative && !other.isNegative) {

            return true;  // Negative numbers are less than positive numbers

        }

        if (!isNegative && other.isNegative) {

            return false;  // Positive numbers are greater than negative numbers

        }

        if (isNegative && other.isNegative) {

            // Comparing absolute values, but in reverse order since negative numbers are smaller

            return compareAbsoluteValue(other) > 0;

        }

        return compareAbsoluteValue(other) < 0;

    }

    BigNumber operator*(const BigNumber& other) const {

        BigNumber result("0");

        result.digits.resize(digits.size() + other.digits.size(), 0);

        for (int i = 0; i < digits.size(); ++i) {

            int carry = 0;

            for (int j = 0; j < other.digits.size() || carry != 0; ++j) {

                int current = result.digits[i + j] + getDigit(i) * other.getDigit(j) + carry;

                result.digits[i + j] = current % 10;

                carry = current / 10;

            }

        }

        result.isNegative = isNegative != other.isNegative;

        result.decimalPoint = decimalPoint + other.decimalPoint;

        result.removeLeadingZeros();

        return result;

    }

    BigNumber operator/(const BigNumber& other) const {

        if (other == BigNumber("0")) {

            throw std::invalid_argument("Division by zero");

        }

        BigNumber dividend(*this);

        BigNumber divisor(other);

        dividend.isNegative = false;

        divisor.isNegative = false;



        // Shift dividend and divisor left by the number of decimal places of the divisor

        int shift = other.decimalPoint;

        for(int i = 0; i < shift; i++){

            dividend = dividend * BigNumber("10");

            divisor = divisor * BigNumber("10");

        }

        BigNumber quotient("0");

        BigNumber remainder("0");

        int precision = 1000;  // Set the precision you want

        for (int i = dividend.digits.size() - 1; i >= -precision; --i) {

            remainder = remainder * BigNumber("10") + BigNumber(std::to_string(dividend.getDigit(i)));

            int count = 0;

            while (remainder >= divisor) {

                remainder = remainder - divisor;

                ++count;

            }

            quotient = quotient * BigNumber("10") + BigNumber(std::to_string(count));

        }

        quotient.isNegative = isNegative != other.isNegative;

        quotient.removeLeadingZeros();

        // Adjust the decimal point position immediately after division

        quotient.decimalPoint = quotient.digits.size() - decimalPoint + other.decimalPoint - shift;

        // Rounding

        if (quotient.getDigit(-precision - 1) >= 5) {  // Check the next digit after precision

            BigNumber round("1" + std::string(precision - 1, '0'));  // Fixed the rounding issue here

            quotient = quotient + round;

        }

        while(quotient.digits.back() == 0 && quotient.decimalPoint > 1){  // To remove leading zeros

            quotient.digits.pop_back();

            quotient.decimalPoint--;

        }

        return quotient;

    }

    std::string toString() const {

        std::ostringstream oss;

        if (isNegative) {

            oss << '-';

        }

        for (int i = digits.size() - 1; i >= 0; --i) {

            if (i == decimalPoint - 1 && decimalPoint != 0) {

                oss << '.';

            }

            oss << static_cast<int>(digits[i]);

        }

        return oss.str();

    }

    bool operator==(const BigNumber& other) const {

        if (isNegative != other.isNegative || decimalPoint != other.decimalPoint || digits.size() != other.digits.size()) {

            return false;

        }

        for (int i = 0; i < digits.size(); ++i) {

            if (digits[i] != other.digits[i]) {

                return false;

            }

        }

        return true;

    }

    bool operator>=(const BigNumber& other) const {

        return !(*this < other);

    }

private:

    int getDigit(int index) const {

        if (index >= 0 && index < digits.size()) {

            return digits[index];

        }

        return 0;

    }

    void setDigit(int index, int value) {

        if (index >= 0 && index < digits.size()) {

            digits[index] = value;

        }

    }

    int compareAbsoluteValue(const BigNumber& other) const {

        if (digits.size() != other.digits.size()) {

            return digits.size() - other.digits.size();

        }

        for (int i = digits.size() - 1; i >= 0; --i) {

            if (digits[i] != other.digits[i]) {

                return digits[i] - other.digits[i];

            }

        }

        return 0;

    }

    BigNumber subtract(const BigNumber& other) const {

        if (*this < other) {

            BigNumber result = other.subtract(*this);

            result.isNegative = !isNegative;

            return result;

        }

        BigNumber result(*this);

        int borrow = 0;

        for (int i = 0; i < digits.size(); ++i) {

            int diff = getDigit(i) - other.getDigit(i) - borrow;

            if (diff < 0) {

                diff += 10;

                borrow = 1;

            } else {

                borrow = 0;

            }

            result.setDigit(i, diff);

        }

        result.removeLeadingZeros();

        return result;

    }

    void removeLeadingZeros() {

        while (!digits.empty() && digits.back() == 0 && digits.size() > decimalPoint + 1) {

            digits.pop_back();

        }

        if (digits.empty()) {

            // If all digits were zeros, add a single zero back

            digits.push_back(0);

            decimalPoint = 0;

        }

    }

};

int main() {

    // Test addition

    BigNumber num1("3141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844.609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755468731159562863882353787593751957781857780532171226806613001927876611195909216420199");

    BigNumber num2("-3141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844.609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233786783165271201909145648566923460348610454326648213393607260249141273724587006606315588174881520920962829254091715364367892590360011330530548820466521384146951941511609433057270365759591953092186117381932611793105118548074462379962749567351885752724891227938183011949129833673362440656643086021394946395224737190702179860943702770539217176293176752384674818467669405132000568127145263560827785771342757789609173637178721468440901224953430146549585371050792279689258923542019956112129021960864034418159813629774771309960518707211349999998372978049951059731732816096318595024459455346908302642522308253344685035261931188171010003137838752886587533208381420617177669147303598253490428755468731159562863882353787593751957781857780532171226806613001927876611195909216420199");

    BigNumber sum = num1 + num2;

    std::cout << num1.toString() << " + " << num2.toString() << " = " << sum.toString() << std::endl;

    // Test subtraction

    BigNumber num3(num1);

    BigNumber num4(num1);

    BigNumber diff = num3 - num4;

    std::cout << num3.toString() << " - " << num4.toString() << " = " << diff.toString() << std::endl;

    BigNumber num5(num1);

    BigNumber num6(num1);

    BigNumber product = num5 * num6;

    std::cout << num5.toString() << " * " << num6.toString() << " = " << product.toString() << std::endl;

    BigNumber num7(num1);

    BigNumber num8(num1);

    BigNumber quotient = num7 / num8;

    std::cout << num7.toString() << " / " << num8.toString() << " = " << quotient.toString() << std::endl;

    return 0;

}
