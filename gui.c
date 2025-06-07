#include "airline.h"
#include "callbacks.h"

// 全局变量定义
Flight* flight_list = NULL;
Customer* customer_list = NULL;
Order* order_list = NULL;
GtkWidget *window;
GtkWidget *notebook;

// 全局存储
GtkListStore *flight_store = NULL;
GtkListStore *customer_store = NULL;
GtkListStore *order_store = NULL;

// 全局树形视图
GtkWidget *flight_tree_view = NULL;
GtkWidget *customer_tree_view = NULL;
GtkWidget *order_tree_view = NULL;

static void create_flight_page(GtkWidget *notebook);
static void create_customer_page(GtkWidget *notebook);
static void create_order_page(GtkWidget *notebook);

// 显示错误消息对话框
void show_error_message(const char* message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_ERROR,
                                             GTK_BUTTONS_CLOSE,
                                             "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// 显示信息消息对话框
void show_info_message(const char* message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK,
                                             "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// 创建航班管理页面
static void create_flight_page(GtkWidget *notebook) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(page), 10);

    // 创建列表存储
    flight_store = gtk_list_store_new(9, 
        G_TYPE_STRING,  // 航班号
        G_TYPE_STRING,  // 航空公司
        G_TYPE_STRING,  // 出发城市
        G_TYPE_STRING,  // 到达城市
        G_TYPE_STRING,  // 出发时间
        G_TYPE_STRING,  // 到达时间
        G_TYPE_DOUBLE,  // 票价
        G_TYPE_DOUBLE,  // 折扣
        G_TYPE_INT     // 可用座位
    );

    // 创建树形视图
    flight_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(flight_store));
    g_object_unref(flight_store);

    // 添加列
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column;

    column = gtk_tree_view_column_new_with_attributes("航班号", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("航空公司", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("出发城市", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("到达城市", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("出发时间", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("到达时间", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("票价", renderer, "text", 6, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("折扣", renderer, "text", 7, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("可用座位", renderer, "text", 8, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(flight_tree_view), column);

    // 创建滚动窗口
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), flight_tree_view);
    gtk_box_pack_start(GTK_BOX(page), scroll, TRUE, TRUE, 0);

    // 创建按钮盒子
    GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(page), button_box, FALSE, FALSE, 0);

    // 添加按钮
    GtkWidget *add_button = gtk_button_new_with_label("添加航班");
    GtkWidget *edit_button = gtk_button_new_with_label("修改航班");
    GtkWidget *delete_button = gtk_button_new_with_label("删除航班");
    GtkWidget *refresh_button = gtk_button_new_with_label("刷新列表");

    gtk_container_add(GTK_CONTAINER(button_box), add_button);
    gtk_container_add(GTK_CONTAINER(button_box), edit_button);
    gtk_container_add(GTK_CONTAINER(button_box), delete_button);
    gtk_container_add(GTK_CONTAINER(button_box), refresh_button);

    // 连接按钮信号
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_flight_clicked), flight_store);
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_flight_clicked), flight_tree_view);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_flight_clicked), flight_tree_view);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(on_refresh_flight_clicked), flight_store);

    GtkWidget *label = gtk_label_new("航班管理");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, label);
}

// 创建客户管理页面
static void create_customer_page(GtkWidget *notebook) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(page), 10);

    // 创建列表存储
    customer_store = gtk_list_store_new(2,
        G_TYPE_STRING,  // 姓名
        G_TYPE_STRING   // 证件号
    );

    // 创建树形视图
    customer_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(customer_store));
    g_object_unref(customer_store);

    // 添加列
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column;

    column = gtk_tree_view_column_new_with_attributes("姓名", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(customer_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("证件号", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(customer_tree_view), column);

    // 创建滚动窗口
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), customer_tree_view);
    gtk_box_pack_start(GTK_BOX(page), scroll, TRUE, TRUE, 0);

    // 创建按钮盒子
    GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(page), button_box, FALSE, FALSE, 0);

    // 添加按钮
    GtkWidget *add_button = gtk_button_new_with_label("添加客户");
    GtkWidget *delete_button = gtk_button_new_with_label("删除客户");
    GtkWidget *refresh_button = gtk_button_new_with_label("刷新列表");

    gtk_container_add(GTK_CONTAINER(button_box), add_button);
    gtk_container_add(GTK_CONTAINER(button_box), delete_button);
    gtk_container_add(GTK_CONTAINER(button_box), refresh_button);

    // 连接按钮信号
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_customer_clicked), customer_store);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_customer_clicked), customer_tree_view);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(on_refresh_customer_clicked), customer_store);

    GtkWidget *label = gtk_label_new("客户管理");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, label);
}

// 创建订单管理页面
static void create_order_page(GtkWidget *notebook) {
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(page), 10);

    // 创建列表存储
    order_store = gtk_list_store_new(5,
        G_TYPE_STRING,  // 订单号
        G_TYPE_STRING,  // 航班号
        G_TYPE_STRING,  // 客户证件号
        G_TYPE_INT,     // 票数
        G_TYPE_STRING   // 订单时间
    );

    // 创建树形视图
    order_tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(order_store));
    g_object_unref(order_store);

    // 添加列
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *column;

    column = gtk_tree_view_column_new_with_attributes("订单号", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(order_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("航班号", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(order_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("客户证件号", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(order_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("票数", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(order_tree_view), column);

    column = gtk_tree_view_column_new_with_attributes("订单时间", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(order_tree_view), column);

    // 创建滚动窗口
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll), order_tree_view);
    gtk_box_pack_start(GTK_BOX(page), scroll, TRUE, TRUE, 0);

    // 创建按钮盒子
    GtkWidget *button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(page), button_box, FALSE, FALSE, 0);

    // 添加按钮
    GtkWidget *book_button = gtk_button_new_with_label("订票");
    GtkWidget *cancel_button = gtk_button_new_with_label("退票");
    GtkWidget *refresh_button = gtk_button_new_with_label("刷新列表");

    gtk_container_add(GTK_CONTAINER(button_box), book_button);
    gtk_container_add(GTK_CONTAINER(button_box), cancel_button);
    gtk_container_add(GTK_CONTAINER(button_box), refresh_button);

    // 连接按钮信号
    g_signal_connect(book_button, "clicked", G_CALLBACK(on_book_ticket_clicked), order_store);
    g_signal_connect(cancel_button, "clicked", G_CALLBACK(on_cancel_ticket_clicked), order_tree_view);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(on_refresh_order_clicked), order_store);

    GtkWidget *label = gtk_label_new("订单管理");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), page, label);
}

// 初始化GUI
void init_gui(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // 创建主窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "航空订票系统");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 创建notebook
    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // 创建各个页面
    create_flight_page(notebook);
    create_customer_page(notebook);
    create_order_page(notebook);

    // 显示所有组件
    gtk_widget_show_all(window);
}
