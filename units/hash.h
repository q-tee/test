#pragma once
#include <q-tee/common/common.h>

#include <iomanip>
#include <iostream>
#include <unordered_map>

#ifdef Q_OS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include "../unit.h"

#include <q-tee/hash/crc32.h>
#include <q-tee/hash/djb2.h>
#include <q-tee/hash/djb2a.h>
#include <q-tee/hash/fnv1a.h>
#include <q-tee/hash/fnv1a_64.h>
#include <q-tee/hash/murmur2.h>
#include <q-tee/hash/murmur2a.h>
#include <q-tee/hash/murmur2_64.h>
#include <q-tee/hash/murmur3.h>

namespace UNIT::HASH
{
	constexpr std::uint32_t uSeed32 = 0x11121314;
	constexpr std::uint64_t ullSeed64 = 0x1112131415161718;
	constexpr const char* szSampleText = "Curabitur luctus orci id ligula fringilla, ut tincidunt tortor feugiat. Duis hendrerit euismod varius. Praesent id nulla rutrum, tincidunt lorem ullamcorper, sagittis purus. Aenean semper neque in ipsum commodo egestas. Ut luctus rhoncus nunc at efficitur.";

	template <typename T>
	struct HashEntry_t
	{
		const char* szSource;
		T(*fnHash)(const char*, const T);
		T uSeed;
		T uExpected;
	};

	const std::array<HashEntry_t<std::uint32_t>, 39U> arrHash32 =
	{{
		// empty string, default seed
		{ "", CRC32::Hash, 0U, 0U },
		{ "", DJB2::Hash, Q_HASH_DJB2_BASIS, Q_HASH_DJB2_BASIS },
		{ "", DJB2A::Hash, Q_HASH_DJB2A_BASIS, Q_HASH_DJB2A_BASIS },
		{ "", FNV1A::Hash, Q_HASH_FNV1A_BASIS, Q_HASH_FNV1A_BASIS },
		{ "", MURMUR2::Hash, 0U, 0U },
		{ "", MURMUR2A::Hash, 0U, 0U },
		{ "", MURMUR3::Hash, 0U, 0U },

		// 1 byte long string, default seed
		{ "a", MURMUR2::Hash, 0U, 0x92685F5E },
		{ "a", MURMUR2A::Hash, 0U, 0x0803888B },
		{ "a", MURMUR3::Hash, 0U, 0x3C2569B2 },

		// 2 bytes long string, default seed
		{ "ab", MURMUR2::Hash, 0U, 0x1AA14063 },
		{ "ab", MURMUR2A::Hash, 0U, 0x618515AF },
		{ "ab", MURMUR3::Hash, 0U, 0x9BBFD75F },

		// 3 bytes long string, default seed
		{ "abc", MURMUR2::Hash, 0U, 0x13577C9B },
		{ "abc", MURMUR2A::Hash, 0U, 0x11589F67 },
		{ "abc", MURMUR3::Hash, 0U, 0xB3DD93FA },

		// 4 bytes long string, default seed
		{ "test", CRC32::Hash, 0U, 0xD87F7E0C },
		{ "test", DJB2::Hash, Q_HASH_DJB2_BASIS, 0x7C9E6865 },
		{ "test", DJB2A::Hash, Q_HASH_DJB2A_BASIS, 0x7C73AF33 },
		{ "test", FNV1A::Hash, Q_HASH_FNV1A_BASIS, 0xAFD071E5 },
		{ "test", MURMUR2::Hash, 0U, 0x1812752E },
		{ "test", MURMUR2A::Hash, 0U, 0x3D31CCC8 },
		{ "test", MURMUR3::Hash, 0U, 0xBA6BD213 },

		// 9 bytes long string, default seed
		{ "testtesta", MURMUR2::Hash, 0U, 0x1C900135 },
		{ "testtesta", MURMUR2A::Hash, 0U, 0x46401C3F },
		{ "testtesta", MURMUR3::Hash, 0U, 0xD65B33A1 },

		// 10 bytes long string, default seed
		{ "testtestab", MURMUR2::Hash, 0U, 0x869B7B21 },
		{ "testtestab", MURMUR2A::Hash, 0U, 0xD5AC0E57 },
		{ "testtestab", MURMUR3::Hash, 0U, 0xE87CA250 },

		// 11 bytes long string, default seed
		{ "testtestabc", MURMUR2::Hash, 0U, 0x90841D17 },
		{ "testtestabc", MURMUR2A::Hash, 0U, 0x0F1321F9 },
		{ "testtestabc", MURMUR3::Hash, 0U, 0x9F633890 },

		// 256 bytes long string, zero seed
		{ szSampleText, CRC32::Hash, 0U, 0x537C04FC },
		{ szSampleText, DJB2::Hash, Q_HASH_DJB2_BASIS, 0xC6D71B82 },
		{ szSampleText, DJB2A::Hash, Q_HASH_DJB2A_BASIS, 0x1037DD42 },
		{ szSampleText, FNV1A::Hash, Q_HASH_FNV1A_BASIS, 0x8C920276 },
		{ szSampleText, MURMUR2::Hash, 0U, 0x1E68204F },
		{ szSampleText, MURMUR2A::Hash, 0U, 0xE998A23D },
		{ szSampleText, MURMUR3::Hash, 0U, 0x7BB926E5 },
	}};

	const std::array<HashEntry_t<std::uint64_t>, 12U> arrHash64 =
	{{
		// empty string, default seed
		{ "", FNV1A_64::Hash, Q_HASH_FNV1A_64_BASIS, Q_HASH_FNV1A_64_BASIS },
		{ "", MURMUR2_64::Hash, 0ULL, 0ULL },

		// 1 byte long string, default seed
		{ "a", MURMUR2_64::Hash, 0ULL, 0x071717D2D36B6B11 },

		// 2 bytes long string, default seed
		{ "ab", MURMUR2_64::Hash, 0ULL, 0x62BE85B2FE53D1F8 },

		// 3 bytes long string, default seed
		{ "abc", MURMUR2_64::Hash, 0ULL, 0x9CC9C33498A95EFB },

		// 4 bytes long string, default seed
		{ "test", FNV1A_64::Hash, Q_HASH_FNV1A_64_BASIS, 0xF9E6E6EF197C2B25 },
		{ "test", MURMUR2_64::Hash, 0ULL, 0x2F4A8724618F4C63 },

		// 9 bytes long string, default seed
		{ "testtesta", MURMUR2_64::Hash, 0ULL, 0xF55BBD374C2B1C93 },

		// 10 bytes long string, default seed
		{ "testtestab", MURMUR2_64::Hash, 0ULL, 0x0ACB80F405D605A1 },

		// 11 bytes long string, default seed
		{ "testtestabc", MURMUR2_64::Hash, 0ULL, 0xF38CC8FBC9693C1E },

		// 256 bytes long string, default seed
		{ szSampleText, FNV1A_64::Hash, Q_HASH_FNV1A_64_BASIS, 0x4D7B3D012FBC0FD6 },
		{ szSampleText, MURMUR2_64::Hash, 0ULL, 0x714E8D7FF06785CD },
	}};

	inline bool Assert()
	{
		CUnitTest test("HASH/RT");

		for (const auto& [szSource, fnHash, uSeed, uExpected] : arrHash32)
			test.Add(szSource, fnHash(szSource, uSeed), uExpected);

		for (const auto& [szSource, fnHash, ullSeed, ullExpected] : arrHash64)
			test.Add(szSource, fnHash(szSource, ullSeed), ullExpected);

		return test.Report();
	}

	consteval bool AssertConst()
	{
		bool bResult = true;

		// 32-bit, empty string, default seed
		bResult &= (CRC32::HashConst("") == 0U);
		bResult &= (DJB2::HashConst("") == Q_HASH_DJB2_BASIS);
		bResult &= (DJB2A::HashConst("") == Q_HASH_DJB2A_BASIS);
		bResult &= (FNV1A::HashConst("") == Q_HASH_FNV1A_BASIS);
		bResult &= (MURMUR2::HashConst("") == 0U);
		bResult &= (MURMUR2A::HashConst("") == 0U);
		bResult &= (MURMUR3::HashConst("") == 0U);
		// 64-bit, empty string, default seed
		bResult &= (FNV1A_64::HashConst("") == Q_HASH_FNV1A_64_BASIS);
		bResult &= (MURMUR2_64::HashConst("") == 0ULL);

		// 32-bit, 1 bytes long string, default seed
		bResult &= (MURMUR2::HashConst("a") == 0x92685F5E);
		bResult &= (MURMUR2A::HashConst("a") == 0x0803888B);
		bResult &= (MURMUR3::HashConst("a") == 0x3C2569B2);
		// 64-bit, 1 bytes long string, default seed
		bResult &= (MURMUR2_64::HashConst("a") == 0x071717D2D36B6B11);

		// 32-bit, 2 bytes long string, default seed
		bResult &= (MURMUR2::HashConst("ab") == 0x1AA14063);
		bResult &= (MURMUR2A::HashConst("ab") == 0x618515AF);
		bResult &= (MURMUR3::HashConst("ab") == 0x9BBFD75F);
		// 64-bit, 2 bytes long string, default seed
		bResult &= (MURMUR2_64::HashConst("ab") == 0x62BE85B2FE53D1F8);

		// 32-bit, 3 bytes long string, default seed
		bResult &= (MURMUR2::HashConst("abc") == 0x13577C9B);
		bResult &= (MURMUR2A::HashConst("abc") == 0x11589F67);
		bResult &= (MURMUR3::HashConst("abc") == 0xB3DD93FA);
		// 64-bit, 3 bytes long string, default seed
		bResult &= (MURMUR2_64::HashConst("abc") == 0x9CC9C33498A95EFB);

		// 32-bit, 4 bytes long string, default seed
		bResult &= (CRC32::HashConst("test") == 0xD87F7E0C);
		bResult &= (DJB2::HashConst("test") == 0x7C9E6865);
		bResult &= (DJB2A::HashConst("test") == 0x7C73AF33);
		bResult &= (FNV1A::HashConst("test") == 0xAFD071E5);
		bResult &= (MURMUR2::HashConst("test") == 0x1812752E);
		bResult &= (MURMUR2A::HashConst("test") == 0x3D31CCC8);
		bResult &= (MURMUR3::HashConst("test") == 0xBA6BD213);
		// 64-bit, 4 bytes long string, default seed
		bResult &= (FNV1A_64::HashConst("test") == 0xF9E6E6EF197C2B25);
		bResult &= (MURMUR2_64::HashConst("test") == 0x2F4A8724618F4C63);

		bResult &= (MURMUR2_64::HashConst("testtest") == 0x64ED72C951229052);

		// 32-bit, 9 bytes long string, default seed
		bResult &= (MURMUR2::HashConst("testtesta") == 0x1C900135);
		bResult &= (MURMUR2A::HashConst("testtesta") == 0x46401C3F);
		bResult &= (MURMUR3::HashConst("testtesta") == 0xD65B33A1);
		// 64-bit, 9 bytes long string, default seed
		bResult &= (MURMUR2_64::HashConst("testtesta") == 0xF55BBD374C2B1C93);

		// 32-bit, 10 bytes long string, default seed
		bResult &= (MURMUR2::HashConst("testtestab") == 0x869B7B21);
		bResult &= (MURMUR2A::HashConst("testtestab") == 0xD5AC0E57);
		bResult &= (MURMUR3::HashConst("testtestab") == 0xE87CA250);
		// 64-bit, 10 bytes long string, default seed
		bResult &= (MURMUR2_64::HashConst("testtestab") == 0x0ACB80F405D605A1);

		// 32-bit, 11 bytes long string, default seed
		bResult &= (MURMUR2::HashConst("testtestabc") == 0x90841D17);
		bResult &= (MURMUR2A::HashConst("testtestabc") == 0x0F1321F9);
		bResult &= (MURMUR3::HashConst("testtestabc") == 0x9F633890);
		// 64-bit, 11 bytes long string, default seed
		bResult &= (MURMUR2_64::HashConst("testtestabc") == 0xF38CC8FBC9693C1E);

		// 32-bit, 256 bytes long string, default seed
		bResult &= (CRC32::HashConst(szSampleText) == 0x537C04FC);
		bResult &= (DJB2::HashConst(szSampleText) == 0xC6D71B82);
		bResult &= (DJB2A::HashConst(szSampleText) == 0x1037DD42);
		bResult &= (FNV1A::HashConst(szSampleText) == 0x8C920276);
		bResult &= (MURMUR2::HashConst(szSampleText) == 0x1E68204F);
		bResult &= (MURMUR2A::HashConst(szSampleText) == 0xE998A23D);
		bResult &= (MURMUR3::HashConst(szSampleText) == 0x7BB926E5);
		// 64-bit, 256 bytes long string, default seed
		bResult &= (FNV1A_64::HashConst(szSampleText) == 0x4D7B3D012FBC0FD6);
		bResult &= (MURMUR2_64::HashConst(szSampleText) == 0x714E8D7FF06785CD);

		return bResult;
	}
}
