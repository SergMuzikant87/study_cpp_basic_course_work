#include "../inc/points.h"
#include <iostream>
#include <fstream>

//Максимальное и минимальное количество строк и столбцов массива точек
#define POINTS_ARRAY_ROWS_COUNT_MAXIMUM ((int32_t)((((uint32_t)1) << (sizeof(uint8_t) << 3)) - ((uint32_t)1)))
#define POINTS_ARRAY_COLS_COUNT_MAXIMUM ((int32_t)((((uint32_t)1) << (sizeof(uint8_t) << 3)) - ((uint32_t)1)))
#define POINTS_ARRAY_ROWS_COUNT_MINUMUM ((int32_t)1)
#define POINTS_ARRAY_COLS_COUNT_MINIMUM ((int32_t)1)

//Расчёт количества двойных слов, требуемых для представления всех столбцов одной строки
#define cols_dwords_count_calc(cols_count) ((cols_count + 63 ) >> 6)
//На входе макроса номер столбца, на выходе номер двойного слова в котором находится этот столбец
#define cols_dword_num_calc(col) ((col) >> 6)
//Формирование маски столбца в двойном слове
#define col_mask_calc(col) (((uint64_t)1) << ((col) & 0x3F))
//
#define dword_update(dword, mask, value)\
{\
    dword &= (~((uint64_t)mask));\
    dword |= (((uint64_t)mask) & (~(((uint64_t)(value != 0)) - ((uint64_t) 1))));\
} 

Points::Points(const char * config_file_addr, error_codes_t * error_code)
{
    this->variables_clean();
    std::ifstream config_file;
    config_file.exceptions(std::ifstream::badbit);

    try
    {
        config_file.open(config_file_addr, std::ifstream::in);
        if(!config_file.is_open())
        {
            throw error_codes_t::CONFIG_FILE_OPEN_ERROR;
        }

        int32_t rows_count_int32 {};
        int32_t cols_count_int32 {};

        if(!(config_file >> rows_count_int32)) 
        {
            throw error_codes_t::CONFIG_FILE_ROWS_COUNT_READ_ERROR;
        }

        if(!(config_file >> cols_count_int32)) 
        {
            throw error_codes_t::CONFIG_FILE_COLS_COUNT_READ_ERROR;
        }

        if(((*error_code) = Points::array_size_check(rows_count_int32, cols_count_int32)) != error_codes_t::OK)
        {
            throw (*error_code);
        }

        this->array_create(static_cast<uint8_t>(rows_count_int32), static_cast<uint8_t>(cols_count_int32));
        if(((*error_code) = this->array_check()) != error_codes_t::OK)
        {
            throw (*error_code);
        }

        uint16_t points_count_read_allowed = (static_cast<uint16_t>(rows_count_int32) * static_cast<uint16_t>(cols_count_int32));

        while(1)
        {

            int32_t row_num_int32 = 0;
            int32_t col_num_int32 = 0;

            if(!(config_file >> row_num_int32)) 
            {
                break;
            }

            if(row_num_int32 >= rows_count_int32)
            {
                throw error_codes_t::ARRAY_ROW_NUM_OVERRANGE;
            }

            if(row_num_int32 < 0)
            {
                throw error_codes_t::ARRAY_ROW_NUM_UNDERRANGE;
            }

            if(!(config_file >> col_num_int32)) 
            {
                throw error_codes_t::CONFIG_FILE_COL_READ_ERROR;
            }

            if(col_num_int32 >= cols_count_int32)
            {
                throw error_codes_t::ARRAY_COL_NUM_OVERRANGE;
            }

            if(col_num_int32 < 0)
            {
                throw error_codes_t::ARRAY_COL_NUM_UNDERRANGE;
            }

            if(points_count_read_allowed == 0)
            {
                error_print("WARNING: Точек в конфигарационном файле больше, чем в созданном массиве.", ((*error_code) = error_codes_t::CONFIG_FILE_POINTS_COUNT_IS_MANY));
            }
            points_count_read_allowed -= (points_count_read_allowed != 0);
            
            this->point_value_set(static_cast<uint8_t>(row_num_int32), static_cast<uint8_t>(col_num_int32), 1);
        }

        (*error_code) = OK;
    }
    catch(error_codes_t ex_code)
    {
        (*error_code) = ex_code;
    }
    catch(...)
    {
        (*error_code) = error_codes_t::POINTS_OTHER_ERROR;
    }

    if(config_file.is_open())
    {
        config_file.close();
    }

}

Points::Points(uint8_t rows_count, uint8_t cols_count, error_codes_t * error_code)
{
    *error_code = error_codes_t::OK;

    if(((*error_code) = Points::array_size_check(static_cast<int32_t>(rows_count), static_cast<int32_t>(cols_count))) != error_codes_t::OK)
    {
        return;
    }
    this->variables_clean();
    this->array_create(rows_count, cols_count);
    *error_code = this->array_check();
}

Points::~Points(void)
{
    error_codes_t error_code = error_codes_t::OK;
    if(this->array_was_created)
    {
        if((error_code = this->array_check()) != error_codes_t::OK)
        {
            error_print("WARNING: Удаление массива. Массив не в порядке.", error_code);
        }
        this->array_delete();
    }
    else
    {
        error_print("WARNING: Массив не был создан.",(error_code = error_codes_t::ARRAY_IS_NOT_FOUND));
    }
}

void Points::variables_clean(void)
{
    this->cols_dwords_count = 0;
    this->enabled_points_count = 0;
    this->disabled_points_count = 0; 
    this->array_was_created = false;    
    this->array_was_changed = false;

    this->array = nullptr;
    this->rows_count = 0;
    this->cols_count = 0;
}

uint64_t ** Points::array_create(uint8_t rows_count, uint8_t cols_count)
{
    if(!this->array_was_created)
    {
        this->rows_count = rows_count;
        this->cols_count = cols_count;
        this->cols_dwords_count = cols_dwords_count_calc(this->cols_count);

        this->array = new uint64_t*[this->rows_count];

        if(this->array != nullptr)
        {
            for(uint8_t row_cur = 0; row_cur < this->rows_count; row_cur++)
            {
                this->array[row_cur] = new uint64_t[this->cols_dwords_count]();
            }   
            this->array_was_created = true;
        }
        this->array_was_changed = true;
    }

    return this->array;
}

void Points::array_delete(void)
{
    this->array_was_changed = true;
    this->array_was_created = false;
    if(this->array != nullptr)
    {
        for(uint8_t row_cur = 0; row_cur < this->rows_count; row_cur++)
        {
            if(this->array[row_cur] != nullptr)
            {
                delete [] this->array[row_cur], this->array[row_cur] = nullptr;
            }
        }
        delete [] this->array, this->array = nullptr;
    }
}

uint64_t ** Points::array_resize(uint8_t rows_count, uint8_t cols_count)
{
    this->array_delete();
    return this->array_create(rows_count, cols_count);
}

error_codes_t Points::array_check(void)
{
    if((Points::array_size_check(static_cast<int32_t>(this->rows_count), static_cast<int32_t>(this->cols_count))) != error_codes_t::OK)
    {
        //Возвращается другой код ошибки потому что проверка именно после того как массив создан.
        return error_codes_t::ARRAY_SIZE_ERROR_AFTER_CREAT; 
    }

    if(this->array == nullptr)
    {
        return error_codes_t::ARRAY_POINTER_ERROR;
    }

    for(uint8_t row_cur = 0; row_cur < this->rows_count; row_cur++)
    {
        if(this->array[row_cur] == nullptr)
        {
            return error_codes_t::SUB_ARRAY_POINTER_ERROR;
        }
    } 

    return error_codes_t::OK;
}

error_codes_t Points::array_size_check(int32_t rows_count, int32_t cols_count)
{
    if(rows_count > POINTS_ARRAY_ROWS_COUNT_MAXIMUM)
    {
        return error_codes_t::ARRAY_ROWS_COUNT_OVERRANGE;
    }

    if(rows_count < POINTS_ARRAY_ROWS_COUNT_MINUMUM)
    {
        return error_codes_t::ARRAY_ROWS_COUNT_UNDERRANGE;
    }

    if(cols_count > POINTS_ARRAY_COLS_COUNT_MAXIMUM)
    {
        return error_codes_t::ARRAY_COLS_COUNT_OVERRANGE;
    }

    if(cols_count < POINTS_ARRAY_COLS_COUNT_MINIMUM)
    {
        return error_codes_t::ARRAY_COLS_COUNT_UNDERRANGE;
    }

    return error_codes_t::OK;
}

void Points::array_clean(void)
{
    for(uint8_t row_num = 0; row_num < this->rows_count; row_num++)
    {
        for(uint8_t cols_dword_num = 0; cols_dword_num < this->cols_dwords_count; cols_dword_num++)
        {
            this->array[row_num][cols_dword_num] = 0;
        }
    }
    this->array_was_changed = true;
}

void Points::array_update(uint64_t ** enable_mask_array, uint64_t ** disable_mask_array)
{
    for(uint8_t row_num = 0; row_num < this->rows_count; row_num++)
    {
        for(uint8_t cols_dword_num = 0; cols_dword_num < this->cols_dwords_count; cols_dword_num++)
        {
            this->array[row_num][cols_dword_num] &= ~(disable_mask_array[row_num][cols_dword_num]);
            this->array[row_num][cols_dword_num] |= enable_mask_array[row_num][cols_dword_num];
        }
    }
    this->array_was_changed = true;
}

void Points::array_print(void)
{
    for(uint8_t row_num = 0; row_num < this->rows_count; row_num++)
    {
        for(uint8_t cols_dword_num = 0; cols_dword_num < this->cols_dwords_count; cols_dword_num++)
        {
            uint64_t current_dword = array[row_num][cols_dword_num];
            
            for(uint8_t col_num = 0; col_num < 64 ; col_num++)
            {
                if((cols_dword_num == (cols_dwords_count - 1)) && (col_num >= (this->cols_count & 0x3F)))
                {
                    break;
                }

                std::cout << ((current_dword & 1) ? "  *  " : "  - ");
                current_dword >>=1;
            }
        }
        std::cout << "\n\n";
    }
}

void Points::point_value_set(uint8_t row, uint8_t col, uint8_t value)
{
    if((row >= this->rows_count) || (col >= this->cols_count)) return;
    uint8_t dword_num = cols_dword_num_calc(col);
    uint64_t col_mask = col_mask_calc(col);
    dword_update((this->array[row][dword_num]), col_mask, value);
    this->array_was_changed = true;
}

uint8_t Points::point_value_get(uint8_t row, uint8_t col)
{
    if((row >= this->rows_count) || (col >= this->cols_count)) return 0;
    uint8_t dword_num = cols_dword_num_calc(col);
    uint64_t col_mask = col_mask_calc(col);
    return ((this->array[row][dword_num]) & col_mask) != 0;
}

uint16_t Points::enabled_points_count_get(void)
{
    if(this->array_was_changed)
    {
        this->array_was_changed = false;
        this->enabled_points_count = 0;
        for(uint8_t row_cur = 0; row_cur < this->rows_count; row_cur++)
        {
            for(uint8_t cols_dword_num = 0; cols_dword_num < this->cols_dwords_count; cols_dword_num++)
            {
                uint64_t dword = this->array[row_cur][cols_dword_num];
                do
                {
                    this->enabled_points_count += static_cast<uint16_t>(dword & 1);
                }
                while((dword >>= 1));
            }
        }
    }

    return this->enabled_points_count; 
}

uint16_t Points::disabled_points_count_get(void)
{
    return (static_cast<uint16_t>(this->rows_count) * static_cast<uint16_t>(this->cols_count)) - this->enabled_points_count_get(); 
}