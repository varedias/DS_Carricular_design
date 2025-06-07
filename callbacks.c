#include "airline.h"
#include "constants.h"
#include "callbacks.h"

// 刷新列表函数的声明
void refresh_flight_list(GtkListStore *store);
void refresh_customer_list(GtkListStore *store);
void refresh_order_list(GtkListStore *store);

// 航班管理回调函数
void on_add_flight_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("添加航班",
                                                  GTK_WINDOW(window),
                                                  GTK_DIALOG_MODAL,
                                                  "确定", GTK_RESPONSE_ACCEPT,
                                                  "取消", GTK_RESPONSE_CANCEL,
                                                  NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // 创建输入框
    GtkWidget *flight_number_entry = gtk_entry_new();
    GtkWidget *airline_entry = gtk_entry_new();
    GtkWidget *departure_city_entry = gtk_entry_new();
    GtkWidget *arrival_city_entry = gtk_entry_new();
    GtkWidget *departure_time_entry = gtk_entry_new();
    GtkWidget *arrival_time_entry = gtk_entry_new();
    GtkWidget *price_entry = gtk_entry_new();
    GtkWidget *discount_entry = gtk_entry_new();
    GtkWidget *total_seats_entry = gtk_entry_new();

    // 添加标签和输入框
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("航班号:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), flight_number_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("航空公司:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), airline_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("出发城市:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), departure_city_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("到达城市:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), arrival_city_entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("出发时间:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), departure_time_entry, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("到达时间:"), 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), arrival_time_entry, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("票价:"), 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), price_entry, 1, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("折扣:"), 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), discount_entry, 1, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("总座位数:"), 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), total_seats_entry, 1, 8, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        Flight new_flight = {0};
        strncpy(new_flight.flight_number, gtk_entry_get_text(GTK_ENTRY(flight_number_entry)), sizeof(new_flight.flight_number) - 1);
        strncpy(new_flight.airline, gtk_entry_get_text(GTK_ENTRY(airline_entry)), sizeof(new_flight.airline) - 1);
        strncpy(new_flight.departure_city, gtk_entry_get_text(GTK_ENTRY(departure_city_entry)), sizeof(new_flight.departure_city) - 1);
        strncpy(new_flight.arrival_city, gtk_entry_get_text(GTK_ENTRY(arrival_city_entry)), sizeof(new_flight.arrival_city) - 1);
        strncpy(new_flight.departure_time, gtk_entry_get_text(GTK_ENTRY(departure_time_entry)), sizeof(new_flight.departure_time) - 1);
        strncpy(new_flight.arrival_time, gtk_entry_get_text(GTK_ENTRY(arrival_time_entry)), sizeof(new_flight.arrival_time) - 1);
        new_flight.price = atof(gtk_entry_get_text(GTK_ENTRY(price_entry)));
        new_flight.discount = atof(gtk_entry_get_text(GTK_ENTRY(discount_entry)));
        new_flight.total_seats = atoi(gtk_entry_get_text(GTK_ENTRY(total_seats_entry)));
        new_flight.available_seats = new_flight.total_seats;

        if (add_flight(&new_flight)) {
            show_info_message("航班添加成功！");
            // TODO: 刷新航班列表
        }
    }

    gtk_widget_destroy(dialog);
}

void on_edit_flight_clicked(GtkWidget *widget, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *flight_number;

    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        show_error_message("请先选择一个航班！");
        return;
    }

    // 获取选中航班的航班号
    gtk_tree_model_get(model, &iter, 0, &flight_number, -1);
    Flight* current_flight = search_flight_by_number(flight_number);
    if (!current_flight) {
        show_error_message("航班不存在！");
        g_free(flight_number);
        return;
    }

    // 创建编辑对话框
    GtkWidget *dialog = gtk_dialog_new_with_buttons("修改航班",
                                                  GTK_WINDOW(window),
                                                  GTK_DIALOG_MODAL,
                                                  "确定", GTK_RESPONSE_ACCEPT,
                                                  "取消", GTK_RESPONSE_CANCEL,
                                                  NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // 创建输入框
    GtkWidget *airline_entry = gtk_entry_new();
    GtkWidget *departure_city_entry = gtk_entry_new();
    GtkWidget *arrival_city_entry = gtk_entry_new();
    GtkWidget *departure_time_entry = gtk_entry_new();
    GtkWidget *arrival_time_entry = gtk_entry_new();
    GtkWidget *price_entry = gtk_entry_new();
    GtkWidget *discount_entry = gtk_entry_new();
    GtkWidget *total_seats_entry = gtk_entry_new();

    // 设置当前值
    gtk_entry_set_text(GTK_ENTRY(airline_entry), current_flight->airline);
    gtk_entry_set_text(GTK_ENTRY(departure_city_entry), current_flight->departure_city);
    gtk_entry_set_text(GTK_ENTRY(arrival_city_entry), current_flight->arrival_city);
    gtk_entry_set_text(GTK_ENTRY(departure_time_entry), current_flight->departure_time);
    gtk_entry_set_text(GTK_ENTRY(arrival_time_entry), current_flight->arrival_time);
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", current_flight->price);
    gtk_entry_set_text(GTK_ENTRY(price_entry), buffer);
    snprintf(buffer, sizeof(buffer), "%.2f", current_flight->discount);
    gtk_entry_set_text(GTK_ENTRY(discount_entry), buffer);
    snprintf(buffer, sizeof(buffer), "%d", current_flight->total_seats);
    gtk_entry_set_text(GTK_ENTRY(total_seats_entry), buffer);

    // 添加标签和输入框
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("航班号:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new(flight_number), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("航空公司:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), airline_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("出发城市:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), departure_city_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("到达城市:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), arrival_city_entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("出发时间:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), departure_time_entry, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("到达时间:"), 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), arrival_time_entry, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("票价:"), 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), price_entry, 1, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("折扣:"), 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), discount_entry, 1, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("总座位数:"), 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), total_seats_entry, 1, 8, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        Flight updated_flight = *current_flight;  // 保持原有的航班号和可用座位数
        
        strncpy(updated_flight.airline, gtk_entry_get_text(GTK_ENTRY(airline_entry)), sizeof(updated_flight.airline) - 1);
        strncpy(updated_flight.departure_city, gtk_entry_get_text(GTK_ENTRY(departure_city_entry)), sizeof(updated_flight.departure_city) - 1);
        strncpy(updated_flight.arrival_city, gtk_entry_get_text(GTK_ENTRY(arrival_city_entry)), sizeof(updated_flight.arrival_city) - 1);
        strncpy(updated_flight.departure_time, gtk_entry_get_text(GTK_ENTRY(departure_time_entry)), sizeof(updated_flight.departure_time) - 1);
        strncpy(updated_flight.arrival_time, gtk_entry_get_text(GTK_ENTRY(arrival_time_entry)), sizeof(updated_flight.arrival_time) - 1);
        updated_flight.price = atof(gtk_entry_get_text(GTK_ENTRY(price_entry)));
        updated_flight.discount = atof(gtk_entry_get_text(GTK_ENTRY(discount_entry)));
        int new_total_seats = atoi(gtk_entry_get_text(GTK_ENTRY(total_seats_entry)));
        
        // 更新可用座位数
        if (new_total_seats >= (updated_flight.total_seats - updated_flight.available_seats)) {
            updated_flight.available_seats += (new_total_seats - updated_flight.total_seats);
            updated_flight.total_seats = new_total_seats;
        } else {
            show_error_message("新的总座位数不能小于已订座位数！");
            g_free(flight_number);
            gtk_widget_destroy(dialog);
            return;
        }

        if (update_flight(&updated_flight)) {
            show_info_message("航班修改成功！");
            refresh_flight_list(GTK_LIST_STORE(model));
        }
    }

    g_free(flight_number);
    gtk_widget_destroy(dialog);
}

void on_delete_flight_clicked(GtkWidget *widget, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *flight_number;

    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        show_error_message("请先选择一个航班！");
        return;
    }

    gtk_tree_model_get(model, &iter, 0, &flight_number, -1);
    if (delete_flight(flight_number)) {
        show_info_message("航班删除成功！");
        // TODO: 刷新航班列表
    }
    g_free(flight_number);
}

// 客户管理回调函数
void on_add_customer_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("添加客户",
                                                  GTK_WINDOW(window),
                                                  GTK_DIALOG_MODAL,
                                                  "确定", GTK_RESPONSE_ACCEPT,
                                                  "取消", GTK_RESPONSE_CANCEL,
                                                  NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // 创建输入框
    GtkWidget *name_entry = gtk_entry_new();
    GtkWidget *id_entry = gtk_entry_new();

    // 添加标签和输入框
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("姓名:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("证件号:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), id_entry, 1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* name = gtk_entry_get_text(GTK_ENTRY(name_entry));
        const char* id = gtk_entry_get_text(GTK_ENTRY(id_entry));

        if (add_customer(name, id)) {
            show_info_message("客户添加成功！");
            refresh_customer_list(GTK_LIST_STORE(data));
        }
    }

    gtk_widget_destroy(dialog);
}

void on_delete_customer_clicked(GtkWidget *widget, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *id_number;

    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        show_error_message("请先选择一个客户！");
        return;
    }

    gtk_tree_model_get(model, &iter, 1, &id_number, -1);
    if (delete_customer(id_number)) {
        show_info_message("客户删除成功！");
        refresh_customer_list(GTK_LIST_STORE(gtk_tree_view_get_model(tree_view)));
    }
    g_free(id_number);
}

// 订单管理回调函数
void on_book_ticket_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("订票",
                                                  GTK_WINDOW(window),
                                                  GTK_DIALOG_MODAL,
                                                  "确定", GTK_RESPONSE_ACCEPT,
                                                  "取消", GTK_RESPONSE_CANCEL,
                                                  NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // 创建输入框
    GtkWidget *flight_entry = gtk_entry_new();
    GtkWidget *customer_entry = gtk_entry_new();
    GtkWidget *count_entry = gtk_entry_new();

    // 添加标签和输入框
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("航班号:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), flight_entry, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("客户证件号:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), customer_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("订票数量:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), count_entry, 1, 2, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* flight_number = gtk_entry_get_text(GTK_ENTRY(flight_entry));
        const char* customer_id = gtk_entry_get_text(GTK_ENTRY(customer_entry));
        int count = atoi(gtk_entry_get_text(GTK_ENTRY(count_entry)));

        if (book_ticket(flight_number, customer_id, count)) {
            show_info_message("订票成功！");
            refresh_flight_list(GTK_LIST_STORE(data));
            refresh_order_list(GTK_LIST_STORE(data));
        }
    }

    gtk_widget_destroy(dialog);
}

void on_cancel_ticket_clicked(GtkWidget *widget, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *order_id;

    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        show_error_message("请先选择一个订单！");
        return;
    }

    gtk_tree_model_get(model, &iter, 0, &order_id, -1);
    if (cancel_ticket(order_id)) {
        show_info_message("退票成功！");
        refresh_flight_list(GTK_LIST_STORE(data));
        refresh_order_list(GTK_LIST_STORE(data));
    }
    g_free(order_id);
}

// 刷新列表函数
void refresh_flight_list(GtkListStore *store) {
    gtk_list_store_clear(store);
    Flight* current = flight_list;
    GtkTreeIter iter;

    while (current) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, current->flight_number,
                          1, current->airline,
                          2, current->departure_city,
                          3, current->arrival_city,
                          4, current->departure_time,
                          5, current->arrival_time,
                          6, current->price,
                          7, current->discount,
                          8, current->available_seats,
                          -1);
        current = current->next;
    }
}

void refresh_customer_list(GtkListStore *store) {
    gtk_list_store_clear(store);
    Customer* current = customer_list;
    GtkTreeIter iter;

    while (current) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, current->name,
                          1, current->id_number,
                          -1);
        current = current->next;
    }
}

void refresh_order_list(GtkListStore *store) {
    gtk_list_store_clear(store);
    Order* current = order_list;
    GtkTreeIter iter;

    while (current) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, current->order_id,
                          1, current->flight_number,
                          2, current->customer_id,
                          3, current->ticket_count,
                          4, current->order_time,
                          -1);
        current = current->next;
    }
}

// 刷新按钮回调函数
void on_refresh_flight_clicked(GtkWidget *widget, gpointer data) {
    refresh_flight_list(GTK_LIST_STORE(data));
}

void on_refresh_customer_clicked(GtkWidget *widget, gpointer data) {
    refresh_customer_list(GTK_LIST_STORE(data));
}

void on_refresh_order_clicked(GtkWidget *widget, gpointer data) {
    refresh_order_list(GTK_LIST_STORE(data));
}
