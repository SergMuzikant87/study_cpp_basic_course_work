
#ifndef _INC_POINTS_H_
#define _INC_POINTS_H_
    
    #include "../inc/errors.h"
    #include <cstdint>

    class Points
    {
        public:
            uint64_t ** array;
            uint8_t rows_count;
            uint8_t cols_count;
        
        private:
            int8_t cols_dwords_count;
            bool array_was_created : 1;
            bool array_was_changed : 1;
            uint16_t enabled_points_count;
            uint16_t disabled_points_count; 

        public:
            Points(uint8_t rows_count, uint8_t cols_count, error_codes_t * error_code);
            Points(const char * config_file_addr, error_codes_t * error_code);
            ~Points(void);
        
        private:
            void variables_clean(void);
        
        public:
            uint64_t ** array_create(uint8_t rows_count, uint8_t cols_count);
            void array_delete(void);
            uint64_t ** array_resize(uint8_t rows_count, uint8_t cols_count);
            error_codes_t array_check(void);
            static error_codes_t array_size_check(int32_t rows_count, int32_t cols_count);
            void array_clean(void);
            void array_update(uint64_t ** enable_mask_array, uint64_t ** disable_mask_array);
            void array_print(void);

            void point_value_set(uint8_t row, uint8_t col, uint8_t value);
            uint8_t point_value_get(uint8_t row, uint8_t col);
            uint16_t enabled_points_count_get(void);
            uint16_t disabled_points_count_get(void);        
    };

#endif