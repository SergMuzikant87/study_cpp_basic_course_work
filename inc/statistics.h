
#ifndef _INC_STATISTICS_H_
#define _INC_STATISTICS_H_

    class Statistics
    {
        private:
            struct
            {
                unsigned short step_num;
                unsigned short enabled_points_count;
                unsigned short disabled_points_count;
                unsigned short enabled_in_step_points_count;
                unsigned short disabled_in_step_points_count;
            }current_step;

        public:
            Statistics(unsigned short enabled_points_count, unsigned short disabled_points_count, unsigned short enabled_in_step_points_count, unsigned short disabled_in_step_points_count);
            
            void clear(void);
            signed short update(unsigned short enabled_points_count, unsigned short disabled_points_count, unsigned short enabled_in_step_points_count, unsigned short disabled_in_step_points_count);
            void print(void);
    };

#endif