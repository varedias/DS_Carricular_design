#ifndef AIRLINE_H
#define AIRLINE_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 航班信息结构体
typedef struct Flight {
    char flight_number[10];        // 航班号
    char airline[50];              // 航空公司
    char departure_city[50];       // 起飞城市
    char arrival_city[50];         // 到达城市
    char departure_time[20];       // 起飞时间
    char arrival_time[20];         // 到达时间
    double price;                  // 票价
    double discount;               // 折扣
    int total_seats;              // 总座位数
    int available_seats;          // 可用座位数
    struct Flight* next;          // 链表指针
} Flight;

// 客户信息结构体
typedef struct Customer {
    char name[50];                // 姓名
    char id_number[20];           // 证件号
    struct Customer* next;        // 链表指针
} Customer;

// 订单信息结构体
typedef struct Order {
    char order_id[20];           // 订单编号
    char flight_number[10];      // 航班号
    char customer_id[20];        // 客户证件号
    int ticket_count;            // 订票数量
    char order_time[20];         // 订单时间
    struct Order* next;          // 链表指针
} Order;

// 全局变量声明
extern Flight* flight_list;
extern Customer* customer_list;
extern Order* order_list;

// 函数声明
void init_gui(int argc, char *argv[]);
void save_flight_data();
void load_flight_data();
void save_customer_data();
void load_customer_data();
void save_order_data();
void load_order_data();
Flight* search_flight_by_number(const char* flight_number);
Flight* search_flights_by_city(const char* city, int departure);
Flight* search_flights_by_airline(const char* airline);
Flight* search_flights_by_discount(double discount);
int book_ticket(const char* flight_number, const char* customer_id, int count);
int cancel_ticket(const char* order_id);
void show_error_message(const char* message);
void show_info_message(const char* message);
int add_flight(const Flight* flight);
int update_flight(const Flight* flight);
int delete_flight(const char* flight_number);
Customer* search_customer_by_id(const char* id_number);
int add_customer(const char* name, const char* id_number);
int delete_customer(const char* id_number);
Order* create_order(const char* flight_number, const char* customer_id, int ticket_count);

#endif // AIRLINE_H
