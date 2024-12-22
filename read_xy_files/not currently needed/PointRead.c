#include  "FPToolkit.c"

int main() {

    printf("what");

    double x[1000], y[1000];

    //gets all the x, y points
    int numPoints;
    scanf("%d", &numPoints);

    for (int i = 0; i < numPoints; i++) {
        scanf("%lf %lf", &x[i], &y[i]);
    }

    
    int con[1000][30];

    //length of each polygon
    int connectionsOnLine[1000];
    
    //how many polygons
    int numConnections;
    scanf("%d", &numConnections);

    for (int i = 0; i < numConnections; i++) {
        scanf("%d", &connectionsOnLine[i]);

        for (int j = 0; j < connectionsOnLine[i]; j++) {
            scanf("%d", &con[i][j]);
        }
    }

    G_init_graphics(800, 800);

    //I have no fucking idea what this does
    G_rgb (0.537, 0.812, 0.941); // blue
    G_clear();

    for (int i = 0; i < numConnections; i++) {
        int prev_x = x[con[i][0] - 1];
        int prev_y = y[con[i][0]];
        for (int j = 1; j < connectionsOnLine[i]; j++) {
            G_line(prev_x, prev_y, x[con[i][j]], y[con[i][j]]);

            prev_x = x[con[i][j]];
            prev_y = y[con[i][j]];
            printf(prev_x);
        }
    }
    
}