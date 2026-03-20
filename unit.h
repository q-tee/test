#pragma once
#include <type_traits>
#include <cstddef>
#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>

class CUnitTest
{
public:
	CUnitTest(const char* szName) :
		szName(szName), nTotalCount(0U), nFailCount(0U) { }

	bool Add(const CUnitTest& child)
	{
		return this->Add(child.Report());
	}

	bool Add(const bool bChildResult)
	{
		++this->nTotalCount;
		this->nFailCount += !bChildResult;
		return bChildResult;
	}

	template <typename T, typename U>
	bool Equal(const T& result, const U& expected)
	{
		char szBuffer[32];
		std::sprintf(szBuffer, "TEST #%zu", this->nTotalCount + 1U);
		return Equal(szBuffer, result, expected);
	}

	template <typename S, typename T, typename U>
	bool Equal(const S& source, const T& result, const U& expected)
	{
		++this->nTotalCount;

		bool bResult;
		if constexpr (std::is_pointer_v<std::decay_t<T>> && std::is_pointer_v<std::decay_t<U>>)
			bResult = (static_cast<const void*>(result) == static_cast<const void*>(expected));
		else if constexpr (std::is_floating_point_v<T> && std::is_floating_point_v<U>)
			bResult = std::isnan(expected) ? (std::isnan(result) && std::signbit(expected) == std::signbit(result)) : (result == expected);
		else
			bResult = (result == expected);

		if (!bResult)
		{
			if constexpr (std::is_pointer_v<std::decay_t<T>> && std::is_pointer_v<std::decay_t<U>>)
				OnFailure(source, reinterpret_cast<const std::uintptr_t>(result), reinterpret_cast<const std::uintptr_t>(expected));
			else
				OnFailure(source, result, expected);
		}

		return bResult;
	}

	template <typename S, typename T>
	bool EqualString(const S& source, const T& result, const T& expected)
	{
		++this->nTotalCount;

		bool bResult = false;
		if (result == nullptr && expected == nullptr)
			bResult = true;
		else if (result != nullptr && expected != nullptr)
		{
			using PlainType_t = std::remove_cv_t<std::remove_pointer_t<std::decay_t<T>>>;

			if constexpr (std::is_same_v<PlainType_t, char>)
				bResult = ::strcmp(result, expected) == 0;
			else if constexpr (std::is_same_v<PlainType_t, wchar_t>)
				bResult = ::wcscmp(result, expected) == 0;
		}

		if (!bResult)
			OnFailure(source, result, expected);

		return bResult;
	}

	bool Report() const
	{
		if (this->szName != nullptr)
			std::printf("\033[94m[%s] ", this->szName);

		if (this->nTotalCount == 0U || this->nFailCount < this->nTotalCount)
			std::printf("%s %zu/%zu TESTS HAVE PASSED\n", (this->nFailCount == 0U) ? "\033[32m[PASS]\033[0m" : "\033[33m[PART]\033[0m", this->nTotalCount - this->nFailCount, this->nTotalCount);
		else
			std::printf("\033[31m[FAIL]\033[0m ALL TESTS HAS BEEN FAILED\n");

		return this->nFailCount == 0U;
	}

private:
	template <typename S, typename T, typename U>
	void OnFailure(const S& source, const T& result, const U& expected)
	{
		++this->nFailCount;

		if (this->szName != nullptr)
			std::printf("\033[94m[%s] ", this->szName);

		std::cout << "\033[31m[FAIL]\033[0m source: ";
		PrintValue(source);
		std::cout << ", result: ";
		PrintValue(result);
		std::cout << ", expected: ";
		PrintValue(expected);
		std::cout << std::endl;
	}

	template <typename T>
	void PrintValue(const T& value) const
	{
		if constexpr (std::is_same_v<T, bool>)
			std::cout << "\033[1m" << std::boolalpha << value << "\033[0m";
		else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>)
			std::cout << "\033[1m0x" << std::hex << std::uppercase << std::setw(sizeof(T) * 2U) << std::setfill('0') << value << "\033[0m";
		else if constexpr (std::is_null_pointer_v<T> || std::is_arithmetic_v<T>)
			std::cout << "\033[1m" << value << "\033[0m";
		else
			std::cout << '\"' << value << '\"';
	}

	const char* szName;
	std::size_t nTotalCount;
	std::size_t nFailCount;
};
