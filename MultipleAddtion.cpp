#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_TERMS 100
#define MAX_LINE 1024

typedef struct {
    double coef;  // 系数
    double exp;   // 指数
} Polynomial;

// 解析多项式字符串
int ParsePolynomial(char* line, Polynomial poly[]) {
    int i = 0, n = strlen(line);
    int idx = 0;
    
    while (i < n) {
        int sign = 1;
        if (line[i] == '+') { sign = 1; ++i; }
        else if (line[i] == '-') { sign = -1; ++i; }
        
        double coef = 0;
        int hasCoef = 0;  // 标记是否读取到系数
        while (i < n && isdigit(line[i])) {
            coef = coef * 10 + (line[i] - '0');
            ++i;
            hasCoef = 1;
        }
        if (!hasCoef) coef = 1;  // 如果没有系数，默认为1
        coef *= sign;
        
        if (i < n && line[i] == 'x') ++i;
        
        double exp = 1;
        if (i < n && line[i] == '^') {
            ++i;
            exp = 0;
            while (i < n && isdigit(line[i])) {
                exp = exp * 10 + (line[i] - '0');
                ++i;
            }
        } else if (i == n || line[i] == '+' || line[i] == '-') {
            exp = (hasCoef && (i == n || line[i-1] != 'x')) ? 0 : 1;
        }
        
        poly[idx].coef = coef;
        poly[idx].exp = exp;
        idx++;
    }
    return idx;
}

// 按序列格式输出多项式
void PrintPolynomialSeq(Polynomial poly[], int n) {
    printf("%d", n);  // 首先输出项数
    for(int i = 0; i < n; i++) {
        printf(",%.0f,%.0f", poly[i].coef, poly[i].exp);
    }
    printf("\n");
}

// 标准格式输出多项式
void PrintPolynomial(Polynomial poly[], int n) {
    int first = 1;
    for(int i = 0; i < n; i++) {
        if(!first && poly[i].coef > 0) printf("+");
        if(poly[i].coef < 0) printf("-");
        double absCoef = fabs(poly[i].coef);
        if(absCoef != 1 || poly[i].exp == 0) printf("%.0f", absCoef);
        if(poly[i].exp > 0) {
            printf("x");
            if(poly[i].exp != 1) printf("^%.0f", poly[i].exp);
        }
        first = 0;
    }
    printf("\n");
}

// 多项式相加
int AddPolynomials(Polynomial a[], int na, Polynomial b[], int nb, Polynomial result[]) {
    Polynomial temp[MAX_TERMS];
    int nt = 0;
    
    // 复制第一个多项式到临时数组
    for(int i = 0; i < na; i++) {
        temp[nt++] = a[i];
    }
    
    // 合并第二个多项式
    for(int i = 0; i < nb; i++) {
        int found = 0;
        for(int j = 0; j < nt; j++) {
            if(temp[j].exp == b[i].exp) {
                temp[j].coef += b[i].coef;
                found = 1;
                break;
            }
        }
        if(!found) {
            temp[nt++] = b[i];
        }
    }
    
    // 删除系数为0的项
    int nr = 0;
    for(int i = 0; i < nt; i++) {
        if(temp[i].coef != 0) {
            result[nr++] = temp[i];
        }
    }
    
    // 按指数降序排序
    for(int i = 0; i < nr - 1; i++) {
        for(int j = 0; j < nr - 1 - i; j++) {
            if(result[j].exp < result[j+1].exp) {
                Polynomial t = result[j];
                result[j] = result[j+1];
                result[j+1] = t;
            }
        }
    }
    
    return nr;
}

// 多项式相减
int SubtractPolynomials(Polynomial a[], int na, Polynomial b[], int nb, Polynomial result[]) {
    Polynomial negB[MAX_TERMS];
    // 将b的系数取反
    for(int i = 0; i < nb; i++) {
        negB[i].coef = -b[i].coef;
        negB[i].exp = b[i].exp;
    }
    // 调用加法函数
    return AddPolynomials(a, na, negB, nb, result);
}

// 计算多项式在x处的值
double EvaluatePolynomial(Polynomial poly[], int n, double x) {
    double result = 0;
    for(int i = 0; i < n; i++) {
        result += poly[i].coef * pow(x, poly[i].exp);
    }
    return result;
}

int main() {
   Polynomial poly1[MAX_TERMS], poly2[MAX_TERMS], result[MAX_TERMS];
    char line1[MAX_LINE], line2[MAX_LINE];
    int n1, n2;
    int inputMode;

    printf("请选择输入方式：1-文件输入  2-键盘输入\n");
    scanf("%d", &inputMode);
    getchar(); // 吸收回车

    if (inputMode == 1) {
        FILE* fin = fopen("input.txt", "r");
        if(!fin) {
            printf("无法打开 input.txt\n");
            return 1;
        }
        fgets(line1, MAX_LINE, fin);
        fgets(line2, MAX_LINE, fin);
        fclose(fin);
    } else {
        printf("请输入第一个多项式（如3x^4+4x^3）：\n");
        fgets(line1, MAX_LINE, stdin);
        printf("请输入第二个多项式（如2x^4-4x^3+5）：\n");
        fgets(line2, MAX_LINE, stdin);
    }

    // 去除换行符
    line1[strcspn(line1, "\n")] = 0;
    line2[strcspn(line2, "\n")] = 0;

    n1 = ParsePolynomial(line1, poly1);
    n2 = ParsePolynomial(line2, poly2);

    
    // 输出原始多项式
    printf("第一个多项式: ");
    PrintPolynomial(poly1, n1);
    printf("序列格式: ");
    PrintPolynomialSeq(poly1, n1);
    
    printf("\n第二个多项式: ");
    PrintPolynomial(poly2, n2);
    printf("序列格式: ");
    PrintPolynomialSeq(poly2, n2);
    
    // 多项式加法
    int n3 = AddPolynomials(poly1, n1, poly2, n2, result);
    printf("\n加法结果: ");
    PrintPolynomial(result, n3);
    printf("序列格式: ");
    PrintPolynomialSeq(result, n3);
    
    // 多项式减法
    int n4 = SubtractPolynomials(poly1, n1, poly2, n2, result);
    printf("\n减法结果: ");
    PrintPolynomial(result, n4);
    printf("序列格式: ");
    PrintPolynomialSeq(result, n4);
    
    // 计算多项式的值
    double x = 2.0;
    printf("\n在 x = %.1f 处的值:\n", x);
    printf("第一个多项式 = %.2f\n", EvaluatePolynomial(poly1, n1, x));
    printf("第二个多项式 = %.2f\n", EvaluatePolynomial(poly2, n2, x));
    
    return 0;
}