#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "libs/matfun.h"
#include "libs/geofun.h"

#define BUFFER_SIZE 2048
#define PORT_NUMBER 7287

void sendHTMLForm(int client_fd, double D1, double D2, double E1, double E2) {
    char *html_template = "<!DOCTYPE html>\n"
                          "<html lang=\"en\">\n"
                          "<head>\n"
                          "<meta charset=\"UTF-8\">\n"
                          "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                          "<title>TANGPAIR</title>\n"
                          "<style>"
                          "html {font-family: Arial, sans-serif;}"
                          "h2 {font-size: 2.0rem; color: blue;}"
                          "</style>"
                          "</head>\n"
                          "<body>\n"
                          "<h2>Finding Tangent Point's </h2>\n"
                          "<h3>Enter coordinates for triangle vertices A, B, C</h3>\n"
                          "<form method=\"post\">\n"
                          "    <label for=\"x1\">Vertex A (x1, y1):</label><br>"
                          "    <input type=\"text\" id=\"x1\" name=\"x1\" step=\"any\" required value=\"1\">"
                          "    <input type=\"text\" id=\"y1\" name=\"y1\" step=\"any\" required value=\"-1\"><br><br>"
                          "    <label for=\"x2\">Vertex B (x2, y2):</label><br>"
                          "    <input type=\"text\" id=\"x2\" name=\"x2\" step=\"any\" required value=\"-4\">"
                          "    <input type=\"text\" id=\"y2\" name=\"y2\" step=\"any\" required value=\"6\"><br><br>"
                          "    <label for=\"x3\">Vertex C (x3, y3):</label><br>"
                          "    <input type=\"text\" id=\"x3\" name=\"x3\" step=\"any\" required value=\"-3\">"
                          "    <input type=\"text\" id=\"y3\" name=\"y3\" step=\"any\" required value=\"-5\"><br><br>"
                          "    <button type=\"submit\">Calculate</button>\n"
                          "</form>\n"
                          "<h3>Results</h3>\n"
                          "<p>Result D: %.6f, %.6f</p>"
                          "<p>Result E: %.6f, %.6f</p>"
                          "</body>\n"
                          "</html>\n";

    char response[BUFFER_SIZE];
    sprintf(response, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n");
    snprintf(response + strlen(response), BUFFER_SIZE - strlen(response),
             html_template, D1, D2, E1, E2);

    send(client_fd, response, strlen(response), 0);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int option = 1;
    socklen_t addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Reusing address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Setting address parameters
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT_NUMBER);

    // Binding socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Printing server address
    printf("Server is listening on port %d\n", PORT_NUMBER);

    while (1) {
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        printf("New connection accepted\n");

        char buffer[BUFFER_SIZE] = {0};
        read(client_fd, buffer, BUFFER_SIZE);
        printf("Received data from client: %s\n", buffer);

        char *body_start = strstr(buffer, "\r\n\r\n");

        if (body_start) {

            body_start += 4;

            double **vertexA, **vertexB, **vertexC, **tangentialPoint, **sideAB, **sideBC, **sideCA;
            int m = 2, n = 1, x1, y1, x2, y2, x3, y3;
            sscanf(body_start, "x1=%d&y1=%d&x2=%d&y2=%d&x3=%d&y3=%d", &x1, &y1, &x2, &y2, &x3, &y3);
            vertexA = createMat(m, n);
            vertexB = createMat(m, n);
            vertexC = createMat(m, n);
            tangentialPoint = createMat(2, 2);
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
            tangentialPoint[0][0] = (y_val * vertexA[0][0] + x_val * vertexB[0][0]) / (y_val + x_val);
            tangentialPoint[0][1] = (y_val * vertexA[1][0] + x_val * vertexB[1][0]) / (y_val + x_val);
            tangentialPoint[1][0] = (x_val * vertexC[0][0] + z_val * vertexA[0][0]) / (x_val + z_val);
            tangentialPoint[1][1] = (x_val * vertexC[1][0] + z_val * vertexA[1][0]) / (x_val + z_val);
            sendHTMLForm(client_fd, tangentialPoint[0][0], tangentialPoint[0][1], tangentialPoint[1][0], tangentialPoint[1][1]);
            printf("Results sent to client\n");

            // Freeing allocated memory
            freeMat(vertexA, m);
            freeMat(vertexB, m);
            freeMat(vertexC, m);
            freeMat(tangentialPoint, 2);
            freeMat(sideAB, m);
            freeMat(sideBC, m);
            freeMat(sideCA, m);
        }

        close(client_fd);
    }

    return 0;
}

