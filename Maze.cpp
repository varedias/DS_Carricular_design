#include <stdio.h>
#include <stdbool.h>
#define MAX_SIZE 100
#define MAX_PATHS 1000

// 定义方向
const int dx[] = {-1, 0, 1, 0};  // 上右下左
const int dy[] = {0, 1, 0, -1};
const char direction[] = {'U', 'R', 'D', 'L'};

// 迷宫结构
int maze[MAX_SIZE][MAX_SIZE];
int visited[MAX_SIZE][MAX_SIZE];
int m, n;

// 存储路径
typedef struct {
    int x, y;
    char dir;
} Step;

Step paths[MAX_PATHS][MAX_SIZE*MAX_SIZE];
int pathLengths[MAX_PATHS];
int pathCount = 0;

// 检查位置是否有效
bool isValid(int x, int y) {
    return x >= 0 && x < m && y >= 0 && y < n && maze[x][y] == 0 && !visited[x][y];
}

// DFS搜索所有路径
void findPaths(int x, int y, Step current_path[], int path_length) {
    // 到达终点
    if (x == m-1 && y == n-1) {
        // 保存当前路径
        for (int i = 0; i < path_length; i++) {
            paths[pathCount][i] = current_path[i];
        }
        pathLengths[pathCount] = path_length;
        pathCount++;
        return;
    }

    visited[x][y] = 1;

    // 尝试所有方向
    for (int i = 0; i < 4; i++) {
        int newX = x + dx[i];
        int newY = y + dy[i];
        
        if (isValid(newX, newY)) {
            current_path[path_length].x = x;
            current_path[path_length].y = y;
            current_path[path_length].dir = direction[i];
            
            findPaths(newX, newY, current_path, path_length + 1);
        }
    }

    visited[x][y] = 0;  // 回溯
}

int main() {
   FILE *fp;



    // 打开文件
    fp = fopen("Maze.txt", "r");
    if (fp == NULL) {
        printf("无法打开文件 %s\n", "Maze.txt");
        return 1;
    }

    // 从文件读取迷宫大小
    fscanf(fp, "%d %d", &m, &n);

    // 从文件读取迷宫数据
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(fp, "%d", &maze[i][j]);
            visited[i][j] = 0;
        }
    }

    // 关闭文件
    fclose(fp);

    // 显示读取到的迷宫
    printf("\n读取到的迷宫：\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", maze[i][j]);
        }
        printf("\n");
    }

    // 寻找所有路径
    Step current_path[MAX_SIZE*MAX_SIZE];
    findPaths(0, 0, current_path, 0);

    // 输出结果
    if (pathCount == 0) {
        printf("\n═══════════════════════════════\n");
        printf("║     没有找到可行的路径！     ║\n");
        printf("═══════════════════════════════\n");
    } else {
        printf("\n═══════════════════════════════\n");
        printf("║    共找到 %d 条可行路径     ║\n", pathCount);
        printf("═══════════════════════════════\n");
        
        for (int i = 0; i < pathCount; i++) {
            printf("\n▶ 路径 %d:\n", i + 1);
            printf("  步骤: ");
            for (int j = 0; j < pathLengths[i]; j++) {
                printf("(%d,%d,%c)", paths[i][j].x, paths[i][j].y, paths[i][j].dir);
                if (j < pathLengths[i] - 1) printf(" → ");
            }
            printf("\n  长度: %d 步\n", pathLengths[i]);
        }

        // 显示第一条路径在迷宫中的表示
        printf("\n╔════ 第一条路径可视化 ════╗\n");
        int showMaze[MAX_SIZE][MAX_SIZE];
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                showMaze[i][j] = maze[i][j];
            }
        }

        for (int i = 0; i < pathLengths[0]; i++) {
            showMaze[paths[0][i].x][paths[0][i].y] = 2;
        }

        // 打印列号
        printf("║    ");
        for (int j = 0; j < n; j++) {
            printf("%2d ", j);
        }
        printf("   ║\n║    ");
        for (int j = 0; j < n; j++) {
            printf("---");
        }
        printf("   ║\n");

        // 打印迷宫内容
        for (int i = 0; i < m; i++) {
            printf("║ %2d| ", i);  // 打印行号
            for (int j = 0; j < n; j++) {
                if (showMaze[i][j] == 2) printf("🟢 ");
                else if (showMaze[i][j] == 1) printf("⬛ ");
                else printf("⬜ ");
            }
            printf(" |║\n");
        }
        printf("╚════════════════════════╝\n");
    }

    return 0;
}