#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>
#include <locale.h>

char publicKey[] = "-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAg3rq1FZAPHwn8pH420K\n"\
"vo8RfKOUiGugaSgJSLNPozj2l4LN9RSI4wd/DbdzzQftj/OCaJAjxs27R4z6x+N\n"\
"mo+IB8iZwZuxu7JI+pE646FLTWeMthnEbRty6o4OLeouxP7zXdwOcdUajV59zO0\n"\
"c0IycE+ymP62Ut02rI6WjxiFWKpyf8H9hXv/kjsEG41/NuVBVPz8sOka/Z0uS4u\n"\
"9oErNLo0t9DG1UoZOrJ8+8jgMSEBmGmXZbnfNp2VM5Ocn3ikyKcHuIC6CYX4yOq\n"\
"cnpehRjVQEvJT6HUnjTghsuWb/t7VHsy59vpCx/aMbcW63QNzr3qBvxqUV2j80y\n"\
"XJ+xNqtQIDAQAB\n"\
"-----END PUBLIC KEY-----\n";

int padding = RSA_PKCS1_OAEP_PADDING;

RSA * createRSA(unsigned char * key, int public)
{
    RSA *rsa = NULL;
    BIO *keybio;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio == NULL)
    {
        printf("Failed to create key BIO");
        return 0;
    }
    if (public)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }
    if (rsa == NULL)
    {
        printf("Failed to create RSA");
    }

    return rsa;
}

int public_encrypt(unsigned char * data, int data_len, unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key, 1);
    int result = RSA_public_encrypt(data_len, data, encrypted, rsa, padding);
    return result;
}

void printLastError(char *msg)
{
    char * err = malloc(130);;
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n", msg, err);
    free(err);
}

void encryptRSA(char mensagem[], char criptografado[]) {
    setlocale(0, "");
    char resp[1000] = "!";
    char plainText[256]; //key length : 2048
    char converted[256];

    strcpy(plainText, mensagem);

    sprintf(converted, "!");

    for (int i = 0; i < strlen(plainText); i++) {
        char var[10];
        if (i != strlen(plainText) - 1) {
            sprintf(var, "%hhx:", plainText[i]);
        }
        else {
            sprintf(var, "%hhx", plainText[i]);
        }
        strcat(converted, var);
    }


    unsigned char encrypted[4098];

    int encrypted_length = public_encrypt(converted, strlen(converted), publicKey, encrypted);
    if (encrypted_length == -1)
    {
        printLastError("Public Encrypt failed ");
        exit(0);
    }
    for (int i = 0; i < 256; i++) {
        char esp[10];
        if (i != 255) {
            sprintf(esp, "%hhx:", encrypted[i]);
        }
        else {
            sprintf(esp, "%hhx", encrypted[i]);
        }
        strcat(resp, esp);
    }
    strcat(resp, "!");
    strcpy(criptografado, resp);
    return;
}

