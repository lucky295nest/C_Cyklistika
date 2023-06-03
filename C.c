#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define IN "cyklistika.txt"
#define OUT "vysledkova_listina.txt"
#define BUFSIZE 150
#define DELIMITERS ";:"

typedef struct CAS
{
    int hodiny;
    int minuty;
    float sekundy;
} CAS;

typedef struct ZAVODNIK
{
    int cislo;
    char prijmeni[20];
    char jmeno[20];
    char narodnost[5];
    int rocnik;
    char klub[35];
    struct CAS cas;
} ZAVODNIK;

int timeToSec(int hours, int minutes, int seconds)
{
    return hours * 3600 + minutes * 60 + seconds;
}

void secToTime(int seconds, char *buf)
{
    sprintf(buf, "%02d:%02d:%02d", seconds / 3600, (seconds - (seconds / 3600) * 3600) / 60, seconds % 60);
}

void swap(ZAVODNIK *x, ZAVODNIK *y)
{
    ZAVODNIK pom = *x;
    *x = *y;
    *y = pom;
}

void bblSort(ZAVODNIK *zav, int pocet)
{
    for (int i = 0; i < pocet; i++)
    {
        for (int j = 0; j < pocet - i - 1; j++)
        {
            if (timeToSec(zav[j].cas.hodiny, zav[j].cas.minuty, zav[j].cas.sekundy) > timeToSec(zav[j + 1].cas.hodiny, zav[j + 1].cas.minuty, zav[j + 1].cas.sekundy))
            {
                swap(&zav[j], &zav[j + 1]);
            }
        }
    }
}

int readfile(ZAVODNIK *zav)
{
    FILE *fr;
    char buffer[BUFSIZE];
    char *data;
    int pocet = 0;
    int sloupec;

    if ((fr = fopen(IN, "r")) == NULL)
    {
        printf("There was an error opening file %s.", IN);
        return EXIT_FAILURE;
    }

    while (fgets(buffer, BUFSIZE, fr) != NULL)
    {
        if (pocet != 0)
        {
            data = strtok(buffer, DELIMITERS);
            sloupec = 0;
            while (data != NULL)
            {
                sloupec++;
                switch (sloupec)
                {
                case 1:
                    zav[pocet - 1].cislo = atoi(data);
                    break;
                case 2:
                    strcpy(zav[pocet - 1].prijmeni, data);
                    break;
                case 3:
                    strcpy(zav[pocet - 1].jmeno, data);
                    break;
                case 4:
                    strcpy(zav[pocet - 1].narodnost, data);
                    break;
                case 5:
                    zav[pocet - 1].rocnik = atoi(data);
                    break;
                case 6:
                    strcpy(zav[pocet - 1].klub, data);
                    break;
                case 7:
                    zav[pocet - 1].cas.hodiny = atoi(data);
                    break;
                case 8:
                    zav[pocet - 1].cas.minuty = atoi(data);
                    break;
                case 9:
                    zav[pocet - 1].cas.sekundy = atof(data);
                    break;
                }
                data = strtok(NULL, DELIMITERS);
            }
        }
        pocet++;
    }
    if (fclose(fr) == EOF)
    {
        printf("There was an error closing file %s.", IN);
    }
    return (pocet - 1);
}

void writeToTerm(ZAVODNIK *zav, int pocet)
{
    int cesi;
    int slovaci;
    int nejstarsi = 3000;
    char nejstarsijmeno[25];

    time_t seconds = time(NULL);
    struct tm *current_time = localtime(&seconds);

    for (int i = 0; i < pocet; ++i)
    {
        if (nejstarsi > zav[i].rocnik)
        {
            nejstarsi = zav[i].rocnik;
            strcpy(nejstarsijmeno, zav[i].prijmeni);
        }

        if (!strcmp(zav[i].narodnost, "CZE"))
        {
            cesi++;
        }

        if (!strcmp(zav[i].narodnost, "SVK"))
        {
            slovaci++;
        }
    }

    printf("================================================================================================================\n");
    printf("======================================= S T A R T O V N Í  L I S T I N A =======================================\n");
    printf("================================================================================================================\n");
    printf(" STARTOVNI CISLO|      PRIJMENI|      JMENO|  NARODNOST|  ROCNIK|                             KLUB|         CAS|\n");
    printf("----------------------------------------------------------------------------------------------------------------\n");

    for (int j = 0; j < pocet; j++)
    {
        printf("%16d|%14s|%11s|%11s|%8d|%33s| %02d:%02d:%1.2f%s|\n",
               zav[j].cislo,
               zav[j].prijmeni,
               zav[j].jmeno,
               zav[j].narodnost,
               zav[j].rocnik,
               zav[j].klub,
               zav[j].cas.hodiny,
               zav[j].cas.minuty,
               zav[j].cas.sekundy,
               zav[j].cas.sekundy < 10 ? " " : "");
    }
    printf("================================================================================================================\n");
    printf("\nNa startu je %d zavodniku.\n", pocet);
    printf("Nejstarsim zavodnikem je %s narozen v roce %d. je mu %d let.\n", nejstarsijmeno, nejstarsi, (current_time->tm_year + 1900) - nejstarsi);
    printf("Zavodnici dle narodnosti:\n");
    printf("   Cesi - %d\nSlovaci - %d\n   Jine - %d\n\n", cesi, slovaci, pocet - (cesi + slovaci));
    printf("Byl vytvoren soubor %s\n", OUT);
}

void writeToFile(ZAVODNIK *zav, int pocet)
{
    char celkem[10];
    char ztrata[10];
    int prvset;
    FILE *fw;
    if ((fw = fopen(OUT, "w")) == NULL)
    {
        printf("FILE %s couldn't be opened.\n", OUT);
    }

    fprintf(fw, "================================================================================================================================\n");
    fprintf(fw, "============================================== V Y S L E D K O V A  L I S T I N A ==============================================\n");
    fprintf(fw, "================================================================================================================================\n");
    fprintf(fw, "PORADI|STARTOVNI CISLO|      PRIJMENI|      JMENO|  NARODNOST|  ROCNIK|                             KLUB|         CAS|   ZTRATA|\n");
    fprintf(fw, "--------------------------------------------------------------------------------------------------------------------------------\n");
    prvset = timeToSec(zav[0].cas.hodiny, zav[0].cas.minuty, zav[0].cas.sekundy);

    for (int i = 0; i < pocet; ++i)
    {
        secToTime(timeToSec(zav[i].cas.hodiny + zav[i].cas.hodiny, zav[i].cas.minuty + zav[i].cas.minuty, zav[i].cas.sekundy + zav[i].cas.sekundy) - prvset, ztrata);
        fprintf(fw, "%6d|%15d|%14s|%11s|%11s|%8d|%33s| %02d:%02d:%0.2f%s|%s%s|\n",
                i + 1,
                zav[i].cislo,
                zav[i].prijmeni,
                zav[i].jmeno,
                zav[i].narodnost,
                zav[i].rocnik,
                zav[i].klub,
                zav[i].cas.hodiny,
                zav[i].cas.minuty,
                zav[i].cas.sekundy,
                zav[i].cas.sekundy > 10 ? "" : " ",
                i == 0 ? " " : "+",
                i == 0 ? "        " : ztrata);
    }

    if (fclose(fw) == EOF)
    {
        printf("FILE %s couldn't be closed.\n", OUT);
    }
}

int main()
{
    ZAVODNIK zav[BUFSIZE];

    int pocet = 0;

    pocet = readfile(zav);
    writeToTerm(zav, pocet);
    bblSort(zav, pocet);
    writeToFile(zav, pocet);

    return EXIT_SUCCESS;
}