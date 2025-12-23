#include "../inc/statistics.h"
#include <iostream>

Statistics::Statistics(unsigned short enabled_points_count,unsigned short disabled_points_count, unsigned short enabled_in_step_points_count, unsigned short disabled_in_step_points_count)
{
    this->clear();
    this->update(enabled_points_count, disabled_points_count, enabled_in_step_points_count, disabled_in_step_points_count);
}

void Statistics::clear(void)
{
    this->current_step.step_num = 0;
    this->current_step.enabled_points_count = 0;
    this->current_step.disabled_points_count = 0;
    this->current_step.enabled_in_step_points_count = 0;
    this->current_step.disabled_in_step_points_count = 0;
}

signed short Statistics::update(unsigned short enabled_points_count, unsigned short disabled_points_count, unsigned short enabled_in_step_points_count, unsigned short disabled_in_step_points_count)
{
    this->current_step.step_num++;
    this->current_step.enabled_points_count = enabled_points_count;
    this->current_step.disabled_points_count = disabled_points_count;
    this->current_step.enabled_in_step_points_count = enabled_in_step_points_count;
    this->current_step.disabled_in_step_points_count = disabled_in_step_points_count;

    return ((signed short)enabled_in_step_points_count) - ((signed short)disabled_in_step_points_count);
}

void Statistics::print(void)
{
    std::cout << "Шаг " << this->current_step.step_num
              << "  включено (всего) " <<  this->current_step.enabled_points_count 
              << "  выключено (всего) " << this->current_step.disabled_points_count 
              << "  включено (за шаг) " << this->current_step.enabled_in_step_points_count 
              << "  выключено (за шаг) " << this->current_step.disabled_in_step_points_count
              << "\n";
}