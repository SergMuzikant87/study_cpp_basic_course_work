#include "../inc/statistics.h"
#include <iostream>

#define STATISTICS_STEP_NUM_MAXIMUM (1000)

Statistics::Statistics(uint16_t enabled_points_count,uint16_t disabled_points_count, uint16_t enabled_in_step_points_count, uint16_t disabled_in_step_points_count)
{
    this->clear();
    this->update(enabled_points_count, disabled_points_count, enabled_in_step_points_count, disabled_in_step_points_count);
}

void Statistics::clear(void)
{
    this->update(0, 0, 0, 0);
    this->current_step.step_num = 0;
}

error_codes_t Statistics::update(uint16_t enabled_points_count, uint16_t disabled_points_count, uint16_t enabled_in_step_points_count, uint16_t disabled_in_step_points_count)
{
    if((this->current_step.step_num = this->current_step.step_num + 1) > STATISTICS_STEP_NUM_MAXIMUM)
    {
        return error_codes_t::STATISTICS_STEP_NUM_OVERRANGE;
    }

    this->current_step.enabled_points_count = enabled_points_count;
    this->current_step.disabled_points_count = disabled_points_count;
    this->current_step.enabled_in_step_points_count = enabled_in_step_points_count;
    this->current_step.disabled_in_step_points_count = disabled_in_step_points_count;

    return error_codes_t::OK;
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