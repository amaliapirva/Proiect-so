#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
 
struct BMPHeader {
    char signature[2];
    uint32_t size;
    uint32_t reserved;
    uint32_t offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    uint32_t x_pixels_per_meter;
    uint32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
};
 
void print_error(char *msg) {
    write(STDERR_FILENO, msg, strlen(msg));
    exit(EXIT_FAILURE);
}
 
int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_error("Usage: ./program <fisier intrare>\n");
    }
 
    char *filename = argv[1];
 
    // Deschide fisierul BMP
    int bmp_fd = open(filename, O_RDONLY);
    if (bmp_fd == -1) {
        print_error("Eroare la deschiderea fisierului BMP\n");
    }
 
    // Obține informații despre fisier
    struct stat file_stat;
    if (fstat(bmp_fd, &file_stat) == -1) {
        print_error("Eroare la obtinerea informatiilor despre fisier\n");
    }
 
    // Citeste header-ul BMP
    struct BMPHeader bmp_header;
    if (read(bmp_fd, &bmp_header, sizeof(struct BMPHeader)) == -1) {
        print_error("Eroare la citirea header-ului BMP\n");
    }
 
    // Creeaza fisierul statistica.txt
    int stats_fd = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (stats_fd == -1) {
        print_error("Eroare la crearea fisierului statistica.txt\n");
    }
 
    // Scrie informatiile in fisier
    char buffer[256];
    sprintf(buffer, "nume fisier: %s\n", filename);
    write(stats_fd, buffer, strlen(buffer));
 
    sprintf(buffer, "inaltime: %u\n", bmp_header.height);
    write(stats_fd, buffer, strlen(buffer));
 
    sprintf(buffer, "lungime: %u\n", bmp_header.width);
    write(stats_fd, buffer, strlen(buffer));
 
    sprintf(buffer, "dimensiune: %lu\n", file_stat.st_size);
    write(stats_fd, buffer, strlen(buffer));
 
    sprintf(buffer, "identificatorul utilizatorului: %d\n", getuid());
    write(stats_fd, buffer, strlen(buffer));
 
    // Obține timpul ultimei modificări
    struct tm *modification_time = localtime(&file_stat.st_mtime);
    strftime(buffer, sizeof(buffer), "timpul ultimei modificari: %d.%m.%Y\n", modification_time);
    write(stats_fd, buffer, strlen(buffer));
 
    // Alte informații și scriere în fișier...
 
    // Închide fișierele
    close(bmp_fd);
    close(stats_fd);
 
    return 0;
}
 
