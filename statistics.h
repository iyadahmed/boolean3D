#pragma once

// Based on: https://www.johndcook.com/blog/standard_deviation/

typedef struct RunningStat
{
    int values_num;
    float old_mean, new_mean;
    float old_variance_denominator, new_variance_denominator;
} RunningStat;

RunningStat create_running_stat()
{
    RunningStat stat;
    stat.values_num = 0;
    stat.old_mean = stat.new_mean = 0.0f;
    stat.old_variance_denominator = stat.new_variance_denominator = 0.0f;
    return stat;
}

void update_stat(RunningStat *stat, float new_value)
{
    stat->values_num++;

    // See Knuth TAOCP vol 2, 3rd edition, page 232
    if (stat->values_num == 1)
    {
        stat->old_mean = stat->new_mean = new_value;
        stat->old_variance_denominator = 0.0;
    }
    else
    {
        stat->new_mean = stat->old_mean + (new_value - stat->old_mean) / stat->values_num;
        stat->new_variance_denominator = stat->old_variance_denominator + (new_value - stat->old_mean) * (new_value - stat->new_mean);

        // set up for next iteration
        stat->old_mean = stat->new_mean;
        stat->old_variance_denominator = stat->new_variance_denominator;
    }
}

float get_mean(const RunningStat *stat)
{
    return (stat->values_num > 0) ? stat->new_mean : 0.0;
}

float get_variance(const RunningStat *stat)
{
    return ((stat->values_num > 1) ? stat->new_variance_denominator / (stat->values_num - 1) : 0.0);
}