/**
 * @file jtest.c
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

#include <jtest.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

static size_t jtest_errorsize;
static char *jtest_errormsg;
#define MSG_SIZE 4096
static char jtest_errorbuffer[MSG_SIZE];
static jmp_buf jtest_err;
static int color_output = 1;
static const char *suite_name;

typedef int (*jtest_filter_func)(struct jtest *);

#define ANSI_BLACK "\033[0;30m"
#define ANSI_RED "\033[0;31m"
#define ANSI_GREEN "\033[0;32m"
#define ANSI_YELLOW "\033[0;33m"
#define ANSI_BLUE "\033[0;34m"
#define ANSI_MAGENTA "\033[0;35m"
#define ANSI_CYAN "\033[0;36m"
#define ANSI_GREY "\033[0;37m"
#define ANSI_DARKGREY "\033[01;30m"
#define ANSI_BRED "\033[01;31m"
#define ANSI_BGREEN "\033[01;32m"
#define ANSI_BYELLOW "\033[01;33m"
#define ANSI_BBLUE "\033[01;34m"
#define ANSI_BMAGENTA "\033[01;35m"
#define ANSI_BCYAN "\033[01;36m"
#define ANSI_WHITE "\033[01;37m"
#define ANSI_NORMAL "\033[0m"

JTEST(suite, test)
{
}

//==============================================================================
// Error printing
//==============================================================================

static void vprint_errormsg(const char *const fmt, va_list ap) JTEST_IMPL_FORMAT_PRINTF(1, 0);
static void print_errormsg(const char *const fmt, ...) JTEST_IMPL_FORMAT_PRINTF(1, 2);

static void vprint_errormsg(const char *const fmt, va_list ap)
{
	// (v)snprintf returns the number that would have been written
	const int ret = vsnprintf(jtest_errormsg, jtest_errorsize, fmt, ap);
	if (ret < 0)
	{
		jtest_errormsg[0] = 0x00;
	}
	else
	{
		const size_t size = (size_t)ret;
		const size_t s = (jtest_errorsize <= size ? size - jtest_errorsize : size);
		// jtest_errorsize may overflow at this point
		jtest_errorsize -= s;
		jtest_errormsg += s;
	}
}

static void print_errormsg(const char *const fmt, ...)
{
	va_list argp;
	va_start(argp, fmt);
	vprint_errormsg(fmt, argp);
	va_end(argp);
}

static void msg_start(const char *color, const char *title)
{
	if (color_output)
	{
		print_errormsg("%s", color);
	}
	print_errormsg("  %s: ", title);
}

static void msg_end(void)
{
	if (color_output)
	{
		print_errormsg(ANSI_NORMAL);
	}
	print_errormsg("\n");
}

void JTEST_LOG(const char *fmt, ...)
{
	va_list argp;
	msg_start(ANSI_BLUE, "LOG");

	va_start(argp, fmt);
	vprint_errormsg(fmt, argp);
	va_end(argp);

	msg_end();
}

void JTEST_ERR(const char *fmt, ...)
{
	va_list argp;
	msg_start(ANSI_YELLOW, "ERR");

	va_start(argp, fmt);
	vprint_errormsg(fmt, argp);
	va_end(argp);

	msg_end();
	longjmp(jtest_err, 1);
}

//==============================================================================
// Assertions
//==============================================================================

void assert_str(const char *exp, const char *real, const char *caller, int line)
{
	if ((exp == NULL && real != NULL) ||
		(exp != NULL && real == NULL) ||
		(exp && real && strcmp(exp, real) != 0))
	{
		JTEST_ERR("%s:%d  expected '%s', got '%s'", caller, line, exp, real);
	}
}

void assert_wstr(const wchar_t *exp, const wchar_t *real, const char *caller, int line)
{
	if ((exp == NULL && real != NULL) ||
		(exp != NULL && real == NULL) ||
		(exp && real && wcscmp(exp, real) != 0))
	{
		JTEST_ERR("%s:%d  expected '%ls', got '%ls'", caller, line, exp, real);
	}
}

void assert_data(const unsigned char *exp, size_t expsize,
				 const unsigned char *real, size_t realsize,
				 const char *caller, int line)
{
	size_t i;
	if (expsize != realsize)
	{
		JTEST_ERR("%s:%d  expected %" PRIuMAX " bytes, got %" PRIuMAX, caller, line, (uintmax_t)expsize, (uintmax_t)realsize);
	}
	for (i = 0; i < expsize; i++)
	{
		if (exp[i] != real[i])
		{
			JTEST_ERR("%s:%d expected 0x%02x at offset %" PRIuMAX " got 0x%02x",
					  caller, line, exp[i], (uintmax_t)i, real[i]);
		}
	}
}

void assert_equal(intmax_t exp, intmax_t real, const char *caller, int line)
{
	if (exp != real)
	{
		JTEST_ERR("%s:%d  expected %" PRIdMAX ", got %" PRIdMAX, caller, line, exp, real);
	}
}

void assert_equal_u(uintmax_t exp, uintmax_t real, const char *caller, int line)
{
	if (exp != real)
	{
		JTEST_ERR("%s:%d  expected %" PRIuMAX ", got %" PRIuMAX, caller, line, exp, real);
	}
}

void assert_not_equal(intmax_t exp, intmax_t real, const char *caller, int line)
{
	if ((exp) == (real))
	{
		JTEST_ERR("%s:%d  should not be %" PRIdMAX, caller, line, real);
	}
}

void assert_not_equal_u(uintmax_t exp, uintmax_t real, const char *caller, int line)
{
	if ((exp) == (real))
	{
		JTEST_ERR("%s:%d  should not be %" PRIuMAX, caller, line, real);
	}
}

void assert_interval(intmax_t exp1, intmax_t exp2, intmax_t real, const char *caller, int line)
{
	if (real < exp1 || real > exp2)
	{
		JTEST_ERR("%s:%d  expected %" PRIdMAX "-%" PRIdMAX ", got %" PRIdMAX, caller, line, exp1, exp2, real);
	}
}

void assert_dbl_near(double exp, double real, double tol, const char *caller, int line)
{
	double diff = exp - real;
	double absdiff = diff;
	/* avoid using fabs and linking with a math lib */
	if (diff < 0)
	{
		absdiff *= -1;
	}
	if (absdiff > tol)
	{
		JTEST_ERR("%s:%d  expected %0.3e, got %0.3e (diff %0.3e, tol %0.3e)", caller, line, exp, real, diff, tol);
	}
}

void assert_dbl_far(double exp, double real, double tol, const char *caller, int line)
{
	double diff = exp - real;
	double absdiff = diff;
	/* avoid using fabs and linking with a math lib */
	if (diff < 0)
	{
		absdiff *= -1;
	}
	if (absdiff <= tol)
	{
		JTEST_ERR("%s:%d  expected %0.3e, got %0.3e (diff %0.3e, tol %0.3e)", caller, line, exp, real, diff, tol);
	}
}

void assert_null(void *real, const char *caller, int line)
{
	if ((real) != NULL)
	{
		JTEST_ERR("%s:%d  should be NULL", caller, line);
	}
}

void assert_not_null(const void *real, const char *caller, int line)
{
	if (real == NULL)
	{
		JTEST_ERR("%s:%d  should not be NULL", caller, line);
	}
}

void assert_true(int real, const char *caller, int line)
{
	if ((real) == 0)
	{
		JTEST_ERR("%s:%d  should be true", caller, line);
	}
}

void assert_false(int real, const char *caller, int line)
{
	if ((real) != 0)
	{
		JTEST_ERR("%s:%d  should be false", caller, line);
	}
}

void assert_fail(const char *caller, int line)
{
	JTEST_ERR("%s:%d  shouldn't come here", caller, line);
}

//==============================================================================
// Suite filter
//==============================================================================

static int suite_all(struct jtest *t)
{
	(void)t;
	return 1;
}

static int suite_filter(struct jtest *t)
{
	return strncmp(suite_name, t->suite_name, strlen(suite_name)) == 0;
}

//==============================================================================
// Current time
//==============================================================================

static uint64_t getCurrentTime(void)
{
	struct timeval now;
	gettimeofday(&now, NULL);
	uint64_t now64 = (uint64_t)now.tv_sec;
	now64 *= 1000000;
	now64 += ((uint64_t)now.tv_usec);
	return now64;
}

//==============================================================================
// Color printing
//==============================================================================

static void color_print(const char *color, const char *text)
{
	if (color_output)
	{
		printf("%s%s" ANSI_NORMAL "\n", color, text);
	}
	else
	{
		printf("%s\n", text);
	}
}

//==============================================================================
// Segfault handler
//==============================================================================

#ifdef JTEST_SEGFAULT
#include <signal.h>
static void sighandler(int signum)
{
	char msg[128];
	snprintf(msg, sizeof(msg), "[SIGNAL %d: %s]", signum, sys_siglist[signum]);
	color_print(ANSI_BRED, msg);
	fflush(stdout);

	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}
#endif

//==============================================================================
// JTest main
//==============================================================================

__attribute__((no_sanitize_address)) int jtest_main(int argc, const char *argv[])
{
	static int total = 0;
	static int num_ok = 0;
	static int num_fail = 0;
	static int num_skip = 0;
	static int idx = 1;
	static jtest_filter_func filter = suite_all;

#ifdef JTEST_SEGFAULT
	signal(SIGSEGV, sighandler);
#endif

	if (argc == 2)
	{
		suite_name = argv[1];
		filter = suite_filter;
	}
#ifdef JTEST_NO_COLORS
	color_output = 0;
#else
	color_output = isatty(1);
#endif
	uint64_t t1 = getCurrentTime();

	struct jtest *jtest_begin = &JTEST_IMPL_TNAME(suite, test);
	struct jtest *jtest_end = &JTEST_IMPL_TNAME(suite, test);
	// find begin and end of section by comparing magics
	while (1)
	{
		struct jtest *t = jtest_begin - 1;
		if (t->magic != JTEST_IMPL_MAGIC)
			break;
		jtest_begin--;
	}
	while (1)
	{
		struct jtest *t = jtest_end + 1;
		if (t->magic != JTEST_IMPL_MAGIC)
			break;
		jtest_end++;
	}
	jtest_end++; // end after last one

	static struct jtest *test;
	for (test = jtest_begin; test != jtest_end; test++)
	{
		if (test == &JTEST_IMPL_TNAME(suite, test))
			continue;
		if (filter(test))
			total++;
	}

	for (test = jtest_begin; test != jtest_end; test++)
	{
		if (test == &JTEST_IMPL_TNAME(suite, test))
			continue;
		if (filter(test))
		{
			jtest_errorbuffer[0] = 0;
			jtest_errorsize = MSG_SIZE - 1;
			jtest_errormsg = jtest_errorbuffer;
			printf("TEST %d/%d %s:%s ", idx, total, test->suite_name, test->test_name);
			fflush(stdout);
			if (test->skip)
			{
				color_print(ANSI_BYELLOW, "[SKIPPED]");
				num_skip++;
			}
			else
			{
				int result = setjmp(jtest_err);
				if (result == 0)
				{
					// Run setup function if present
					if (test->setup && *test->setup)
					{
						(*test->setup)(test->data);
					}
					// If there is associated data with the test
					if (test->data)
					{
						test->run(test->data);
					}
					// No data associated
					else
					{
						test->run();
					}
					// Run teardown if present
					if (test->teardown && *test->teardown)
					{
						(*test->teardown)(test->data);
					}
					// if we got here it's ok
#ifdef JTEST_COLOR_OK
					color_print(ANSI_BGREEN, "[OK]");
#else
					printf("[OK]\n");
#endif
					num_ok++;
				}
				else
				{
					color_print(ANSI_BRED, "[FAIL]");
					num_fail++;
				}
				if (jtest_errorsize != MSG_SIZE - 1)
					printf("%s", jtest_errorbuffer);
			}
			idx++;
		}
	}
	uint64_t t2 = getCurrentTime();

	const char *color = (num_fail) ? ANSI_BRED : ANSI_GREEN;
	char results[80];
	snprintf(results, sizeof(results), "RESULTS: %d tests (%d ok, %d failed, %d skipped) ran in %" PRIu64 " ms", total, num_ok, num_fail, num_skip, (t2 - t1) / 1000);
	color_print(color, results);
	return num_fail;
}

//==============================================================================
// Test executable main
//==============================================================================

int main(int argc, const char **argv)
{
	return jtest_main(argc, argv);
}

//==============================================================================
// End of file
//==============================================================================