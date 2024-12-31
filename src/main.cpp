#include <iostream>
#include <stdio.h>
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include<math.h>
#include <stdlib.h>
#include <time.h>
#include<Windows.h>
#include<tchar.h>
#include<Psapi.h>

using namespace std;

#define NPoint 70

typedef unsigned char U_CHAR;

using namespace std;
typedef struct Coordinate
{
    double x;
    double y;
}Coordinate;

//1025
typedef struct coord
{
    int r, c;
    struct coord* next;
}coord;

typedef struct Node
{
    unsigned int* arr;
    struct Node* down;
    coord* right;
}Node;

typedef struct Area
{
    int len;
    Node* point;
    int size;
}Area;

bool compare(unsigned int* a, unsigned int* b, int len);
Node* lookfor(Area* ar, unsigned int* data);
unsigned int*** compress1(unsigned int*** x, int len, int M, int N);
void write1(unsigned int*** goal, int len, int M, int N);
Area* unique(unsigned int*** data, int len, int M, int N);
unsigned int** compress2(Area* comArea, unsigned int** data);
void write2(Area* comArea, unsigned int** data_status, unsigned int** data, int M, int N);
void write3(Area* comArea, unsigned int** data_status, unsigned int** a, int M, int N);
//1025

void read0(unsigned int***& goal, int len, int M, int N);
void read1(unsigned int***& goal, int len, int M, int N);
void read2(Area*& comArea, unsigned int**& data_status, unsigned int**& data, int M, int N);
void read3(Area*& comArea, unsigned int**& data_status, unsigned int**& a, int M, int N);
//1108

int equals(unsigned int* a, unsigned int* b, int len)
{
    int i = 0;
    if (NULL == a || NULL == b)
        return 0;
    while (i < len)
    {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return 1;
}
int* decode(unsigned int x[], int n)
{
    int i, k;
    int* ch;
    ch = (int*)malloc(n * sizeof(int));
    for (i = 0; i < n; i++)
        ch[i] = 0;
    k = 0;
    int lencomp = n / 32 + (n % 32 == 0 ? 0 : 1);
    for (i = 0; i < lencomp; i++)
    {
        unsigned int temp = x[i];
        k = i * 32;
        while (temp != 0)
        {
            ch[k] = (temp % 2 == 1) ? 1 : 0;
            temp = temp / 2;
            k++;
        }
    }
    return ch;
}

unsigned int* encode(int* temp, int len)//将编码压缩为整型数组
{
    //cout << "hello\n" << endl;
    int lenComp = len / 32 + (len % 32 == 0 ? 0 : 1);
    unsigned int* tempIntcode = (unsigned int*)malloc(lenComp * sizeof(unsigned int));
    unsigned int t = 0;
    int k = 0;
    for (int i = 0; i < lenComp && k < len; i++)
    {
        t = 0;
        for (int j = 0; j < 32; j++)
        {
            if (temp[k] == 1)
            {
                int js = 1;
                for (int r = 1; r <= j; r++)
                {
                    js *= 2;
                }
                t += js;
            }
            k++;
        }
        tempIntcode[i] = t;
    }
    return tempIntcode;
}

int* inquire_landscapeImagery0(unsigned int*** x, int len, int M, int N, Coordinate left, Coordinate right, int coorMinX, int coorMinY, double resolution)
{
    int rowStart, rowEnd, colStart, colEnd;
    double minx, miny, maxx, maxy;
    double leftb = coorMinX, lowb = coorMinY;
    int* temp = (int*)malloc(len * sizeof(int));
    for (int i = 0; i < len; i++)
        temp[i] = 0;
    if (left.x < right.x)
    {
        minx = left.x;
        maxx = right.x;
    }
    else
    {
        maxx = left.x;
        minx = right.x;
    }
    if (left.y < right.y)
    {
        miny = left.y;
        maxy = right.y;
    }
    else
    {
        maxy = left.y;
        miny = right.y;
    }
    rowStart = (M - 1) - (int)((maxy - lowb) / resolution);
    rowEnd = (M - 1) - (int)((miny - lowb) / resolution);
    colStart = (int)((minx - leftb) / resolution);
    colEnd = (int)((maxx - leftb) / resolution);
    for (int i = rowStart; i < rowEnd; i++)
    {
        for (int j = colStart; j < colEnd; j++)
        {

            for (int k = 0; k < len; k++)
            {
                temp[k] = temp[k] | x[k][i][j];
            }

        }
    }
    return temp;
}

int* inquire_landscapeImagery1(unsigned int*** x, int lencomp, int len, int M, int N, Coordinate left, Coordinate right, int coorMinX, int coorMinY, double resolution)
{
    int rowStart, rowEnd, colStart, colEnd;
    double minx, miny, maxx, maxy;
    double leftb = coorMinX, lowb = coorMinY;
    unsigned int* temp = (unsigned int*)malloc(lencomp * sizeof(unsigned int));
    for (int i = 0; i < lencomp; i++)
        temp[i] = 0;
    if (left.x < right.x)
    {
        minx = left.x;
        maxx = right.x;
    }
    else
    {
        maxx = left.x;
        minx = right.x;
    }
    if (left.y < right.y)
    {
        miny = left.y;
        maxy = right.y;
    }
    else
    {
        maxy = left.y;
        miny = right.y;
    }
    rowStart = (M - 1) - (int)((maxy - lowb) / resolution);
    rowEnd = (M - 1) - (int)((miny - lowb) / resolution);
    colStart = (int)((minx - leftb) / resolution);
    colEnd = (int)((maxx - leftb) / resolution);
    for (int i = rowStart; i < rowEnd; i++)
    {
        for (int j = colStart; j < colEnd; j++)
        {

            for (int k = 0; k < lencomp; k++)
            {
                temp[k] = temp[k] | x[k][i][j];
            }

        }
    }
    int* p = decode(temp, len);
    return p;
}

int* inquire_landscapeImagery2(unsigned int** x, unsigned int indexStatus, unsigned int** dataStatus, int lencomp, int len, int M, int N, Coordinate left, Coordinate right, int coorMinX, int coorMinY, double resolution)
{
    int rowStart, rowEnd, colStart, colEnd;
    double minx, miny, maxx, maxy;
    double leftb = coorMinX, lowb = coorMinY;
    unsigned int* temp = (unsigned int*)malloc(lencomp * sizeof(unsigned int));
    for (int i = 0; i < lencomp; i++)
        temp[i] = 0;
    if (left.x < right.x)
    {
        minx = left.x;
        maxx = right.x;
    }
    else
    {
        maxx = left.x;
        minx = right.x;
    }
    if (left.y < right.y)
    {
        miny = left.y;
        maxy = right.y;
    }
    else
    {
        maxy = left.y;
        miny = right.y;
    }
    rowStart = (M - 1) - (int)((maxy - lowb) / resolution);
    rowEnd = (M - 1) - (int)((miny - lowb) / resolution);
    colStart = (int)((minx - leftb) / resolution);
    colEnd = (int)((maxx - leftb) / resolution);
    for (int i = rowStart; i < rowEnd; i++)
    {
        for (int j = colStart; j < colEnd; j++)
        {

            for (int k = 0; k < lencomp; k++)
            {
                temp[k] = temp[k] | dataStatus[x[i][j]][k];
            }

        }
    }
    int* p = decode(temp, len);
    return p;
}

int** inquire_location0(unsigned int*** x, int len, int M, int N, int* landscapeImagery)
{
    int** p = (int**)malloc(M * sizeof(int*));
    for (int i = 0; i < M; i++)
    {
        p[i] = (int*)malloc(N * sizeof(int));
    }
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            p[i][j] = 1;
    }
    //cout << "======" << endl;
    for (int k = 0; k < len; k++)
    {
        if (landscapeImagery[k] == 1)
        {
            //cout << "88888888" << endl;
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++)
                    p[i][j] = p[i][j] && x[k][i][j];
        }
    }
    return p;
}

int** inquire_location1(unsigned int*** x, int lencomp, int M, int N, unsigned int* landscapeImagery)
{
    int** p = (int**)malloc(M * sizeof(int*));
    unsigned int* temp = (unsigned int*)malloc(sizeof(unsigned int) * lencomp);
    for (int i = 0; i < M; i++)
    {
        p[i] = (int*)malloc(N * sizeof(int));
    }
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            p[i][j] = 0;
    }
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < lencomp; k++)
                temp[k] = x[k][i][j];
            if (equals(temp, landscapeImagery, lencomp))
                p[i][j] = 1;
        }
    }
    return p;
}

int** inquire_location2(unsigned int** x, int M, int N, unsigned int indexLandscapeImagery)
{
    int** p = (int**)malloc(M * sizeof(int*));
    for (int i = 0; i < M; i++)
    {
        p[i] = (int*)malloc(N * sizeof(int));
    }
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            p[i][j] = 0;
    }
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (x[i][j] = indexLandscapeImagery)
                p[i][j] = 1;
        }
    }
    return p;
}

int main()
{
    clock_t start, finish;
    double duration;


    FILE* file;

    GDALAllRegister();  // 注册所有驱动程序，准备读研究区域高程数据   
    GDALDataset* poDataset;
    double M_x, M_y;
    int MM, NN;
    unsigned int*** arr = (unsigned int***)malloc(NPoint * sizeof(unsigned int**));
    for (int i = 0; i < NPoint; i++)
    {
        char strin[5];
        sprintf(strin, "%d", i);
        char file_in[30] = "f:/in/100/in_";
        strcat(file_in, strin);
        strcat(file_in, ".tif");

        poDataset = static_cast<GDALDataset*>(GDALOpen(file_in, GA_ReadOnly));

        if (poDataset == NULL) {
            std::cout << "Cannot open TIFF file." << std::endl;
            return 1;
        }
        int nWidth = poDataset->GetRasterXSize();
        int nHeight = poDataset->GetRasterYSize();

        MM = nHeight;
        NN = nWidth;

        arr[i] = (unsigned int**)malloc(nHeight * sizeof(unsigned int*));

        for (int j = 0; j < nHeight; j++)
        {
            arr[i][j] = (unsigned int*)malloc(nWidth * sizeof(unsigned int));
        }

        //获取研究区域的坐标
        double adfGeoTransform[6];
        poDataset->GetGeoTransform(adfGeoTransform);
        double minX = adfGeoTransform[0];
        double maxY = adfGeoTransform[3];
        double maxX = adfGeoTransform[0] + adfGeoTransform[1] * nWidth + adfGeoTransform[2] * nHeight;
        double minY = adfGeoTransform[3] + adfGeoTransform[4] * nWidth + adfGeoTransform[5] * nHeight;
        M_x = minX;
        M_y = minY;

        // 获取 TIFF 文件的第一个波段
        GDALRasterBand* poBand = poDataset->GetRasterBand(1);

        for (int j = 0; j < nHeight; j++) {//nHeight
            for (int k = 0; k < nWidth; k++) {//nWidth
                float pixel;
                GDALRasterIO(poBand, GF_Read, k, j, 1, 1, &pixel, 1, 1, GDT_Float32, 0, 0);
                int temp = (int)pixel;
                arr[i][j][k] = temp;
                if (1 != arr[i][j][k])
                    arr[i][j][k] = 0;
            }
        }
        GDALClose(poDataset);
    }

    //三维原始输出out
    file = fopen("f:/out1229/benchmark.txt", "w");
    for (int i = 0; i < NPoint; i++) {
        //fprintf(file, "%d\n", i + 1);
        for (int j = 0; j < MM; j++) {//nHeight
            for (int k = 0; k < NN; k++) {//nWidth

                fprintf(file, "%u ", arr[i][j][k]);
            }
            fprintf(file, "\n\n");
        }
    }
    fclose(file);

    unsigned int*** goal;

    start = clock();

    goal = compress1(arr, NPoint, MM, NN);//compress 1
    int lencompress = NPoint / 32 + (NPoint % 32 == 0 ? 0 : 1);

    //三维压缩输出
    write1(goal, lencompress, MM, NN);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << endl << "第一次压缩时间:" << duration << endl;



    start = clock();
    Area* comArea = unique(goal, lencompress, MM, NN);
    unsigned int** data2;

    data2 = (unsigned int**)malloc(MM * sizeof(unsigned int*));
    for (int i = 0; i < MM; i++)
        data2[i] = (unsigned int*)malloc(NN * sizeof(unsigned int));
    unsigned int** data_status = compress2(comArea, data2);//compress 2	

    //去重之后输出
    write2(comArea, data_status, data2, MM, NN);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << endl << "第二次压缩时间:" << duration << endl;

    start = clock();
    //游程编码后输出
    write3(comArea, data_status, data2, MM, NN);
    finish = clock();
    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << endl << "第三次压缩时间:" << duration << endl;


    Coordinate first[] = { {518800,4506041},{518300,4504041},{516800,4503188},{515800,4502400},{517240,4501000} };
    Coordinate second[] = { {521300,4506666},{521300,4506666},{521300,4506666},{521300,4506666},{521210,4508888} };




    cout << "现在输出查特征点可见性的时间" << endl;
    for (int kk = 0; kk < 5; kk++)
    {
        start = clock();
        read0(arr, NPoint, MM, NN);
        for (int k = 0; k < 1000; k++)
            inquire_landscapeImagery0(arr, NPoint, MM, NN, first[kk], second[kk], M_x, M_y, 12.5);
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
        cout << endl << "benchmark:" << duration << "   ";

        start = clock();
        read1(goal, lencompress, MM, NN);
        for (int k = 0; k < 1000; k++)
            inquire_landscapeImagery1(goal, lencompress, NPoint, MM, NN, first[kk], second[kk], M_x, M_y, 12.5);
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
        cout << endl << "firstCompress:" << duration << "   ";

        start = clock();
        read2(comArea, data_status, data2, MM, NN);
        for (int k = 0; k < 1000; k++)
            inquire_landscapeImagery2(data2, 2, data_status, lencompress, NPoint, MM, NN, first[kk], second[kk], M_x, M_y, 12.5);
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
        cout << endl << "secondCompress:" << duration << endl;

        start = clock();
        read3(comArea, data_status, data2, MM, NN);
        for (int k = 0; k < 1000; k++)
            inquire_landscapeImagery2(data2, 2, data_status, lencompress, NPoint, MM, NN, first[kk], second[kk], M_x, M_y, 12.5);
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
        cout << endl << "thirdCompress:" << duration << endl;
    }

    cout << "现在输出查区位的时间" << endl;

    int** landscapeIm = (int**)malloc(5 * sizeof(int*));
    int number = 70;
    for (int j = 0; j < 5; j++)
    {
        landscapeIm[j] = (int*)malloc(number * sizeof(int));
        for (int k = 0; k < number; k++)
            landscapeIm[j][k] = 0;
        for (int k = 0; k < (j + 1) * 10; k++)
            landscapeIm[j][k] = 1;

        start = clock();
        read0(arr, NPoint, MM, NN);
        for (int k = 0; k < 1000; k++)
        {
            int** temploc = inquire_location0(arr, number, MM, NN, landscapeIm[j]);
            for (int q = 0; q < MM; q++)
                delete (temploc[q]);
            delete temploc;
        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
        cout << endl << "基准结构上查询:" << duration << "   ";
        unsigned* landcom = NULL;

        if (NULL != landcom)
            free(landcom);

        landcom = encode(landscapeIm[j], number);
        start = clock();
        read1(goal, lencompress, MM, NN);
        for (int k = 0; k < 1000; k++)
        {
            int** temploc = inquire_location1(goal, 3, MM, NN, landcom);
            for (int q = 0; q < MM; q++)
                delete (temploc[q]);
            delete temploc;
        }

        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
        cout << endl << "第一次压缩结构查询:" << duration << "   ";

        srand(time(NULL)); // 设置随机数种子为当前时间
        int randomNumber = (rand() % (10 - 1 + 1)) + 1; // 生成1到10之间的随机数
        // printf("随机数为：%d\n", randomNumber);
        start = clock();
        read2(comArea, data_status, data2, MM, NN);
        for (int k = 0; k < 1000; k++)
        {
            int** temploc = inquire_location2(data2, MM, NN, randomNumber);
            for (int q = 0; q < MM; q++)
                delete (temploc[q]);
            delete temploc;
        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
        cout << endl << "第二次压缩结构查询:" << duration << endl;

        start = clock();
        read3(comArea, data_status, data2, MM, NN);
        for (int k = 0; k < 1000; k++)
        {
            int** temploc = inquire_location2(data2, MM, NN, randomNumber);
            for (int q = 0; q < MM; q++)
                delete (temploc[q]);
            delete temploc;
        }
        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC * 1000;
        cout << endl << "第三次压缩结构查询:" << duration << endl;
    }
    return 0;
}


//1025
bool compare(unsigned int* a, unsigned int* b, int len)
{
    //int flag = true;
    for (int i = 0; i < len; i++)
        if (a[i] != b[i])
            return false;
    return true;
}

Node* lookfor(Area* ar, unsigned int* data)
{
    Node* head = ar->point;
    //bool flag = false;
    while (head != NULL)
    {
        if (compare(head->arr, data, ar->len))
        {
            return head;
        }
        head = head->down;
    }
    return NULL;
}

unsigned int*** compress1(unsigned int*** x, int len, int M, int N)//三维len,M,N.
{
    int lenComp = len / 32 + (len % 32 == 0 ? 0 : 1);
    unsigned int*** p = (unsigned int***)malloc(lenComp * sizeof(unsigned int**));
    for (int i = 0; i < lenComp; i++)
    {
        p[i] = (unsigned int**)malloc(M * sizeof(unsigned int*));
        for (int j = 0; j < M; j++)
            p[i][j] = (unsigned int*)malloc(N * sizeof(unsigned int));
    }
    //for(int k=0;k<lenComp;k++){
    for (int row = 0; row < M; row++)
        for (int col = 0; col < N; col++)
        {
            unsigned int t = 0;
            int k = 0;
            for (int i = 0; i < lenComp; i++)//&& k < len
            {
                t = 0;
                for (int j = 0; j < 32; j++)
                {
                    //if (x[k][row][col] == true)
                    if (x[k][row][col] == true)
                    {
                        int js = 1;
                        for (int r = 1; r <= k % 32; r++)
                        {
                            js *= 2;
                        }
                        t += js;
                    }
                    k++;
                    if (k == len)
                        break;
                }
                p[i][row][col] = t;
            }
        }
    return p;
}

void write1(unsigned int*** goal, int len, int M, int N)
{
    FILE* file = fopen("f:/out1229/firstCompress.txt", "w"); // 打开文件进行写入，以文本方式打开

    if (file == NULL) {
        printf("文件打开失败！");
        return;
    }
    for (int k = 0; k < len; k++) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                fprintf(file, "%u ", goal[k][i][j]); // 将每个元素写入文件，使用空格分隔
            }
            fprintf(file, "\n"); // 写入换行符，表示一行结束
        }
        fprintf(file, "\n\n\n"); // 写入换行符，表示一行结束
    }
    fclose(file);
}

Area* unique(unsigned int*** data, int len, int M, int N)
{
    unsigned int* temp = (unsigned int*)malloc(len * sizeof(unsigned int));
    Area* ha = (Area*)malloc(sizeof(Area));
    ha->len = len;
    ha->point = NULL;
    int size = 0;
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            for (int k = 0; k < len; k++)
                temp[k] = data[k][i][j];
            Node* p = lookfor(ha, temp);
            if (NULL == p)//新建一个景观视觉状态
            {
                size++;
                Node* q = (Node*)malloc(sizeof(Node));
                q->arr = (unsigned int*)malloc(len * sizeof(unsigned int));
                for (int t = 0; t < len; t++)
                    q->arr[t] = temp[t];
                q->right = (coord*)malloc(sizeof(coord));
                q->right->r = i;
                q->right->c = j;
                q->right->next = NULL;
                q->down = ha->point;
                ha->point = q;
            }
            else //将坐标加在已有景观视觉状态的末尾。
            {
                coord* q = (coord*)malloc(sizeof(coord));
                q->r = i;
                q->c = j;
                q->next = p->right;
                p->right = q;
            }
        }
    }
    ha->size = size;
    return ha;
}

unsigned int** compress2(Area* comArea, unsigned int** data)
{
    unsigned int** data_status = (unsigned int**)malloc(comArea->size * sizeof(unsigned int*));
    for (int i = 0; i < comArea->size; i++)
    {
        data_status[i] = (unsigned int*)malloc(comArea->len * sizeof(unsigned int));
    }
    Node* l = comArea->point;
    int t = 0;
    while (l != NULL)
    {
        for (int k = 0; k < comArea->len; k++)
        {
            data_status[t][k] = l->arr[k];
        }
        coord* cp = l->right;
        while (cp != NULL)
        {
            data[cp->r][cp->c] = t;
            cp = cp->next;
        }
        l = l->down;
        t++;
    }
    return data_status;
}

void write2(Area* comArea, unsigned int** data_status, unsigned int** data, int M, int N)
{
    FILE* file = fopen("f:/out1229/secondCompress.txt", "w"); // 打开文件进行写入，以文本方式打开
    if (file == NULL) {
        printf("文件打开失败！");
        return;
    }
    //打印景观状态
    for (int i = 0; i < comArea->size; i++)
    {
        fprintf(file, "%u:", i);
        for (int k = 0; k < comArea->len; k++)
            fprintf(file, "%u ", data_status[i][k]);
        fprintf(file, "\n");
    }
    //打印二次压缩数据
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
            fprintf(file, "%u ", data[i][j]);
        fprintf(file, "\n");
    }
    fclose(file);
}

void write3(Area* comArea, unsigned int** data_status, unsigned int** a, int M, int N)
{
    FILE* file = fopen("f:/out1229/thirdCompress.txt", "w"); // 打开文件进行写入，以文本方式打开

    if (file == NULL) {
        printf("文件打开失败！");
        return;
    }
    //打印景观状态
    for (int i = 0; i < comArea->size; i++)
    {
        fprintf(file, "%u:", i);
        for (int k = 0; k < comArea->len; k++)
            fprintf(file, "%u ", data_status[i][k]);
        fprintf(file, "\n");
    }
    // fprintf(file, "abcdefg\n");
     //打印感知细节（感知栅格的三次压缩）
    for (int i = 0; i < M; i++)
    {
        unsigned int flag = a[i][0];
        int count = 0;
        for (int j = 1; j < N; j++)//一行行程编码
        {
            if (a[i][j] == flag)
                count++;
            else
            {
                fprintf(file, "%u:%u,", flag, count);
                count = 1;
                flag = a[i][j];
            }
        }
        fprintf(file, "%u:%u,", flag, count);
        fprintf(file, "\n");
    }
}

void read0(unsigned int***& goal, int len, int M, int N) {
    FILE* file = fopen("f:/out1229/benchmark.txt", "r");
    if (file == NULL) {
        printf("文件打开失败！");
        return;
    }

    for (int k = 0; k < len; k++) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                fscanf(file, "%u", &goal[k][i][j]);
            }
        }
    }

    fclose(file);
}

void read1(unsigned int***& goal, int len, int M, int N) {
    FILE* file = fopen("f:/out1229/firstCompress.txt", "r");
    if (file == NULL) {
        printf("文件打开失败！");
        return;
    }

    for (int k = 0; k < len; k++) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                fscanf(file, "%u", &goal[k][i][j]);
            }
        }
    }

    fclose(file);
}



void read2(Area*& comArea, unsigned int**& data_status, unsigned int**& data, int M, int N) {
    FILE* file = fopen("f:/out1229/secondCompress.txt", "r");
    if (file == NULL) {
        printf("文件打开失败！");
        return;
    }

    // 读取景观状态
    for (int i = 0; i < comArea->size; i++) {
        unsigned int index;
        fscanf(file, "%u:", &index);
        for (int k = 0; k < comArea->len; k++) {
            fscanf(file, "%u", &data_status[i][k]);
        }
    }

    // 读取二次压缩数据
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(file, "%u", &data[i][j]);
        }
    }

    fclose(file);
}

void read3(Area*& comArea, unsigned int**& data_status, unsigned int**& a, int M, int N) {
    FILE* file = fopen("f:/out1229/thirdCompress.txt", "r");
    if (file == NULL) {
        printf("文件打开失败！");
        return;
    }

    // 读取景观状态
    for (int i = 0; i < comArea->size; i++) {
        unsigned int index;
        fscanf(file, "%u:", &index);
        for (int k = 0; k < comArea->len; k++) {
            fscanf(file, "%u", &data_status[i][k]);
        }
    }

    // 读取行程编码数据
    for (int i = 0; i < M; i++) {
        unsigned int flag, count;
        int j = 0;
        while (fscanf(file, "%u:%u,", &flag, &count) == 2) {
            for (int k = 0; k < count + 1; k++) {
                if (j < N) {
                    a[i][j++] = flag;
                }
            }
        }
    }

    fclose(file);
}