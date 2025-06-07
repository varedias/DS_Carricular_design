#include "airline.h"
#include "constants.h"
#include <time.h>

// 生成订单编号
static void generate_order_id(char* order_id) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    sprintf(order_id, "ORD%04d%02d%02d%02d%02d%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
}

void save_order_data() {
    FILE* fp = fopen(ORDER_DATA_FILE, "wb");
    if (!fp) {
        show_error_message("无法保存订单数据！");
        return;
    }

    Order* current = order_list;
    while (current) {
        fwrite(current, sizeof(Order), 1, fp);
        current = current->next;
    }
    fclose(fp);
}

void load_order_data() {
    FILE* fp = fopen(ORDER_DATA_FILE, "rb");
    if (!fp) {
        return; // 文件不存在，这是首次运行
    }

    Order temp;
    Order* last = NULL;
    while (fread(&temp, sizeof(Order), 1, fp) == 1) {
        Order* new_order = (Order*)malloc(sizeof(Order));
        memcpy(new_order, &temp, sizeof(Order));
        new_order->next = NULL;

        if (!order_list) {
            order_list = new_order;
        } else {
            last->next = new_order;
        }
        last = new_order;
    }
    fclose(fp);
}

Order* create_order(const char* flight_number, const char* customer_id, int ticket_count) {
    Order* new_order = (Order*)malloc(sizeof(Order));
    generate_order_id(new_order->order_id);
    strncpy(new_order->flight_number, flight_number, sizeof(new_order->flight_number) - 1);
    strncpy(new_order->customer_id, customer_id, sizeof(new_order->customer_id) - 1);
    new_order->ticket_count = ticket_count;
    
    time_t now = time(NULL);
    strftime(new_order->order_time, sizeof(new_order->order_time),
             "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    new_order->next = NULL;

    if (!order_list) {
        order_list = new_order;
    } else {
        Order* current = order_list;
        while (current->next) {
            current = current->next;
        }
        current->next = new_order;
    }

    save_order_data();
    return new_order;
}

int cancel_order(const char* order_id) {
    Order* current = order_list;
    Order* prev = NULL;

    while (current && strcmp(current->order_id, order_id) != 0) {
        prev = current;
        current = current->next;
    }

    if (!current) {
        show_error_message("订单不存在！");
        return 0;
    }

    // 恢复航班座位
    Flight* flight = search_flight_by_number(current->flight_number);
    if (flight) {
        flight->available_seats += current->ticket_count;
        save_flight_data();
    }

    // 删除订单
    if (prev) {
        prev->next = current->next;
    } else {
        order_list = current->next;
    }
    free(current);
    save_order_data();
    
    return 1;
}

int cancel_ticket(const char* order_id) {
    Order* current = order_list;
    Order* prev = NULL;

    while (current && strcmp(current->order_id, order_id) != 0) {
        prev = current;
        current = current->next;
    }

    if (!current) {
        show_error_message("订单不存在！");
        return 0;
    }

    // 恢复航班座位
    Flight* flight = search_flight_by_number(current->flight_number);
    if (flight) {
        flight->available_seats += current->ticket_count;
        save_flight_data();
    }

    // 删除订单
    if (prev) {
        prev->next = current->next;
    } else {
        order_list = current->next;
    }

    free(current);
    save_order_data();
    return 1;
}
