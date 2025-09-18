/**
 * @FilePath: /vboot/lib/time.c
 * @Description:
 * @Author: scuec_weiqiang scuec_weiqiang@qq.com
 * @Date: 2025-08-29 00:50:51
 * @LastEditTime: 2025-09-17 23:15:46
 * @LastEditors: scuec_weiqiang scuec_weiqiang@qq.com
 * @Copyright    : G AUTOMOBILE RESEARCH INSTITUTE CO.,LTD Copyright (c) 2025.
 */
#include "time.h"
#include "types.h"

// 固定地址的时间缓冲区
volatile struct system_time *const system_time = (struct system_time *)0x50000000;

// 定义每月的天数（非闰年）
static const char days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// 判断是否为闰年
static int is_leap_year(u32 year)
{
    if (year % 4 != 0)
        return 0;
    if (year % 100 != 0)
        return 1;
    if (year % 400 != 0)
        return 0;
    return 1;
}

// 计算从1970年到指定年份的总天数
static u32 days_since_epoch(u32 year, u32 month, u32 day)
{
    u32 days = 0;

    // 累加完整年份的天数
    for (u32 y = 1970; y < year; y++)
    {
        days += is_leap_year(y) ? 366 : 365;
    }

    // 累加当月之前的月份天数
    for (u32 m = 1; m < month; m++)
    {
        days += days_in_month[m - 1];
        // 闰年2月加1天
        if (m == 2 && is_leap_year(year))
        {
            days += 1;
        }
    }

    // 加当月天数（减1是因为day从1开始）
    days += (day - 1);

    return days;
}

void adjust_timezone(struct system_time *t, enum UTC utc)
{
    int hour = (int)t->hour - utc;
    
    // 处理小时为负的情况（向前调整日期）
    while (hour < 0)
    {
        hour += 24;
        // 借一天
        t->day--;
        if (t->day == 0)
        {
            t->month--;
            if (t->month == 0)
            {
                t->month = 12;
                t->year -= 1;
            }
            // 获取当前月份的天数并设置
            t->day = days_since_epoch(t->year, t->month, 0); // 假设0表示获取当月天数
        }
    }

    // 处理小时超过24的情况（向后调整日期）
    while (hour >= 24)
    {
        hour -= 24;
        // 进一天
        t->day++;
        // 获取当前月份的最大天数
        int max_days = days_since_epoch(t->year, t->month, 0); // 假设0表示获取当月天数
        
        if (t->day > max_days)
        {
            t->day = 1;
            t->month++;
            if (t->month > 12)
            {
                t->month = 1;
                t->year += 1;
            }
        }
    }

    t->hour = (u32)hour;
}

// 将system_time_t转换为Unix时间戳（自1970-01-01 00:00:00 UTC以来的秒数）
u32 system_time_to_unix_timestamp(const struct system_time *t)
{
    // 基本校验
    if (t->year < 1970 || t->month < 1 || t->month > 12 ||
        t->day < 1 || t->day > 31 || t->hour > 23 ||
        t->minute > 59 || t->second > 59)
    {
        return 0; // 无效时间返回0
    }

    // 计算总秒数 = 天数×86400 + 小时×3600 + 分钟×60 + 秒
    u32 days = days_since_epoch(t->year, t->month, t->day);
    return days * 86400 + t->hour * 3600 + t->minute * 60 + t->second;
}

// 读取当前时间的函数
void get_current_time(struct system_time *t)
{
    // 简单的内存读取操作
    *t = *system_time;
}

u32 get_current_unix_timestamp(enum UTC utc)
{
    struct system_time t;
    get_current_time(&t);
    adjust_timezone(&t, utc);
    return system_time_to_unix_timestamp(&t);
}
