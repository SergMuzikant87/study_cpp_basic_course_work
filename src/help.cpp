#include "../inc/settings.h"
#include <iostream>

void help_print(void)
{
    std::cout << "\nОпции программы:\n";
    std::cout << "\t" << "-h             Вызов справки. По умолчанию " << ((DEFAULT_HELP_PRINT) ? "\'вкл\'" : "\'выкл\'") << ".\n";
    std::cout << "\t" << "-с <файл>      Путь к файлу конфигурации. По умолчанию \'" << DEFAULT_CONFIG_FILE_ADDR << "\'.\n";
    std::cout << "\t" << "-t <секунды>   Время переключения шагов в секундах (от " << MINIMUM_STEP_TIME_SEC << " до " 
                                                                                   << MAXIMUM_STEP_TIME_SEC << "). По умолчанию " 
                                                                                   << DEFAULT_STEP_TIME_SEC << " сек.\n\n";
}