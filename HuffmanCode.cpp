#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR 128
#define MAX_CODE_LEN 128

typedef struct HuffmanNode {
    char ch;
    int weight;
    int parent, left, right;
} HuffmanNode;

typedef struct {
    int n; // 字符集大小
    HuffmanNode *nodes;
    char **codes; // 哈夫曼编码
} HuffmanTree;

// 工具函数：查找权值最小且未被选中的节点
void select2(HuffmanNode *nodes, int n, int *s1, int *s2) {
    int i;
    *s1 = *s2 = -1;
    for (i = 0; i < n; ++i) {
        if (nodes[i].parent == -1) {
            if (*s1 == -1 || nodes[i].weight < nodes[*s1].weight)
                *s1 = i;
        }
    }
    for (i = 0; i < n; ++i) {
        if (nodes[i].parent == -1 && i != *s1) {
            if (*s2 == -1 || nodes[i].weight < nodes[*s2].weight)
                *s2 = i;
        }
    }
}

// 初始化：从文件读取字符集和权值，建立哈夫曼树并保存到文件
void Initialization() {
    int n, i, m;
    char chs[MAX_CHAR];
    int weights[MAX_CHAR];
    char filename[128] = "charset.txt";
    printf("请输入字符集文件名（默认 charset.txt）：");
    char input[128];
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') {
        sscanf(input, "%s", filename);
    }
    FILE *fc = fopen(filename, "r");
    if (!fc) {
        printf("无法打开字符集文件 %s\n", filename);
        return;
    }
    fscanf(fc, "%d", &n);
    for (i = 0; i < n; ++i) {
        fscanf(fc, " %c", &chs[i]);
    }
    for (i = 0; i < n; ++i) {
        fscanf(fc, "%d", &weights[i]);
    }
    fclose(fc);

    m = 2 * n - 1;
    HuffmanNode *nodes = (HuffmanNode*)malloc(sizeof(HuffmanNode) * m);
    for (i = 0; i < n; ++i) {
        nodes[i].ch = chs[i];
        nodes[i].weight = weights[i];
        nodes[i].parent = nodes[i].left = nodes[i].right = -1;
    }
    for (i = n; i < m; ++i) {
        nodes[i].ch = 0;
        nodes[i].weight = 0;
        nodes[i].parent = nodes[i].left = nodes[i].right = -1;
    }
    // 构建哈夫曼树
    for (i = n; i < m; ++i) {
        int s1, s2;
        select2(nodes, i, &s1, &s2);
        nodes[s1].parent = i;
        nodes[s2].parent = i;
        nodes[i].left = s1;
        nodes[i].right = s2;
        nodes[i].weight = nodes[s1].weight + nodes[s2].weight;
    }
    // 保存到文件
    FILE *fp = fopen("huffman", "wb");
    fwrite(&n, sizeof(int), 1, fp);
    fwrite(nodes, sizeof(HuffmanNode), m, fp);
    fclose(fp);
    free(nodes);
    printf("哈夫曼树已建立并保存到文件 huffman。\n");
}

// 读取哈夫曼树
HuffmanTree* LoadHuffmanTree() {
    FILE *fp = fopen("huffman", "rb");
    if (!fp) {
        printf("未找到 huffman 文件，请先初始化。\n");
        return NULL;
    }
    int n, m, i;
    fread(&n, sizeof(int), 1, fp);
    m = 2 * n - 1;
    HuffmanNode *nodes = (HuffmanNode*)malloc(sizeof(HuffmanNode) * m);
    fread(nodes, sizeof(HuffmanNode), m, fp);
    fclose(fp);

    // 生成哈夫曼编码
    char **codes = (char**)malloc(sizeof(char*) * n);
    for (i = 0; i < n; ++i) {
        char temp[MAX_CODE_LEN];
        int cur = i, p, len = 0;
        temp[MAX_CODE_LEN-1] = '\0';
        int pos = MAX_CODE_LEN-2;
        while ((p = nodes[cur].parent) != -1) {
            if (nodes[p].left == cur)
                temp[pos--] = '0';
            else
                temp[pos--] = '1';
            cur = p;
        }
        codes[i] = strdup(&temp[pos+1]);
    }
    HuffmanTree *tree = (HuffmanTree*)malloc(sizeof(HuffmanTree));
    tree->n = n;
    tree->nodes = nodes;
    tree->codes = codes;
    return tree;
}

// 释放哈夫曼树
void FreeHuffmanTree(HuffmanTree *tree) {
    if (!tree) return;
    for (int i = 0; i < tree->n; ++i) free(tree->codes[i]);
    free(tree->codes);
    free(tree->nodes);
    free(tree);
}

// 编码
void Coding() {
    HuffmanTree *tree = LoadHuffmanTree();
    if (!tree) return;
    // 读取待编码文件
    FILE *fin = fopen("tobetrans.dat", "r");
    if (!fin) {
        printf("无法打开 tobetrans.dat\n");
        FreeHuffmanTree(tree);
        return;
    }
    FILE *fout = fopen("codefile", "w");
    int ch;
    while ((ch = fgetc(fin)) != EOF) {
        int found = 0;
        for (int i = 0; i < tree->n; ++i) {
            if (tree->nodes[i].ch == ch) {
                fputs(tree->codes[i], fout);
                found = 1;
                break;
            }
        }
        if (!found && ch != '\n' && ch != '\r') {
            printf("警告：字符 '%c' 未在字符集内，已忽略。\n", ch);
        }
    }
    fclose(fin);
    fclose(fout);
    printf("编码完成，结果已保存到 codefile。\n");
    FreeHuffmanTree(tree);
}

// 解码
void Decoding() {
    HuffmanTree *tree = LoadHuffmanTree();
    if (!tree) return;
    FILE *fin = fopen("codefile", "r");
    if (!fin) {
        printf("无法打开 codefile\n");
        FreeHuffmanTree(tree);
        return;
    }
    FILE *fout = fopen("textfile", "w");
    int m = 2 * tree->n - 1;
    int root = m - 1;
    int cur = root;
    int ch;
    while ((ch = fgetc(fin)) != EOF) {
        if (ch == '0') cur = tree->nodes[cur].left;
        else if (ch == '1') cur = tree->nodes[cur].right;
        if (tree->nodes[cur].left == -1 && tree->nodes[cur].right == -1) {
            fputc(tree->nodes[cur].ch, fout);
            cur = root;
        }
    }
    fclose(fin);
    fclose(fout);
    printf("解码完成，结果已保存到 textfile。\n");
    FreeHuffmanTree(tree);
}

// 打印编码文件
void PrintCodeFile() {
    FILE *fin = fopen("codefile", "r");
    FILE *fout = fopen("codeprint", "w");
    if (!fin || !fout) {
        printf("无法打开 codefile 或 codeprint\n");
        if (fin) fclose(fin);
        if (fout) fclose(fout);
        return;
    }
    int cnt = 0, ch;
    while ((ch = fgetc(fin)) != EOF) {
        putchar(ch);
        fputc(ch, fout);
        cnt++;
        if (cnt % 50 == 0) {
            putchar('\n');
            fputc('\n', fout);
        }
    }
    if (cnt % 50 != 0) putchar('\n');
    fclose(fin);
    fclose(fout);
    printf("编码文件已打印到终端并写入 codeprint。\n");
}

// 递归打印哈夫曼树（凹入表）
void PrintTreeRec(HuffmanNode *nodes, int root, int depth, FILE *fout) {
    if (root == -1) return;
    // 先打印右子树
    PrintTreeRec(nodes, nodes[root].right, depth + 1, fout);

    // 打印当前节点
    for (int i = 0; i < depth; ++i) {
        printf("    ");
        if (fout) fprintf(fout, "    ");
    }
    if (nodes[root].left == -1 && nodes[root].right == -1)
        printf("%c(%d)\n", nodes[root].ch, nodes[root].weight),
        fout ? fprintf(fout, "%c(%d)\n", nodes[root].ch, nodes[root].weight) : 0;
    else
        printf("#(%d)\n", nodes[root].weight),
        fout ? fprintf(fout, "#(%d)\n", nodes[root].weight) : 0;

    // 再打印左子树
    PrintTreeRec(nodes, nodes[root].left, depth + 1, fout);
}

// 打印哈夫曼树
void PrintTree() {
    HuffmanTree *tree = LoadHuffmanTree();
    if (!tree) return;
    int root = 2 * tree->n - 2;
    printf("哈夫曼树结构如下：\n");
    FILE *fout = fopen("treeprint", "w");
    PrintTreeRec(tree->nodes, root, 0, fout);
    if (fout) fclose(fout);
    printf("哈夫曼树已写入 treeprint。\n");
    FreeHuffmanTree(tree);
}

// 主程序
int main() {
    char cmd;
    printf("哈夫曼编/译码系统\n");
    printf("I: 初始化  C: 编码  D: 解码  P: 打印编码文件  T: 打印哈夫曼树  Q: 退出\n");
    while (1) {
        printf("\n请输入命令(I/C/D/P/T/Q): ");
        scanf(" %c", &cmd);
        getchar(); // 吸收回车
        if (cmd == 'I' || cmd == 'i') Initialization();
        else if (cmd == 'C' || cmd == 'c') Coding();
        else if (cmd == 'D' || cmd == 'd') Decoding();
        else if (cmd == 'P' || cmd == 'p') PrintCodeFile();
        else if (cmd == 'T' || cmd == 't') PrintTree();
        else if (cmd == 'Q' || cmd == 'q') break;
        else printf("无效命令，请重新输入。\n");
    }
    printf("程序结束。\n");
    return 0;
}