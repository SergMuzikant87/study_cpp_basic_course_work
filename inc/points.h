
#ifndef _INC_POINTS_H_
#define _INC_POINTS_H_
    
    #include "../inc/status_codes.h"
    #include <cstdint>

    class Points
    {
        public:
            uint32_t ** array;
            unsigned short rows_count;
            unsigned short cols_count;
        
        private:
            unsigned short cols_dwords_count;
            unsigned short enabled_points_count;
            unsigned short disabled_points_count; 
            bool array_was_created : 1;
            bool array_was_changed : 1;
        
        public:
            Points(unsigned short rows_count, unsigned short cols_count, status_t * status);
            Points(const char * config_file_addr, status_t * status);
            ~Points(void);
        
        private:
            void variables_clean(void);
        
        public:
            uint32_t ** array_create(unsigned short rows_count, unsigned short cols_count);
            void array_delete(void);
            uint32_t ** array_resize(unsigned short rows_count, unsigned short cols_count);
            status_t array_check(void);
            status_t array_size_check(unsigned short rows_count, unsigned short cols_count);
            void array_clean(void);
            void array_update(uint32_t ** enable_mask_array, uint32_t ** disable_mask_array);
            void array_print(void);

            void point_value_set(unsigned short row, unsigned short col, unsigned short value);
            unsigned short point_value_get(unsigned short row, unsigned short col);
            unsigned short enabled_points_count_get(void);
            unsigned short disabled_points_count_get(void);        
    };

#endif