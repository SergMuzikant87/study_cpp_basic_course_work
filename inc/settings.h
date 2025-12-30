#ifndef _INC_SETTINGS_H_
#define _INC_SETTINGS_H_

    #include "errors.h"
    #include <cstdint>

    #define DEFAULT_HELP_PRINT (false)
    #define DEFAULT_CONFIG_FILE_ADDR ((char *)"config.txt")
    #define DEFAULT_STEP_TIME_SEC (3)
    #define MINIMUM_STEP_TIME_SEC (0)
    #define MAXIMUM_STEP_TIME_SEC (30)
    
    class Settings
    {
        public:
            char * config_file_addr;
            uint16_t step_time_sec;
            bool help_print;

            Settings(int arguments_count, char * arguments[], error_codes_t * error_code);
            ~Settings(void);

            error_codes_t get(int arguments_count, char * arguments[]);
            void print(void);
    };

#endif