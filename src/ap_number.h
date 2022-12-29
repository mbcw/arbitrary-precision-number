#ifndef	ap_number_h
#define ap_number_h

#include <cmath>
#include <deque>
#include <iostream>
#include <iomanip>
#include <utility>
#include <string>
#include <stdexcept>
#include <cassert>

namespace ArbitraryPrecision {

class Number
{

public:
	static const int DEFAULT_BASE = 640000000;
	static const int DEFAULT_PRECISION_IN_10 = 2000;

	inline static const double EXTRA_PRECISION = 0.1;

protected:

	enum class PatternNew { V };
	enum class BasePrecision { V };

	int _base;
	int _ms_exp;
	double _precision;
	int _safe_precision;

	std::deque<int> _digits;

	template<class T>
	Number(BasePrecision, T value, double precision_in_base, int new_base)
		:
		_base{ new_base },
		_ms_exp{ 0 },
		_precision{ precision_in_base },
		_safe_precision{ cal_safe_precision() },
		_digits{ 0 }
	{
		*this = value;
	}

	template<class T>
	Number(PatternNew, T value, const Number& pattern)
		:
		_base{ pattern._base },
		_ms_exp{ 0 },
		_precision{ pattern._precision },
		_safe_precision{ pattern._safe_precision },
		_digits{ 0 }
	{
		*this = value;
	}

public:

	Number(const std::string& number_string, int precision_in_10 = DEFAULT_PRECISION_IN_10, int base = DEFAULT_BASE)
		: Number(0, precision_in_10, base)
	{
		*this = parse(number_string, precision_in_10);
	}

	Number(const char* number_string, int precision_in_10 = DEFAULT_PRECISION_IN_10, int base = DEFAULT_BASE)
		: Number(std::string(number_string), precision_in_10, base)
	{ }

	template<class T>
	Number(T value, int precision_in_10, int base)
		:
		_base{ base },
		_ms_exp{ 0 },
		_precision{ cal_precision(precision_in_10) },
		_safe_precision{ cal_safe_precision() },
		_digits{ 0 }
	{
		*this = value;
	}

	Number(): Number(0) {}
	template<class T> Number(T value) : Number(value, DEFAULT_PRECISION_IN_10, DEFAULT_BASE) {}
	template<class T> Number(T value, int precision_in_10) : Number(value, precision_in_10, DEFAULT_BASE) {}
	template<class T> Number pattern_new(T x) const { return Number(PatternNew::V, x, *this); }

	int get_base() const { return _base; }
	int get_precision_in_10() const { return (int)std::round(_precision * std::log10(_base) / std::log10(10)); }

	int ms_exp() const { return _ms_exp; }
	int ls_exp() const { return _ms_exp + 1 - (int)_digits.size(); }

	int most_significant_exp() const { return ms_exp(); }
	int least_significant_exp() const { return ls_exp(); }

	int min_exp() const { return -_safe_precision; }
	int min_comp_exp() const { return -(int)std::ceil(_precision); }

	int& ms_digit() { return _digits.front(); }
	int& ls_digit() { return _digits.back(); }

	int& most_significant_digit() { return ms_digit(); }
	int& least_significant_digit() { return ls_digit(); }

	int ms_value() const { return c_ms_digit(); }
	int ls_value() const { return c_ls_digit(); }

	int most_significant_value() const { return ms_value(); }
	int least_significant_value() const { return ls_value(); }

	bool is_one() const { return _digits.size() == 1 && ms_value() == 1; }
	bool is_zero() const { return ms_exp() < min_comp_exp() || is_zero_strict(); }
	bool is_not_zero() const { return !is_zero(); }

	bool is_zero_strict() const { return ms_value() == 0; }
	bool is_not_zero_strict() const { return !is_zero_strict(); }

	bool is_positive() const { return ms_value() > 0; }
	bool is_non_neg() const { return ms_value() >= 0; }
	bool is_negative() const { return !is_non_neg(); }

	int operator () (int exp) const { return digit_value(exp); }
	int& operator [] (int exp) { return get_digit(exp); }

	template<class T> bool operator > (const T cmp_no) { return get_relation(cmp_no) > 0; }
	template<class T> bool operator < (const T cmp_no) { return get_relation(cmp_no) < 0; }
	template<class T> bool operator >= (const T cmp_no) { return get_relation(cmp_no) >= 0; }
	template<class T> bool operator <= (const T cmp_no) { return get_relation(cmp_no) <= 0; }
	template<class T> bool operator != (const T cmp_no) { return get_relation(cmp_no) != 0; }
	template<class T> bool operator == (const T cmp_no) const { return get_relation(cmp_no) == 0; }

	Number  operator - () const { return (*this) * -1; }
	Number& operator + () { return *this; }
	Number& operator <<= (int shifts) { _ms_exp += shifts; return *this; }
	Number& operator >>= (int shifts) { _ms_exp -= shifts; return *this; }

	template <class T> Number& operator  = (T value) { clear(); return *this += value; }
	template <class T> Number& operator -= (T no) { return (*this) += (-no); }
	template <class T> Number& operator += (T value) { return add_equ(value); }
	template <class T> Number& operator ^= (T exp) { return power_equ(exp); }

	bool operator > (const Number& y) const { return get_relation(y) > 0; }
	bool operator >= (const Number& y) const { return get_relation(y) >= 0; }
	bool operator < (const Number& y) const { return get_relation(y) < 0; }
	bool operator <= (const Number& y) const { return get_relation(y) <= 0; }
	bool operator != (const Number& y) const { return get_relation(y) != 0; }
	bool operator == (const Number& y) const { return get_relation(y) == 0; }

	Number& operator += (const Number& no) { return oper_prepare<'+'>(no); }
	Number& operator -= (const Number& no) { return oper_prepare<'-'>(no); }
	Number& operator *= (const Number& no) { return oper_prepare<'*'>(no); }
	Number& operator /= (const Number& no) { return oper_prepare<'/'>(no); }

	Number& operator *= (int value) { return mul_equ(value, 0); }
	Number& operator *= (double value) { return mul_equ(pattern_new(value)); }

	Number& operator /= (int value) { return div_equ(value); }
	Number& operator /= (double value) { return div_equ(pattern_new(value)); }

	friend Number pow(int x, int exp) { return Number{ x } ^= exp; }
	friend Number abs(const Number& no0) { return no0 >= 0 ? no0 : no0.operator-(); }

	template <class T> friend Number operator ^ (const Number& no0, T exp) { return Number{ no0 } ^= exp; }
	template <class T> friend Number operator + (const Number& no0, T value) { return Number(no0) += value; }
	template <class T> friend Number operator - (const Number& no0, T value) { return Number(no0) -= value; }
	template <class T> friend Number operator * (const Number& no0, T value) { return Number(no0) *= value; }
	template <class T> friend Number operator / (const Number& no0, T value) { return Number(no0) /= value; }

	std::string to_bin_string() { return  to_string(2); }
	std::string to_oct_string() { return  to_string(8); }
	std::string to_dec_string() { return  to_string(10); }
	std::string to_hex_string() { return  to_string(16); }

protected:

	const int& c_ms_digit() const { return _digits.front(); }
	const int& c_ls_digit() const { return _digits.back(); }

	bool is_self(const Number& other_no) const { return this == &other_no; }
	bool same_base(const Number& no) const { return no.get_base() == get_base(); }

	bool is_valid_exp(int exp) const { return exp <= (int)ms_exp() && exp >= (int)ls_exp(); }

	bool reach_precision() const { return ls_exp() <= min_exp(); }
	bool exceed_precision(int exp) const { return exp < min_exp(); }

	double cal_precision(int precision, int pre_base = 10) const { return precision * std::log(pre_base) / std::log(_base); }
	int cal_safe_precision() const { return (int)std::ceil(std::ceil(_precision) * (1.0 + EXTRA_PRECISION)); }

	int get_digit_index(int exp) const { return int(_ms_exp - exp); }

	Number& add_equ(long long value)
	{
		int exp = 0;
		while (value)
		{
			int i_value = value % _base;
			value /= _base;
			add_equ(i_value, exp++);
		}

		return *this;
	}

	Number& power_equ(int exp)
	{
		if (exp == 0)
			return *this = 1;

		if (exp == 1)
			return *this;

		Number this_clone{ *this };

		if (exp == -1)
		{
			*this = 1;
			*this /= this_clone;
			return *this;
		}

		*this ^= (exp / 2);
		*this *= *this;

		if (exp & 1)
		{
			if (exp > 0)
				*this *= this_clone;
			else
				*this /= this_clone;
		}

		return *this;
	}

	Number& add_equ(double value)
	{
		long long long_value = (long long)value;
		double frac_value = value - long_value;

		*this += long_value;
		this->add_equ_frac(frac_value);

		return *this;
	}

	Number& add_equ_frac(double frac_value)
	{
		int exp = -1;
		while (!exceed_precision(exp) && frac_value)
		{
			frac_value *= _base;
			int i_value = (int)std::floor(frac_value);
			frac_value -= i_value;
			add_equ(i_value, exp--);
		}

		return *this;
	}

	bool equal(int cmp_no) const
	{
		if (cmp_no == 0)
			return is_zero();

		auto no = pattern_new(cmp_no);
		return equal(no);
	}

	bool equal(const Number& cmp_no) const
	{
		if (!same_base(cmp_no))
			return equal(cmp_no.convert_base(get_base()));

		Number delta{ (*this) - cmp_no };
		return delta.is_zero();
	}

	std::string to_string(int base)
	{
		if (base > 27 || base < 2)
			throw std::invalid_argument{ "Base must between [2, 27]" };

		int max_int = 1 << 30;
		int exp = int(std::floor(std::log(max_int) / std::log(base)));
		int real_base = int(std::pow(base, exp));

		auto out_no = convert_base(real_base);
		return out_no.extract_string(base, exp, get_precision_in_10());
	}

	std::string extract_string(int base, int digits_per_int, int precision_in_10)
	{
		static const char digit_chars[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		assert(int(std::pow(base, digits_per_int)) == this->get_base());

		const Number& out_no = (*this >= 0) ? *this : Number{ *this } *-1;

		int hi_exp = std::max(0, out_no.ms_exp());
		int lo_exp = std::max(out_no.ls_exp(), out_no.min_comp_exp());
		if (lo_exp > 0) lo_exp = 0;
		int len = digits_per_int * (hi_exp - lo_exp + 2);

		std::string str;
		str.reserve((decltype(str.capacity()))len);
		if (*this < 0) str += '-';

		char buffer[0x100] = { 0 };
		auto int_to_string = [base, digits_per_int, &buffer](int x, bool leading_zero)
		{
			buffer[1] = 0;

			int first = digits_per_int - 1;
			for (int i = digits_per_int - 1; i >= 0; i--)
			{
				buffer[i] = digit_chars[x % base];
				x /= base;

				if (buffer[i] != '0')
					first = i;
			}

			if (leading_zero)
				return buffer;

			return buffer + first;
		};

		int dot_pos = -1;
		for (int exp = hi_exp; exp >= lo_exp; exp--)
		{
			int digit = out_no(exp);
			if (exp == -1)
			{
				str += '.';
				dot_pos = (int)str.size();
			}

			auto d_str = int_to_string(digit, exp == hi_exp ? false : true);
			str += d_str;
		}

		// check precision in base 10.
		if (dot_pos >= 0)
		{
			if (str.size() > dot_pos + precision_in_10)
				str.erase(str.begin() + dot_pos + precision_in_10, str.end());

			// trim tail-zero
			auto index = str.size();
			while (str[--index] == '0')
				;
			str.erase(str.begin() + index + 1, str.end());
		}

		return str;
	}

	void set_precision(int new_precision_10)
	{
		_precision = cal_precision(new_precision_10);
		_safe_precision = cal_safe_precision();

		int index = get_digit_index(min_exp());
		if (index > _digits.size())
			_digits.erase(_digits.begin() + index, _digits.end());

		trim_low();
		_digits.shrink_to_fit();
	}

protected:

	template<class T> int get_relation(const T& y) const { return get_relation_same_base(pattern_new(y)); }

	int get_relation(const Number& y) const
	{
		if (same_base(y))
			return get_relation_same_base(y);

		// use larger base to compare (faster)
		if (get_base() > y.get_base())
			return get_relation_same_base(y.convert_base(_base));

		return -y.get_relation_same_base(convert_base(y._base));
	}

	int get_relation_same_base(const Number& y) const
	{
		int hi_exp = std::max(ms_exp(), y.ms_exp());
		int lo_exp = std::min(ls_exp(), y.ls_exp());
		if (lo_exp < min_comp_exp())
			lo_exp = min_comp_exp();

		int carry = 0;
		for (int exp = hi_exp; exp >= lo_exp; exp--)
		{
			auto xd = (*this)(exp);
			auto yd = y(exp);
			carry = carry * _base + xd - yd;

			if (std::abs(carry) > 1)
				return carry > 1 ? 1 : -1;
		}

		if (!carry)
			return 0;

		// When carry != 0, need to check round result of remain digits.
		int xd = (*this)(lo_exp - 1);
		int yd = y(lo_exp - 1);
		carry = carry * _base + xd - yd;

		// Round carry
		if (std::abs(carry) < _base / 2)
			return 0;

		return carry > 0 ? 1 : -1;
	}

	int digit_value(int exp) const
	{
		if (is_valid_exp(exp))
			return c_digit(exp);

		return 0;
	}

	const int& c_digit(int exp) const
	{
		assert(is_valid_exp(exp));
		int index = get_digit_index(exp);
		return _digits[index];
	}

	int& get_digit(int exp)
	{
		assert(is_valid_exp(exp));
		int index = get_digit_index(exp);
		return _digits[index];
	}

	Number& clear()
	{
		_ms_exp = 0;
		_digits = { 0 };
		return *this;
	}

	int& safe_get_digit(int exp)
	{
		int index = get_digit_index(exp);

		if (index < 0)
			return expand_hi_to(exp);

		if (index >= _digits.size())
			return expand_low_to(exp);

		return (*this)[exp];
	}

	void expand_hi_once()
	{
		_ms_exp++;
		_digits.push_front(int(0));
	}

	int& expand_hi_to(int to_exp)
	{
		int count = to_exp - _ms_exp;
		while (count--)
			expand_hi_once();

		return ms_digit();
	}

	void expand_lo_once()
	{
		if (reach_precision())
			throw std::length_error{ "exceed digits limit." };

		_digits.push_back(int(0));
	}

	int& expand_low_to(int to_exp)
	{
		if (exceed_precision(to_exp))
			throw std::out_of_range{ "exceed lo precision" };

		int count = ls_exp() - to_exp;
		while (count--)
			expand_lo_once();

		return ls_digit();
	}

	// ex: _base=10, -29 -> v:1 c:-2; -40 -> v:0, c:-4
	int gen_carry(int& digit)
	{
		int carry = digit / _base;
		digit %= _base;

		if (digit >= 0)
			return carry;

		// negative  => positive, carry--
		digit += _base;
		return --carry;
	}

	int gen_carry(int& digit, int carry)
	{
		digit += carry;
		return gen_carry(digit);
	}

	bool need_trim() const
	{
		int msv = ms_value();

		if (_digits.size() == 1)
		{
			if (msv == 0 && ms_exp() != 0)
				return true;
			return false;
		}

		int nd = msv * _base + digit_value(ms_exp() - 1);
		if (std::abs(nd) < _base)
			return false;

		int lsv = ls_value();
		return lsv == 0;
	}

	void trim_high()
	{
		assert(get_digit_index(ms_exp()) >= 0);

		// Keep last digit
		while (_digits.size() > 1)
		{
			int msd = _digits[0];
			if (msd > 0)
				return;

			if (msd == 0)
			{
				_ms_exp--;
				_digits.pop_front();
				continue;
			}

			// When msd < 0:
			// -1, base-1, x, y, z, ...  ==> -1, x, y, z, ...
			// -msd * base + next > -_base => reduce
			if (msd == -1 && (*this)(ms_exp() - 1) == _base - 1)
			{
				_ms_exp--;
				_digits.pop_front();
				_digits[0] = -1;
				continue;
			}

			return;
		}
	}

	void trim_low()
	{
		int exp = std::max(ls_exp(), min_exp());
		int index = get_digit_index(exp);
		assert(index >= 0);

		while (index && !_digits[index])
			index--;

		if (index + 1 < _digits.size())
			_digits.erase(_digits.begin() + index + 1, _digits.end());
	}

	void check_zero()
	{
		if (_digits.size() == 1 && !_digits[0])
			_ms_exp = 0;
	}

	void trim()
	{
		if (exceed_precision(ms_exp()))
		{
			clear();
			return;
		}

		trim_high();
		trim_low();
		check_zero();
	}

	Number& add_equ(int value)
	{
		if (value == 0)
			return *this;

		int& digit = safe_get_digit(0);
		digit += value;

		int carry = gen_carry(digit);
		add_carry(carry, 1);
		return *this;
	}

	Number& add_equ(int value, int exp)
	{
		if (value == 0 || exceed_precision(exp))
			return *this;

		int& digit = safe_get_digit(exp++);
		digit += value;

		int carry = gen_carry(digit);
		add_carry(carry, exp);
		return *this;
	}

	void add_carry(int carry, int exp = 0)
	{
		while (carry != 0 && exp <= ms_exp())
			carry = gen_carry(get_digit(exp++), carry);

		while (carry)
		{
			expand_hi_once();

			int& digit = ms_digit();
			digit = carry;

			// ms_digit < 0 -> neg number, it's ok, done.
			if (carry <= 0 && carry > -_base)
				break;

			carry = gen_carry(digit);
		}

		trim();
	}

	template<char OPER>
	Number& call_oper(const Number& no)
	{
		if (OPER == '+') return add_equ<1>(no);
		if (OPER == '-') return add_equ<-1>(no);

		if (OPER == '*') return mul_equ(no);
		if (OPER == '/') return div_equ(no);
	}

	template<char OPER>
	Number& oper_prepare(const Number& no)
	{
		if (same_base(no))
			return call_oper<OPER>(no);

		Number new_no{ no.convert_base(get_base()) };
		return call_oper<OPER>(new_no);
	}

	template<int SIGN>
	Number& add_equ(const Number& no)
	{
		assert(same_base(no));

		if (no.is_zero_strict())
			return *this;

		if (is_self(no))
		{
			if (SIGN == 1)
				return *this *= 2;

			return *this = 0;
		}

		int ms_exp = no.ms_exp();
		int ls_exp = std::max(no.ls_exp(), min_exp());

		int carry = 0;
		for (int exp = ls_exp; exp <= ms_exp; exp++)
		{
			int& digit = safe_get_digit(exp);
			digit = digit + SIGN * no(exp) + carry;
			carry = gen_carry(digit);
		}

		add_carry(carry, ms_exp + 1);
		return *this;
	}

	int mul_single_digit(int& digit, int multiplier, int carry)
	{
		//assert(digit >= 0 && multiplier >= 0); 
		long long r = (long long)digit * multiplier + carry;

		carry = (int)(r / _base);
		digit = (int)(r % _base);

		carry += gen_carry(digit);
		return carry;
	}

	Number& mul_minus_1()
	{
		int carry = 0;
		int exp = ls_exp();
		for (; exp <= ms_exp(); exp++)
		{
			int& digit = (*this)[exp];
			digit = -digit + carry;
			carry = gen_carry(digit);
		}

		add_carry(carry, exp);
		return *this;
	}

	Number mul(int multiplier, int value_exp) { return Number{ *this }.mul_equ(multiplier, value_exp); }

	Number& mul_equ(int multiplier, int value_exp)
	{
		*this <<= value_exp;
		trim();

		return mul_equ(multiplier);
	}

	Number& mul_equ(int multiplier)
	{
		if (multiplier == 1)
			return *this;

		if (multiplier == -1)
			return mul_minus_1();

		if (multiplier == 0)
			return clear();

		int carry = 0;
		int exp = ls_exp();
		for (; exp <= ms_exp(); exp++)
		{
			int& digit = (*this)[exp];
			carry = mul_single_digit(digit, multiplier, carry);
		}

		add_carry(carry, exp);
		return *this;
	}

	Number& mul_equ(const Number& no)
	{
		if (is_self(no))
			return *this *= Number{ no };

		Number clone_of_this(std::move(*this));
		this->clear();

		for (int exp = no.ms_exp(); exp >= no.ls_exp(); exp--)
		{
			Number tmp = clone_of_this.mul(no(exp), exp);
			*this += tmp;
		}

		return *this;
	}

	Number convert_frac_result(int new_base, double precision_in_base) const
	{
		Number frac_result(BasePrecision::V, 0, precision_in_base, new_base);
		Number pow_base = frac_result.pattern_new(1);

		int old_base = get_base();
		if (ms_exp() < -1)
		{
			pow_base = old_base;
			pow_base ^= (ms_exp() + 1);
		}

		for (int exp = std::min(-1, ms_exp());
			exp >= ls_exp() && !pow_base.is_zero_strict();
			exp--)
		{
			int value = (*this)(exp);
			pow_base /= old_base;
			Number pv = pow_base * value;

			frac_result += pv;
		}

		return frac_result;
	}

	Number convert_int_base(int new_base, double precision_in_base) const
	{
		Number int_result(BasePrecision::V, 0, precision_in_base, new_base);
		Number pow_base = int_result.pattern_new(1);

		int old_base = get_base();

		if (ls_exp() > 0)
		{
			pow_base = old_base;
			pow_base ^= (ls_exp() - 1);
		}

		int_result = (*this)(0);
		for (int exp = std::max(1, ls_exp()); exp <= ms_exp(); exp++)
		{
			pow_base *= old_base;
			int value = (*this)(exp);
			Number pv = pow_base * value;
			int_result += pv;
		}

		return int_result;
	}

	Number get_frac_part()
	{
		Number result = pattern_new(0);
		if (ls_exp() >= 0)
			return result;

		if (ms_exp() >= 0)
		{
			result._ms_exp = -1;
			result._digits.clear();
			result._digits.insert(result._digits.begin(), this->_digits.begin() + ms_exp() + 1, this->_digits.end());
		}
		else
		{
			result._ms_exp = ms_exp();
			result._digits = this->_digits;
		}

		return result;
	}

	Number get_int_part()
	{
		Number result = pattern_new(0);
		if (ms_exp() < 0)
			return result;

		result._ms_exp = this->ms_exp();
		if (ls_exp() >= 0)
			result._digits = this->_digits;
		else
		{
			result._digits.clear();
			result._digits.insert(result._digits.begin(), this->_digits.begin(), this->_digits.begin() + ms_exp() + 1);
		}

		return result;
	}

	// binary example: -1011.101e-1011
	// dec-number example: -123.456E+123 / 4567e-123 / 456.e78
	int parse_number(Number& no, const char* no_str)
	{
		int base = no.get_base();
		const char* p = no_str;
		no.clear();

		bool neg_no = *p == '-';
		if (*p == '-' || *p == '+')
			p++;

		bool has_dot = false;
		const char* p_save = p;
		while (*p && (base != 10 || (*p != 'e' && *p != 'E')))
		{
			if (*p == '.')
			{
				// We can not forget that there is a '0' on the head of _digits...
				no._ms_exp = int(p - p_save);
				p++;
				has_dot = true;
				continue;
			}

			if (has_dot && exceed_precision(ls_exp()))
				break;

			char ch = *p;

			int x = (ch <= '9') ? ch - '0' : (ch >= 'a' ? ch - 'a' + 10 : ch - 'A' + 10);
			if (x < 0 || x > base)
				throw std::invalid_argument{ "number string format error." };

			p++;
			no._digits.push_back(x);
		}

		if (!has_dot)
			no._ms_exp = int(p - p_save);

		while (*p && *p != 'e' && *p != 'E')
			p++;

		no.trim();

		if (neg_no)
			no *= -1;

		return int(p - no_str);
	}

	int parse_exp_number(const char* exp_str, int base)
	{
		auto p = exp_str;
		if (*p != 'e' && *p != 'E')
			return 0;

		p++;
		bool neg_exp = *p == '-';
		if (*p == '-' || *p == '+')
			p++;

		int exp = 0;
		while (*p)
		{
			int x = *p - '0';
			if (x < 0 || x > 9)
				throw std::invalid_argument{ "number string format error." };

			exp *= base;
			exp += x;
			p++;
		}

		if (neg_exp)
			exp *= -1;

		return exp;
	}

	//int parse_dec_number(Number& no, const char* no_str)
	//{
	//	const char* p = no_str;
	//	no.clear();

	//	bool neg_no = *p == '-';
	//	if (*p == '-' || *p == '+')
	//		p++;

	//	bool has_dot = false;
	//	const char* p_save = p;
	//	while (*p && *p != 'e' && *p != 'E')
	//	{
	//		if (*p == '.')
	//		{
	//			// We can not forget that there is a '0' on the head of _digits...
	//			no._ms_exp = int(p - p_save);
	//			p++;
	//			has_dot = true;
	//			continue;
	//		}

	//		if (has_dot && exceed_precision(ls_exp()))
	//			break;

	//		int x = *p - '0';
	//		if (x < 0 || x > 9)
	//			throw std::invalid_argument{ "number string format error." };

	//		p++;
	//		no._digits.push_back(x);
	//	}

	//	if (!has_dot)
	//		no._ms_exp = int(p - p_save);

	//	while (*p && *p != 'e' && *p != 'E')
	//		p++;

	//	no.trim();

	//	if (neg_no)
	//		no *= -1;

	//	return int(p - no_str);
	//}

	//int parse_dec_exp_number(const char* exp_str)
	//{
	//	auto p = exp_str;
	//	if (*p != 'e' && *p != 'E')
	//		return 0;

	//	p++;
	//	bool neg_exp = *p == '-';
	//	if (*p == '-' || *p == '+')
	//		p++;

	//	int exp = 0;
	//	while (*p)
	//	{
	//		int x = *p - '0';
	//		if (x < 0 || x > 9)
	//			throw std::invalid_argument{ "number string format error." };

	//		exp *= 10;
	//		exp += x;
	//		p++;
	//	}

	//	if (neg_exp)
	//		exp *= -1;

	//	return exp;
	//}

	/*
	Division Method
	* The effecient method is complicated.

	Define:
		[x0,x1,...]<exp,B> = x0*B^exp + x1*B^(exp-1) + ...

	* Note x0 will be 0 only when the whole number is zero.
	* We can omit B/[] when not cofused.

	For:
		M => [m0,m1,m2,...]<eM,B>
		N => [n0,n1,n2,...]<eN,B>

		N != 0

	I. Method Explianation:
		Set: Q = 0			==> for M/N

	1) Get q* (try-out digit of Q)

		To calculate Q's first non-zeor digit, we can just take
		look on the first two digits.
			e* = eM - eN
			q# = [m0,m1] / [n0,n1]
			q* = floor(q#)
			q- = q-min = floor([m0,m1] / [n0,(n1+1)])
			q+ = q-max = floor([m0,(m1+1)] / [n0,n1])

		There will be:
			q- <= q <= q+
			q- <= q* <= q+

		And we can be prove easily, that:
			<1>	q* = q+ (When B >= 2)
			<2>	q - q- = 0/1
			<3>	q = q* - 0/1

		When q* < 1:
			q* = [q# * B]
			e*--
		After multiplecation q* will be great/equal than 1 for sure.

	2) Try q*

		NQ = N * q*<e*>
		new_M = M - NQ

		Check new_M:
			1. new_M < 0:
				a> q* > 1:
					q = q* - 1
					new_M += N * 1<e*>

				b> q* == 1:
					q = B - 1
					e*--
					new_M += N * 1<e*>

			2. new_M >= 0
				q = q*

	3) Update
		Q += q<e*>
		M = new_m;
		Repeat to 1)

	II. Two-Digits Division

	Use float/double div operation.
		x = m0 + m1 / B
		y = n0 + n1 / B
		q# = x / y
	* This method only correct when B <= 2^32.
	*/

	/*
	III. Example: 1800/25800

	1) Get q*

		Q: [0]<0, 10>
		M: [1,8]<3, 10>
		N: [2,5,8]<4, 10>

		e* = 3 - 4 = -1
		q# = [1,8]/[2,5] = 0.72
		q* = [q#] = 0

		q* = [q# * B] = 7
		e* = e* - 1 = -2

	2) Try q*

		q* = 7, e* = -2
		nq = N * q*<e*> = [2,5,8]<4> * 7<-2> = 1806 = [1,8,0,6]<3>
		new_m = M - tmp_nq = [1,8]<3> - [1,8,0,6]<3> = -6<0>

		Note that new_m < 0, so:
			q = q* - 1
			new_m += N * 1<e*> = -6<0> + [2,5,8]<4> * 1<-2> = 258<2> -6<0> = 252<2>

	3) Update Q

		Q = Q + q<e*> = 0 + 6<-2> = 6<-2>
		M = new_m = [2,5,2]<2>

	4) Get q*

		Q: [6]<-2>
		M: [2,5,2]<2>
		N: [2,5,8]<4>

		e* = -2
		q# = [2,5] / [2,5] = 1
		q* = 1

	5) Try q*

		q* = 1, e* = -2
		tmp_nq = N * q*<e*> = 258
		new_m = M - tmp_nq = 252 - 258 = -6

		new_m < 0:
			q* == 1:
				q = B - 1 = 9
				e* = e* - 1 = -3
				new_m = new_m + N * 1<e*> = -6<0> + [2,5,8]<4> * 1<-3> = -6<0> + [2,5,8]<1> = [1,9,8]<1>

		Q = Q + q*<e*> = 6<-2> + 9<-3> = 69<-2>
		M = new_m = 198<1>

	6) Get q*

		Q = 69<-2>
		M = 192<1>
		N = 258<4>

		e* = 1 - 4 = -3
		q# = 19/25 = 0.76
		q_exp* = -3
		q# = 1,9/2,5 = 0.76

		q* = [q# * 10] = 7
		e* = e* - 1 = -4

	7) Try q*
		q* = 7
		e* = -4

		tmp_nq = N * q*<-4> = 258<4> * 7<-4> = 1806<1>
		new_m = M - tmp_nq = 198<1> - 1806<1> = 174<0>

		q = q*
		Q = Q + q<e*> = 69<-2> + 7<-4> = 697<-2>
	...

	*/

	std::pair<int, int> get_q_star(const Number& m, const Number& n)
	{
		int exp_m = m.ms_exp(), m0 = m(exp_m), m1 = m(exp_m - 1);
		int exp_n = n.ms_exp(), n0 = n(exp_n), n1 = n(exp_n - 1);

		double mv = m0 + m1 * 1.0 / _base;
		double nv = n0 + n1 * 1.0 / _base;

		int e = exp_m - exp_n;
		double q_p = mv / nv;

		if (q_p >= 1)
			return std::make_pair((int)std::floor(q_p), e);

		return std::make_pair((int)std::floor(q_p * _base), e - 1);
	}

	void div_positive(Number& M, const Number& N, Number& Q)
	{
		assert(!M.need_trim() && !N.need_trim() && !Q.need_trim());
		assert(Q.same_base(M) && Q.same_base(N));

		if (N.is_zero_strict())
			throw std::overflow_error{ "divided by 0!" };

		Q.clear();
		if (N.is_one())
		{
			M >>= N.ms_exp();
			Q = M;
			return;
		}

		Number dup_N{ N };
		while (M.ms_value())
		{
			// get q* to try-out
			auto [q_star, e_star] = get_q_star(M, N);
			assert(q_star);
			if (exceed_precision(e_star))
				break;

			Number NQ{ dup_N };
			NQ.mul_equ(q_star, e_star);
			if (NQ.is_zero_strict())
				break;

			M -= NQ;
			if (M.is_negative())
			{
				if (q_star > 1)
					q_star--;
				else
				{
					assert(q_star == 1);
					q_star = _base - 1;
					--e_star;
				}

				dup_N <<= e_star;
				M += dup_N;
				dup_N >>= e_star;
			}

			Q.add_equ(q_star, e_star);
		}
	}

	std::pair<int, int> get_q_star(const Number& m, int n)
	{
		int e = m.ms_exp(), m0 = m(e), m1 = m(e - 1);
		double mv = m0 + m1 * 1.0 / _base;
		double q_p = mv / n;

		while (q_p < 1)
		{
			q_p *= _base;
			e--;
		}
		return std::make_pair((int)std::floor(q_p), e);
	}

	Number& div_equ(int N)
	{
		if (N == 1) return *this;
		if (N == -1) return *this *= -1;

		if (!N) throw std::overflow_error{ "divided by 0!" };

		auto M{ *this };
		this->clear();

		auto& Q{ *this };
		assert(!M.need_trim() && !Q.need_trim());

		bool neg_M = M < 0;
		bool neg_N = N < 0;
		bool neg_Q = neg_M != neg_N;

		if (neg_N) N *= -1;
		if (neg_M) M *= -1;

		while (M.ms_value())
		{
			auto [q_star, e_star] = get_q_star(M, N);
			assert(q_star);
			if (exceed_precision(e_star))
				break;

			Number NQ = pattern_new(N);
			NQ.mul_equ(q_star, e_star);

			M -= NQ;
			Q.add_equ(q_star, e_star);
		}

		if (neg_Q) Q *= -1;
		return *this;
	}

	Number& div_equ(const Number& N)
	{
		if (N == 0)
			throw std::overflow_error{ "divided by 0!" };

		if (is_self(N))
			return *this = 1;

		Number M{ std::move(*this) };
		Number& Q{ this->clear() };

		bool neg_M = M.is_negative();
		bool neg_N = N.is_negative();
		bool neg_Q = neg_M != neg_N;

		if (neg_M) M *= -1;
		div_positive(M, neg_N ? -N : N, Q);
		if (neg_Q) Q *= -1;

		return *this;
	}

	void compile_test()
	{
		Number no0(1.0);
		Number no1(1, 10);
		Number no2((long long)1e15);

		no2 += no1;
		no2 -= no1;
		no2 *= no1;
		no2 /= no1;

		no1 += 0.5;
		no1 -= 0.5;

		no1 *= 0.7;
		no1 *= no2;

		no2 ^= 4;
		no1 = no2 ^ 0;

		no0 = no2 / 4.5;
		no1 = no1 * 48.78;
	}

public:

	Number convert_base(int new_base, double precision_in_new_base) const
	{
		if (new_base == this->get_base())
			return *this;

		Number int_result = convert_int_base(new_base, precision_in_new_base);
		Number frac_result = convert_frac_result(new_base, precision_in_new_base);

		int_result += frac_result;
		int_result.trim();
		return int_result;
	}

	Number convert_base(int new_base) const
	{
		double precision_in_new_base = _precision * std::log(_base) / std::log(new_base);
		return convert_base(new_base, precision_in_new_base);
	}

	Number convert_precision(int new_precision_in_10)
	{
		Number no{ *this };
		no.set_precision(new_precision_in_10);
		return no;
	}

	int get_int_digits()
	{
		int int_exp = ms_exp();
		if (int_exp < 0)
			return 1;

		int len = (int)std::floor(int_exp * std::log10(_base) + std::log10(ms_value())) + 1;
		return len;
	}

	int get_frac_digits()
	{
		int frac_exp = ls_exp();
		if (frac_exp >= 0)
			return 0;

		int len = (int)std::floor(-frac_exp * std::log10(_base));
		return len;
	}

	const char* parse_base(const char* p_str, int& base)
	{
		if (*p_str == '0')
		{
			p_str++;
			if (*p_str == 'x') { base = 16; return p_str + 1; }
			if (*p_str == 'b') { base = 2; return p_str + 1; }
			if (*p_str >= '0' && *p_str <= '9') { base = 8; return p_str; }

			p_str--;
		}

		base = 10;
		return p_str;
	}

	Number parse(const std::string& number_string, int precision)
	{
		int src_base;
		const char* p_str = parse_base(number_string.c_str(), src_base);

		Number no{ 0, precision, src_base };
		int no_len = parse_number(no, p_str);
		int exp = parse_exp_number(p_str + no_len, src_base);
		no <<= exp;

		return no.convert_base(this->get_base());
	}

};

}

#endif

