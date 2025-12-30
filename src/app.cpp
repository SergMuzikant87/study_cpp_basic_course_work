#include "../inc/app.h"
#include "../inc/points.h"
#include "../inc/statistics.h"
#include <iostream>
#include <cstdlib>
#ifdef _WIN32
    #include "windows.h"
#else
    #include <unistd.h>
#endif

App::App(Settings * settings)
{
   this->settings = settings; 
}

error_codes_t App::run(void)
{
    error_codes_t error_code = error_codes_t::OK;
    
    //Точки пространства
    Points points(this->settings->config_file_addr, &error_code); 
    if(error_code != error_codes_t::OK) 
    {
        error_print("ERROR: Ошибка создания и инициализации массива точек пространства.", error_code);
        return error_code;
    }
    
    //Маски разрешений на включение точек
    Points points_disable_mask(points.rows_count, points.cols_count, &error_code); 
    if(error_code != error_codes_t::OK) 
    {
        error_print("ERROR: Ошибка создания и инициализации массива масок разрешений на включение точек пространства.", error_code);
        return error_code;
    }
    
    //Маски разрешений на выключение точек
    Points points_enable_mask(points.rows_count, points.cols_count, &error_code); 
    if(error_code != error_codes_t::OK) 
    {
        error_print("ERROR: Ошибка создания и инициализации массива масок разрешений на выключение точек пространства.", error_code);
        return error_code;
    }
    
    //Статистика
    Statistics statistics
    (
        points.enabled_points_count_get(),  
        points.disabled_points_count_get(), 
        points.enabled_points_count_get(),  
        points.disabled_points_count_get()  
    );
    
    //Запускаем шарманку.
    std::cout << "Для запуска нажмите клавишу \'ENTER\': ", std::cin.get();

    //Вывод на экран начального состояния точек
    #ifdef _WIN32
        std::system("CLS");
    #else
        std::system("clear");
    #endif
    points.array_print();
    statistics.print();

    //Перебор шагов.
    int8_t permision_stop {(int8_t)(((int8_t)0) - ((int8_t)(points.enabled_points_count_get() == 0)))};
    while(permision_stop == 0)
    {
        //Пауза
        #ifdef _WIN32
            Sleep(static_cast<uint32_t>(this->settings->step_time_sec) * static_cast<uint32_t>(1000));
        #else
            sleep(static_cast<unsigned int>(this->settings->step_time_sec));
        #endif
        
        //Очистка масок разрешений на включение и выключение точек
        points_enable_mask.array_clean();
        points_disable_mask.array_clean();

        //Перебор строк пространства точек
        for(uint8_t row_num = 0; row_num < points.rows_count; row_num++)
        {
            //Перебор точек (столбцов) внутри текущей строки пространства точек
            for(uint8_t col_num = 0; col_num < points.cols_count; col_num++)
            {
                //Состояние текущей точки
                uint8_t point_value = points.point_value_get(row_num, col_num); 

                //Перебор точек, находящихся рядом с текущей точкой и подсчёт их количества
                uint16_t count_of_near_points_enabled = 0;
                for(uint8_t row_num_of_near_point = ((row_num > 0) ? (row_num - 1) : 0); (row_num_of_near_point <= (row_num + 1)) && (row_num_of_near_point < points.rows_count); row_num_of_near_point++)
                {
                    for(uint8_t col_num_of_near_point = ((col_num > 0) ? (col_num - 1) : 0); (col_num_of_near_point <= (col_num + 1)) && (col_num_of_near_point < points.cols_count); col_num_of_near_point++)
                    {
                        if((row_num == row_num_of_near_point) && (col_num == col_num_of_near_point))
                        {
                            continue;
                        }

                        //Инкрементирования количества точек, которые находятся рядом с текущей
                        count_of_near_points_enabled += points.point_value_get(row_num_of_near_point, col_num_of_near_point);
                    }
                }

                //Формирование разрешений на включение и выключение текущей точки
                (!point_value) 
                ? points_enable_mask.point_value_set( row_num, col_num, (count_of_near_points_enabled == 3) )
                : points_disable_mask.point_value_set( row_num, col_num, ((count_of_near_points_enabled != 2) && (count_of_near_points_enabled != 3)) );   
            }
        }

        //Включение точек, у которых были сформированны разрешения на включение
        //Выключение точек, у которых были сформированны разрешения на выключение
        points.array_update(points_enable_mask.array, points_disable_mask.array);
        
        //Обновление статистики
        error_code
        =
        statistics.update
        (
            points.enabled_points_count_get(),             
            points.disabled_points_count_get(),            
            points_enable_mask.enabled_points_count_get(), 
            points_disable_mask.enabled_points_count_get() 
        );
        if(error_code != error_codes_t::OK)
        {
            error_print("ERROR: Номер текущего шага больше максимально допустимого значения.", error_code);
            return error_code;
        }

        //Вывод на экран точек и статистики за текущий шаг
        #ifdef WIN32
            std::system("CLS");
        #else
            std::system("clear");
        #endif
        points.array_print();
        statistics.print();
        
        //Формирование разрешений на выполнение следующего шага (0 - следующий шаг, -1 - выход по причине того что нет включенных точек, 1 - выход потому что точки не меняются).
        permision_stop += ((int8_t)((points.enabled_points_count_get() > 0) && (points_enable_mask.enabled_points_count_get() == 0) && (points_disable_mask.enabled_points_count_get() == 0)));
        permision_stop -= ((int8_t)(points.enabled_points_count_get() == 0)); 
     
    }
    //Подведение итогов
    std::cout << (((permision_stop < 0) ? "Все точки выключены." : "Значения точек не меняются.")) << std::endl; 
  
    return error_codes_t::OK;
}
