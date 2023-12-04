#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_PATH 1024

void afisareEroare(const char *mesaj) {
    perror(mesaj);
    exit(EXIT_FAILURE);
}

typedef struct {
    char signature[2];
    unsigned int fileSize;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int dataOffset;
    unsigned int headerSize;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int imageSize;
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    unsigned int totalColors;
    unsigned int importantColors;
} BMPHeader;

void convertToGrayScale(const char *inputPath, const char *outputPath) {
    int inputFile = open(inputPath, O_RDONLY);
    if (inputFile == -1) {
        afisareEroare("Eroare la deschiderea fișierului BMP pentru citire");
    }

    BMPHeader header;
    ssize_t bytesRead = read(inputFile, &header, sizeof(BMPHeader));
    if (bytesRead != sizeof(BMPHeader)) {
        close(inputFile);
        afisareEroare("Eroare la citirea header-ului BMP");
    }

    int outputFile = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (outputFile == -1) {
        close(inputFile);
        afisareEroare("Eroare la deschiderea fișierului de ieșire pentru scriere");
    }

    if (write(outputFile, &header, sizeof(BMPHeader)) != sizeof(BMPHeader)) {
        close(inputFile);
        close(outputFile);
        afisareEroare("Eroare la scrierea header-ului BMP in fisierul de iesire");
    }

    for (int i = 0; i < header.height; ++i) {
        for (int j = 0; j < header.width; ++j) {
            unsigned char pixel[3];
            if (read(inputFile, pixel, sizeof(pixel)) != sizeof(pixel)) {
                close(inputFile);
                close(outputFile);
                afisareEroare("Eroare la citirea pixelului din fisierul BMP");
            }

            unsigned char grayValue = (unsigned char)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);

            if (write(outputFile, &grayValue, sizeof(grayValue)) != sizeof(grayValue)) {
                close(inputFile);
                close(outputFile);
                afisareEroare("Eroare la scrierea pixelului in fisierul de iesire");
            }
        }
    }

    close(inputFile);
    close(outputFile);
}

void processFile(const char *inputPath, const char *outputDir) {
    int inputFile = open(inputPath, O_RDONLY);
    if (inputFile == -1) {
        afisareEroare("Eroare la deschiderea fisierului pentru citire");
    }

    struct stat statFisier;
    if (fstat(inputFile, &statFisier) == -1) {
        close(inputFile);
        afisareEroare("Eroare la obtinerea informatiilor despre fisier");
    }

    char numeFisierStatistica[MAX_PATH];
    snprintf(numeFisierStatistica, sizeof(numeFisierStatistica), "%s/%s_statistica.txt", outputDir, inputPath);

    int outputFile = open(numeFisierStatistica, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (outputFile == -1) {
        close(inputFile);
        afisareEroare("Eroare la deschiderea fisierului de iesire pentru scriere");
    }

    dprintf(outputFile, "Nume fisier: %s\n", inputPath);
    dprintf(outputFile, "Dimensiune: %ld\n", statFisier.st_size);

    close(inputFile);
    close(outputFile);
}

void processDirectory(const char *inputDir, const char *outputDir) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(inputDir);
    if (dir == NULL) {
        afisareEroare("Eroare la deschiderea directorului de intrare");
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char inputPath[MAX_PATH];
            snprintf(inputPath, sizeof(inputPath), "%s/%s", inputDir, entry->d_name);

            char outputPath[MAX_PATH];
            snprintf(outputPath, sizeof(outputPath), "%s/%s_statistica.txt", outputDir, entry->d_name);

            pid_t childPid = fork();

            if (childPid == 0) {
                processFile(inputPath, outputPath);
                exit(EXIT_SUCCESS);
            } else if (childPid > 0) {
                int status;
                waitpid(childPid, &status, 0);

                if (WIFEXITED(status)) {
                    printf("S-a încheiat procesul cu PID-ul %d și codul %d\n", childPid, WEXITSTATUS(status));

                    int fileStatistica = open(outputPath, O_RDONLY);
                    if (fileStatistica == -1) {
                        afisareEroare("Eroare la deschiderea fișierului de statistică pentru citire");
                    }

                    char buffer[10];
                    ssize_t bytesRead = read(fileStatistica, buffer, sizeof(buffer) - 1);
                    buffer[bytesRead] = '\0';
                    close(fileStatistica);

                    printf("Procesul cu PID-ul %d a scris %s linii in %s\n", childPid, buffer, outputPath);
                } else {
                    printf("Procesul cu PID-ul %d nu s-a încheiat corect\n", childPid);
                }
            } else {
                afisareEroare("Eroare la fork");
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        afisareEroare("Usage: ./program <dir_input> <dir_output>");
    }

    const char *inputDir = argv[1];
    const char *outputDir = argv[2];

    processDirectory(inputDir, outputDir);

    return EXIT_SUCCESS;
}
