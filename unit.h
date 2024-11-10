#pragma once
#include <iostream>
#include <iomanip>

class CUnitTest
{
public:
	CUnitTest(const char* szName) :
		szName(szName), nTotalCount(0U), nFailCount(0U) { }

	template <typename T>
	void Add(const T& result, const T& expectedResult)
	{
		++this->nTotalCount;
		this->nFailCount += result != expectedResult;
	}

	template <typename S, typename T>
	void Add(const S& source, const T& result, const T& expectedResult)
	{
		++this->nTotalCount;

		bool bResult;

		using PlainType_t = std::remove_pointer_t<std::remove_cvref_t<std::remove_all_extents_t<T>>>;
		if constexpr (std::is_same_v<PlainType_t, char>)
			bResult = ::strcmp(result, expectedResult) == 0;
		else if constexpr (std::is_same_v<PlainType_t, wchar_t>)
			bResult = ::wcscmp(result, expectedResult) == 0;
		else if constexpr (std::is_floating_point_v<T>)
			bResult = std::isnan(expectedResult) ? std::isnan(result) && std::signbit(expectedResult) == std::signbit(result) : (result == expectedResult);
		else
			bResult = (result == expectedResult);

		if (!bResult)
		{
			++this->nFailCount;

			if (this->szName != nullptr)
				std::printf("\033[94m[%s] ", this->szName);

			std::cout << "\033[31m[FAIL]\033[0m source: ";
			PrintValue(source);
			std::cout << ", result: ";
			PrintValue(result);
			std::cout << ", expected: ";
			PrintValue(expectedResult);
			std::cout << std::endl;
		}
	}

	bool Report()
	{
		if (this->szName != nullptr)
			std::printf("\033[94m[%s] ", this->szName);

		if (this->nFailCount < this->nTotalCount)
			std::printf("%s %d/%d TESTS HAS BEEN PASSED\n", (this->nFailCount == 0U) ? "\033[32m[PASS]\033[0m" : "\033[33m[PART]\033[0m", static_cast<int>(this->nTotalCount - this->nFailCount), static_cast<int>(this->nTotalCount));
		else
			std::printf("\033[31m[FAIL]\033[0m ALL TESTS HAS BEEN FAILED\n");

		return this->nFailCount == 0U;
	}

private:
	template <typename T>
	void PrintValue(const T& value) const
	{
		if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>)
			std::cout << "0x" << std::hex << std::uppercase << std::setw(sizeof(T) * 2U) << std::setfill('0') << value;
		else
			std::cout << '\"' << value << '\"';
	}

	const char* szName;
	std::size_t nTotalCount;
	std::size_t nFailCount;
};
