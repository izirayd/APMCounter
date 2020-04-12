
#include <Windows.h>

#include <fmt/fmt/format.h>



#include <iostream>

#include <chrono>
#include <vector>
#include <thread>


using namespace fmt;
using namespace std::chrono_literals;


using time_point_t = std::chrono::steady_clock::time_point;
using duration_t = std::chrono::duration<double, std::milli>;

class key_info_t
{
public:
    key_info_t() = default;
    key_info_t(int index_key_arg) {
        index_key = index_key_arg;
    }

    int  index_key = -1;
    bool is_pressed = false;
    bool is_released = false;

    time_point_t start_pressed;
    time_point_t end_pressed;

    duration_t duration_press = 0ms;
};

class keys_info_t
{
public:
    std::vector<key_info_t> info;

    keys_info_t() {

        for (int i = 0; i < 255; i++)
        {
            info.push_back({ i + 1 });
        }
    }
};

class statistics_t
{
public:
    struct counts_t
    {
        // current
        uint64_t press_all = 0;
        uint64_t press_mouse = 0;
        uint64_t press_keys = 0;

        // last
        uint64_t last_press_all = 0;
        uint64_t last_press_mouse = 0;
        uint64_t last_press_keys = 0;


        uint64_t left_click  = 0;
        uint64_t right_click = 0;


        uint64_t last_left_click = 0;
        uint64_t last_right_click = 0;

        void reset()
        {
             press_all = 0;
             press_mouse = 0;
             press_keys = 0;

            // last
             last_press_all = 0;
             last_press_mouse = 0;
             last_press_keys = 0;


             left_click = 0;
             right_click = 0;


             last_left_click = 0;
             last_right_click = 0;
        }
    };

    counts_t total;
    counts_t per_second;
    counts_t per_min;

    time_point_t timer_for_sec;
    time_point_t timer_for_min;

    struct avg_t
    {
        double result   = 0;

        uint64_t sum    = 0;
        uint64_t count  = 0;

        double result_left_click = 0;
        double result_right_click = 0;

        uint64_t sum_left_click = 0;
        uint64_t count_left_click = 0;
        
        uint64_t sum_right_click = 0;
        uint64_t count_right_click = 0;

        void iteration(const counts_t &counts)
        {
            sum += counts.press_all;

            count++;
            result = (double) sum / (double) count;

            sum_left_click += counts.left_click;

            count_left_click++;
            result_left_click = (double)sum_left_click / (double)count_left_click;

            sum_right_click += counts.right_click;

            count_right_click++;
            result_right_click = (double)sum_right_click / (double)count_right_click;
        }

        void reset()
        {
             result = 0;

             sum = 0;
             count = 0;

             result_left_click = 0;
             result_right_click = 0;

             sum_left_click = 0;
             count_left_click = 0;

             sum_right_click = 0;
             count_right_click = 0;
        }
    };

    avg_t avg_sec;
    avg_t avg_minute;

    int max_apm_sec      = 0;
    int max_apm_min      = 0;

    void reset()
    {
         total.reset();
         per_second.reset();
         per_min.reset();
         avg_sec.reset();
         avg_minute.reset();
         max_apm_sec = 0;
         max_apm_min = 0;
    }
};


class keys_reader_t
{
public:
    keys_info_t* keys_info = nullptr;
    statistics_t* statistics_info = nullptr;

    bool is_close_state = false;

    bool is_process_close_released = false;

    void close()
    {
        is_close_state = true;
    }

    void keys_read_loop(keys_info_t* keys, statistics_t* statistics)
    {
        is_process_close_released = false;
        is_close_state = false;

        keys_info = keys;
        statistics_info = statistics;

        if (keys_info == nullptr)
            return;

        if (statistics_info)
        {
            statistics_info->timer_for_sec = std::chrono::high_resolution_clock::now();
            statistics_info->timer_for_min = std::chrono::high_resolution_clock::now();
        }

        while (!is_close_state)
        {
            if (statistics_info)
            {
                duration_t duration = std::chrono::high_resolution_clock::now() - statistics_info->timer_for_sec;

                if (duration > 1s)
                {
                    statistics_info->timer_for_sec = std::chrono::high_resolution_clock::now();

                    statistics_info->avg_sec.iteration(statistics_info->per_second);


                    if (statistics_info->max_apm_sec < statistics_info->per_second.press_all)
                    {
                        statistics_info->max_apm_sec = statistics_info->per_second.press_all;
                    }

                    statistics_info->per_second.last_press_all   = statistics_info->per_second.press_all;
                    statistics_info->per_second.last_press_keys  = statistics_info->per_second.press_keys;
                    statistics_info->per_second.last_press_mouse = statistics_info->per_second.press_mouse;

                    statistics_info->per_second.last_left_click  = statistics_info->per_second.left_click;
                    statistics_info->per_second.last_right_click = statistics_info->per_second.right_click;

                    statistics_info->per_second.press_all   = 0;
                    statistics_info->per_second.press_keys  = 0;
                    statistics_info->per_second.press_mouse = 0;               
                    statistics_info->per_second.left_click  = 0;               
                    statistics_info->per_second.right_click = 0;               
                }

                duration = std::chrono::high_resolution_clock::now() - statistics_info->timer_for_min;

                if (duration > 60s)
                {
                    statistics_info->timer_for_min = std::chrono::high_resolution_clock::now();

                    statistics_info->avg_minute.iteration(statistics_info->per_min);

                    if (statistics_info->max_apm_min < statistics_info->per_min.press_all)
                    {
                        statistics_info->max_apm_min = statistics_info->per_min.press_all;
                    }

                    statistics_info->per_min.last_press_all   = statistics_info->per_min.press_all;
                    statistics_info->per_min.last_press_keys  = statistics_info->per_min.press_keys;
                    statistics_info->per_min.last_press_mouse = statistics_info->per_min.press_mouse;

                    statistics_info->per_min.last_left_click = statistics_info->per_min.left_click;
                    statistics_info->per_min.last_right_click = statistics_info->per_min.right_click;

                    statistics_info->per_min.press_all   = 0;
                    statistics_info->per_min.press_keys  = 0;
                    statistics_info->per_min.press_mouse = 0;

                    statistics_info->per_min.left_click  = 0;
                    statistics_info->per_min.right_click = 0;

                }
            }

            for (int i = 0; i < 255; i++) {

                // ignore shift, ctrl and alt, because why use left or right opcodes
                // we ignore only unique opcodes
                if (
                    keys_info->info[i].index_key == 160 ||
                    keys_info->info[i].index_key == 161 ||
                    keys_info->info[i].index_key == 162 ||
                    keys_info->info[i].index_key == 163 ||
                    keys_info->info[i].index_key == 164 ||
                    keys_info->info[i].index_key == 165
                    )
                    continue;

                if (GetAsyncKeyState(keys_info->info[i].index_key))
                {
                    if (keys_info->info[i].is_pressed)
                    {

                    }
                    else
                    {
                        keys_info->info[i].is_pressed  = true;
                        keys_info->info[i].is_released = false;

                        keys_info->info[i].start_pressed = std::chrono::high_resolution_clock::now();

                        printf("%d is pressed\n", keys_info->info[i].index_key);

                        if (statistics_info)
                        {
                            statistics_info->total.press_all++;

                            if (keys_info->info[i].index_key == 1)
                            {
                                statistics_info->total.left_click++;
                                statistics_info->per_second.left_click++;
                                statistics_info->per_min.left_click++;
                            }

                            if (keys_info->info[i].index_key == 2)
                            {
                                statistics_info->total.right_click++;
                                statistics_info->per_second.right_click++;
                                statistics_info->per_min.right_click++;
                            }


                            if (keys_info->info[i].index_key > 5)
                            {
                                statistics_info->total.press_keys++;
                            }
                            else
                            {
                                statistics_info->total.press_mouse++;
                            }

                            statistics_info->per_second.press_all++;

                            if (keys_info->info[i].index_key > 5)
                            {
                                statistics_info->per_second.press_keys++;
                            }
                            else
                            {
                                statistics_info->per_second.press_mouse++;
                            }

                            statistics_info->per_min.press_all++;

                            if (keys_info->info[i].index_key > 5)
                            {
                                statistics_info->per_min.press_keys++;
                            }
                            else
                            {
                                statistics_info->per_min.press_mouse++;
                            }
                        }
                    }
                }
                else
                {
                    if (keys_info->info[i].is_released)
                        keys_info->info[i].is_released = false;

                    if (keys_info->info[i].is_pressed)
                    {
                        keys_info->info[i].is_released = true;


                        keys_info->info[i].end_pressed = std::chrono::high_resolution_clock::now();
                        keys_info->info[i].duration_press = keys_info->info[i].end_pressed - keys_info->info[i].start_pressed;

                        keys_info->info[i].is_pressed = false;
                    }

                    if (keys_info->info[i].is_released)
                    {
                        printf("%d is released duration: %f ms\n", keys_info->info[i].index_key, keys_info->info[i].duration_press.count());
                    }
                }
            }

            std::this_thread::sleep_for(1ms);
        }

        is_process_close_released = true;
    }
};


class apm_t
{
public:

    keys_reader_t apm_base;
    keys_info_t   keys_info;
    statistics_t  statistics;

    void async_run()
    {
        std::thread th(&keys_reader_t::keys_read_loop, &apm_base, &keys_info, &statistics);
        th.detach();
    }

    void close()
    {
        apm_base.close();

        while (!apm_base.is_process_close_released)
        {
            std::this_thread::sleep_for(1ms);
        }
    }
};

int main()
{

    apm_t apm;

    apm.async_run();

    std::this_thread::sleep_for(1s);

    apm.close();

    std::cout << "Hello World!\n";


    for (;;) {


       std::string title = fmt::format("title per/sec: {} per/min: {} avg sec: {} avg min: {} max sec: {} max min: {}", apm.statistics.per_second.last_press_all, apm.statistics.per_min.press_all,

           apm.statistics.avg_sec.result,
           apm.statistics.avg_minute.result,
           apm.statistics.max_apm_sec,
           apm.statistics.max_apm_min

       );
       std::system(title.c_str());

        std::this_thread::sleep_for(1s);
    }
}