#include "../inc/status_codes.h"
#include "../inc/settings.h"
#include "../inc/app.h"
#include "../inc/help.h"
#include <iostream>
#include <clocale>

int main(int argc, char * argv [])
{   
    setlocale(LC_ALL, "Russian");

    status_t error_code = status_t::OK;
    Settings settings(argc, argv, &error_code);
    if(error_code != status_t::OK)
    {
        std::cout << "ERROR: Ошибка чтения опций программы. Код ошибки: " << error_code << std::endl;
        return error_code;
    }
    
    if(!settings.help_print)
    {
        settings.print();
    }
    else
    {
        help_print(); 
        return status_t::OK;
    }

    App app(&settings);
    if((error_code = app.run()) != status_t::OK)
    {
        std::cout << "ERROR: Ошибка выполнения программы. Код ошибки: " << error_code << std::endl;
        return error_code;
    }

    return status_t::OK;
}
