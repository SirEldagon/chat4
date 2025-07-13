#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 12345;

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Регистрация или вход
    std::string login, password;
    std::cout << "Введите логин: ";
    std::cin >> login;
    std::cout << "Введите пароль: ";
    std::cin >> password;

    std::string auth_request = "LOGIN " + login + " " + password;
    send(sock, auth_request.c_str(), auth_request.size(), 0);

    // Получение истории сообщений
    char buffer[1024] = {0};
    read(sock, buffer, 1024);
    std::cout << "История сообщений:\n" << buffer << std::endl;

    // Отправка сообщений
    while (true) {
        std::string message;
        std::cout << "Введите сообщение: ";
        std::getline(std::cin, message);
        send(sock, ("MSG " + message).c_str(), message.size() + 4, 0);
    }

    close(sock);
    return 0;
}
