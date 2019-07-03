/**
 * @file jtest.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-02
 * 
 * @brief JTest driver. Built with inspiration from CTest.
 * 
 * @copyright Copyright (C) 2019,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 * 
 */

#ifndef JTEST_H
#define JTEST_H

#define JTEST_COLOR_OK

//==============================================================================
// Format printf
//==============================================================================

#ifdef __GNUC__
#define JTEST_IMPL_FORMAT_PRINTF(a, b) __attribute__((format(printf, a, b)))
#else
#define JTEST_IMPL_FORMAT_PRINTF(a, b)
#endif

#include <inttypes.h>
#include <stddef.h>

//==============================================================================
// Diagnostics
//==============================================================================

#define JTEST_IMPL_PRAGMA(x) _Pragma(#x)

#if defined(__GNUC__)
#if defined(__clang__) || __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#define JTEST_IMPL_DIAG_PUSH_IGNORED(w)    \
	JTEST_IMPL_PRAGMA(GCC diagnostic push) \
	JTEST_IMPL_PRAGMA(GCC diagnostic ignored "-W" #w)
#define JTEST_IMPL_DIAG_POP() \
	JTEST_IMPL_PRAGMA(GCC diagnostic pop)
#else
#define JTEST_IMPL_DIAG_PUSH_IGNORED(w) \
	JTEST_IMPL_PRAGMA(GCC diagnostic ignored "-W" #w)
#define JTEST_IMPL_DIAG_POP()
#endif
#else
#define JTEST_IMPL_DIAG_PUSH_IGNORED(w)
#define JTEST_IMPL_DIAG_POP()
#endif

//==============================================================================
// Test structure definition
//==============================================================================

typedef void (*jtest_setup_func)(void *);
typedef void (*jtest_teardown_func)(void *);

struct jtest
{
	const char *suite_name;
	const char *test_name;

	void (*run)();

	void *data;
	jtest_setup_func *setup;
	jtest_teardown_func *teardown;

	int skip;

	unsigned int magic;
};

#define JTEST_IMPL_NAME(name) jtest_##name
#define JTEST_IMPL_FNAME(sname, tname) JTEST_IMPL_NAME(sname##_##tname##_run)
#define JTEST_IMPL_TNAME(sname, tname) JTEST_IMPL_NAME(sname##_##tname)
#define JTEST_IMPL_DATA_SNAME(sname) JTEST_IMPL_NAME(sname##_data)
#define JTEST_IMPL_DATA_TNAME(sname, tname) JTEST_IMPL_NAME(sname##_##tname##_data)
#define JTEST_IMPL_SETUP_FNAME(sname) JTEST_IMPL_NAME(sname##_setup)
#define JTEST_IMPL_SETUP_FPNAME(sname) JTEST_IMPL_NAME(sname##_setup_ptr)
#define JTEST_IMPL_TEARDOWN_FNAME(sname) JTEST_IMPL_NAME(sname##_teardown)
#define JTEST_IMPL_TEARDOWN_FPNAME(sname) JTEST_IMPL_NAME(sname##_teardown_ptr)

#define JTEST_IMPL_MAGIC (0x1243FEAB)
#ifdef __APPLE__
#define JTEST_IMPL_SECTION __attribute__((used, section("__DATA, .jtest"), aligned(1)))
#else
#define JTEST_IMPL_SECTION __attribute__((used, section(".jtest"), aligned(1)))
#endif

#define JTEST_IMPL_STRUCT(sname, tname, tskip, tdata, tsetup, tteardown)      \
	static struct jtest JTEST_IMPL_TNAME(sname, tname) JTEST_IMPL_SECTION = { \
		.suite_name = #sname,                                                 \
		.test_name = #tname,                                                  \
		.run = JTEST_IMPL_FNAME(sname, tname),                                \
		.data = tdata,                                                        \
		.setup = (jtest_setup_func *)tsetup,                                  \
		.teardown = (jtest_teardown_func *)tteardown,                         \
		.skip = tskip,                                                        \
		.magic = JTEST_IMPL_MAGIC}

#define JTEST_SETUP(sname)                                                                                                 \
	static void JTEST_IMPL_SETUP_FNAME(sname)(struct JTEST_IMPL_DATA_SNAME(sname) * data);                                 \
	static void (*JTEST_IMPL_SETUP_FPNAME(sname))(struct JTEST_IMPL_DATA_SNAME(sname) *) = &JTEST_IMPL_SETUP_FNAME(sname); \
	static void JTEST_IMPL_SETUP_FNAME(sname)(struct JTEST_IMPL_DATA_SNAME(sname) * data)

#define JTEST_TEARDOWN(sname)                                                                                                    \
	static void JTEST_IMPL_TEARDOWN_FNAME(sname)(struct JTEST_IMPL_DATA_SNAME(sname) * data);                                    \
	static void (*JTEST_IMPL_TEARDOWN_FPNAME(sname))(struct JTEST_IMPL_DATA_SNAME(sname) *) = &JTEST_IMPL_TEARDOWN_FNAME(sname); \
	static void JTEST_IMPL_TEARDOWN_FNAME(sname)(struct JTEST_IMPL_DATA_SNAME(sname) * data)

#define JTEST_DATA(sname)                                                                    \
	struct JTEST_IMPL_DATA_SNAME(sname);                                                     \
	static void (*JTEST_IMPL_SETUP_FPNAME(sname))(struct JTEST_IMPL_DATA_SNAME(sname) *);    \
	static void (*JTEST_IMPL_TEARDOWN_FPNAME(sname))(struct JTEST_IMPL_DATA_SNAME(sname) *); \
	struct JTEST_IMPL_DATA_SNAME(sname)

#define JTEST_IMPL_JTEST(sname, tname, tskip)                 \
	static void JTEST_IMPL_FNAME(sname, tname)(void);         \
	JTEST_IMPL_STRUCT(sname, tname, tskip, NULL, NULL, NULL); \
	static void JTEST_IMPL_FNAME(sname, tname)(void)

#define JTEST_IMPL_JTEST2(sname, tname, tskip)                                                                                                         \
	static struct JTEST_IMPL_DATA_SNAME(sname) JTEST_IMPL_DATA_TNAME(sname, tname);                                                                    \
	static void JTEST_IMPL_FNAME(sname, tname)(struct JTEST_IMPL_DATA_SNAME(sname) * data);                                                            \
	JTEST_IMPL_STRUCT(sname, tname, tskip, &JTEST_IMPL_DATA_TNAME(sname, tname), &JTEST_IMPL_SETUP_FPNAME(sname), &JTEST_IMPL_TEARDOWN_FPNAME(sname)); \
	static void JTEST_IMPL_FNAME(sname, tname)(struct JTEST_IMPL_DATA_SNAME(sname) * data)

//==============================================================================
// Logging
//==============================================================================

void JTEST_LOG(const char *fmt, ...) JTEST_IMPL_FORMAT_PRINTF(1, 2);
void JTEST_ERR(const char *fmt, ...) JTEST_IMPL_FORMAT_PRINTF(1, 2);

//==============================================================================
// Test definition
//==============================================================================

#define JTEST(sname, tname) JTEST_IMPL_JTEST(sname, tname, 0)
#define JTEST_SKIP(sname, tname) JTEST_IMPL_JTEST(sname, tname, 1)

#define JTEST2(sname, tname) JTEST_IMPL_JTEST2(sname, tname, 0)
#define JTEST2_SKIP(sname, tname) JTEST_IMPL_JTEST2(sname, tname, 1)

//==============================================================================
// Assertions
//==============================================================================

#define ASSERT_STR(exp, real) assert_str(exp, real, __FILE__, __LINE__)
#define ASSERT_WSTR(exp, real) assert_wstr(exp, real, __FILE__, __LINE__)
#define ASSERT_DATA(exp, expsize, real, realsize) \
	assert_data(exp, expsize, real, realsize, __FILE__, __LINE__)
#define ASSERT_EQUAL(exp, real) assert_equal(exp, real, __FILE__, __LINE__)
#define ASSERT_EQUAL_U(exp, real) assert_equal_u(exp, real, __FILE__, __LINE__)
#define ASSERT_NOT_EQUAL(exp, real) assert_not_equal(exp, real, __FILE__, __LINE__)
#define ASSERT_NOT_EQUAL_U(exp, real) assert_not_equal_u(exp, real, __FILE__, __LINE__)
#define ASSERT_INTERVAL(exp1, exp2, real) assert_interval(exp1, exp2, real, __FILE__, __LINE__)
#define ASSERT_NULL(real) assert_null((void *)real, __FILE__, __LINE__)
#define ASSERT_NOT_NULL(real) assert_not_null(real, __FILE__, __LINE__)
#define ASSERT_TRUE(real) assert_true(real, __FILE__, __LINE__)
#define ASSERT_FALSE(real) assert_false(real, __FILE__, __LINE__)
#define ASSERT_FAIL() assert_fail(__FILE__, __LINE__)
#define ASSERT_DBL_NEAR(exp, real) assert_dbl_near(exp, real, 1e-4, __FILE__, __LINE__)
#define ASSERT_DBL_NEAR_TOL(exp, real, tol) assert_dbl_near(exp, real, tol, __FILE__, __LINE__)
#define ASSERT_DBL_FAR(exp, real) assert_dbl_far(exp, real, 1e-4, __FILE__, __LINE__)
#define ASSERT_DBL_FAR_TOL(exp, real, tol) assert_dbl_far(exp, real, tol, __FILE__, __LINE__)

void assert_str(const char *exp, const char *real, const char *caller, int line);
void assert_wstr(const wchar_t *exp, const wchar_t *real, const char *caller, int line);
void assert_data(const unsigned char *exp, size_t expsize,
				 const unsigned char *real, size_t realsize,
				 const char *caller, int line);
void assert_equal(intmax_t exp, intmax_t real, const char *caller, int line);
void assert_equal_u(uintmax_t exp, uintmax_t real, const char *caller, int line);
void assert_not_equal(intmax_t exp, intmax_t real, const char *caller, int line);
void assert_not_equal_u(uintmax_t exp, uintmax_t real, const char *caller, int line);
void assert_interval(intmax_t exp1, intmax_t exp2, intmax_t real, const char *caller, int line);
void assert_null(void *real, const char *caller, int line);
void assert_not_null(const void *real, const char *caller, int line);
void assert_true(int real, const char *caller, int line);
void assert_false(int real, const char *caller, int line);
void assert_fail(const char *caller, int line);
void assert_dbl_near(double exp, double real, double tol, const char *caller, int line);
void assert_dbl_far(double exp, double real, double tol, const char *caller, int line);

int jtest_main(int argc, const char *argv[]);

#endif

//==============================================================================
// End of file
//==============================================================================
