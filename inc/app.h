#ifndef _INC_APP_H_
#define _INC_APP_H_

    #include "../inc/settings.h"
    #include "../inc/errors.h"

    class App
    {
        private:
            Settings * settings;
            
        public:
            App(Settings * settings);
            error_codes_t run(void);
            void pause(void);
            error_codes_t display_clear(void);
    };

#endif