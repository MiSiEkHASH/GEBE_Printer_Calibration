#include "lib/serialib.h"
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

byte parameter93_6[112] = {0x1E, 0x5B, 0x32, 0x3A, 0x33, 0x35, 0x2C, 0x31, 0x3A, 0x54, 0x5D, 0x1E, 0x5B, 0x33, 0x3A, 0x39, 0x33, 0x2C, 0x36, 0x3A, 0x36, 0x30, 0x30, 0x5D, 0x1E, 0x5B, 0x32, 0x3A, 0x33, 0x35, 0x2C, 0x31, 0x3A, 0x46, 0x5D, 0x1E, 0x5B, 0x31, 0x32, 0x3A, 0x39, 0x33, 0x2C, 0x36, 0x3A, 0x30, 0x5D, 0x1E, 0x5B, 0x31, 0x31, 0x3A, 0x39, 0x33, 0x2C, 0x36, 0x3A, 0x30, 0x5D};
byte parameter93_7[112] = {0x1E, 0x5B, 0x32, 0x3A, 0x33, 0x35, 0x2C, 0x31, 0x3A, 0x54, 0x5D, 0x1E, 0x5B, 0x33, 0x3A, 0x39, 0x33, 0x2C, 0x37, 0x3A, 0x36, 0x30, 0x30, 0x5D, 0x1E, 0x5B, 0x32, 0x3A, 0x33, 0x35, 0x2C, 0x31, 0x3A, 0x46, 0x5D, 0x1E, 0x5B, 0x31, 0x32, 0x3A, 0x39, 0x33, 0x2C, 0x37, 0x3A, 0x30, 0x5D, 0x1E, 0x5B, 0x31, 0x31, 0x3A, 0x39, 0x33, 0x2C, 0x37, 0x3A, 0x30, 0x5D};
byte parameter93_8[104] = {0x1E, 0x5B, 0x32, 0x3A, 0x33, 0x35, 0x2C, 0x31, 0x3A, 0x54, 0x5D, 0x1E, 0x5B, 0x33, 0x3A, 0x39, 0x33, 0x2C, 0x38, 0x3A, 0x36, 0x31, 0x5D, 0x1E, 0x5B, 0x32, 0x3A, 0x33, 0x35, 0x2C, 0x31, 0x3A, 0x46, 0x5D, 0x1E, 0x5B, 0x31, 0x32, 0x3A, 0x39, 0x33, 0x2C, 0x38, 0x3A, 0x30, 0x5D, 0x1E, 0x5B, 0x31, 0x31, 0x3A, 0x39, 0x33, 0x2C, 0x38, 0x3A, 0x30, 0x5D};
byte getStatus[3] = {0x1E, 0x6B, 0xFF};

byte received[3] = {0x00, 0x00, 0x00};


// u¿ycie obiekt serial portu z biblioteki portu szeregowego
    serialib serial;

void EXIT_APP () {
    cout << "\n\nWcisnij dowolny klawisz aby zakonczyc program...";
    getch();
    exit(0);
}

void WRITE_CALIBRATION () {
    cout << "\n*** WGRYWANIE PARAMETROW KALIBRACYJNYCH ***\n";
    //max predkosc silnika prezentera do przodu (600) [mm/s]
    serial.writeBytes(parameter93_6, sizeof(parameter93_6));
    Sleep(2000);
    //max predkosc silnika prezentera do tylu (600) [mm/s]
    serial.writeBytes(parameter93_7, sizeof(parameter93_7));
    Sleep(2000);
    //predkosc prezentera (61) [pps %]
    serial.writeBytes(parameter93_8, sizeof(parameter93_8));
    Sleep(2000);
}

void CHECK_STATUS () {
    //wysyla zapytanie o status do drukarki
    serial.writeBytes(getStatus, sizeof(getStatus));

    //odczyt odpowiedzi z drukarki
    serial.readBytes(received, 3, 2000, 1000);

    //interpretacja odpowiedzi z drukarki
    if (received[0] == 0x80 && received[1] == 0xCE) {
        cout << "\nNiski poziom papieru! Prosze uzupelnic!\n";
        EXIT_APP();
    }

    if (received[0] == 0x81 && received[1] == 0xCE) {
        cout << "\nPoziom papieru w normie!\n";
        WRITE_CALIBRATION ();
    }

    if (received[0] == 0x82 && received[1] == 0xCF) {
        cout << "\nBrak papieru w drukarce i na rolce! Prosze uzupelnic!\n";
        EXIT_APP();
    }

    if (received[0] == 0x83 && received[1] == 0xCF) {
        cout << "\nBrak papieru w drukarce, papier na rolce w normie! Prosze uzupelnic!\n";
        EXIT_APP();
    }

    if (received[0] == 0x00 && received[1] == 0x00) {
        cout << "\nBrak komunikacji z drukarka!\n";
        EXIT_APP();
    }
}

void CHECK_STATUS_AFTER_WRITE () {
    if (received[0] != 0x00 && received[1] != 0x00) {
        cout << "\n*** KALIBRACJA ZAKONCZONA POZYTYWNIE! ***\n";
    }
    else {
        cout << "\n*** KALIBRACJA ZAKONCZONA NEGATYWNIE! ***\n";
    }

    //zamkniecie portu szeregowego
    serial.closeDevice();
    EXIT_APP();
}

int main( /*int argc, char *argv[]*/) {
    //logo info
    cout << "*****************************************************\n";
    cout << "*** Kalibracja prezentera drukarki GEBE GPT-4673  ***\n";
    cout << "*** by Michal Swidzinski for FREEMATIC Sp. z o.o. ***\n";
    cout << "*****************************************************\n";

    cout << "Podaj port drukarki GeBE, COM: ";
    int a;
    //wczytuje do zmiennej "a" podany numer portu szeregowego
    scanf("%d", &a);
    //dopisanie \\\.\\COM do poprawnej pracy z portami wiêkszymi ni¿ 9
    char gebePort[256];
    sprintf(gebePort, "\\\\.\\COM%d", a);

    //podlaczenie do portu szeregowego
    char errorOpening = serial.openDevice(gebePort, 115200);

    //przy bledzie polaczenia, wywala blad
    if (errorOpening!=1) return errorOpening;
        printf ("Polaczono z portem: %s\n",gebePort);

    // wykonywane void'y
    CHECK_STATUS ();
    CHECK_STATUS_AFTER_WRITE ();
}
