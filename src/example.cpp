#include "ap_number.h"

using ArbitraryPrecision::Number;

Number cal_e(int precision = 1000)
{
	Number e(1, precision);
	Number one_over_fact_n(1, precision);

	for (int i = 1; one_over_fact_n.is_not_zero_strict(); i++)
	{
		one_over_fact_n /= i;
		e += one_over_fact_n;
	} 

	return e;
}

Number cal_pi(int precision = 1000)
{
	// pi = 88*arctan(3/79)-20*arctan(685601/69049993) 
	const int x = 3, y = 79, pxy = 88;
	const int u = 685601, v = 69049993, puv = -20;

	Number sum_xy(0, precision);
	Number sum_uv(0, precision);

	Number x1(x, precision);
	Number y1(y, precision);

	Number u1(u, precision);
	Number v1(v, precision);

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
	return pi;
}

int main()
{
	auto cal = [](auto precision, auto name, auto func) 
	{
		int st = clock();
		std::cout << name << "[" << precision << "] digits ...\n";
		auto x = func(precision);
		std::cout << name << "=" << x.to_dec_string() << "\n";
		std::cout << name << "[" << precision << "] time cost: " << std::round((clock() - st) * 10 / CLOCKS_PER_SEC) / 10 << " sec.\n";
		std::cout << "\n";
	};

	cal(10000, "e", cal_e);
	cal(10000, "pi", cal_pi);

	return 0;
}

