#include "../inc/errors.h"
#include <iostream>

void error_print(const char * error_text, error_codes_t error_code)
{
    if(error_code != error_codes_t::OK)
    {
        std::cout << ((error_text != nullptr ) ? error_text : "" ) << " Код ошибки: " << error_code << "\n";
    }
}
