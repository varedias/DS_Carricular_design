#include "airline.h"
#include "constants.h"
#include <sys/stat.h>
#include <errno.h>

// 创建数据目录
static void create_data_directory() {
    struct stat st = {0};
    if (stat("data", &st) == -1) {
        if (mkdir("data", 0700) == -1) {
            fprintf(stderr, "Error creating data directory: %s\n", strerror(errno));
            exit(1);
        }
    }
}

// 保存所有数据
static void save_all_data() {
    save_flight_data();
    save_customer_data();
    save_order_data();
}

int main(int argc, char *argv[]) {
    // 创建数据目录
    create_data_directory();

    // 加载数据
    load_flight_data();
    load_customer_data();
    load_order_data();

    // 初始化GUI
    init_gui(argc, argv);

    // 注册退出时的回调函数
    atexit(save_all_data);

    // 启动主循环
    gtk_main();

    return 0;
}
