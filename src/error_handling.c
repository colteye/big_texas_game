#include "error_handling.h"

void set_error(enum error_codes error)
{
	current_error = error;
}

int is_null(void *ptr)
{
	if (ptr == NULL)
	{
		set_error(ERROR_NULL_POINTER);
		return 1;
	} 
	return 0;
}