clear

mkdir -p build

# echo "Compilando..."
gcc src/main.c src/blocos_virtuais.c src/controle_arquivo_id.c -o build/main

if [ $? -eq 0 ]; then
    ./build/main
else
    echo "Erro de compilação."
    exit 1
fi

echo "" # Só para garantir logs com quebra de linha

# Visualização para blocos criados
# hexdump -C ./src/resources/virtual_disk.bin
