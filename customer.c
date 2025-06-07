#include "airline.h"
#include "constants.h"

void save_customer_data() {
    FILE* fp = fopen(CUSTOMER_DATA_FILE, "wb");
    if (!fp) {
        show_error_message("无法保存客户数据！");
        return;
    }

    Customer* current = customer_list;
    while (current) {
        fwrite(current, sizeof(Customer), 1, fp);
        current = current->next;
    }
    fclose(fp);
}

void load_customer_data() {
    FILE* fp = fopen(CUSTOMER_DATA_FILE, "rb");
    if (!fp) {
        return; // 文件不存在，这是首次运行
    }

    Customer temp;
    Customer* last = NULL;
    while (fread(&temp, sizeof(Customer), 1, fp) == 1) {
        Customer* new_customer = (Customer*)malloc(sizeof(Customer));
        memcpy(new_customer, &temp, sizeof(Customer));
        new_customer->next = NULL;

        if (!customer_list) {
            customer_list = new_customer;
        } else {
            last->next = new_customer;
        }
        last = new_customer;
    }
    fclose(fp);
}

Customer* search_customer_by_id(const char* id_number) {
    Customer* current = customer_list;
    while (current) {
        if (strcmp(current->id_number, id_number) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int add_customer(const char* name, const char* id_number) {
    if (search_customer_by_id(id_number)) {
        show_error_message("客户已存在！");
        return 0;
    }

    Customer* new_customer = (Customer*)malloc(sizeof(Customer));
    strncpy(new_customer->name, name, sizeof(new_customer->name) - 1);
    strncpy(new_customer->id_number, id_number, sizeof(new_customer->id_number) - 1);
    new_customer->next = NULL;

    if (!customer_list) {
        customer_list = new_customer;
    } else {
        Customer* current = customer_list;
        while (current->next) {
            current = current->next;
        }
        current->next = new_customer;
    }

    save_customer_data();
    return 1;
}

int delete_customer(const char* id_number) {
    // 检查该客户是否有未完成的订单
    Order* current_order = order_list;
    while (current_order) {
        if (strcmp(current_order->customer_id, id_number) == 0) {
            show_error_message("该客户有未完成的订单，无法删除！");
            return 0;
        }
        current_order = current_order->next;
    }

    Customer* current = customer_list;
    Customer* prev = NULL;

    while (current && strcmp(current->id_number, id_number) != 0) {
        prev = current;
        current = current->next;
    }

    if (!current) {
        show_error_message("客户不存在！");
        return 0;
    }

    if (prev) {
        prev->next = current->next;
    } else {
        customer_list = current->next;
    }

    free(current);
    save_customer_data();
    return 1;
}
