int test_disk(){
    printf("----- Começo de código -----!\n");
    printf("Criando o disco...\n");

    if (create_disk() != 0){
        printf("Erro ao inicializar o disco!\n");
        return -1;
    }
    printf("Inicializando os blocos em branco do disco...\n");
    if (init_disk() != 0){
        printf("Erro ao gravar dados no disco!\n");
        return -1;
    }
    printf("Disco iniciado com sucesso!\n");

    if (open_disk() != 0){
        printf("O disco não conseguiu ser aberto!\n");
        return -1;
    };
    printf("Disco aberto!\n");

    char escrita[BLOCK_SIZE];
    memset(escrita, 0, BLOCK_SIZE);
    strcpy(escrita, "Trilha 0 - Iniciando o sistema de arquivos B)");

    if (write_block(0, escrita) != 0){
        printf("Erro!\n");
        return -1;
    } 
    printf("Escrita realizada no disco...\n");
    
    char leitura[BLOCK_SIZE];
    if(read_block(0, leitura) != 0){
        printf("Erro ao ler o bloco!\n");
        return -1;
    }
    printf("Leitura realizada com sucesso:\n");
    printf("%s\n", leitura);

    if (close_disk() != 0) {
        printf("Erro ao fechar o disco!\n");
        return -1;
    }
    printf("Disco fechado com sucesso!\n");

    printf("----- Fim de código -----!\n");
    return 0;
}

int test_bitmap(void){ //Em desuso
    if (reset_disk() != 0){
        perror("Erro ao resetar o disco!");
        return -1;
    }
    if (init_fat() != 0){
        perror("Erro ao inicializar o bitmap!\n");
        return -1;
    }
    return 0;
}

int test_set_block(void){
    int nfb = next_free_block();
    int value = -1;
    printf("Próximo bloco livre: %d\n", nfb);

    if(set_block(nfb, value) == 0){
        printf("Bloco %d definido como ", nfb);
        printf("%d com sucesso!\n", value);
    }
    
    return 0;
}

int test_dir_entry(){
    create_root_folder(); // Define o próximo bloco vazio como ocupado, que será referente ao root dir
    // Se chamado mais de uma vez, irá ocupar o próximo bloco vazio e o anterior ficará no esquecimento
    
    // touch Arquivo1.txt:
    if (new_dir_entry("Arquivo1", "txt", 1, -1) != 0){
        printf("Erro ao criar entrada de diretório!\n");
    }

    // touch Arquivo2.txt:
    if (new_dir_entry("Arquivo2", "txt", 1, -1) != 0){
        printf("Erro ao criar entrada de diretório!\n");
    }

    printf("Novas entradas de diretório criada com sucesso!\n");
    return 0;
}

int test_rw_file(){
    test_dir_entry();

    char file_data[BLOCK_SIZE];
    file_data[0] = 'O';
    file_data[1] = 'i';
    file_data[2] = '\0';

    if (write_file("Arquivo1", "txt", file_data) != 0){
        printf("Erro ao gravar no arquivo!\n");
        return -1;
    }

    printf("Arquivo gravado com sucesso!\n");
    
    if(read_file("Arquivo1", "txt", file_data) != 0){
        printf("Erro ao ler o arquivo.\n");
        return -1;
    }

    printf("Arquivo lido com sucesso:\n");

    printf("%s\n", file_data);
}