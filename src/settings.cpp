#include "../inc/settings.h"
#include <iostream>
#include <cstdint>
#include <clocale>

static uint16_t array_of_char_to_int(const char * str);

Settings::Settings(int arguments_count, char * arguments[], error_codes_t * status)
{
    if((*status = this->get(arguments_count, arguments)) != OK )
    {
        return;
    }
}

Settings::~Settings(void)
{

}

error_codes_t Settings::get(int arguments_count, char * arguments[])
{
    try
    {
        this->config_file_addr = DEFAULT_CONFIG_FILE_ADDR;
        this->step_time_sec = DEFAULT_STEP_TIME_SEC;
        this->help_print = DEFAULT_HELP_PRINT;

        for(int num = 1; num < arguments_count; num++)
        {
            
            if(arguments[num][0] != '-')
            {
                throw error_codes_t::SETTINGS_ARGUMENT_ERROR;
            }

            if((arguments[num][1] == '\0') || (arguments[num][1] < 'a') || (arguments[num][1] > 'z'))
            {
                throw error_codes_t::SETTINGS_ARGUMENT_ERROR;
            }

            if(arguments[num][2] != '\0')
            {
                throw error_codes_t::SETTINGS_ARGUMENT_ERROR;
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
                        throw error_codes_t::SETTINGS_VALUE_ERROR;
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

            throw error_codes_t::SETTINGS_ARGUMENT_OR_VALUE_ERROR;
        }

        return error_codes_t::OK;
    }
    catch(error_codes_t error_code)
    {
        return error_code;
    }
    catch(...)
    {
        return error_codes_t::SETTINGS_UNKNOWN_ERROR;
    }
}

void Settings::print(void)
{
    std::cout << "\nНастройки программы:\n\n";
    std::cout << "Settings\n";
    std::cout << "{\n";
    std::cout << "\tconfig_file_addr : " << this->config_file_addr << "\n";
    std::cout << "\tstep_time_sec    : " << this->step_time_sec << "\n";
    std::cout << "\thelp_print       : " << this->help_print << "\n";
    std::cout << "}\n\n";
}

static uint16_t array_of_char_to_int(const char * str)
{
    uint16_t result = 0;
    
    while(char symbol = *str++)
    {
        result += (static_cast<uint16_t>((symbol >= '0') && (symbol <= '9'))) * ((result * 9) + static_cast<uint16_t>(symbol - '0'));
    }

    return result;
}