#include "../inc/points.h"
#include "../inc/status_codes.h"
#include <iostream>
#include <fstream>

//Максимальное количество строк и столбцов массива точек
#define POINTS_ARRAY_ROWS_COUNT_MAX ((1 << (sizeof(unsigned short) << 2)) >> 1)
#define POINTS_ARRAY_COLS_COUNT_MAX ((1 << (sizeof(unsigned short) << 2)) >> 1)

//Расчёт количества двойных слов, требуемых для представления всех столбцов одной строки
#define cols_dwords_count_calc(cols_count) ((cols_count + 31 ) >> 5)
//На входе макроса номер столбца, на выходе номер двойного слова в котором находится этот столбец
#define cols_dword_num_calc(col) ((col) >> 5)
//Формирование маски столбца в двойном слове
#define col_mask_calc(col) (((uint32_t)1) << ((col) & 31))
//
#define dword_update(dword, mask, value)\
{\
    dword &= (~((uint32_t)mask));\
    dword |= (((uint32_t)mask) & (~(((uint32_t)(value != 0)) - ((uint32_t) 1))));\
} 


Points::Points(const char * config_file_addr, status_t * status)
{
    this->variables_clean();

    std::ifstream config_file;
    config_file.open(config_file_addr);
    if(!config_file.is_open())
    {
        (*status) = status_t::CONFIG_FILE_OPEN_ERROR;
        return;
    }
    
    unsigned short rows_count {};
    unsigned short cols_count {};

    if(!(config_file >> rows_count)) 
    {
        (*status) = status_t::CONFIG_FILE_ROWS_COUNT_READ_ERROR;
        config_file.close();
        return;
    }

    if(!(config_file >> cols_count)) 
    {
        (*status) = status_t::CONFIG_FILE_COLS_COUNT_READ_ERROR;
        config_file.close();
        return;
    }

    if(((*status) = this->array_size_check(rows_count, cols_count)) != status_t::OK)
    {
        config_file.close();
        return;
    }

    this->array_create(rows_count, cols_count);
    if(((*status) = this->array_check()) != status_t::OK)
    {
        config_file.close();
        return;
    }

    unsigned short points_count_read_allowed = (rows_count * cols_count);

    while(1)
    {
        unsigned short row = 0;
        unsigned short col = 0;

        if(!(config_file >> row)) 
        {
            config_file.close();
            break;
        }

        if(row >= rows_count)
        {
            (*status) = status_t::ARRAY_ROW_NUM_OVERRANGE;
            this->array_delete();
            config_file.close();
            return;
        }

        if(!(config_file >> col)) 
        {
            (*status) = status_t::CONFIG_FILE_COL_READ_ERROR;
            config_file.close();
            return;
        }

        if(col >= cols_count)
        {
            (*status) = status_t::ARRAY_COL_NUM_OVERRANGE;
            this->array_delete();
            config_file.close();
            return;
        }

        if(points_count_read_allowed == 0)
        {
            (*status) = status_t::CONFIG_FILE_POINTS_COUNT_IS_MANY;
            std::cout << "WARNING: Точек в конфигарационном файле больше, чем в созданном массиве. Код ошибки: " << (*status) << "\n";
        }
        points_count_read_allowed -= (points_count_read_allowed != 0);

        this->point_value_set(row, col, 1);
    }
    
    (*status) = OK;
    config_file.close();
    return;
}

Points::Points(unsigned short rows_count, unsigned short cols_count, status_t * status)
{
    *status = status_t::OK;

    if(((*status) = this->array_size_check(rows_count, cols_count)) != status_t::OK)
    {
        return;
    }
    this->variables_clean();
    this->array_create(rows_count, cols_count);
    *status = this->array_check();
}

Points::~Points(void)
{
    status_t status = status_t::OK;
    if(this->array_was_created)
    {
        if((status = this->array_check()) != status_t::OK)
        {
            std::cout << "WARNING: Удаление массива. Массив не в порядке. Код ошибки: " << status << "\n";
        }
        this->array_delete();
    }
    else
    {
        std::cout << "WARNING: Массив не был создан. Код ошибки: " << (status = status_t::ARRAY_IS_NOT_FOUND) << "\n";
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

uint32_t ** Points::array_create(unsigned short rows_count, unsigned short cols_count)
{
    if(!this->array_was_created)
    {
        this->rows_count = rows_count;
        this->cols_count = cols_count;
        this->cols_dwords_count = cols_dwords_count_calc(this->cols_count);

        this->array = new uint32_t*[this->rows_count];

        for(unsigned short row_cur = 0; row_cur < this->rows_count; row_cur++)
        {
            this->array[row_cur] = new uint32_t[this->cols_dwords_count]();
        } 

        this->array_was_changed = true;
        this->array_was_created = true;
    }

    return this->array;
}

void Points::array_delete(void)
{
    this->array_was_changed = true;
    this->array_was_created = false;
    if(this->array != nullptr)
    {
        for(unsigned short row_cur = 0; row_cur < this->rows_count; row_cur++)
        {
            if(this->array[row_cur] != nullptr)
            {
                delete [] this->array[row_cur], this->array[row_cur] = nullptr;
            }
        }
        delete [] this->array, this->array = nullptr;
    }
}

uint32_t ** Points::array_resize(unsigned short rows_count, unsigned short cols_count)
{
    this->array_delete();
    return this->array_create(rows_count, cols_count);
}

status_t Points::array_check(void)
{
    if((this->array_size_check(this->rows_count, this->cols_count)) != status_t::OK)
    {
        //Возвращается другой код ошибки потому что проверка именно после того как массив создан.
        return status_t::ARRAY_SIZE_ERROR_AFTER_CREAT; 
    }

    if(this->array == nullptr)
    {
        return status_t::ARRAY_POINTER_ERROR;
    }

    for(unsigned short row_cur = 0; row_cur < this->rows_count; row_cur++)
    {
        if(this->array[row_cur] == nullptr)
        {
            return status_t::SUB_ARRAY_POINTER_ERROR;
        }
    } 

    return status_t::OK;
}

status_t Points::array_size_check(unsigned short rows_count, unsigned short cols_count)
{
    if(rows_count >= POINTS_ARRAY_ROWS_COUNT_MAX)
    {
        return status_t::ARRAY_ROWS_COUNT_OVERRANGE;
    }

    if(rows_count == 0)
    {
        return status_t::ARRAY_ROWS_COUNT_UNDERRANGE;
    }

    if(cols_count >= POINTS_ARRAY_COLS_COUNT_MAX)
    {
        return status_t::ARRAY_COLS_COUNT_OVERRANGE;
    }

    if(cols_count == 0)
    {
        return status_t::ARRAY_COLS_COUNT_UNDERRANGE;
    }

    return status_t::OK;
}

void Points::array_clean(void)
{
    for(unsigned short row_num = 0; row_num < this->rows_count; row_num++)
    {
        for(unsigned short cols_dword_num = 0; cols_dword_num < this->cols_dwords_count; cols_dword_num++)
        {
            this->array[row_num][cols_dword_num] = 0;
        }
    }
    this->array_was_changed = true;
}

void Points::array_update(uint32_t ** enable_mask_array, uint32_t ** disable_mask_array)
{
    for(unsigned short row_num = 0; row_num < this->rows_count; row_num++)
    {
        for(unsigned short cols_dword_num = 0; cols_dword_num < this->cols_dwords_count; cols_dword_num++)
        {
            this->array[row_num][cols_dword_num] &= ~(disable_mask_array[row_num][cols_dword_num]);
            this->array[row_num][cols_dword_num] |= enable_mask_array[row_num][cols_dword_num];
        }
    }
    this->array_was_changed = true;
}

void Points::array_print(void)
{
    for(unsigned short row_num = 0; row_num < this->rows_count; row_num++)
    {
        for(unsigned short cols_dword_num = 0; cols_dword_num < this->cols_dwords_count; cols_dword_num++)
        {
            uint32_t current_dword = array[row_num][cols_dword_num];
            
            for(unsigned short col_num = 0; col_num < 32 ; col_num++)
            {
                if((cols_dword_num == (cols_dwords_count - 1)) && (col_num >= (this->cols_count & 0x1F)))
                {
                    break;
                }

                std::cout << ((current_dword & 1) ? "  *  " : "  - ");
                current_dword >>=1;
            }
        }
        std::cout << std::endl << std::endl;
    }
}

void Points::point_value_set(unsigned short row, unsigned short col, unsigned short value)
{
    if((row >= this->rows_count) || (col >= this->cols_count)) return;
    unsigned short dword_num = cols_dword_num_calc(col);
    unsigned short col_mask = col_mask_calc(col);
    dword_update((this->array[row][dword_num]), col_mask, value);
    this->array_was_changed = true;
}

unsigned short Points::point_value_get(unsigned short row, unsigned short col)
{
    if((row >= this->rows_count) || (col >= this->cols_count)) return 0;
    unsigned short dword_num = cols_dword_num_calc(col);
    unsigned short col_mask = col_mask_calc(col);
    return ((this->array[row][dword_num]) & col_mask) != 0;
}

unsigned short Points::enabled_points_count_get(void)
{
    if(this->array_was_changed)
    {
        this->array_was_changed = false;
        this->enabled_points_count = 0;
        for(unsigned short row_cur = 0; row_cur < this->rows_count; row_cur++)
        {
            for(unsigned short cols_dword_num = 0; cols_dword_num < this->cols_dwords_count; cols_dword_num++)
            {
                uint32_t dword = this->array[row_cur][cols_dword_num];
                do
                {
                    this->enabled_points_count += (unsigned short)(dword & 1);
                }
                while(dword = (dword >> 1));
            }
        }
    }

    return this->enabled_points_count; 
}

unsigned short Points::disabled_points_count_get(void)
{
    return (this->rows_count * this->cols_count) - this->enabled_points_count_get(); 
}