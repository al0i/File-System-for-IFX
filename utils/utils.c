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