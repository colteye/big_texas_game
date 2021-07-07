#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdint.h>

enum error_codes
{
	ERROR_MISC, 
	ERROR_OUT_OF_BOUNDS, 
	ERROR_BUFFER_OVERRUN, 
	ERROR_INVALID_INSTANTIATION, 
	ERROR_NULL_POINTER 
};

enum error_codes current_error;
void set_error(enum error_codes error);
int is_null(void *ptr);

#endif /* ERROR_HANDLING_H */