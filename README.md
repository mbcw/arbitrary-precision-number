# arbitray precision number

**Arbitray Precision Number for Arithmatic operations.** <br>
Number with arbitray precision (default precision 10^-2000), all arithmatic operations will be calculated to the limit of precision.
<br>

## Project settings:
Visual Studio: open CMakeLists.txt directly.<br>
Linux/WSL:<br>
	cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug<br>
	cd build/debug && make && ./ap_test<br>
	<br>
	cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release<br>
	cd build/release && make && ./ap_test<br>

## Project targets:
1. ap_expample (example.cpp)<br>
	Calculate 10000 digits of e & pi. <br>
	e=2.7182818284590452353602874713526624 ... (10000 digits) <br>
	pi=3.141592653589793238462643383279502 ... (10000 digits) <br>

2. ap_test (test.cpp)<br>
	Test correction of project, include base convert, to_string, pow oper, div oper, negative number ...<br>

## Usage example:
One header file only: **ap_number.h**
<br>
```
#include "ap_number.h"
#include <iostream>

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

int main()
{
	int precision = 10000;
	auto e = cal_e(precision);
	std::cout << "e = " << e.to_dec_string() << "\n";
	return 0;
}
```

## Functions:
Arithmatic operations: **+ - * / pow += -= *= /= ^=**<br>
Logic operations: **== != > >= < <=**<br>
Base conversion: **convert_base(int new_base)**, new base in [2, 1e9]<br>
Input from string: **parse(string no_string, int precision)**<br>
Output to string: **to_string(int base)**  base: 2/8/10/16<br>

```
	Number no{1, 10000, 10000};
	no += no;
	no *= 5;
	no /= pow(no, 2);
	str = no.to_dec_string();

	Number new_pi{"3.1415926"};
	Number new_no255 { "0b11111111" };
	Number new_no65535 { "0xffff" };

	auto pi_base_16 = new_pi.convert_base(16);
	// ...
	
```

## Future considerations:
- Improve efficency, goal: reach 100M precision in the reasonable time
- Make a command-line caculator
- Add more math-operation, such as logrithim.
...

