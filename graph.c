#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>


const float PI = 3.1416;
const int N[] = {0, 3, 0, 8};
typedef struct Node{
    int x;
    int y;
    int number;
} Node;

char ProgName[]="Лабораторна робота 6";
char header[] = "Лабораторна робота 6. Виконав Р.В. Доценко";
int** MAT = NULL;
int pathsCount[] = {0,0,0};
int isVisited[10][10];
HDC* backHdc;
int** shadowMatrix;
int** treeMatrix = NULL;
int** weightMatrix;
Node** currentNodes;
HWND* hwnd;
int count;
int scriptState = 1;
int isDrawingFirst = 1;
int dx = 16, dy = 16, dtx = 5;
DWORD WINAPI executeReportScript(LPVOID lpParam);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

struct twoD_arr{
    int arr[10][10];
};

int** mallocMatrix(int size)
{
    int **matrix = (int **)malloc(sizeof(int *)*size);
    for(int i = 0;i < size; i++)
    {
        matrix[i] = (int *)malloc(sizeof(int)*size);
        for(int j = 0; j<size;j++)
        {
            matrix[i][j] = 0;
        }
    }
    return matrix;
}

double** mallocDoubleMatrix(int size)
{
    double **matrix = (double **)malloc(sizeof(double *)*size);
    for(int i = 0;i < size; i++)
    {
        matrix[i] = (double *)malloc(sizeof(double)*size);
    }
    return matrix;
}


double **randm()
{
    double **matrix = (double **)malloc(sizeof(double *)*10);
    for(int i = 0;i < 10; i++)
    {
        matrix[i] = (double *)malloc(sizeof(double)*10);
        for(int j = 0; j < 10; j++)
        {
            double t = (double)rand()/RAND_MAX*2.0;
            matrix[i][j] = t;
        }
    }
    return matrix;
}

double **randmByHundred()
{
    double **matrix = (double **)malloc(sizeof(double *)*10);
    for(int i = 0;i < 10; i++)
    {
        matrix[i] = (double *)malloc(sizeof(double)*10);
        for(int j = 0; j < 10; j++)
        {
            double t = (double)rand()/RAND_MAX*2.0;
            matrix[i][j] = t*100;
        }
    }
    return matrix;
}

int **mulmr(double k, double **T)
{
    int **A = (int **)malloc(sizeof(int *)*10);
    for(int i=0;i<10;i++)
    {
        A[i] = (int *)malloc(sizeof(int)*10);
        for(int j = 0; j < 10; j++)
        {

            double t = T[i][j]*k;
            int a = t <= 1.0 ? 0 : 1;
            A[i][j] = a;
        }
    }
    return A;
}

void outMatrix(int **M, int size)
{
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            printf("%d", (int)M[i][j]);
            printf(" ");
        }
        printf("\n");
    }
}

int **multiplyMatrix(int **A, int **B)
{
    int **C = (int **)malloc(sizeof(int*)*10);

     for (int i = 0; i < 10; i++)
        {
            C[i] = (int *)malloc(sizeof(int)*10);
		for (int j = 0; j < 10; j++)
		{
			C[i][j] = 0;
			for (int k = 0; k < 10; k++)
				C[i][j] = C[i][j] || A[i][k] && B[k][j];
		}
        }
	return C;
}

int** transponateMatrix(int** A)
{
	int** B = (int **)malloc(sizeof(int*)*10);

    for (int i = 0; i < 10; i++)
	{
	    B[i] = (int *)malloc(sizeof(int)*10);
	}

	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			B[i][j] = A[j][i];
	return B;
}

int** getSymmetricMatrix(int** A)
{
	int** matrix = mallocMatrix(10);

	for (int i = 0; i < 10; i++)
	{
		for (int j = i; j < 10; j++)
		{
			matrix[i][j] = A[i][j];
			matrix[j][i] = A[i][j];
		}
	}
	return matrix;
}
void pause(HWND* hWnd)
{
	while (1) {
		if (_kbhit()) {

			char c = _getch();
            isDrawingFirst = 0;
			//InvalidateRect(*hWnd, NULL, TRUE);
			break;
		}
	}
}

void drawGraph(HDC* hdc, HPEN pen, HPEN highlightPen, int** MAT, int offset) {
	char * nn[3] = {
		"1",
		"2",
		"3"
	};
	int nx[3] = {
		100,
		200,
		300
	};
	int ny[3] = {
		70,
		70,
		70
	};
	//const int NODES_COUNT = 10 + N[2];
	int lastX = 100 + 450 * offset;
	int lastY = 70;
	int difX = 200;
	int difY = 140;
	int isGoingRight = 1;
	int isGoingDown = 0;

	Node nodes[10];

	SelectObject(hdc, pen);
    char out[30];

    for(int i = 0; i< 10; i++)currentNodes[offset][i].number = i+1;

	for (int i = 0; i < 10; i++) {
		int x,y;

		if (isGoingRight && !isGoingDown) {
			x = lastX + difX * 0.75;
			y = lastY;
			if (i == 2) {
				isGoingDown = 1;
			}
		}
		if (isGoingRight && isGoingDown) {
			x = lastX;
			y = lastY + difY;
			if (i == 4) {
				isGoingRight = 0;
			}
		}
		if (!isGoingRight && isGoingDown) {
			x = lastX - difX / 2;
			y = lastY;
			if (i == 7) {
				isGoingDown = 0;
			}
		}
		if (!isGoingRight && !isGoingDown) {
			x = lastX;
			y = lastY - difY;
			if (i == 8) {
				nodes[i].x = x;
				nodes[i].y = y;
                _itoa_s(i+1, out, 30, 10);
                if(currentNodes[offset][i].number != -1)
                    {
                        _itoa_s(currentNodes[offset][i].number, out, 30, 10);
                        BuildNode(hdc, x, y, *out, 1 , 8);
                    }

                else
                    BuildNode(hdc, x, y, out, 0 , 8);

				nodes[i + 1].x = x + difX * 3 / 4;
				nodes[i + 1].y = y + difY;
                _itoa_s(i+2, out, 30, 10);
				if(currentNodes[offset][i].number != -1)
                    {
                        _itoa_s(currentNodes[offset][i+1].number, out, 30, 10);
                        if(currentNodes[offset][i+1].number == 10)
                        {
                            BuildNode(hdc, x + difX * 3 / 4, y + difY, *"X", 1, 9);
                            break;
                        }
                        BuildNode(hdc, x + difX * 3 / 4, y + difY, *out, 1, 9);
                    }
				else
                    BuildNode(hdc, x + difX * 3 / 4, y + difY, *out, 0, 9);
				break;
			}
		}
		nodes[i].x = x;
		nodes[i].y = y;
		lastX = x;
		lastY = y;
        _itoa_s(i+1, out, 30, 10);
		if(currentNodes[offset][i].number != -1)
        {
            _itoa_s(currentNodes[offset][i].number, out, 30, 10);
            BuildNode(hdc, x, y, *out, 1, i);
        }
        else
            BuildNode(hdc, x, y, *out, 0, i);

	}
    if(highlightPen != NULL)
        SelectObject(hdc, highlightPen);
	//LineTo(hdc, nx[1], ny[1]);

	//arrow(0, nx[1] - dx, ny[1]);
	for (int i = 0; i < 10; i++) {
		for (int j = i; j < 10; j++) {
			if (MAT[i][j] != 0 && i != j) {
                if(isDrawingFirst == 1 || offset == 1)
                    drawConnection(hdc, nodes[i].x, nodes[j].x, nodes[i].y, nodes[j].y, shadowMatrix[i][j]);
                else drawConnection(hdc, nodes[i].x, nodes[j].x, nodes[i].y, nodes[j].y, -1);
			}
		}
	}
}

Node** mallocNodes(int size)
{
    Node** nodes = (Node**)malloc(sizeof(Node*)*size);
    for(int i = 0; i < size; i++)
    {
        nodes[i] = (Node*)malloc(sizeof(Node)*size);
        for(int j = 0; j < size; j++)
        {
            nodes[i][j].number = -1;
        }
    }
    return nodes;
}

void drawGraphs(HDC* hdc)
{
	HPEN BPen = CreatePen(PS_SOLID, 2, RGB(50, 0, 255));
	HPEN KPen = CreatePen(PS_SOLID, 1, RGB(20, 20, 5));
	HPEN GPen = CreatePen(PS_SOLID, 2, RGB(180, 180, 180));

    if(MAT != NULL)
    {
        drawGraph(*hdc, BPen, KPen, MAT, 0);
    }
    if(treeMatrix != NULL)
    {
        drawGraph(*hdc, BPen, NULL, treeMatrix, 1);
    }

}

int** getUpperTr(int** A)
{
    int** matrix = mallocMatrix(10);
    for(int i = 0; i < 10; i++)
    {
        for(int j = i+1; j < 10; j++)
        {
            matrix[i][j] = 1;
        }
    }
    return matrix;
}

int** sumByElement(int** A, int** B)
{
    int** matrix = mallocMatrix(10);
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            matrix[i][j] = A[i][j] + B[i][j];
        }
    }
    return matrix;
}

int** multiplyIntByElement(int** A, int** B)
{
    int** matrix = mallocMatrix(10);
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            matrix[i][j] = (int) A[i][j] * B[i][j];
        }
    }
    return matrix;
}

double** multiplyByElement(double** A, double** B)
{
    double** matrix = mallocDoubleMatrix(10);
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            matrix[i][j] = A[i][j] * B[i][j];
        }
    }
    return matrix;
}

int** roundMatrix(double** A)
{
    int** matrix = mallocMatrix(10);
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            matrix[i][j] = (int)(A[i][j]);

        }
    }
    return matrix;
}

int** getWeightMatrix(int** W)
{
    int** B = mallocMatrix(10);
    int** C = mallocMatrix(10);
    int** D = mallocMatrix(10);

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            B[i][j] = W[i][j] <= 0 ? 0 : 1;
        }
    }
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            C[i][j] = B[i][j] != B[j][i] ? 1 : 0;
            D[i][j] = B[i][j] == B[j][i] && B[i][j] == 1 ? 1 : 0;
        }
    }

    W = multiplyIntByElement(sumByElement(C, multiplyIntByElement(D, getUpperTr(W))), W);

    W = getSymmetricMatrix(W);

    return W;
}

double** intToDoubleMatrix(int** A)
{
    double** matrix = mallocDoubleMatrix(10);
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            matrix[i][j] = (double)A[i][j];
        }
    }
    return matrix;
}

int isCycle(int** A, int* visited, int point, int parrent)
{
    visited[point] = 1;
    for(int i = 0; i < 10; i++)
    {
        if(A[point][i] != 0 && visited[i] != 1)
        {
            if(isCycle(A, visited, i, point) == 1) return 1;
        }
        else{
            if(i != parrent && A[point][i] != 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

void getKraskal(int** weights)
{
    //find minimum
    int minWeight = 1e9;
    int minDi = -1;
    int minDj = -1;
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            if(weights[i][j] < minWeight && weights[i][j] != 0)
            {
                minWeight = weights[i][j];
                minDi = i;
                minDj = j;
            }
        }
    }
    weights[minDi][minDj] = 0;
    weights[minDj][minDi] = 0;

    //add to treeMAtrix if not cycle
    treeMatrix[minDi][minDj] = minWeight;
    int* visited = malloc(sizeof(int)*10);
    for(int i = 0; i < 10; i++)
    {
        visited[i] = 0;
    }
    if(isCycle(treeMatrix, visited, minDj, minDi) == 1)
    {
        treeMatrix[minDi][minDj] = 0;
    }else
    {
        treeMatrix[minDj][minDi] = minWeight;
    }
    //end if weights = NULL, move to get kraskal
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            if(weights[i][j] != 0)
            {
                getKraskal(weights);
                return;
            }
        }
    }
    return;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE  hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASS w;
    w.lpszClassName = ProgName;
    w.hInstance = hInstance;
    w.lpfnWndProc = WndProc;
    w.hCursor = LoadCursor(NULL, IDC_ARROW);
    w.hIcon = 0;
    w.lpszMenuName = 0;
    w.hbrBackground = WHITE_BRUSH;
    w.style = CS_HREDRAW|CS_VREDRAW;
    w.cbClsExtra = 0;
    w.cbWndExtra = 0;
    if(!RegisterClass(&w)) return 0;
    HWND hWnd;
    MSG lpMsg;
    HANDLE scriptThread = CreateThread(NULL, 0, executeReportScript, NULL, 0, NULL);
    hWnd = CreateWindow(ProgName, header, WS_OVERLAPPEDWINDOW, 100, 100, 2100, 900, (HWND)NULL, (HMENU)NULL,(HINSTANCE)hInstance, (HINSTANCE)NULL);
    ShowWindow(hWnd, nCmdShow);
    hwnd = hWnd;

    while(GetMessage(&lpMsg, hWnd, 0, 0)){
        TranslateMessage(&lpMsg);
        DispatchMessage(&lpMsg);
    }

    return (lpMsg.lParam);
}

DWORD WINAPI executeReportScript(LPVOID lpParam)
{
    srand(308);
    double **T = randm();
    HWND hWnd = hwnd;
    int **A = mulmr((1.0 - N[3]*0.01 - N[4]*0.005 - 0.05), T);
    RECT whole;
    whole.bottom = 1600;
    whole.right = 900;
    whole.top = 0;
    whole.left = 0;


	Node **nodes  = mallocNodes(10);
	currentNodes = mallocNodes(10);
    printf("Graph adjacency matrix:\n\n");
	outMatrix(A, 10);
    A = getSymmetricMatrix(A);
    printf("Graph symetric adjacency matrix:\n\n");
	outMatrix(A, 10);
    MAT = A;
    UpdateWindow(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);

    double** doubleA = intToDoubleMatrix(A);
	weightMatrix = roundMatrix(multiplyByElement(randmByHundred(), doubleA));

    weightMatrix = getWeightMatrix(weightMatrix);shadowMatrix = weightMatrix;

	printf("\n\nGenerating a minimal spanning tree of the graph. Press any key to proceed.\n\n");
	pause(hWnd);
	treeMatrix = mallocMatrix(10);
	getKraskal(weightMatrix);
    shadowMatrix = treeMatrix;
    UpdateWindow(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
    printf("Minimum spaning tree matrix:\n");
	outMatrix(treeMatrix, 10);
}

int getNonOrientedRegularityDegree(int* degrees)
{
	int degree = degrees[0];
	for (int i = 1; i < 10; i++)
	{
		if (degree != degrees[i])
			return -1;
	}
	return degree;
}


void nullIsVisited()
{
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            isVisited[i][j] = 0;
        }
    }
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT messg,  WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch(messg){
        case WM_PAINT :
             hdc = BeginPaint(hWnd, &ps);
             drawGraphs(&hdc);
             //Arc(hdc, x1+abs(x1-x2), y1-30, x2-abs(x1-x2), y2+30, x1, y1, x2, y2);

             EndPaint(hWnd, &ps);
             break;
        case WM_KEYDOWN:
             //scriptState = (scriptState == 1) ? 2 : 1;
             InvalidateRect(hWnd, NULL, TRUE);
             break;
        case WM_DESTROY:
             PostQuitMessage(0);
             break;
        default:
             return(DefWindowProc(hWnd, messg, wParam, lParam));
    }
}

void drawConnection(HDC *hdc,int x1, int x2, int y1, int y2, int cost)
{
    char weightCost[30];
    _itoa_s(cost, weightCost, 30, 10);
    int length = strlen(weightCost);
    int hasWritten = 0;
    void arrow(float fi, int px, int py)
    {
        fi = PI * (180.0 - fi) / 180.0;
        int lx, ly, rx, ry;
        lx = px + 15 * cos(fi+0.3);
        rx = px + 15 * cos(fi-0.3);
        ly = py + 15 * sin(fi+0.3);
        ry = py + 15 * sin(fi-0.3);
        MoveToEx(hdc, lx, ly, NULL);
        LineTo(hdc, px, py);
        LineTo(hdc, rx, ry);
        //return 0;
    }


    //double radius = dx/2+1;
        int xVector = 0;
        int yVector = 0;
        double rnd = 0;//((double)rand()/RAND_MAX*0.1);
        if(y1>y2)
        {
            y1-=dy*(0.8-5*rnd);
            y2+=dy*(0.8-5*rnd);
            yVector = -1;
        }
        if(y1<y2)
        {
            y1+=dy*(0.8-5*rnd);
            y2-=dy*(0.8-5*rnd);
            yVector = 1;
        }

        if(x1<x2)
        {
            x1+=dx/(2-5*rnd);
            x2-=dx/(2-5*rnd);
            xVector = 1;
        }
        if(x1>x2)
        {
            x1-=dx/(2-10*rnd);
            x2+=dx/(2-10*rnd);
            xVector = -1;
        }
        if(x1 == x2 && abs(y1-y2)>200)
        {
            int fakeX = 40 + x1;
            int fakeY =(y1+y2)/2;
            MoveToEx(hdc, x1, y1, NULL);
            LineTo(hdc, fakeX, fakeY);
            if(cost!=-1){
                TextOutA(hdc, fakeX, fakeY, weightCost, length);
                hasWritten = 1;
            }
            MoveToEx(hdc, fakeX, fakeY, NULL);

            LineTo(hdc, x2, y2);
            return;
        }
        if(y1==y2)
        {
            if(x1<x2)
            {
                x1+=dx/(2-5*rnd);
                x2-=dx/(2-5*rnd);
            }
            if(x1>x2)
            {
                x1-=dx/(2-5*rnd);
                x2+=dx/(2-5*rnd);
            }
            if(x1==x2)
            {


                int fakeX = x1 - 40;
                int fakeY = y1 - 40;

                MoveToEx(hdc, x1-dx, y1, NULL);
                LineTo(hdc, fakeX, fakeY);
                MoveToEx(hdc, fakeX, fakeY, NULL);
                if(cost!=-1){
                    TextOutA(hdc, fakeX+10, fakeY+10, weightCost, length);
                    hasWritten = 1;
                }
                fakeX += 80;

                LineTo(hdc, fakeX, fakeY);

                MoveToEx(hdc, fakeX, fakeY, NULL);
                LineTo(hdc, x1+dx-3, y1);
                return;
            }
        }
    if(xVector==1||(x1-x2)>100)
    {
        MoveToEx(hdc, x1, y1, NULL);
        x1 = x1+(xVector*abs(x1-x2))/(2-10*rnd);
        y1 = y2 + 70 * -yVector;

        if(yVector == 0)
        {
            y1 += 70;
        }
        if(cost!=-1){
            TextOutA(hdc, x1, y1, weightCost, length);
            hasWritten = 1;
        }
        LineTo(hdc, x1, y1);
    }

    double angle = 0;
	if (x1 - x2 == 0)
	{
		if (y1 > y2)
			angle = 90;
		else
			angle = 270;
	}
	else
	{
		double riseoverrun = (double)(y1 - y2) / (double)(x1 - x2);
		double radians = atan(riseoverrun);

		angle = radians * ((double)180 / PI);

		if ((x1 - x2) < 0 || (y1 - y2) < 0)
			angle -= 180;
		if ((x1 - x2) > 0 && (y1 - y2) < 0)
			angle += 180;
		if (angle < 0)
			angle += 360;
	}

    if(cost!=-1 && hasWritten == 0)
        TextOutA(hdc, (x1+x2)/2, (y1+y2)/2-20, weightCost, length);
   MoveToEx(hdc, x1, y1, NULL);
   LineTo(hdc, x2, y2);
   //arrow(angle, x2, y2);
}

void BuildNode(HDC *hdc, int x, int y, char* index, int hasText, int i_index)
{
    backHdc = hdc;
    Ellipse(hdc, x - dx, y - dy, x + dx, y + dy);
    if(hasText)
    {
        TextOut(hdc, x - dtx, y - dy / 2, &index, 1);
    }
}
