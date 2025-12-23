#ifndef _INC_APP_H_
#define _INC_APP_H_

    #include "../inc/settings.h"
    #include "../inc/status_codes.h"

    class App
    {
        public:
            Settings * settings;

            App(Settings * settings);
            status_t run(void);
    };

#endif