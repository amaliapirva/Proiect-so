#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

typedef struct {
    int file_size;
    int width;
    int height;
    int image_size;
} BMPHeader;

// Funcția pentru citirea informațiilor din fișierul BMP
int readBMPHeader(const char *filename, BMPHeader *header) {
    int fin = open(filename, O_RDONLY);
    if (fin == -1) {
        perror("Eroare la deschiderea fisierului BMP");
        return -1;
    }

    char signature[2];
    read(fin, signature, 2);
    if (signature[0] != 'B' || signature[1] != 'M') {
        close(fin);
        return -1;
    }

    lseek(fin, 4, SEEK_SET);
    read(fin, header, sizeof(BMPHeader));

    close(fin);

    return 0;
}

// Funcția pentru scrierea statisticilor în fișier
int writeStatistics(const char *filename, const BMPHeader *header) {
    int fout = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fout == -1) {
        perror("Eroare la deschiderea fisierului de statistica");
        return -1;
    }

    struct stat BMPinfo;
    if (stat(filename, &BMPinfo) == -1) {
        perror("Eroare la obtinerea informatiilor");
        close(fout);
        return -1;
    }

    char timp[20];
    strftime(timp, sizeof(timp), "%d.%m.%Y", localtime(&BMPinfo.st_mtime));

    char buffer[250];
    int var = sprintf(buffer,
                    "nume fisier: %s\n"
                    "inaltime: %d\n"
                    "lungime: %d\n"
                    "dimensiune: %d\n"
                    "identificatorul utilizatorului: %d\n"
                    "timpul ultimei modificari: %s\n"
                    "contorul de legaturi: %ld\n"
                    "drepturi de acces user: %c%c%c\n"
                    "drepturi de acces grup: %c%c%c\n"
                    "drepturi de acces altii: %c%c%c\n"
                    "\n",
                    filename, header->height, header->width, header->file_size, BMPinfo.st_uid,
                    timp,
                    BMPinfo.st_nlink,
                    (BMPinfo.st_mode & S_IRUSR) ? 'R' : '-',
                    (BMPinfo.st_mode & S_IWUSR) ? 'W' : '-',
                    (BMPinfo.st_mode & S_IXUSR) ? 'X' : '-',
                    (BMPinfo.st_mode & S_IRGRP) ? 'R' : '-', 
                    (BMPinfo.st_mode & S_IWGRP) ? 'W' : '-',
                    (BMPinfo.st_mode & S_IXGRP) ? 'X' : '-',
                    (BMPinfo.st_mode & S_IROTH) ? 'R' : '-',
                    (BMPinfo.st_mode & S_IWOTH) ? 'W' : '-',
                    (BMPinfo.st_mode & S_IXOTH) ? 'X' : '-');

    if (write(fout, buffer, var) < 0) {
        perror("Eroare la scrierea datelor despre director in fisier");
        close(fout);
        return -1;
    }

    close(fout);

    return 0;
}

int main(int argc, char *argv[]) {
    BMPHeader header;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fisier_intrare>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (readBMPHeader(argv[1], &header) != 0) {
        perror("Eroare la citirea informatiilor din fisierul BMP");
        return EXIT_FAILURE;
    }

    if (writeStatistics(argv[1], &header) != 0) {
        perror("Eroare la scrierea statisticilor in fisier");
        return EXIT_FAILURE;
    }

    return 0;
}
