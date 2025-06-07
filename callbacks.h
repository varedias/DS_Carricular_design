#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>
#include "airline.h"

// 全局变量声明
extern GtkWidget *window;
extern GtkWidget *notebook;
extern GtkListStore *flight_store;
extern GtkListStore *customer_store;
extern GtkListStore *order_store;
extern GtkWidget *flight_tree_view;
extern GtkWidget *customer_tree_view;
extern GtkWidget *order_tree_view;

// 航班管理回调函数
void on_add_flight_clicked(GtkWidget *widget, gpointer data);
void on_edit_flight_clicked(GtkWidget *widget, gpointer data);
void on_delete_flight_clicked(GtkWidget *widget, gpointer data);
void on_refresh_flight_clicked(GtkWidget *widget, gpointer data);

// 客户管理回调函数
void on_add_customer_clicked(GtkWidget *widget, gpointer data);
void on_delete_customer_clicked(GtkWidget *widget, gpointer data);
void on_refresh_customer_clicked(GtkWidget *widget, gpointer data);

// 订单管理回调函数
void on_book_ticket_clicked(GtkWidget *widget, gpointer data);
void on_cancel_ticket_clicked(GtkWidget *widget, gpointer data);
void on_refresh_order_clicked(GtkWidget *widget, gpointer data);

// 列表刷新函数
void refresh_flight_list(GtkListStore *store);
void refresh_customer_list(GtkListStore *store);
void refresh_order_list(GtkListStore *store);

// 对话框辅助函数
void show_flight_dialog(GtkWidget *parent, Flight *flight, gboolean is_edit);
void show_customer_dialog(GtkWidget *parent, Customer *customer, gboolean is_edit);
void show_ticket_dialog(GtkWidget *parent);

#endif // CALLBACKS_H
