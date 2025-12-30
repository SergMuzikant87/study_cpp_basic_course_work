#include "../inc/errors.h"
#include "../inc/settings.h"
#include "../inc/app.h"
#include "../inc/help.h"
#include <iostream>
#include <clocale>

int main(int arguments_count, char * arguments [])
{   
    setlocale(LC_ALL, "Russian");

    error_codes_t error_code = error_codes_t::OK;
    Settings settings(arguments_count, arguments, &error_code);
    if(error_code != error_codes_t::OK)
    {
        error_print("ERROR: Ошибка чтения опций программы.", error_code);
        return error_code;
    }
    
    if(!settings.help_print)
    {
        settings.print();
    }
    else
    {
        help_print(); 
        return error_codes_t::OK;
    }

    App app(&settings);
    if((error_code = app.run()) != error_codes_t::OK)
    {
        error_print("ERROR: Ошибка выполнения программы.", error_code);
        return error_code;
    }

    return error_codes_t::OK;
}
