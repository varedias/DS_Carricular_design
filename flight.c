#include "airline.h"
#include "constants.h"
#include <string.h>

void save_flight_data() {
    FILE* fp = fopen(FLIGHT_DATA_FILE, "wb");
    if (!fp) {
        show_error_message("无法保存航班数据！");
        return;
    }

    Flight* current = flight_list;
    while (current) {
        fwrite(current, sizeof(Flight), 1, fp);
        current = current->next;
    }
    fclose(fp);
}

void load_flight_data() {
    FILE* fp = fopen(FLIGHT_DATA_FILE, "rb");
    if (!fp) {
        return; // 文件不存在，这是首次运行
    }

    Flight temp;
    Flight* last = NULL;
    while (fread(&temp, sizeof(Flight), 1, fp) == 1) {
        Flight* new_flight = (Flight*)malloc(sizeof(Flight));
        memcpy(new_flight, &temp, sizeof(Flight));
        new_flight->next = NULL;

        if (!flight_list) {
            flight_list = new_flight;
        } else {
            last->next = new_flight;
        }
        last = new_flight;
    }
    fclose(fp);
}

Flight* search_flight_by_number(const char* flight_number) {
    Flight* current = flight_list;
    while (current) {
        if (strcmp(current->flight_number, flight_number) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Flight* search_flights_by_city(const char* city, int departure) {
    Flight* result_list = NULL;
    Flight* last = NULL;
    Flight* current = flight_list;

    while (current) {
        if ((departure && strcmp(current->departure_city, city) == 0) ||
            (!departure && strcmp(current->arrival_city, city) == 0)) {
            Flight* new_flight = (Flight*)malloc(sizeof(Flight));
            memcpy(new_flight, current, sizeof(Flight));
            new_flight->next = NULL;

            if (!result_list) {
                result_list = new_flight;
            } else {
                last->next = new_flight;
            }
            last = new_flight;
        }
        current = current->next;
    }
    return result_list;
}

Flight* search_flights_by_airline(const char* airline) {
    Flight* result_list = NULL;
    Flight* last = NULL;
    Flight* current = flight_list;

    while (current) {
        if (strcmp(current->airline, airline) == 0) {
            Flight* new_flight = (Flight*)malloc(sizeof(Flight));
            memcpy(new_flight, current, sizeof(Flight));
            new_flight->next = NULL;

            if (!result_list) {
                result_list = new_flight;
            } else {
                last->next = new_flight;
            }
            last = new_flight;
        }
        current = current->next;
    }
    return result_list;
}

Flight* search_flights_by_discount(double discount) {
    Flight* result_list = NULL;
    Flight* last = NULL;
    Flight* current = flight_list;

    while (current) {
        if (current->discount <= discount) {
            Flight* new_flight = (Flight*)malloc(sizeof(Flight));
            memcpy(new_flight, current, sizeof(Flight));
            new_flight->next = NULL;

            if (!result_list) {
                result_list = new_flight;
            } else {
                last->next = new_flight;
            }
            last = new_flight;
        }
        current = current->next;
    }
    return result_list;
}

int add_flight(const Flight* flight) {
    if (search_flight_by_number(flight->flight_number)) {
        show_error_message("航班号已存在！");
        return 0;
    }

    Flight* new_flight = (Flight*)malloc(sizeof(Flight));
    memcpy(new_flight, flight, sizeof(Flight));
    new_flight->next = NULL;

    if (!flight_list) {
        flight_list = new_flight;
    } else {
        Flight* current = flight_list;
        while (current->next) {
            current = current->next;
        }
        current->next = new_flight;
    }

    save_flight_data();
    return 1;
}

int update_flight(const Flight* flight) {
    Flight* current = search_flight_by_number(flight->flight_number);
    if (!current) {
        show_error_message("航班不存在！");
        return 0;
    }

    memcpy(current, flight, sizeof(Flight));
    current->next = current->next; // 保持原有的链表结构

    save_flight_data();
    return 1;
}

int delete_flight(const char* flight_number) {
    Flight* current = flight_list;
    Flight* prev = NULL;

    while (current && strcmp(current->flight_number, flight_number) != 0) {
        prev = current;
        current = current->next;
    }

    if (!current) {
        show_error_message("航班不存在！");
        return 0;
    }

    if (prev) {
        prev->next = current->next;
    } else {
        flight_list = current->next;
    }
    
    free(current);
    save_flight_data();
    return 1;
}

int book_ticket(const char* flight_number, const char* customer_id, int count) {
    Flight* flight = search_flight_by_number(flight_number);
    if (!flight) {
        show_error_message("航班不存在！");
        return 0;
    }

    if (flight->available_seats < count) {
        show_error_message("该航班座位不足！");
        return 0;
    }

    // 检查客户是否存在
    Customer* customer = search_customer_by_id(customer_id);
    if (!customer) {
        show_error_message("客户不存在！");
        return 0;
    }

    // 创建订单
    Order* order = create_order(flight_number, customer_id, count);
    if (!order) {
        show_error_message("创建订单失败！");
        return 0;
    }

    // 更新航班座位
    flight->available_seats -= count;
    save_flight_data();

    return 1;
}
