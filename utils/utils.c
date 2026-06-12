int reset_block(void *buffer){
    memset(buffer, 0, BLOCK_SIZE);
    return 0;
}

int reset_disk(){
    if (create_disk() || init_disk() != 0){
        return -1;
    }
    return 0;
}

int read_external_file(void *buffer){
    int fd = open("./utils/dom_casmurro.txt", O_RDONLY);
    read(fd, buffer, BLOCK_SIZE);
    close(fd);

    return 0;
}

int record_external_file(){
    int fd = open("./utils/dom_casmurro.txt", O_RDONLY);
    char buffer[BLOCK_SIZE];
    create_root_folder();
    new_dir_entry("casmurro", "txt", 1, -1);
    int fb = get_first_block("casmurro", "txt");
    
    int file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    //read(fd, buffer, BLOCK_SIZE);
    //write_file("casmurro", "txt", buffer);

    for (int i=0;i<(file_size/BLOCK_SIZE)+1;i++){
        
        read(fd, buffer, BLOCK_SIZE);
        int nfb = next_free_block();
        set_block(fb, nfb); // Essa lógica está errada!
        set_block(nfb, -1);
        open_disk();
        write_block(nfb, buffer);
        lseek(fd, BLOCK_SIZE, SEEK_CUR);
    }

    close_disk();
    return 0;
}