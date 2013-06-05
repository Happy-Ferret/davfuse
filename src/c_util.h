#ifndef C_UTIL_H
#define C_UTIL_H

#define UNUSED(x) ((void)(x))
#define NELEMS(arr) (sizeof(arr) / sizeof(arr[0]))

#ifdef __GNUC__

#define UNUSED_FUNCTION_ATTR __attribute__ ((unused))

#define CONST_FUNCTION __attribute__ ((const))
#define PURE_FUNCTION __attribute__ ((pure))

#define NON_NULL_ARGS0() __attribute__ ((nonnull))
#define NON_NULL_ARGS1(a) __attribute__ ((nonnull (a)))
#define NON_NULL_ARGS2(a, b) __attribute__ ((nonnull (a, b)))
#define NON_NULL_ARGS3(a, b, c) __attribute__ ((nonnull (a, b, c)))
#define NON_NULL_ARGS4(a, b, c, d) __attribute__ ((nonnull (a, b, c, d)))

#else /* __GNUC__ */

#define UNUSED_FUNCTION_ATTR

#define CONST_FUNCTION
#define PURE_FUNCTION

#define NON_NULL_ARGS0()
#define NON_NULL_ARGS1(a)
#define NON_NULL_ARGS2(a, b)
#define NON_NULL_ARGS3(a, b, c)
#define NON_NULL_ARGS4(a, b, c, d)

#endif /* __GNUC__ */

#define HEADER_FUNCTION static UNUSED_FUNCTION_ATTR

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* C_UTIL_H */
#endif