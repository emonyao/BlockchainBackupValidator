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
![image](https://github.com/user-attachments/assets/f1becc97-d02e-48c9-805c-d4ec6300f93d)
![image](https://github.com/user-attachments/assets/a7fe20da-fd0b-4735-b641-0e9620f3c64f)
![image](https://github.com/user-attachments/assets/152fc5ef-1ae6-4160-94e2-ad4bb1db363f)
![image](https://github.com/user-attachments/assets/e0d6bb52-fd75-4834-8ef8-5343d62b9a1e)



