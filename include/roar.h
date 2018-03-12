/*
 * Copyright (C) 2017 Université Clermont Auvergne, CNRS/IN2P3, LPC
 * Author: Valentin NIESS (niess@in2p3.fr)
 *
 * A basic eRrOr hAndling libRary (ROAR).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef ROAR_H_
#define ROAR_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#ifndef FILE
#include <stdio.h>
#endif

/**
 * Customise this flag according to your needs, e.g. set it to `static` for an
 * internal implementation. By default the API functions are exported as
 * `extern`.
 */
#ifndef ROAR_API
#define ROAR_API extern
#endif

/** A generic function pointer. */
typedef void roar_function_t(void);

struct roar_handler;
/**
 * Callback function called when an error is triggered.
 * @param  handler  the error handler object.
 * @param  referent a related function from where the error was issued.
 * @param  code     the error code to return.
 * @return          `EXIT_SUCCESS` if the error should be ignored or
 *                    `ÈXIT_FAILURE` otherwise.
 *
 * Note that errors can be ignored only at the pre-processing, before they
 * are dumped.
 */
typedef int roar_callback_t(
    struct roar_handler * handler, roar_function_t * referent, int code);

/**
 * Base data required for handling errors and dumping logs. You can extend this
 * structure if you need to pass additional data.
 *
 * Warning : writing to the output stream might not be thread safe depending on
 * your OS and standard library. Writing to the string buffer is not thread
 * safe.
 */
struct roar_handler {
        /* An output stream where to log errors, or `NULL`. */
        FILE * stream;
        /* A buffer string where to write errors, or `NULL`. */
        char * string;
        /* A callback for pre processing of errors, or `NULL`. */
        roar_callback_t * pre;
        /* A callback for post processing of errors, or `NULL`. */
        roar_callback_t * post;
};

/**
 * Raise an error with a formated body message from variable arguments.
 * @param  handler  a pointer to an error handler.
 * @param  referent a related function from where the error was issued.
 * @param  code     the error code to return.
 * @param  format   a `printf` format string.
 * @param  VARARGS  extra arguments for the formated message.
 * @return          the error code, for convenience.
 */
#define ROAR_ERROR_FORMAT(handler, referent, code, format, ...)                \
        roar_handler_raise(handler, __func__, __FILE__, __LINE__,              \
            (roar_function_t *)referent, code, 0, format, NULL,                \
            ##__VA_ARGS__);

/**
* Raise an error with a formated body message from a variadic list.
* @param  handler  a pointer to an error handler.
* @param  referent a related function from where the error was issued.
* @param  code     the error code to return.
* @param  format   a `printf` format string.
* @param  args     a variadic list of extra arguments for the formated message.
* @return          the error code, for convenience.
*/
#define ROAR_ERROR_VFORMAT(handler, referent, code, format, ...)               \
        roar_handler_vraise(handler, __func__, __FILE__, __LINE__,             \
            (roar_function_t *)referent, code, 0, format, NULL, args);

/**
 * Raise an error with a static body message.
 * @param  handler  a pointer to an error handler.
 * @param  referent a related function where the error was issued.
 * @param  code     the error code to return.
 * @param  message  the message to log.
 * @return          the error code, for convenience.
 */
#define ROAR_ERROR_MESSAGE(handler, referent, code, message)                   \
        roar_handler_raise(handler, __func__, __FILE__, __LINE__,              \
            (roar_function_t *)referent, code, 0, message, NULL);

/**
* Raise an error message with a formated body parameter from variable
*   arguments.
* @param  handler  a pointer to an error handler.
* @param  referent a related function from where the error was issued.
* @param  code     the error code to return.
* @param  message  a string for the body message.
* @param  format   a `printf` format string for the body parameter.
* @param  VARARGS  extra arguments for the formated message.
* @return          the error code, for convenience.
*/
#define ROAR_ERRWP_FORMAT(handler, referent, code, message, format, ...)       \
        roar_handler_raise(handler, __func__, __FILE__, __LINE__,              \
            (roar_function_t *)referent, code, 0, message, format,             \
            ##__VA_ARGS__);

/**
* Raise an error message with a formated body parameter from a variadic list.
* @param  handler  a pointer to an error handler.
* @param  referent a related function from where the error was issued.
* @param  code     the error code to return.
* @param  message  a string for the body message.
* @param  format   a `printf` format string for the body parameter.
* @param  args     a variadic list of extra arguments for the formated message.
* @return          the error code, for convenience.
*/
#define ROAR_ERRWP_VFORMAT(handler, referent, code, message, format, ...)      \
        roar_handler_vraise(handler, __func__, __FILE__, __LINE__,             \
            (roar_function_t *)referent, code, 0, message, format, args);

/**
* Raise an error message with a static body parameter.
* @param  handler    a pointer to an error handler.
* @param  referent   a related function where the error was issued.
* @param  code       the error code to return.
* @param  message    the body message.
* @param  parameter  a static body parameter.
* @return            the error code, for convenience.
*/
#define ROAR_ERRWP_MESSAGE(handler, referent, code, message, parameter)        \
        roar_handler_raise(handler, __func__, __FILE__, __LINE__,              \
            (roar_function_t *)referent, code, 0, message, parameter);

/**
 * Raise an error with an errno body message, i.e. as returned by strerror.
 * @param  handler  a pointer to an error handler.
 * @param  referent a related function where the error was issued.
 * @param  errnum   the errno code to return, or `0` in order to use errno.
 * @return          the corresponding errno value, for convenience.
 */
#define ROAR_ERRNO(handler, referent, code)                                    \
        roar_handler_raise(handler, __func__, __FILE__, __LINE__,              \
            (roar_function_t *)referent, code, 1, NULL, NULL);

/**
 * Raise an error with an errno body message and a formated body parameter from
 *   variable arguments.
 * @param  handler  a pointer to an error handler.
 * @param  referent a related function where the error was issued.
 * @param  errnum   the errno code to return, or `0` in order to use errno.
 * @param  format   a `printf` format string.
 * @param  VARARGS  extra arguments for the formated print.
 * @return          the corresponding errno value, for convenience.
 */
#define ROAR_ERRNO_FORMAT(handler, referent, code, format, ...)                \
        roar_handler_raise(handler, __func__, __FILE__, __LINE__,              \
            (roar_function_t *)referent, code, 1, NULL, format,                \
            ##__VA_ARGS__);

/**
 * Raise an error with an errno body message and a formated body parameter from
 *   a variadic list.
 * @param  handler  a pointer to an error handler.
 * @param  referent a related function where the error was issued.
 * @param  errnum   the errno code to return, or `0` in order to use errno.
 * @param  format   a `printf` format string.
 * @param  args     a variadic list of extra arguments for the formated print.
 * @return          the corresponding errno value, for convenience.
 */
#define ROAR_ERRNO_VFORMAT(handler, referent, code, format, args)              \
        roar_handler_raise(handler, __func__, __FILE__, __LINE__,              \
            (roar_function_t *)referent, code, 1, NULL, format, args);

/**
 * Raise an error with an errno body message and a static body parameter.
 * @param  handler    a pointer to an error handler.
 * @param  referent   a related function where the error was issued.
 * @param  errnum     the errno code to return, or `0` in order to use errno.
 * @param  parameter  the static body parameter.
 * @return            the corresponding errno value, for convenience.
 */
#define ROAR_ERRNO_MESSAGE(handler, referent, code, parameter)                 \
        roar_handler_raise(handler, __func__, __FILE__, __LINE__,              \
            (roar_function_t *)referent, code, 1, NULL, parameter);

/* General purpose functions for raising an error. These functions are not
 * meant to be called directly. Use the ROAR_ERROR and ROAR_ERRNO macros
 * instead.
 */
ROAR_API int roar_handler_raise(void * handler, const char * function,
    const char * file, int line, roar_function_t * referent, int code,
    int use_errno, const char * message, const char * parameter, ...);

ROAR_API int roar_handler_vraise(void * handler, const char * function,
    const char * file, int line, roar_function_t * referent, int code,
    int use_errno, const char * message, const char * parameter, va_list args);

#ifdef __cplusplus
extern
}
#endif
#endif /* ROAR_H_ */

/* ------------------- Below is the library implementation ------------------ */
#ifdef ROAR_IMPLEMENTATION

#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* Customize the maximum depth of the string buffer by defining the following
 * macro.
 */
#ifndef ROAR_STRING_SIZE
#define ROAR_STRING_SIZE 80
#endif

/* Customize the error log by defining the following macros. */
#ifndef ROAR_FORMAT_HEADER
#define RED(x) "\x1B[31m" x "\x1B[0m"
#define CYN(x) "\x1B[36m" x "\x1B[0m"
#define ROAR_FORMAT_HEADER RED("error") " { " CYN("%s, %s:%d") " } : \x1B[31m"
#endif
#ifndef ROAR_FORMAT_BRA
#define ROAR_FORMAT_BRA " \x1B[0m{ \x1B[36m"
#endif
#ifndef ROAR_FORMAT_CKET
#define ROAR_FORMAT_CKET "\x1B[0m }"
#endif

/* General purpose function for printing an error. */
static void handler_vprint(struct roar_handler * h, const char * function,
    const char * file, int line, roar_function_t * referent, int code,
    int use_errno, const char * message, const char * parameter, va_list args)
{
        if (h->stream != NULL) {
                /* Dump the error message to the stream. */
                fprintf(h->stream, ROAR_FORMAT_HEADER, function, file, line);
                if (use_errno) {
                        fprintf(h->stream, "%s", strerror(code));
                } else if (parameter != NULL) {
                        fprintf(h->stream, "%s", message);
                } else {
                        vfprintf(h->stream, message, args);
                }

                if (parameter != NULL) {
                        fputs(ROAR_FORMAT_BRA, h->stream);
                        vfprintf(h->stream, parameter, args);
                        fputs(ROAR_FORMAT_CKET "\n", h->stream);
                } else {
                        fputs("\n", h->stream);
                }
        }

        if (h->string != NULL) {
                /* Dump the error message to the buffer. */
                int n = snprintf(h->string, ROAR_STRING_SIZE,
                    ROAR_FORMAT_HEADER, function, file, line);
                if (n >= ROAR_STRING_SIZE) goto end;
                if (use_errno) {
                        n += snprintf(h->string + n, ROAR_STRING_SIZE - n, "%s",
                            strerror(code));
                        if (n >= ROAR_STRING_SIZE) goto end;
                } else if (parameter != NULL) {
                        n += snprintf(
                            h->string + n, ROAR_STRING_SIZE - n, "%s", message);
                        if (n >= ROAR_STRING_SIZE) goto end;
                } else {
                        n += vsnprintf(
                            h->string + n, ROAR_STRING_SIZE - n, message, args);
                        if (n >= ROAR_STRING_SIZE) goto end;
                }

                if (parameter != NULL) {
                        n += snprintf(h->string + n, ROAR_STRING_SIZE - n,
                            ROAR_FORMAT_BRA);
                        if (n >= ROAR_STRING_SIZE) goto end;
                        n += vsnprintf(h->string + n, ROAR_STRING_SIZE - n,
                            parameter, args);
                        if (n >= ROAR_STRING_SIZE) goto end;
                        n += snprintf(h->string + n, ROAR_STRING_SIZE - n,
                            ROAR_FORMAT_CKET);
                        if (n >= ROAR_STRING_SIZE) goto end;
                }
        end:
                h->string[ROAR_STRING_SIZE - 1] = 0x0;
        }
}

#define ROAR_RAISE_PREPROCESS                                                  \
        if ((use_errno) && (code == 0)) code = errno;                          \
        if (handler == NULL) return code;                                      \
        struct roar_handler * h = (struct roar_handler *)handler;              \
        if ((h->pre != NULL) && (h->pre(h, referent, code)) == EXIT_SUCCESS)   \
                return code;

#define ROAR_RAISE_POSTPROCESS                                                 \
        if (h->post != NULL) h->post(h, referent, code);                       \
        return code;

/* General purpose functions for raising an error. */
int roar_handler_raise(void * handler, const char * function, const char * file,
    int line, roar_function_t * referent, int code, int use_errno,
    const char * message, const char * parameter, ...)
{
        ROAR_RAISE_PREPROCESS
        va_list args;
        va_start(args, parameter);
        handler_vprint(handler, function, file, line, referent, code, use_errno,
            message, parameter, args);
        va_end(args);
        ROAR_RAISE_POSTPROCESS
}

int roar_handler_vraise(void * handler, const char * function,
    const char * file, int line, roar_function_t * referent, int code,
    int use_errno, const char * message, const char * parameter, va_list args)
{
        ROAR_RAISE_PREPROCESS
        handler_vprint(handler, function, file, line, referent, code, use_errno,
            message, parameter, args);
        ROAR_RAISE_POSTPROCESS
}

#endif /* ROAR_IMPLEMENTATION */
