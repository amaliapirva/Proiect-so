#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        afisareEroare("Usage: ./program <fisier_intrare>");
    }

    char *numeFisier = argv[1];
 
    int fisier = open(numeFisier, O_RDONLY);
    if (fisier == -1) {
        afisareEroare("Eroare la deschiderea fisierului");
    }
 
    BMPHeader header;
    if (read(fisier, &header, sizeof(BMPHeader)) == -1) {
        afisareEroare("Eroare la citirea din fisier");
    }

    struct stat statFisier;
    if (fstat(fisier, &statFisier) == -1) {
        afisareEroare("Eroare la obtinerea informatiilor despre fisier");
    }

    struct tm *timeInfo;
    time_t timpModificare = statFisier.st_mtime;
    timeInfo = localtime(&timpModificare);
    char dataModificare[20];
    strftime(dataModificare, sizeof(dataModificare), "%d.%m.%Y", timeInfo);

    char continut[300]; 
    sprintf(continut, "nume fisier: %s\ninaltime: %d\nlungime: %d\ndimensiune: %ld\nidentificatorul utilizatorului: %d\ntimpul ultimei modificari: %s\ncontorul de legaturi: %ld\ndrepturi de acces user: %c%c%c\ndrepturi de acces grup: %c%c%c\ndrepturi de acces altii: %c%c%c\n",
            numeFisier,
            header.height, header.width,
            statFisier.st_size,
            statFisier.st_uid,
            dataModificare,
            statFisier.st_nlink,
            (statFisier.st_mode & S_IRUSR) ? 'R' : '-',
            (statFisier.st_mode & S_IWUSR) ? 'W' : '-',
            (statFisier.st_mode & S_IXUSR) ? 'X' : '-',
            (statFisier.st_mode & S_IRGRP) ? 'R' : '-',
            (statFisier.st_mode & S_IWGRP) ? 'W' : '-',
            (statFisier.st_mode & S_IXGRP) ? 'X' : '-',
            (statFisier.st_mode & S_IROTH) ? 'R' : '-',
            (statFisier.st_mode & S_IWOTH) ? 'W' : '-',
            (statFisier.st_mode & S_IXOTH) ? 'X' : '-');

    int fisierStatistica = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fisierStatistica == -1) {
        afisareEroare("Eroare la deschiderea fisierului statistica.txt");
    }

    if (write(fisierStatistica, continut, strlen(continut)) == -1) {
        afisareEroare("Eroare la scrierea in fisierul statistica.txt");
    }

    close(fisier);
    close(fisierStatistica);

    return 0;
}
