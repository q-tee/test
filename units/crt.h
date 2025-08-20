#pragma once
#include <q-tee/common/common.h>

#define Q_CRT_MEMORY_CRYPTO
#define Q_CRT_STRING_WIDE_TYPE
#define Q_CRT_STRING_NATURAL
#define Q_CRT_STRING_CONVERT
#define Q_CRT_STRING_ENCODE
#define Q_CRT_STRING_FORMAT
#include <q-tee/crt/crt.h>

#include <numbers>

#include "../unit.h"

namespace UNIT::CRT
{
	inline bool AssertConvert()
	{
		CUnitTest test("CRT/CVT");

		constexpr const char* arrIntegerStrings[] =
		{
			"0",
			"123",
			"-456",
			"  789",
			"  -123  ",
			"2147483647", // max int
			"-2147483648", // min int
			"9999999999", // overflow
			"-9999999999", // underflow
			"", // empty string
			"abc", // invalid chars
			"123abc", // trailing chars
			"12.3", // decimal point
			"   " // all whitespace
		};

		// @todo: we must also validate "errno" value
		CUnitTest stringToInteger("CRT/CVT/ATOI");
		for (const auto szNumber : arrIntegerStrings)
			stringToInteger.Add(szNumber, ::CRT::StringToInteger<long>(szNumber), ::strtol(szNumber, nullptr, 10));
		test.Add(stringToInteger.Report(), true);

		CUnitTest integerToString("CRT/CVT/ITOA");
		constexpr std::int32_t arrIntegerNumbers[] =
		{
			0,
			123,
			-456,
			(std::numeric_limits<std::int32_t>::max)(), // max int
			(std::numeric_limits<std::int32_t>::min)() // min int
		};
		char szIntegerResultBuffer[::CRT::IntegerToString_t<std::uint64_t, 2U>::MaxCount()];
		char szIntegerExpectBuffer[::CRT::IntegerToString_t<std::uint64_t, 2U>::MaxCount()];
		for (const auto iNumber : arrIntegerNumbers)
		{
			::sprintf(szIntegerExpectBuffer, "%d", iNumber);
			integerToString.Add(iNumber, ::CRT::IntegerToString<std::int32_t>(iNumber, szIntegerResultBuffer, Q_ARRAYSIZE(szIntegerResultBuffer)), static_cast<char*>(szIntegerExpectBuffer));
		}
		test.Add(integerToString.Report(), true);

		constexpr const char* arrFloatStrings[] =
		{
			// special
			"abc", // no digits
			"0.0",
			"-0.0",
			"inf",
			"-inf",
			"nan",
			"-nan",
			// denormalized
			"1e-324", // min exponent
			"1e308", // max exponent
			"1e-400", // underflow exponent
			"1e400", // overflow exponent
			"4.9406564584124654e-324", // near min denormal
			"0.000001", // precision test
			"8e-16",
			// normalized
			"2.2250738585072014e-308", // smallest normalized
			"2.225073858507201e-308", // round to smallest normalized
			"2.5e-5",
			"123.456",
			"-789.012",
			// large
			"1.7976931348623157e308", // near max double
			"179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368", // near max double full
			"1.0e10", // 10000000000
			"123456789.123456789",
			"1234567890123456789" // no decimal point
		};

		CUnitTest stringToDouble("CRT/CVT/ATOD");
		CUnitTest stringToDoubleError("CRT/CVT/ATOD/ERRN");
		for (const auto szNumber : arrFloatStrings)
		{
			int iResultErrno = 0;
			const double dlResult = ::CRT::StringToReal<double>(szNumber, static_cast<char**>(nullptr), &iResultErrno);
			errno = 0;
			const double dlExpect = ::strtod(szNumber, nullptr);
			const int iExpectErrno = errno;
			stringToDouble.Add(szNumber, dlResult, dlExpect);
			stringToDoubleError.Add(szNumber, iResultErrno, iExpectErrno);
		}
		test.Add(stringToDouble.Report(), true);
		test.Add(stringToDoubleError.Report(), true);

		CUnitTest stringToFloat("CRT/CVT/ATOF");
		CUnitTest stringToFloatError("CRT/CVT/ATOF/ERRN");
		for (const auto szNumber : arrFloatStrings)
		{
			int iResultErrno = 0;
			const float flResult = ::CRT::StringToReal<float>(szNumber, static_cast<char**>(nullptr), &iResultErrno);
			errno = 0;
			const float flExpect = ::strtof(szNumber, nullptr);
			const int iExpectErrno = errno;
			stringToFloat.Add(szNumber, flResult, flExpect);
			stringToFloatError.Add(szNumber, iResultErrno, iExpectErrno);
		}
		test.Add(stringToFloat.Report(), true);
		test.Add(stringToFloatError.Report(), true);

		constexpr double arrDoubleNumbers[] =
		{
			// special
			0.0,
			-0.0,
			std::numeric_limits<double>::infinity(),
			-std::numeric_limits<double>::infinity(),
			std::numeric_limits<double>::quiet_NaN(),
			//-std::numeric_limits<double>::quiet_NaN(), // implementation-defined, may print "-nan(ind)"
			// denormalized
			std::numeric_limits<double>::denorm_min(),
			std::numeric_limits<double>::epsilon(),
			// normalized
			1e-6,
			8e-16,
			2.5e-5,
			-std::numbers::pi_v<double>,
			123.456,
			-789.012,
			// rounding
			0.135,
			0.25,
			0.5,
			0.75,
			0.3049589,
			0.9999999,
			0.9999989,
			0.9099995,
			0.9095994,
			0.9999996,
			// large
			(std::numeric_limits<double>::max)(),
			1e10, // 10000000000
			123456789.123456789,
			1234567890123456789.0
		};

		char szFloatResultBuffer[384];
		char szFloatExpectBuffer[384];

		CUnitTest doubleToString("CRT/CVT/DTOA");
		for (const double dlNumber : arrDoubleNumbers)
		{
			for (int iPrecision = 0; iPrecision < std::numeric_limits<double>::max_digits10; ++iPrecision)
			{
				::sprintf(szFloatExpectBuffer, "%.*f", iPrecision, dlNumber);
				doubleToString.Add(dlNumber, ::CRT::RealToString<double>(dlNumber, szFloatResultBuffer, Q_ARRAYSIZE(szFloatResultBuffer), iPrecision), static_cast<char*>(szFloatExpectBuffer));
			}
		}
		test.Add(doubleToString.Report(), true);

		CUnitTest stringToDoubleRoundTrip("CRT/CVT/DTOA/RNDTRP");
		for (const double dlNumber : arrDoubleNumbers)
		{
			for (int iPrecision = 0; iPrecision < std::numeric_limits<double>::max_digits10; ++iPrecision)
			{
				// convert number to string
				::sprintf(szFloatExpectBuffer, "%.*f", iPrecision, dlNumber);
				const char* szFloatResult = ::CRT::RealToString<double>(dlNumber, szFloatResultBuffer, Q_ARRAYSIZE(szFloatResultBuffer), iPrecision);

				// check if we do produce bit-equivalent value
				stringToDoubleRoundTrip.Add(dlNumber, ::strtod(szFloatResult, nullptr), ::strtod(szFloatExpectBuffer, nullptr));
			}
		}
		test.Add(stringToDoubleRoundTrip.Report(), true);

		constexpr float arrFloatNumbers[] =
		{
			// special
			0.0f,
			-0.0f,
			std::numeric_limits<float>::infinity(),
			-std::numeric_limits<float>::infinity(),
			std::numeric_limits<float>::quiet_NaN(),
			//-std::numeric_limits<double>::quiet_NaN(), // implementation-defined, may print "-nan(ind)"
			// denormalized
			std::numeric_limits<float>::denorm_min(),
			std::numeric_limits<float>::epsilon(),
			// normalized
			1e-6f,
			8e-8f,
			2.5e-5f,
			-std::numbers::pi_v<float>,
			123.456f,
			-789.012f,
			16777219.f,
			// rounding
			0.135f,
			0.25f,
			0.5f,
			0.75f,
			0.3049589f,
			0.9999999f,
			0.9999989f,
			0.9099995f,
			0.9095994f,
			0.9999996f,
			// large
			(std::numeric_limits<float>::max)(),
			3.402823e38f,
			1.234567e35f,
			9.876543e30f,
			5.555555e25f,
			1e10f, // 10000000000
			123456789.123456789f,
			1234567890123456789.f
		};

		CUnitTest floatToString("CRT/CVT/FTOA");
		for (const float flNumber : arrFloatNumbers)
		{
			for (int iPrecision = 0; iPrecision < std::numeric_limits<float>::max_digits10; ++iPrecision)
			{
				::sprintf(szFloatExpectBuffer, "%.*f", iPrecision, flNumber);
				floatToString.Add(flNumber, ::CRT::RealToString<float>(flNumber, szFloatResultBuffer, Q_ARRAYSIZE(szFloatResultBuffer), iPrecision), static_cast<char*>(szFloatExpectBuffer));
			}
		}
		test.Add(floatToString.Report(), true);

		CUnitTest stringToFloatRoundTrip("CRT/CVT/FTOA/RNDTRP");
		for (const float flNumber : arrFloatNumbers)
		{
			for (int iPrecision = 0; iPrecision < std::numeric_limits<float>::max_digits10; ++iPrecision)
			{
				// convert number to string
				::sprintf(szFloatExpectBuffer, "%.*f", iPrecision, flNumber);
				const char* szFloatResult = ::CRT::RealToString<float>(flNumber, szFloatResultBuffer, Q_ARRAYSIZE(szFloatResultBuffer), iPrecision);

				// check if we do produce bit-equivalent value
				stringToFloatRoundTrip.Add(flNumber, ::strtof(szFloatResult, nullptr), ::strtof(szFloatExpectBuffer, nullptr));
			}
		}
		test.Add(stringToFloatRoundTrip.Report(), true);

		// January 15, 2024, Monday, 14:30:45
		tm timePoint =
		{
			.tm_sec = 45,
			.tm_min = 30,
			.tm_hour = 14,
			.tm_mday = 15,
			.tm_mon = 0,
			.tm_year = 124,
			.tm_wday = 1,
			.tm_yday = 14,
			.tm_isdst = 0
		};

		constexpr const char* arrTimeFormat[] =
		{
			// single
			"%a",
			"%A",
			"%b",
			"%B",
			"%c",
			"%C",
			"%d",
			"%D",
			"%e",
			"%F",
			"%g",
			"%G",
			"%h",
			"%H",
			"%I",
			"%j",
			"%m",
			"%M",
			"%n",
			"%p",
			"%r",
			"%R",
			"%S",
			"%t",
			"%T",
			"%u",
			"%U",
			"%V",
			"%w",
			"%W",
			"%x",
			"%X",
			"%y",
			"%Y",
			//"%z",	// @note: implementation-defined
			//"%Z",	// @note: implementation-defined
			"%%",
			// custom
			"abc def", // no token
			"%A, %B %d, %Y",
			"Today at %I:%M %p",
		};

		char szTimeResultBuffer[128];
		char szTimeExpectBuffer[128];

		CUnitTest timeToString("CRT/CVT/TTOA");
		for (const auto szTimeFormat : arrTimeFormat)
		{
			::CRT::TimeToString(szTimeResultBuffer, Q_ARRAYSIZE(szTimeResultBuffer), szTimeFormat, &timePoint);
			::strftime(szTimeExpectBuffer, sizeof(szTimeExpectBuffer), szTimeFormat, &timePoint);
			timeToString.Add(szTimeFormat, szTimeResultBuffer, szTimeExpectBuffer);
		}

		// add special cases
		// test last day of the year (Dec 31, 2024)
		timePoint.tm_mday = 31;
		timePoint.tm_mon = 11;
		timePoint.tm_wday = 2;
		timePoint.tm_yday = 365;
		::CRT::TimeToString(szTimeResultBuffer, Q_ARRAYSIZE(szTimeResultBuffer), "%j", &timePoint);
		::strftime(szTimeExpectBuffer, sizeof(szTimeExpectBuffer), "%j", &timePoint);
		timeToString.Add("last day of the year (%j)", szTimeResultBuffer, szTimeExpectBuffer);
		// test for leap year day (Feb 29, 2024)
		timePoint.tm_mday = 29;
		timePoint.tm_mon = 1;
		timePoint.tm_wday = 4;
		timePoint.tm_yday = 59;
		::CRT::TimeToString(szTimeResultBuffer, Q_ARRAYSIZE(szTimeResultBuffer), "%B %d, %Y", &timePoint);
		::strftime(szTimeExpectBuffer, sizeof(szTimeExpectBuffer), "%B %d, %Y", &timePoint);
		timeToString.Add("day of the leap year (%B %d, %Y)", szTimeResultBuffer, szTimeExpectBuffer);
		test.Add(timeToString.Report(), true);

		// test for strict buffer size and return value
		CUnitTest timeToStringReturn("CRT/CVT/TTOA/RET");
		std::memset(szTimeResultBuffer, 0xCC, sizeof(szTimeResultBuffer));
		std::memset(szTimeExpectBuffer, 0xCC, sizeof(szTimeExpectBuffer));
		for (std::size_t nSize = 1U; nSize < 32U; ++nSize)
		{
			const std::size_t nTimeWrittenResult = ::CRT::TimeToString(szTimeResultBuffer, nSize, "%a %b %e %H:%M:%S %Y", &timePoint);
			const std::size_t nTimeWrittenExpect = ::strftime(szTimeExpectBuffer, nSize, "%a %b %e %H:%M:%S %Y", &timePoint);
			timeToStringReturn.Add("strict buffer return (%a %b %e %H:%M:%S %Y)", nTimeWrittenResult, nTimeWrittenExpect);
			timeToStringReturn.Add("strict buffer (%a %b %e %H:%M:%S %Y)", static_cast<int>(szTimeResultBuffer[nSize]), static_cast<int>(szTimeExpectBuffer[nSize]));
		}
		test.Add(timeToStringReturn.Report(), true);

		return test.Report();
	}

	inline bool AssertCharacterType()
	{
		CUnitTest test("CRT/CTYPE");

		for (int nCodePoint = 0; nCodePoint < 255; ++nCodePoint)
		{
			bool bResult = true;

			const int nControl = ::iscntrl(nCodePoint);
			bResult &= ::CRT::IsControl(nCodePoint) == (nControl != 0);
			const int nSpace = ::isspace(nCodePoint);
			bResult &= ::CRT::IsSpace(nCodePoint) == (nSpace != 0);
			const int nBlank = ::isblank(nCodePoint);
			bResult &= ::CRT::IsBlank(nCodePoint) == (nBlank != 0);
			const int nPunct = ::ispunct(nCodePoint);
			bResult &= ::CRT::IsPunct(nCodePoint) == (nPunct != 0);
			const int nDigit = ::isdigit(nCodePoint);
			bResult &= ::CRT::IsDigit(nCodePoint) == (nDigit != 0);
			const int nHex = ::isxdigit(nCodePoint);
			bResult &= ::CRT::IsHexDigit(nCodePoint) == (nHex != 0);
			const int nUpper = ::isupper(nCodePoint);
			bResult &= ::CRT::IsUpper(nCodePoint) == (nUpper != 0);
			const int nLower = ::islower(nCodePoint);
			bResult &= ::CRT::IsLower(nCodePoint) == (nLower != 0);
			const int nAlpha = ::isalpha(nCodePoint);
			bResult &= ::CRT::IsAlpha(nCodePoint) == (nAlpha != 0);
			const int nAlNum = ::isalnum(nCodePoint);
			bResult &= ::CRT::IsAlphaNum(nCodePoint) == (nAlNum != 0);
			const int nPrint = ::isprint(nCodePoint);
			bResult &= ::CRT::IsPrint(nCodePoint) == (nPrint != 0);
			const int nGraph = ::isgraph(nCodePoint);
			bResult &= ::CRT::IsGraph(nCodePoint) == (nGraph != 0);

			test.Add(nCodePoint, bResult, true);
		}

		return test.Report();
	}

	inline bool AssertWideCharacterType()
	{
		CUnitTest test("CRT/WCTYPE");

		for (wint_t nCodePoint = 0U; nCodePoint < 0xFFFF; ++nCodePoint)
		{
			bool bResult = true;

			const int nControl = ::iswcntrl(nCodePoint);
			bResult &= ::CRT::IsControl(nCodePoint) == (nControl != 0);
			const int nSpace = ::iswspace(nCodePoint);
			bResult &= ::CRT::IsSpace(nCodePoint) == (nSpace != 0);
			const int nBlank = ::iswblank(nCodePoint);
			bResult &= ::CRT::IsBlank(nCodePoint) == (nBlank != 0);
			const int nPunct = ::iswpunct(nCodePoint);
			bResult &= ::CRT::IsPunct(nCodePoint) == (nPunct != 0);
			const int nDigit = ::iswdigit(nCodePoint);
			bResult &= ::CRT::IsDigit(nCodePoint) == (nDigit != 0);
			const int nHex = ::iswxdigit(nCodePoint);
			bResult &= ::CRT::IsHexDigit(nCodePoint) == (nHex != 0);
			const int nUpper = ::iswupper(nCodePoint);
			bResult &= ::CRT::IsUpper(nCodePoint) == (nUpper != 0);
			const int nLower = ::iswlower(nCodePoint);
			bResult &= ::CRT::IsLower(nCodePoint) == (nLower != 0);
			const int nAlpha = ::iswalpha(nCodePoint);
			bResult &= ::CRT::IsAlpha(nCodePoint) == (nAlpha != 0);
			const int nAlNum = ::iswalnum(nCodePoint);
			bResult &= ::CRT::IsAlphaNum(nCodePoint) == (nAlNum != 0);
			const int nPrint = ::iswprint(nCodePoint);
			bResult &= ::CRT::IsPrint(nCodePoint) == (nPrint != 0);
			const int nGraph = ::iswgraph(nCodePoint);
			bResult &= ::CRT::IsGraph(nCodePoint) == (nGraph != 0);

			//(nControl | nSpace | nBlank | nPunct | nDigit | nHex | nUpper | nLower | nAlpha | nAlNum | nPrint | nGraph) != 0

			if (bResult)
				test.Add(nCodePoint, bResult, true);
			else
			{
				std::uint8_t uExpectedFlags = 0U;
				if (nControl)
					uExpectedFlags |= ::CRT::TYPE_CONTROL;
				if (nSpace)
					uExpectedFlags |= ::CRT::TYPE_SPACE;
				if (nBlank)
					uExpectedFlags |= ::CRT::TYPE_BLANK;
				if (nPunct)
					uExpectedFlags |= ::CRT::TYPE_PUNCT;
				if (nDigit)
					uExpectedFlags |= ::CRT::TYPE_DIGIT;
				//if (nHex)
				//	uExpectedFlags |= ::CRT::TYPE_HEX;
				if (nLower)
					uExpectedFlags |= ::CRT::TYPE_LOWER;
				if (nUpper)
					uExpectedFlags |= ::CRT::TYPE_UPPER;
				if (nAlpha)
					uExpectedFlags |= ::CRT::TYPE_ALPHA;

				const auto uFlags = ::CRT::arrWideCharacterTypeLUT[::CRT::arrWideCharacterTypeOffsets[nCodePoint >> 5U] + (nCodePoint & 0x1F)];
				test.Add(nCodePoint, uFlags, uExpectedFlags);
			}
			
		}

		return test.Report();
	}

	inline bool Assert()
	{
		bool bResult = true;
		bResult &= AssertConvert();
		bResult &= AssertCharacterType();
		bResult &= AssertWideCharacterType();
		return bResult;
	}

	consteval bool AssertConst()
	{
		bool bResult = true;
		bResult &= ::CRT::StringNaturalCompare("test 1.txt", "test 1.txt") == 0;
		bResult &= ::CRT::StringNaturalCompare("a10", "a20") < 0;
		bResult &= ::CRT::StringNaturalCompare("1.002", "1.001") > 0;
		return bResult;
	}
}