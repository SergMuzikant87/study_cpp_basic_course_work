#ifndef _INC_STATISTICS_H_
#define _INC_STATISTICS_H_

    #include "errors.h"
    #include <cstdint>

    class Statistics
    {
        private:
            struct
            {
                uint16_t step_num;
                uint16_t enabled_points_count;
                uint16_t disabled_points_count;
                uint16_t enabled_in_step_points_count;
                uint16_t disabled_in_step_points_count;
            }current_step;

        public:
            Statistics(uint16_t enabled_points_count, uint16_t disabled_points_count, uint16_t enabled_in_step_points_count, uint16_t disabled_in_step_points_count);
            
            void clear(void);
            error_codes_t update(uint16_t enabled_points_count, uint16_t disabled_points_count, uint16_t enabled_in_step_points_count, uint16_t disabled_in_step_points_count);
            void print(void);
    };

#endif