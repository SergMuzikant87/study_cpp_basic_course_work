#include "../inc/app.h"
#include "../inc/points.h"
#include "../inc/statistics.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

App::App(Settings * settings)
{
   this->settings = settings; 
}

status_t App::run(void)
{
    status_t error_code = status_t::OK;

    //Точки пространства
    Points points(this->settings->config_file_addr, &error_code); 
    if(error_code != status_t::OK) 
    {
        return error_code;
    }
    
    //Маски разрешений на включение точек
    Points points_disable_mask(points.rows_count, points.cols_count, &error_code); 
    if(error_code != status_t::OK) 
    {
        return error_code;
    }
    
    //Маски разрешений на выключение точек
    Points points_enable_mask(points.rows_count, points.cols_count, &error_code); 
    if(error_code != status_t::OK) 
    {
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
    std::system("clear");
    points.array_print();
    statistics.print();

    //Перебор шагов.
    signed short permision_stop {(signed short)(0 - ((signed short)(points.enabled_points_count_get() == 0)))};
    while(permision_stop == 0)
    {
        //Пауза
        sleep(this->settings->step_time_sec);
        
        //Очистка масок разрешений на включение и выключение точек
        points_enable_mask.array_clean();
        points_disable_mask.array_clean();

        //Перебор строк пространства точек
        for(unsigned short row_num = 0; row_num < points.rows_count; row_num++)
        {
            //Перебор точек (столбцов) внутри текущей строки пространства точек
            for(unsigned short col_num = 0; col_num < points.cols_count; col_num++)
            {
                //Состояние текущей точки
                unsigned short point_value = points.point_value_get(row_num, col_num); 

                //Перебор точек, находящихся рядом с текущей точкой и подсчёт их количества
                unsigned short enabled_compare_points_count = 0;
                for(unsigned short compare_row_num = ((row_num > 0) ? (row_num - 1) : 0); (compare_row_num <= (row_num + 1)) && (compare_row_num < points.rows_count); compare_row_num++)
                {
                    for(unsigned short compare_col_num = ((col_num > 0) ? (col_num - 1) : 0); (compare_col_num <= (col_num + 1)) && (compare_col_num < points.cols_count); compare_col_num++)
                    {
                        if((row_num == compare_row_num) && (col_num == compare_col_num))
                        {
                            continue;
                        }

                        //Инкрементирования количества точек, которые находятся рядом с текущей
                        enabled_compare_points_count += points.point_value_get(compare_row_num, compare_col_num);
                    }
                }

                //Формирование разрешений на включение и выключение текущей точки
                (!point_value) 
                ? points_enable_mask.point_value_set( row_num, col_num, (enabled_compare_points_count == 3) )
                : points_disable_mask.point_value_set( row_num, col_num, ((enabled_compare_points_count != 2) && (enabled_compare_points_count != 3)) );   
            }
        }

        //Включение точек, у которых были сформированны разрешения на включение
        //Выключение точек, у которых были сформированны разрешения на выключение
        points.array_update(points_enable_mask.array, points_disable_mask.array);
        
        //Обновление статистики
        statistics.update
        (
            points.enabled_points_count_get(),             
            points.disabled_points_count_get(),            
            points_enable_mask.enabled_points_count_get(), 
            points_disable_mask.enabled_points_count_get() 
        );

        //Вывод на экран точек и статистики за текущий шаг
        std::system("clear");
        points.array_print();
        statistics.print();
        
        //Формирование разрешений на выполнение следующего шага.
        permision_stop += ((signed short)((points.enabled_points_count_get() > 0) && (points_enable_mask.enabled_points_count_get() == 0) && (points_disable_mask.enabled_points_count_get() == 0)));
        permision_stop -= ((signed short)(points.enabled_points_count_get() == 0)); 
     
    }
    //Подведение итогов
    std::cout << (((permision_stop < 0) ? "Все точки выключены." : "Значения точек не меняются.")) << std::endl; 
  
    return status_t::OK;
}