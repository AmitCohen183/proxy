/**
 //@author Created by Amit Cohen , id 315147330 .
 //@date on 27/11/2021.
 //@version 1.4
 */
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/stat.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>

#include <fcntl.h>

#include <netdb.h>

#define TRUE 1
#define FALSE 0
#define MAX_SIZE_of_buffer 10000

typedef struct URL {
    char * server_protocol;
    char * file_path;
    int port;
    char * host;
    char * path;
}
        URL;
struct stat st = {
        0
};

int finding_amount_of_characters(const char * string, char character);
int check_protocol(char * url);
URL * URL_parsing(char * url);
int get_index_of_third_slash(const char * url);
void create_the_path(URL * url);
void free_the_URL(URL * url);
long print_the_file_from_filesystem(URL * url, int flag, long size2);
int connect_to_socket();
int connect_to_server(URL * url, char * HTTP, unsigned char * buffer);
char * build_the_request(URL * url);
void show_the_page(char * HTTP_request, URL * url);

int main(int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage : only one parameter must be entered");
        exit(EXIT_FAILURE);
    }
    URL * url = URL_parsing(argv[1]);
    char * HTTP_request = build_the_request(url);
    show_the_page(HTTP_request, url);
    free(HTTP_request);
    free_the_URL(url);
    exit(EXIT_SUCCESS);
}
/**
 *This function counts the number of occurrences of a particular character within a string.
 * @param string The string for testing
 * @param character The requested character
 * @return The number of times the character appears.
 */
int finding_amount_of_characters(const char * string, char character) {
    int counter = 0;
    for (int i = 0; string[i] != '\0'; i++)
        if (string[i] == character)
            counter++;
    return counter;
}
/**
 *The function checks if the URL protocol is HTTP.
 * @param url The address to check.
 * @return Returns 1 if the protocol is correct, otherwise returns 0.
 */
int check_protocol(char * url) {
    if (strncmp(url, "http://", 7) == 0)
        return TRUE;
    return FALSE;
}
/**
 *This function composes the structure of the URL, breaks down the address into path, host, port, protocol.
 * @param url The same address is required to parsing.
 * @return URL structure with all the details.
 */
URL * URL_parsing(char * url) {
    if (strlen(url) == 7 || !check_protocol(url)) {
        fprintf(stderr, "usage : in this format 'http://host/path'");
        exit(EXIT_FAILURE);
    }
    char * ptr = NULL;
    URL * parsing = (URL * ) malloc(sizeof(URL));
    if (parsing == NULL) {
        perror("malloc ");
        exit(EXIT_FAILURE);
    }
    parsing -> path = NULL;
    parsing -> host = NULL;
    parsing -> file_path = NULL;
    parsing -> server_protocol = NULL;

    int end_host = get_index_of_third_slash(url);
    if (end_host == -1) {
        end_host = (int) strlen(url);
        parsing -> file_path = (char * ) malloc(sizeof(char) * (11));
        if (parsing -> file_path == NULL) {
            perror("malloc ");
            free_the_URL(parsing);
            exit(EXIT_FAILURE);
        }
        strcpy(parsing -> file_path, "index.html");
        parsing -> file_path[strlen(parsing -> file_path)] = '\0';
    } else {
        parsing -> file_path = (char * ) malloc(sizeof(char) * ((int) strlen(url) - end_host + 2));
        memset(parsing -> file_path, '\0', (int) strlen(url) - end_host + 2);
        if (parsing -> file_path == NULL) {
            perror("malloc ");
            free_the_URL(parsing);
            exit(EXIT_FAILURE);
        }
        strcpy(parsing -> file_path, & url[end_host + 1]);
        parsing -> file_path[strlen(parsing -> file_path)] = '\0';
    }
    parsing -> host = (char * ) malloc(sizeof(char) * (end_host - 6));
    memset(parsing -> host, '\0', end_host - 6);
    if (parsing -> host == NULL) {
        perror("malloc ");
        free_the_URL(parsing);
        exit(EXIT_FAILURE);
    }
    strncpy(parsing -> host, & url[7], end_host - 7);
    parsing -> host[strlen(parsing -> host)] = '\0';
    parsing -> server_protocol = (char * ) malloc(sizeof(char) * (8));
    memset(parsing -> server_protocol, '\0', 8);
    if (parsing -> server_protocol == NULL) {
        perror("malloc ");
        free_the_URL(parsing);
        exit(EXIT_FAILURE);
    }
    if (check_protocol(url)) {
        strncpy(parsing -> server_protocol, url, 7);
    }
    if (finding_amount_of_characters(parsing -> host, ':') == 0)
        parsing -> port = 80;
    else {
        ptr = strtok(parsing -> host, ":");
        ptr = strtok(NULL, ":");
        char * ptr1;
        parsing -> port = (int) strtol(ptr, & ptr1, 10);
    }
    parsing -> path = (char * ) malloc(sizeof(char) * (strlen(parsing -> host) + 1));
    memset(parsing -> path, '\0', strlen(parsing -> host) + 1);
    if (parsing -> path == NULL) {
        perror("malloc ");
        free_the_URL(parsing);
        exit(EXIT_FAILURE);
    }
    strcat(parsing -> path, parsing -> host);
    parsing -> path = realloc(parsing -> path, sizeof(char) * ((int) strlen(parsing -> path) + 2));
    if (parsing -> path == NULL) {
        perror("realloc ");
        free_the_URL(parsing);
        exit(EXIT_FAILURE);
    }
    strcat(parsing -> path, "/");
    parsing -> path = realloc(parsing -> path, sizeof(char) * ((int) strlen(parsing -> path) + (int) strlen(parsing -> file_path) + 1));
    strcat(parsing -> path, parsing -> file_path);
    return parsing;
}
/**
 *The function searches for the third slash and returns its index.
 * @param url The requested address.
 * @return Returns the index if found, otherwise returns -1.
 */
int get_index_of_third_slash(const char * url) {
    int counter = 0;
    for (int i = 0; url[i] != '\0'; i++)
        if (url[i] == '/') {
            counter++;
            if (counter == 3)
                return i;
        }
    return -1;
}
/**
 *The function creates folders for the entire path and the latter produces as a file.
 * @param url
 */
void create_the_path(URL * url) {
    char * a = (char * ) malloc(sizeof(char) * ((int) strlen(url -> path) + 1));
    if (a == NULL) {
        perror("malloc ");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    memset(a, '\0', (int) strlen(url -> path) + 1);
    strcpy(a, url -> path);
    int amount = finding_amount_of_characters(a, '/'), fd;
    if (stat(url -> host, & st) == -1) {
        mkdir(url -> host, 0700);
    }
    if (amount == 1) {
        fd = open(a, O_CREAT, 0644);
        close(fd);
        free(a);
    } else {
        char * ptr = strtok(a, "/");
        char * dir_first = (char * ) malloc(sizeof(char) * (strlen(ptr) + 1));
        if (dir_first == NULL) {
            perror("malloc ");
            free_the_URL(url);
            exit(EXIT_FAILURE);
        }
        memset(dir_first, '\0', strlen(ptr) + 1);
        strcpy(dir_first, ptr);
        if (stat(dir_first, & st) == -1)
            mkdir(dir_first, 0700);
        while (amount > 1) {
            ptr = strtok(NULL, "/");
            dir_first = realloc(dir_first, sizeof(char) * (strlen(dir_first) + 2));
            if (dir_first == NULL) {
                perror("realloc ");
                free_the_URL(url);
                exit(EXIT_FAILURE);
            }
            strcat(dir_first, "/");
            dir_first = realloc(dir_first, sizeof(char) * (strlen(dir_first) + strlen(ptr) + 1));
            if (dir_first == NULL) {
                perror("realloc ");
                free_the_URL(url);
                exit(EXIT_FAILURE);
            }
            strcat(dir_first, ptr);
            if (stat(dir_first, & st) == -1)
                mkdir(dir_first, 0700);
            amount--;
        }
        dir_first = realloc(dir_first, sizeof(char) * (strlen(dir_first) + 2));
        if (dir_first == NULL) {
            perror("realloc ");
            exit(EXIT_FAILURE);
        }
        strcat(dir_first, "/");
        ptr = strtok(NULL, "/");
        dir_first = realloc(dir_first, sizeof(char) * (strlen(dir_first) + strlen(ptr) + 1));
        if (dir_first == NULL) {
            perror("realloc ");
            free_the_URL(url);
            exit(EXIT_FAILURE);
        }
        strcat(dir_first, ptr);
        fd = open(dir_first, O_CREAT, 0644);
        close(fd);
        free(dir_first);
        free(a);
    }
}
/**
 *The function releases the structure and all its features.
 * @param url
 */
void free_the_URL(URL * url) {
    if (url != NULL) {
        if (url -> host != NULL)
            free(url -> host);
        if (url -> file_path != NULL)
            free(url -> file_path);
        if (url -> server_protocol != NULL)
            free(url -> server_protocol);
        if (url -> path != NULL)
            free(url -> path);
        if (url != NULL)
            free(url);
    }
}
/**
 * The function opens the file according to the path obtained from the structure,. And prints the contents of the file.
 * @param url The relevant structure.
 * @param flag If the flag is equal to 0, print that the file already exists on the computer and if the flag is equal to 1. Only print the contents of the file.
 * @return The amount of characters written so far.
 */
long print_the_file_from_filesystem(URL * url, int flag, long size2) {
    long size_of_size = 0;
    int fd = open(url -> path, O_RDWR);
    if (fd < 0) {
        perror("open");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    unsigned char * buffer = (unsigned char * ) malloc(sizeof(unsigned char) * MAX_SIZE_of_buffer);
    if (buffer == NULL) {
        perror("malloc ");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, MAX_SIZE_of_buffer);
    if (flag == 0) {
        puts("File is given from local filesystem");
        puts("HTTP/1/0 200 OK\r\n");
    }
    stat(url -> path, & st);
    int size = (int) st.st_size, check;
    if (flag == 0) {
        printf("Content-Length: %d\r\n\r\n", size);
        char a[10];
        sprintf(a, "%d", size);
        size_of_size = (int) strlen(a) + size + 34;
    } else
        size_of_size = size2;
    while (TRUE) {
        memset(buffer, 0, MAX_SIZE_of_buffer);
        if ((check = (int) read(fd, buffer, MAX_SIZE_of_buffer - 1)) < 0) {
            perror("read ");
            free_the_URL(url);
            free(buffer);
            exit(EXIT_FAILURE);
        }
        if (check == 0)
            break;
        printf("%s", buffer);
    }

    free(buffer);
    printf("\nTotal response bytes: %ld\n", size_of_size);
    close(fd);
    return size_of_size;
}
/**
 *Socket connection.
 * @return The socket file descriptor.
 */
int connect_to_socket() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return socket_fd;
}
/**
 *Socket connection
 * @param url
 * @return The socket file descriptor.
 */
int connect_to_server(URL * url, char * HTTP, unsigned char * buffer) {
    struct sockaddr_in serv_address;
    struct hostent * server;
    int socket_fd = connect_to_socket();
    server = gethostbyname(url -> host);
    if (server == NULL) {
        herror("get host by name ");
        free_the_URL(url);
        free(HTTP);
        free(buffer);
        exit(EXIT_FAILURE);
    }
    serv_address.sin_family = AF_INET;
    bcopy((char * ) server -> h_addr_list[0], (char * ) & serv_address.sin_addr.s_addr, server -> h_length);
    serv_address.sin_port = htons(url -> port);
    if (connect(socket_fd, (const struct sockaddr * ) & serv_address, sizeof(serv_address)) < 0) {
        perror("connect");
        free_the_URL(url);
        close(socket_fd);
        exit(EXIT_FAILURE);
    }
    return socket_fd;
}
/**
 * The function generates the HTTP request according to the fixed format.
 * @param url
 * @return String of request.
 */
char * build_the_request(URL * url) {
    char * HTTP_request = (char * ) malloc(sizeof(char));
    if (HTTP_request == NULL) {
        perror("malloc ");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    HTTP_request[0] = '\0';
    HTTP_request = realloc(HTTP_request, sizeof(char) * ((int) strlen(HTTP_request) + 6));
    if (HTTP_request == NULL) {
        perror("realloc :");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    strcat(HTTP_request, "GET /");
    HTTP_request = realloc(HTTP_request, sizeof(char) * ((int) strlen(HTTP_request) + (int) strlen(url -> file_path) + 1));
    if (HTTP_request == NULL) {
        perror("realloc ");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    strcat(HTTP_request, url -> file_path);
    HTTP_request[strlen(HTTP_request)] = '\0';
    HTTP_request = realloc(HTTP_request, sizeof(char) * ((int) strlen(HTTP_request) + 20));
    if (HTTP_request == NULL) {
        perror("realloc :");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    strcat(HTTP_request, " HTTP/1.0\r\nHost: ");
    HTTP_request = realloc(HTTP_request, sizeof(char) * ((int) strlen(HTTP_request) + (int) strlen(url -> host) + 1));
    if (HTTP_request == NULL) {
        perror("realloc ");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    strcat(HTTP_request, url -> host);
    HTTP_request = realloc(HTTP_request, sizeof(char) * (strlen(HTTP_request) + 5));
    if (HTTP_request == NULL) {
        perror("realloc ");
        free_the_URL(url);
        exit(EXIT_FAILURE);
    }
    strcat(HTTP_request, "\r\n\r\n");
    return HTTP_request;
}
/**
 *The function writes the request to socket and reads what socket into the file we created and prints the extensions.
 * @param HTTP_request
 * @param url
 */
void show_the_page(char * HTTP_request, URL * url) {

    long counter = 0;
    if (stat(url -> path, & st) == -1) {
        unsigned char * buffer = (unsigned char * ) malloc(MAX_SIZE_of_buffer * (sizeof(unsigned char)));
        if (buffer == NULL) {
            perror("malloc  ");
            free_the_URL(url);
            free(HTTP_request);
            exit(EXIT_FAILURE);
        }
        char * ptr = NULL;
        memset(buffer, 0, MAX_SIZE_of_buffer);
        int socket_fd = connect_to_server(url, HTTP_request, buffer), check, flag = 0, flag_200_OK = 0;
        if ((write(socket_fd, HTTP_request, strlen(HTTP_request))) < 0) {
            perror("write  ");
            free_the_URL(url);
            free(buffer);
            free(HTTP_request);
            exit(EXIT_FAILURE);
        }
        FILE * fd = NULL;
        printf("HTTP request =\n%s\nLEN = %d\n", HTTP_request, (int) strlen(HTTP_request));
        while (TRUE) {
            memset(buffer, 0, MAX_SIZE_of_buffer);
            if ((check = (int) read(socket_fd, buffer, MAX_SIZE_of_buffer - 1)) < 0) {
                puts("line 384");
                perror("read ");
                free_the_URL(url);
                free(buffer);
                free(HTTP_request);
                exit(EXIT_FAILURE);
            }
            counter += check;
            if (!check && flag_200_OK) {
                fclose(fd);
                free(buffer);
                break;
            }
            if (!flag_200_OK) {
                if (strstr((char * ) buffer, "200 OK")) {
                    create_the_path(url);
                    fd = fopen(url -> path, "w+");
                    flag_200_OK = 1;
                    if (fd == NULL) {
                        perror("fopen ");
                        free_the_URL(url);
                        free(buffer);
                        free(HTTP_request);
                        exit(EXIT_FAILURE);
                    }
                }
                ptr = strstr((char * ) buffer, "\r\n\r\n");
            }
            if (!ptr && flag_200_OK && !flag) {
                printf("%s", buffer);
                continue;
            }
            if (ptr && !flag && flag_200_OK) {
                printf("%.*s", (int) strlen((char * ) buffer) - (int) strlen(ptr) + 4, buffer);
                flag = 1;
                int i = 0, offset = 0;
                for (; i < MAX_SIZE_of_buffer; i++)
                    if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n') {
                        offset = i + 4;
                        break;
                    }
                if ((fwrite( & buffer[offset], sizeof(unsigned char), check - offset, fd)) < 1) {
                    perror("fwrite  ");
                    free_the_URL(url);
                    free(buffer);
                    free(HTTP_request);
                    exit(EXIT_FAILURE);
                }
                continue;
            } else if (flag) {
                if ((fwrite(buffer, sizeof(unsigned char), check, fd)) < 1) {
                    perror("fwrite  ");
                    free_the_URL(url);
                    free(buffer);
                    free(HTTP_request);
                    exit(EXIT_FAILURE);
                }
            } else {
                printf("%s", buffer);
                printf("\nTotal response bytes: %d\n", (int) strlen((char * ) buffer));
                free_the_URL(url);
                free(buffer);
                free(HTTP_request);
                exit(EXIT_SUCCESS);
            }

        }
    } else if (stat(url -> path, & st) >= -1) {
        print_the_file_from_filesystem(url, 0, 0);
        return;
    }
    print_the_file_from_filesystem(url, 1, counter);
}