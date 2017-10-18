#include <stdlib.h>

#define ROAR_IMPLEMENTATION
#include "roar.h"

/* Prototypes of the pre and post error callbacks. */
int handle_pre_error(
    struct roar_handler * handler, roar_function_t * caller, int code);
int handle_post_error(
    struct roar_handler * handler, roar_function_t * caller, int code);

int main()
{
        /* Initialise the error handler with logging to stderr and pre and
         * post processing of errors.
         */
        struct roar_handler handler = { stderr, NULL, &handle_pre_error,
                &handle_post_error };

        /* Generate a file error. It will be discarded in the pre-error
         * callback.
         */
        FILE * fid;
        const char * path = "nofile.nop";
        if ((fid = fopen(path, "r")) == NULL) ROAR_ERRNO(&handler, &main, 0);

        /* Trigger a specific errno like error with an extra argument. */
        ROAR_ERRNO_FORMAT(&handler, &main, EINVAL, "\"%s\"", path);

        exit(EXIT_SUCCESS);
}

/* The pre-error callback*/
int handle_pre_error(
    struct roar_handler * handler, roar_function_t * referent, int code)
{
        /* Discard file errors issued by the main. */
        if ((referent == (roar_function_t *)&main) && (code == ENOENT))
                return EXIT_SUCCESS;
        return EXIT_FAILURE;
}

/* The post-error callback*/
int handle_post_error(
    struct roar_handler * handler, roar_function_t * caller, int code)
{
        /* Exit to the OS with a faillure code. */
        exit(EXIT_FAILURE);
}
