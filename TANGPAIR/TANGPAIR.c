#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "libs/matfun.h"
#include "libs/geofun.h"

int main() {
    double **vertexA, **vertexB, **vertexC, **tangentialPoint, **sideAB, **sideBC, **sideCA;
    int m = 2, n = 1;
    int x1, y1, x2, y2, x3, y3;

    printf("Enter the coordinates of point A (x1 y1): ");
    scanf("%d %d", &x1, &y1);
    printf("Enter the coordinates of point B (x2 y2): ");
    scanf("%d %d", &x2, &y2);
    printf("Enter the coordinates of point C (x3 y3): ");
    scanf("%d %d", &x3, &y3);

    vertexA = createMat(m, n);
    vertexB = createMat(m, n);
    vertexC = createMat(m, n);

    vertexA[0][0] = x1;
    vertexA[1][0] = y1;
    vertexB[0][0] = x2;
    vertexB[1][0] = y2;
    vertexC[0][0] = x3;
    vertexC[1][0] = y3;

    
    sideAB = Matsub(vertexA, vertexB, m, n);
    sideBC = Matsub(vertexB, vertexC, m, n);
    sideCA = Matsub(vertexC, vertexA, m, n);
    double a = Matnorm(sideAB, m);
    double b = Matnorm(sideBC, m);
    double c = Matnorm(sideCA, m);

    double x_val = (a + c - b) / 2;
    double y_val = (a + b - c) / 2;
    double z_val = (b + c - a) / 2;

    tangentialPoint = createMat(2, 2);
    tangentialPoint[0][0] = (y_val * vertexA[0][0] + x_val * vertexB[0][0]) / (y_val + x_val);
    tangentialPoint[0][1] = (y_val * vertexA[1][0] + x_val * vertexB[1][0]) / (y_val + x_val);
    tangentialPoint[1][0] = (x_val * vertexC[0][0] + z_val * vertexA[0][0]) / (x_val + z_val);
    tangentialPoint[1][1] = (x_val * vertexC[1][0] + z_val * vertexA[1][0]) / (x_val + z_val);

    printf("Coordinates of tangential point E: %.6f, %.6f\n", tangentialPoint[0][0], tangentialPoint[0][1]);
    printf("Coordinates of tangential point F: %.6f, %.6f\n", tangentialPoint[1][0], tangentialPoint[1][1]);

    freeMat(vertexA, m);
    freeMat(vertexB, m);
    freeMat(vertexC, m);
    freeMat(tangentialPoint, 2);
    freeMat(sideAB, m);
    freeMat(sideBC, m);
    freeMat(sideCA, m);

    return 0;
}

