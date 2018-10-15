#include <stdio.h>
#include <string.h>
#include <stdlib.h>

unsigned long long elevate(char pass, int times) {
    unsigned long long int anw = (unsigned long long int) pass;
    for (int i = 0; i < times-1; i++) {
        anw = anw * pass;
    }
    return anw;
}

char* encriptRSA(char password[]) {
    char resp[1000];
    strcpy(resp, "!");
    unsigned long long int *criptword = malloc(128 * sizeof(unsigned long long int));
    int e = 5;
    unsigned long long int n = 32193636006586997;
    char *ovw = malloc(25 * sizeof(char));
    for (int i = 0; i < strlen(password); i++) {
     //   printf("%c - %d : ", password[i], password[i]);
        criptword[i] = elevate(password[i], e) % n;
        if (i == strlen(password) - 1) {
            sprintf(ovw, "%llu", criptword[i]);
        }
        else {
            sprintf(ovw, "%llu:", criptword[i]);
        }

        strcat(resp, ovw);
    }
    strcat(resp, "!");
    free(criptword);
    free(ovw);
    return resp;
}