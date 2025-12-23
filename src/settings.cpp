#include "../inc/settings.h"
#include "../inc/status_codes.h"
#include <iostream>
#include <clocale>

static unsigned short array_of_char_to_int(const char * str);

Settings::Settings(int arguments_count, char * arguments[], status_t * status)
{
    if((*status = this->get(arguments_count, arguments)) != OK )
    {
        return;
    }
}

Settings::~Settings(void)
{

}

status_t Settings::get(int arguments_count, char * arguments[])
{
    this->config_file_addr = DEFAULT_CONFIG_FILE_ADDR;
    this->step_time_sec = DEFAULT_STEP_TIME_SEC;
    this->help_print = DEFAULT_HELP_PRINT;

    for(int num = 1; num < arguments_count; num++)
    {
        if(arguments[num][0] != '-')
        {
            return status_t::SETTINGS_ARGUMENT_ERROR;
        }

        if((arguments[num][1] == '\0') || (arguments[num][1] < 'a') || (arguments[num][1] > 'z'))
        {
            return status_t::SETTINGS_ARGUMENT_ERROR;
        }

        if(arguments[num][2] != '\0')
        {
            return status_t::SETTINGS_ARGUMENT_ERROR;
        }

        if((num + 1) < arguments_count)
        {
            //Файл конфигурации
            if(arguments[num][1] == 'c')
            {
                this->config_file_addr = arguments[++num];
                continue;
            }

            //Время переключения шагов
            if(arguments[num][1] == 't')
            {    
                this->step_time_sec = array_of_char_to_int(arguments[++num]);
                if(this->step_time_sec > MAXIMUM_STEP_TIME_SEC)
                {
                    return status_t::SETTINGS_VALUE_ERROR;
                } 
                continue;
            }
        }

        //Вызов справки
        if(arguments[num][1] == 'h')
        {
            this->help_print = true;
            continue;
        }

        return status_t::SETTINGS_ARGUMENT_OR_VALUE_ERROR;
    }

    return status_t::OK;
}

void Settings::print(void)
{
    std::cout << "\nОпции программы:\n\n";
    std::cout << "Settings\n";
    std::cout << "{\n";
    std::cout << "\tconfig_file_addr : " << this->config_file_addr << "\n";
    std::cout << "\tstep_time_sec    : " << (unsigned short)this->step_time_sec << "\n";
    std::cout << "\thelp_print       : " << this->help_print << "\n";
    std::cout << "}\n\n";
}

static unsigned short array_of_char_to_int(const char * str)
{
    unsigned short result = 0;
    
    while(char symbol = *str++)
    {
        result += ((unsigned short)((symbol >= '0') && (symbol <= '9'))) * ((result * 9) + (unsigned short)(symbol - '0'));
    }

    return result;
}