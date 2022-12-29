#include "ap_number.h"
#include <iostream>
#include <string>

namespace ap = ArbitraryPrecision;
using ap::Number;

bool proximate_equal(const std::string& no, const std::string& src_no)
{
	// 0.2 -> 0.1999999...
	// 0.70 -> 0.6999999999...  
	size_t i = 0;
	while (no[i] && src_no[i])
	{
		if (no[i] != src_no[i])
			break;
		i++;
	}

	if (!no[i])
		return true;

	if (!src_no[i])
		return false;

	if (src_no[i] - no[i] != 1)
		return false;

	while (no[++i])
		if (no[i] != '9')
			return false;

	return true;
}

bool load_test()
{
	int st = clock();
	std::cout << "Load number test... ";

	auto failed = [](auto& str0, auto& str1)
	{
		std::cout << "\n" << str0 << "\n";
		std::cout << str1.substr(0, str0.size()) << "\n";

		std::cout << "Failed!\n";
		return false;
	};

	extern std::string pi_100_string;
	Number pi100 = Number(pi_100_string, 200);
	auto pi100_str = pi100.to_dec_string();
	if (!proximate_equal(pi100_str, pi_100_string))
		return failed(pi100_str, pi_100_string);

	extern std::string pi_1000_string;
	Number pi1k = Number(pi_1000_string, 1000);
	auto pi1k_str = pi1k.to_dec_string();
	if (!proximate_equal(pi1k_str, pi_1000_string))
		return failed(pi1k_str, pi_1000_string);

	extern std::string pi_2000_string;
	int base = 1234567;
	Number pi2k = Number(pi_2000_string, 1000, base);
	auto pi2k_str = pi2k.to_dec_string();
	if (!proximate_equal(pi2k_str, pi_2000_string))
		return failed(pi2k_str, pi_2000_string);

	extern std::string e_2000_string;
	Number e2k = Number(e_2000_string, 2000);
	auto e2k_str = e2k.to_dec_string();
	if (!proximate_equal(e2k_str, e_2000_string))
		return failed(e2k_str, e_2000_string);

	std::cout << "Passed. "; std::cout << "Time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
	return true;
}

bool print_test()
{
	int st = clock();
	std::cout << "Print test... ";

	auto failed = [](auto& str0, auto& str1)
	{
		std::cout << "\n" << str0 << "\n";
		std::cout << str1.substr(0, str0.size()) << "\n";

		std::cout << "Failed!\n";
		return false;
	};

	const std::string float_string{ "0.2" };
	Number f(float_string, 100);
	auto f_str = f.to_dec_string();
	if (!proximate_equal(f_str, float_string))
		return failed(f_str, float_string);

	extern std::string fact_1000_string;
	auto f1k = Number(fact_1000_string);
	std::string f1k_str = f1k.to_dec_string();
	if (!proximate_equal(f1k_str, fact_1000_string))
		return failed(f1k_str, fact_1000_string);

	extern std::string const_2000_string;
	auto c2k = Number(const_2000_string);
	std::string c2k_str = c2k.to_dec_string();
	if (!proximate_equal(c2k_str, const_2000_string))
		return failed(c2k_str, const_2000_string);

	extern std::string const_100_string;
	auto c100 = Number(const_100_string);

	std::string c100_2_str = c100.to_bin_string();
	Number c100_bin{ "0b" + c100_2_str };
	if (c100 != c100_bin)
	{
		std::cout << "\n";
		std::cout << c100.to_dec_string() << "\n";
		std::cout << c100_bin.to_dec_string() << "\n";
		std::cout << c100_2_str << "\n";
		return false;
	}

	std::string c100_16_str = c100.to_hex_string();
	Number c100_hex{ "0x" + c100_16_str };
	if (c100 != c100_hex)
	{
		std::cout << "\n";
		std::cout << c100.to_dec_string() << "\n";
		std::cout << c100_hex.to_dec_string() << "\n";
		std::cout << c100_16_str << "\n";
		return false;
	}

	std::cout << "Passed. "; std::cout << "Time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
	return true;
}

bool base_test()
{
	const std::initializer_list bases{ 2, 7, 10, 17, 12345, 1234567, 100000000 };
	//const std::initializer_list bases{ 100000000 };

	auto factorial = [](Number& no, int n) {
		no = 1;
		for (int i = 2; i <= n; i++)
			no *= i;
		return no;
	};

	int st = clock();
	std::cout << "Base test... ";

	extern std::string fact_1000_string;
	auto fact_1000_from_book = Number(fact_1000_string);

	const int N = 1000;
	const int precision = 4000;

	for (auto b : bases)
	{
		Number no(0, precision, b);
		factorial(no, N);

		const Number& dr = abs(1 - no / fact_1000_from_book);
		if (dr > 1e-20)
		{
			std::cout << "Failed.\n";
			return false;
		}
	}

	std::cout << "Passed. "; std::cout << "Time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
	return true;
}

bool neg_test()
{
	int st = clock();
	std::cout << "Negative test... ";

	auto failed = [](auto& no0)
	{
		std::cout << "\n" << no0.to_dec_string() << "\n";

		std::cout << "Failed!\n";
		return false;
	};

	Number no{ 0, 1000 };
	no = -no;
	if (no != 0) return failed(no);

	no *= -1;
	if (no != 0) return failed(no);

	no = -100;
	if (no != -100) return failed(no);

	no /= 7;
	Number new_no = no * -7;
	if (new_no != 100) return failed(new_no);

	double x = 123.456789;
	new_no = no * x;
	no = new_no / -no;
	if (no != -x) return failed(no);

	std::cout << "Passed. "; std::cout << "Time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
	return true;
}

bool pow_test()
{
	int st = clock();
	std::cout << "Power test... ";

	auto failed = [](auto& no0, auto& no1)
	{
		std::cout << "\n";
		std::cout << no0.to_dec_string() << "\n";
		std::cout << no1.to_dec_string() << "\n";

		std::cout << "Failed.\n"; return false;
	};

	Number n0 = Number{ 1, 200 } / 1234;
	Number n1_1 = n0 * n0;
	Number n1_2 = n0 / 1234;
	if (n1_1 != n1_2)
		return failed(n1_1, n1_2);

	extern std::string e_2000_string;
	Number E{ e_2000_string };
	Number n2 = E * E; n2 *= n2; n2 *= E;
	Number n3 = E ^ 5;
	if (n2 != n3)
		return failed(n2, n3);

	extern std::string pi_2000_string;
	Number PI{ pi_2000_string, 2000, 10000 };
	Number n4{ PI, 200 };
	Number n5{ PI, 200 };
	if (n4 != n5)
		return failed(n4, n5);

	n4 ^= -1;
	n4 ^= -1;
	if (n4 != n5)
		return failed(n4, n5);

	n5 ^= -1;
	Number n6{ n4 * n5 };
	if (n6 != 1)
		return failed(n6, n6);

	std::cout << "Passed. "; std::cout << "Time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
	return true;
}

bool div_test()
{
	int st = clock();
	std::cout << "Division test... ";

	Number pi10{ "3.14159265358979", 500 };
	Number pi10_sav{ pi10 };

	pi10 ^= -1;
	pi10 ^= -1;

	if (pi10 != pi10_sav)
	{
		std::cout << "Failed.\n";
		return false;
	}

	// 355 / 113 ~= 3.14159
	const int TEST_BASE = 12345;
	int numeritor = 355;
	int denometer = 113;

	Number no(numeritor);
	Number no_test(numeritor, Number::DEFAULT_PRECISION_IN_10, TEST_BASE);
	no /= denometer;
	no_test /= denometer;

	if (no != no_test)
	{
		auto no_str = no.to_dec_string();
		auto test_str = no_test.to_dec_string();
		std::cout << "\n" << no_str << "\n";
		std::cout << test_str << "\n";

		std::cout << "failed.\n";
		return false;
	}

	auto factorial = [](Number& no, int n)
	{
		no = 1;
		for (int i = 2; i <= n; i++)
			no *= i;
		return no;
	};

	// C(1000, 100) = 6.3850511926305130236698511142022e+139
	Number f1000(1), f900(1), f100(1);
	factorial(f1000, 1000);
	factorial(f900, 900);
	factorial(f100, 100);

	Number r = f1000 / f900 / f100;
	Number correct_r{ "6.3850511926305130236698511142022e+139" };

	auto d0 = r / correct_r;
	Number dr = abs(r / correct_r - 1);
	if (dr > 1e-20)
	{
		std::cout << "\n" << r.to_dec_string() << "\n";
		std::cout << correct_r.to_dec_string() << "\n";

		std::cout << dr.to_dec_string() << "\n";
		std::cout << "Failed.\n";
		return false;
	}

	Number x{ 1 , 100, 111 };
	x /= 1001;
	Number y{ x };
	y *= 1001;
	if (y != 1)
	{
		std::cout << "Failed.\n";
		return false;
	}

	std::cout << "Passed. "; std::cout << "Time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
	return true;
}

bool e_test()
{
#ifdef _DEBUG
	const int PRECISION = 500;
#else 
	const int PRECISION = 10000;
#endif

	int st = clock();
	std::cout << "E(" << PRECISION << " digits) test... ";
	const int LOG10_BASE = (int)std::log10(Number::DEFAULT_BASE) + 1;

	Number e(1, PRECISION);
	Number one_over_fact_n(1, PRECISION);

	// 3250! -> 10^10000
	for (int i = 1; !one_over_fact_n.is_zero(); i++)
	{
		one_over_fact_n /= i;
		e += one_over_fact_n;
	}

	extern std::string e_10000_string;
	auto e_str = e.to_dec_string();
	if (!proximate_equal(e_str, e_10000_string))
	{
		std::cout << "\n";
		std::cout << e_str << "\n";
		std::cout << e_10000_string.substr(0, e_str.size()) << "\n";

		std::cout << "Failed!\n";
		return false;
	}

	std::cout << "Passed. "; std::cout << "Time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
	return true;
}

bool pi_test()
{
	// pi = 88*arctan(3/79)-20*arctan(685601/69049993)

#ifdef _DEBUG
	const int PRECISION = 500;
#else 
	const int PRECISION = 10000;
#endif

	const int x = 3, y = 79, pxy = 88;
	const int u = 685601, v = 69049993, puv = -20;

	int st = clock();
	std::cout << "PI(" << PRECISION << " digits) test... ";

	Number sum_xy(0, PRECISION);
	Number sum_uv(0, PRECISION);

	Number x1(x, PRECISION);
	Number y1(y, PRECISION);

	Number u1(u, PRECISION);
	Number v1(v, PRECISION);

	Number XY = x1 / y1;
	Number UV = u1 / v1;

	Number x2 = x1 * x1;
	Number y2 = y1 * y1;

	Number u2 = u1 * u1;
	Number v2 = v1 * v1;

	int i = 0;
	while (XY != 0 || UV != 0)
	{
		Number na = XY / (2 * i + 1);
		Number nb = UV / (2 * i + 1);

		XY *= x2; XY /= y2;
		UV *= u2; UV /= v2;

		if (i++ & 1)
		{
			sum_xy -= na;
			sum_uv -= nb;
		}
		else
		{
			sum_xy += na;
			sum_uv += nb;
		}
	}

	sum_xy *= pxy;
	sum_uv *= puv;
	Number pi = sum_xy + sum_uv;

	extern std::string pi_10000_string;
	auto pi_str = pi.to_dec_string();
	if (!proximate_equal(pi_str, pi_10000_string))
	{
		std::cout << "\n" << pi_str << "\n";
		std::cout << pi_10000_string.substr(0, PRECISION + 2) << "\n";

		std::cout << "Failed!\n";
		return false;
	}

	std::cout << "Passed. "; std::cout << "Time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
	return true;
}

void apn_test()
{
	load_test();
	print_test();
	base_test();
	div_test();
	e_test();
	pi_test();
	pow_test();
	neg_test();
}

int main()
{
	apn_test();
	return 0;
}

