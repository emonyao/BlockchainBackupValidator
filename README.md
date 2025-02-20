fetch block backup data: cmd /C "set PATH=C:\msys64\mingw64\bin;%PATH% && fetch_backup.exe"
![image](https://github.com/user-attachments/assets/94cb8a96-e202-4f5b-8085-e127a609ac40)
![image](https://github.com/user-attachments/assets/0baac7b8-1f80-48ce-9abf-7679423e39e1)

hash validation: 
    g++ -o src/blockchain_validator src/main.cpp src/blockchain_validation.cpp -I include -lssl -lcrypto -lcurl -ljsoncpp
    cmd /C "set PATH=C:\msys64\mingw64\bin;%PATH% && src\hash_validator.exe"

block link validation:
    g++ -o src/blockchain_link_validator src/main.cpp src/block_link_validation.cpp -I include -ljsoncpp
    cmd /C "set PATH=C:\msys64\mingw64\bin;%PATH% && src\blockchain_link_validator.exe"

transaction validation:
    g++ -o src/transaction_validator src/main.cpp src/transaction_validation.cpp -I include -lcurl -ljsoncpp
    cmd /C "set PATH=C:\msys64\mingw64\bin;%PATH% && src\transaction_validator.exe"

main blockchain validation:
    g++ -o src/main_blockchain_validator src/main.cpp src/hash_validation.cpp src/block_link_validation.cpp src/transaction_validation.cpp src/http_utils.cpp -I include -lssl -lcrypto -lcurl -ljsoncpp
    cmd /C "set PATH=C:\msys64\mingw64\bin;%PATH% && src\main_blockchain_validator.exe"

