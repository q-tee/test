#pragma once
#include <q-tee/common/common.h>

#define Q_CRT_MEMORY_CRYPTO
#define Q_CRT_STRING_WIDE_TYPE
#define Q_CRT_STRING_NATURAL
#define Q_CRT_STRING_CONVERT
#define Q_CRT_STRING_ENCODE
#define Q_CRT_STRING_FORMAT
#include <q-tee/crt/crt.h>

#include <cinttypes>
#include <numbers>

#include "../unit.h"

namespace UNIT::CRT
{
	inline bool AssertMemory()
	{
		CUnitTest test("CRT/MEM");

		{
			CUnitTest compare("CRT/MEM/CMP");

			// naive
			const std::uint8_t arrNaiveLeft[] = "abcdef";
			const std::uint8_t arrNaiveRight[] = "abcdez";
			compare.Equal(::CRT::MemoryCompare(arrNaiveLeft, arrNaiveRight, 0U), 0); // no size
			compare.Equal(::CRT::MemoryCompare(arrNaiveLeft, arrNaiveRight, 5U), 0); // exact match
			compare.Equal(::CRT::MemoryCompare(arrNaiveRight, arrNaiveLeft, 5U), 0); // exact match
			compare.Equal(::CRT::MemoryCompare(arrNaiveLeft, arrNaiveRight, 6U) < 0, true); // differs at N

			// unsigned comparison
			const std::uint8_t arrUnsignedLeft[] = { 0xFF, 0x80 };
			const std::uint8_t arrUnsignedRight[] = { 0x01, 0x80 };
			compare.Equal(::CRT::MemoryCompare(arrUnsignedLeft, arrUnsignedRight, 2U) > 0, true);
			compare.Equal(::CRT::MemoryCompare(arrUnsignedRight, arrUnsignedLeft, 2U) < 0, true);

			// chunk alignment
			const char arrAlignNone[] = "AAAAAAAAAAAAAAAA"; // oword of 'A'
			const char arrAlignStart[] = "BAAAAAAAAAAAAAAA"; // mismatch at index 0
			const char arrAlignMiddle[] = "AAAAAAABAAAAAAAA"; // mismatch at index 7 (end of first 8-byte word)
			const char arrAlignTrail[] = "AAAAAAAAAAAAAAAB"; // mismatch at index 15
			// exact
			compare.Equal(::CRT::MemoryCompare(arrAlignNone, arrAlignNone, 0U), 0);
			compare.Equal(::CRT::MemoryCompare(arrAlignNone, arrAlignNone, 16U), 0);
			// mismatch in the trailing byte
			compare.Equal(::CRT::MemoryCompare(arrAlignNone, arrAlignTrail, 4U), 0);
			compare.Equal(::CRT::MemoryCompare(arrAlignNone, arrAlignTrail, 8U), 0);
			compare.Equal(::CRT::MemoryCompare(arrAlignNone, arrAlignTrail, 15U), 0);
			compare.Equal(::CRT::MemoryCompare(arrAlignNone, arrAlignTrail, 16U) < 0, true);
			// early chunk break
			compare.Equal(::CRT::MemoryCompare(arrAlignNone, arrAlignStart, 16U) < 0, true);
			compare.Equal(::CRT::MemoryCompare(arrAlignNone, arrAlignMiddle, 16U) < 0, true);

			// large buffer
			std::uint8_t arrLargeLeft[256];
			std::uint8_t arrLargeRight[256];
			for (int i = 0; i < 256; ++i)
			{
				arrLargeLeft[i] = i;
				arrLargeRight[i] = i;
			}
			compare.Equal(::CRT::MemoryCompare(arrLargeLeft, arrLargeRight, 256U), 0);
			// last byte
			arrLargeRight[255] = 0;
			compare.Equal(::CRT::MemoryCompare(arrLargeLeft, arrLargeRight, 256U) > 0, true);
			arrLargeRight[255] = 255;
			// 3rd 8-byte chunk
			arrLargeRight[24] = 0;
			compare.Equal(::CRT::MemoryCompare(arrLargeLeft, arrLargeRight, 256U) > 0, true);

			test.Add(compare);
		}

		{
			CUnitTest search("CRT/MEM/CHR");

			const std::uint8_t arrNaive[] = "XXYXXYXD";
			search.Equal(arrNaive, ::CRT::MemoryChar(arrNaive, 'X', 0U), nullptr); // no size
			search.Equal(arrNaive, ::CRT::MemoryChar(arrNaive, 'Z', 8U), nullptr); // not found
			search.Equal(arrNaive, ::CRT::MemoryChar(arrNaive, 'Y', 8U), &arrNaive[2]); // first occurence
			search.Equal(arrNaive, ::CRT::MemoryChar(arrNaive, 'D', 8U), &arrNaive[7]); // exact match at the very end

			// unsigned comparison
			const std::uint8_t arrUnsigned[] = { 0x00, 0x7F, 0x80, 0xFF, 0x01 };
			search.Equal(::CRT::MemoryChar(arrUnsigned, 0x80, 5U), &arrUnsigned[2]);
			search.Equal(::CRT::MemoryChar(arrUnsigned, 0xFF, 5U), &arrUnsigned[3]);

			// unaligned
			std::uint8_t arrUnaligned[32];
			::memset(arrUnaligned, 'Z', 32U);
			arrUnaligned[12] = 'X';
			search.Equal(::CRT::MemoryChar(arrUnaligned + 1, 'X', 31), &arrUnaligned[12]); // start at offset +1 (3/7 bytes before next boundary)
			search.Equal(::CRT::MemoryChar(arrUnaligned + 5, 'X', 27), &arrUnaligned[12]); // start at offset +5 (1/3 bytes after/before next boundary)

			// xor false positive
			std::uint8_t arrXor[16] = { 0x81, 0x7F, 0x80, 0xFE, 0xFF, 0x42 };
			search.Equal(::CRT::MemoryChar(arrXor, 0x42, 16U), &arrXor[5]);

			test.Add(search);
		}

		{
			CUnitTest search("CRT/MEM/MEM");

			// boundary matches
			const char arrSource[] = "PREFIX_AND_SUFFIX";
			search.Equal(::CRT::MemoryMemory(arrSource, 10U, "PREFIX_AND", 10U), arrSource); // search length is exactly equal to Source length
			search.Equal(::CRT::MemoryMemory(arrSource, 10U, "PREFIX_ANY", 10U), nullptr); // last char differs
			search.Equal(::CRT::MemoryMemory(arrSource, 17U, "PRE", 3U), &arrSource[0]); // exact match at the beginning
			search.Equal(::CRT::MemoryMemory(arrSource + 7, 10U, "FIX", 3U), &arrSource[14]); // exact match at the very end
			search.Equal(::CRT::MemoryMemory(arrSource, 16U, "SUFFIX", 6U), nullptr); // boundary limit
			search.Equal(::CRT::MemoryMemory(arrSource, 10U, "NONE", 0U), nullptr); // search length is null @test: this is not POSIX compliant, even though FreeBSD behaviour is same
			search.Equal(::CRT::MemoryMemory(arrSource, 0U, "NONE", 0U), nullptr); // source and search length is null @test: this is not POSIX compliant, even though FreeBSD behaviour is same
			search.Equal(::CRT::MemoryMemory(arrSource, 3U, "PREFIX", 6U), nullptr); // search length is greater than source length
			// false positive prefix recovery
			const char arrSourcePrefix[] = "ababababc";
			search.Equal(::CRT::MemoryMemory(arrSourcePrefix, 9U, "ababc", 5U), &arrSourcePrefix[4]);
			// overlapping sub-patterns
			const char arrSourceOverlap[] = "AAAAA";
			search.Equal(::CRT::MemoryMemory(arrSourceOverlap, 5U, "AAA", 3U), &arrSourceOverlap[0]);
			// binary data
			const std::uint8_t arrSourceBin[] = { 0x01, 0x00, 0x02, 0x00, 0x03, 0x04 };
			const std::uint8_t arrSearchBin[] = { 0x02, 0x00, 0x03 };
			search.Equal(::CRT::MemoryMemory(arrSourceBin, 6U, arrSearchBin, 3U), &arrSourceBin[2]);
			// searching for nulls specifically
			const std::uint8_t arrSearchNull[] = { 0x00, 0x00 };
			search.Equal(::CRT::MemoryMemory(arrSourceBin, 6U, arrSearchNull, 2U), nullptr); // doesn't exist contiguously

			test.Add(search);
		}

		const std::size_t arrSizes[] = { 1, 2, 4, 8, 15, 16, 17, 31, 32 };
		const std::size_t arrOffsets[] = { 0, 1, 3, 7, 15 };
		const std::uint8_t arrValues[] = { 0x00, 0x7F, 0x80, 0xFE, 0xFF };

		{
			CUnitTest set("CRT/MEM/SET");

			std::uint8_t arrBuffer[1024];
			std::uint8_t arrCompare[1024];
			
			for (const auto& nSize : arrSizes)
			{
				for (const auto& nOffset : arrOffsets)
				{
					for (const auto& uValue : arrValues)
					{
						::memset(arrCompare, uValue, nSize);
						set.Equal(::CRT::MemorySet(arrBuffer + nOffset, uValue, nSize), arrBuffer + nOffset + nSize);
						set.Equal(::memcmp(arrBuffer + nOffset, arrCompare, nSize), 0);
					}
				}
			}

			::memset(arrBuffer, 0, 1024U);
			::memset(arrCompare, 0, 1024U);

			::memset(arrCompare, 0x77, 1003U);
			set.Equal(::CRT::MemorySet(arrBuffer, 0x77, 1003U), arrBuffer + 1003U); // (62 SIMD blocks + qword + word + byte)
			set.Equal(::memcmp(arrBuffer, arrCompare, 1024U), 0);

			test.Add(set);
		}

		{
			CUnitTest copy("CRT/MEM/CPY");

			std::uint8_t arrBuffer[1024];
			std::uint8_t arrCopy[1024];

			std::size_t nPseudoCounter = 1U;
			for (const auto& nSize : arrSizes)
			{
				nPseudoCounter *= nSize + 0x5A6B;

				for (const auto& nOffset : arrOffsets)
				{
					nPseudoCounter += nOffset * 0xBF07;
					
					for (std::size_t i = 0U; i < nSize; ++i)
					{
						nPseudoCounter += i * 33U;

						arrCopy[i] = nPseudoCounter & 0xFF;
					}

					copy.Equal(::CRT::MemoryCopy(arrBuffer + nOffset, arrCopy, nSize), arrBuffer + nOffset + nSize);
					copy.Equal(::memcmp(arrBuffer + nOffset, arrCopy, nSize), 0);
				}
			}

			::memset(arrBuffer, 0, 1024U);
			::memset(arrCopy, 0, 1024U);
			for (std::size_t i = 0U; i < 1003U; ++i)
			{
				nPseudoCounter += i * 0x8F2043E8;

				arrCopy[i] = nPseudoCounter & 0xFF;
			}
			

			copy.Equal(::CRT::MemoryCopy(arrBuffer, arrCopy, 1003U), arrBuffer + 1003U); // (62 SIMD blocks + qword + word + byte)
			copy.Equal(::memcmp(arrBuffer, arrCopy, 1024U), 0);

			test.Add(copy);
		}

		return test.Report();
	}

	inline bool AssertString()
	{
		CUnitTest test("CRT/STR");

		{
			CUnitTest length("CRT/STR/LEN");

			alignas(8) char szAligned[32];
			::memset(szAligned, 'A', 32U);

			// perfectly aligned
			szAligned[0] = '\0';
			length.Equal(::CRT::StringLength(szAligned), 0U);
			szAligned[0] = 'A';
			// misaligned by 1, null found during alignment
			szAligned[1] = '\0';
			length.Equal(::CRT::StringLength(szAligned), 1U);
			szAligned[1] = 'A';
			// misaligned by 7, null found right before block loop
			szAligned[7] = '\0';
			length.Equal(::CRT::StringLength(szAligned), 7U);

			// exact word boundaries
			::memset(szAligned, 'B', 32U);
			// null is first byte of the second chunk
			szAligned[8] = '\0';
			length.Equal(::CRT::StringLength(szAligned), 8U);
			szAligned[8] = 'B';
			// null is last byte of the second chunk
			szAligned[15] = '\0';
			length.Equal(::CRT::StringLength(szAligned), 15U);

			char szUnaligned[18];
			::memset(szUnaligned + 3, 'U', 15U);

			// terminate at index 0 (perfectly aligned)
			szUnaligned[3] = '\0';
			length.Equal(::CRT::StringLength(szUnaligned + 3), 0U);
			szUnaligned[3] = 'U';
			// terminate at index 1 (misaligned by 1, null found during alignment)
			szUnaligned[4] = '\0';
			length.Equal(::CRT::StringLength(szUnaligned + 3), 1U);
			szUnaligned[4] = 'U';
			// terminate at index 7 (misaligned by 7, null found right before block loop)
			szUnaligned[10] = '\0';
			length.Equal(::CRT::StringLength(szUnaligned + 3), 7U);

			// exact word boundaries
			::memset(szUnaligned + 3, 'B', 15U);
			// null is first byte of the second chunk
			szUnaligned[11] = '\0';
			length.Equal(::CRT::StringLength(szUnaligned + 3), 8U);
			szUnaligned[11] = 'B';
			// null is pre-last byte of the second chunk
			szUnaligned[17] = '\0';
			length.Equal(::CRT::StringLength(szUnaligned + 3), 14U);

			// high-bit / signed-char false zero trap
			char arrHighBit[16] = "\x80\xFF\x7F\x01\x80\xFF\x7F\x01";
			length.Equal(::CRT::StringLength(arrHighBit), 8U);

			// consecutive nulls
			char arrMultiNull[16] = "AAAAAAAAA\0\0\0"; // first block is solid
			length.Equal(::CRT::StringLength(arrMultiNull), 9U);

			// large string
			char arrLarge[1024];
			::memset(arrLarge, 'X', 1024);
			arrLarge[1000] = '\0';
			length.Equal(::CRT::StringLength(arrLarge), 1000);

			test.Add(length);
		}

		return test.Report();
	}

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
			stringToInteger.Equal(szNumber, ::CRT::StringToInteger<long>(szNumber), ::strtol(szNumber, nullptr, 10));
		test.Equal(stringToInteger.Report(), true);

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
			::sprintf(szIntegerExpectBuffer, "%" PRId32, iNumber);
			integerToString.EqualString(iNumber, ::CRT::IntegerToString<std::int32_t>(iNumber, szIntegerResultBuffer, Q_ARRAYSIZE(szIntegerResultBuffer)), static_cast<char*>(szIntegerExpectBuffer));
		}
		test.Add(integerToString);

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

			stringToDouble.Equal(szNumber, dlResult, dlExpect);
			stringToDoubleError.Equal(szNumber, iResultErrno, iExpectErrno);
		}
		test.Add(stringToDouble);
		test.Add(stringToDoubleError);

		CUnitTest stringToFloat("CRT/CVT/ATOF");
		CUnitTest stringToFloatError("CRT/CVT/ATOF/ERRN");
		for (const auto szNumber : arrFloatStrings)
		{
			int iResultErrno = 0;
			const float flResult = ::CRT::StringToReal<float>(szNumber, static_cast<char**>(nullptr), &iResultErrno);

			errno = 0;
			const float flExpect = ::strtof(szNumber, nullptr);
			const int iExpectErrno = errno;

			stringToFloat.Equal(szNumber, flResult, flExpect);
			stringToFloatError.Equal(szNumber, iResultErrno, iExpectErrno);
		}
		test.Add(stringToFloat);
		test.Add(stringToFloatError);

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
				doubleToString.EqualString(dlNumber, ::CRT::RealToString<double>(dlNumber, szFloatResultBuffer, Q_ARRAYSIZE(szFloatResultBuffer), iPrecision), static_cast<char*>(szFloatExpectBuffer));
			}
		}
		test.Add(doubleToString);

		CUnitTest stringToDoubleRoundTrip("CRT/CVT/DTOA/RNDTRP");
		for (const double dlNumber : arrDoubleNumbers)
		{
			for (int iPrecision = 0; iPrecision < std::numeric_limits<double>::max_digits10; ++iPrecision)
			{
				// convert number to string
				::sprintf(szFloatExpectBuffer, "%.*f", iPrecision, dlNumber);
				const char* szFloatResult = ::CRT::RealToString<double>(dlNumber, szFloatResultBuffer, Q_ARRAYSIZE(szFloatResultBuffer), iPrecision);

				// check if we do produce bit-equivalent value
				stringToDoubleRoundTrip.Equal(dlNumber, ::strtod(szFloatResult, nullptr), ::strtod(szFloatExpectBuffer, nullptr));
			}
		}
		test.Add(stringToDoubleRoundTrip);

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
				floatToString.EqualString(flNumber, ::CRT::RealToString<float>(flNumber, szFloatResultBuffer, Q_ARRAYSIZE(szFloatResultBuffer), iPrecision), static_cast<char*>(szFloatExpectBuffer));
			}
		}
		test.Add(floatToString);

		CUnitTest stringToFloatRoundTrip("CRT/CVT/FTOA/RNDTRP");
		for (const float flNumber : arrFloatNumbers)
		{
			for (int iPrecision = 0; iPrecision < std::numeric_limits<float>::max_digits10; ++iPrecision)
			{
				// convert number to string
				::sprintf(szFloatExpectBuffer, "%.*f", iPrecision, flNumber);
				const char* szFloatResult = ::CRT::RealToString<float>(flNumber, szFloatResultBuffer, Q_ARRAYSIZE(szFloatResultBuffer), iPrecision);

				// check if we do produce bit-equivalent value
				stringToFloatRoundTrip.Equal(flNumber, ::strtof(szFloatResult, nullptr), ::strtof(szFloatExpectBuffer, nullptr));
			}
		}
		test.Add(stringToFloatRoundTrip);

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
			timeToString.EqualString(szTimeFormat, szTimeResultBuffer, szTimeExpectBuffer);
		}

		// add special cases
		// test last day of the year (Dec 31, 2024)
		timePoint.tm_mday = 31;
		timePoint.tm_mon = 11;
		timePoint.tm_wday = 2;
		timePoint.tm_yday = 365;
		::CRT::TimeToString(szTimeResultBuffer, Q_ARRAYSIZE(szTimeResultBuffer), "%j", &timePoint);
		::strftime(szTimeExpectBuffer, sizeof(szTimeExpectBuffer), "%j", &timePoint);
		timeToString.EqualString("last day of the year (%j)", szTimeResultBuffer, szTimeExpectBuffer);
		// test for leap year day (Feb 29, 2024)
		timePoint.tm_mday = 29;
		timePoint.tm_mon = 1;
		timePoint.tm_wday = 4;
		timePoint.tm_yday = 59;
		::CRT::TimeToString(szTimeResultBuffer, Q_ARRAYSIZE(szTimeResultBuffer), "%B %d, %Y", &timePoint);
		::strftime(szTimeExpectBuffer, sizeof(szTimeExpectBuffer), "%B %d, %Y", &timePoint);
		timeToString.EqualString("day of the leap year (%B %d, %Y)", szTimeResultBuffer, szTimeExpectBuffer);
		test.Add(timeToString);

		// test for strict buffer size and return value
		CUnitTest timeToStringReturn("CRT/CVT/TTOA/RET");
		std::memset(szTimeResultBuffer, 0xCC, sizeof(szTimeResultBuffer));
		std::memset(szTimeExpectBuffer, 0xCC, sizeof(szTimeExpectBuffer));
		for (std::size_t nSize = 1U; nSize < 32U; ++nSize)
		{
			const std::size_t nTimeWrittenResult = ::CRT::TimeToString(szTimeResultBuffer, nSize, "%a %b %e %H:%M:%S %Y", &timePoint);
			const std::size_t nTimeWrittenExpect = ::strftime(szTimeExpectBuffer, nSize, "%a %b %e %H:%M:%S %Y", &timePoint);
			timeToStringReturn.Equal("strict buffer return (%a %b %e %H:%M:%S %Y)", nTimeWrittenResult, nTimeWrittenExpect);
			timeToStringReturn.Equal("strict buffer (%a %b %e %H:%M:%S %Y)", szTimeResultBuffer[nSize], szTimeExpectBuffer[nSize]);
		}
		test.Add(timeToStringReturn);

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

			test.Equal(nCodePoint, bResult, true);
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
				test.Equal(nCodePoint, bResult, true);
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
				test.Equal(nCodePoint, uFlags, uExpectedFlags);
			}
		}

		return test.Report();
	}

	inline bool Assert()
	{
		bool bResult = true;
		bResult &= AssertMemory();
		bResult &= AssertString();
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