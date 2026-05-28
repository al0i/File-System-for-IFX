#include <stdio.h>
#include <stdlib.h>

#include "ifx_fs.h"

/*
 * Funcoes auxiliares para evitar o uso de <string.h>, 
 * conforme exigido pelo CODEX.md (Regras tecnicas fixas).
 */
static int str_len(const char *str)
{
    int len = 0;
    while (str && str[len] != '\0') {
        len++;
    }
    return len;
}

static int str_cmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

static void strip_newline(char *str)
{
    while (*str) {
        if (*str == '\n' || *str == '\r') {
            *str = '\0';
            break;
        }
        str++;
    }
}

static void split_command(char *input, char **cmd, char **arg1, char **arg2)
{
    char *p = input;

    *cmd = input;
    *arg1 = NULL;
    *arg2 = NULL;

    while (*p && *p != ' ') {
        p++;
    }

    if (*p == ' ') {
        *p = '\0';
        p++;
        while (*p == ' ') {
            p++;
        }
        if (*p) {
            *arg1 = p;
            char *q = p;
            while (*q && *q != ' ') {
                q++;
            }
            if (*q == ' ') {
                *q = '\0';
                q++;
                while (*q == ' ') {
                    q++;
                }
                if (*q) {
                    *arg2 = q;
                }
            }
        }
    }
}

int main()
{
    IFXFileSystem fs;
    char input[256];
    char *cmd, *arg1, *arg2;
    int fd, bytesRead;
    unsigned char buffer[512];

    if (ifx_init(&fs) != 0) {
        printf("Erro: nao foi possivel inicializar o sistema de arquivos.\n");
        return -1;
    }

    if (ifx_mount(&fs) != 0) {
        printf("Erro: falha ao montar ou formatar o disco virtual.\n");
        ifx_close(&fs);
        return -1;
    }

    printf("=== IFX Shell ===\n");
    printf("O disco virtual foi montado com sucesso.\n");
    printf("Comandos disponiveis:\n");
    printf("  create NOME.EXT\n");
    printf("  write NOME.EXT <texto_para_gravar>\n");
    printf("  read NOME.EXT\n");
    printf("  exit\n");

    while (1) {
        printf("\nifx> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        strip_newline(input);

        if (str_len(input) == 0) {
            continue;
        }

        split_command(input, &cmd, &arg1, &arg2);

        if (str_cmp(cmd, "exit") == 0) {
            break;
        } else if (str_cmp(cmd, "create") == 0) {
            if (arg1 == NULL) {
                printf("Uso: create NOME.EXT\n");
            } else {
                if (ifx_create_file(&fs, arg1) == 0) {
                    printf("Arquivo '%s' criado com sucesso.\n", arg1);
                } else {
                    printf("Erro ao criar o arquivo '%s' (pode ja existir ou nome invalido).\n", arg1);
                }
            }
        } else if (str_cmp(cmd, "write") == 0) {
            if (arg1 == NULL || arg2 == NULL) {
                printf("Uso: write NOME.EXT <texto>\n");
            } else {
                int len = str_len(arg2);
                if (ifx_write_file(&fs, arg1, (const unsigned char *)arg2, len) == 0) {
                    printf("Escritos %d bytes no arquivo '%s'.\n", len, arg1);
                } else {
                    printf("Erro ao escrever no arquivo '%s'.\n", arg1);
                }
            }
        } else if (str_cmp(cmd, "read") == 0) {
            if (arg1 == NULL) {
                printf("Uso: read NOME.EXT\n");
            } else {
                fd = ifx_open_file(&fs, arg1);
                if (fd < 0) {
                    printf("Erro ao abrir o arquivo '%s' (nao encontrado ou limite de fd atingido).\n", arg1);
                } else {
                    printf("Conteudo de '%s':\n", arg1);
                    while ((bytesRead = ifx_read_file(&fs, fd, buffer, sizeof(buffer) - 1)) > 0) {
                        buffer[bytesRead] = '\0';
                        printf("%s", buffer);
                    }
                    printf("\n");
                    ifx_close_file(&fs, fd);
                }
            }
        } else {
            printf("Comando desconhecido: '%s'\n", cmd);
        }
    }

    if (ifx_close(&fs) != 0) {
        printf("Erro ao fechar o sistema de arquivos.\n");
        return -1;
    }

    printf("Disco desmontado com seguranca. Ate mais!\n");
    return 0;
}
