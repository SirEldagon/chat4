#include <iostream>
#include <sqlite3.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <string>

const int PORT = 12345;

sqlite3* db;

void initDatabase() {
    sqlite3_open("chat.db", &db);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, login TEXT, password TEXT);", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY, sender TEXT, message TEXT, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);", nullptr, nullptr, nullptr);
}

bool registerUser(const std::string& login, const std::string& password) {
    std::string query = "INSERT INTO users (login, password) VALUES ('" + login + "', '" + password + "');";
    return sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) == SQLITE_OK;
}

bool authenticateUser(const std::string& login, const std::string& password) {
    std::string query = "SELECT COUNT(*) FROM users WHERE login='" + login + "' AND password='" + password + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count > 0;
}

void saveMessage(const std::string& sender, const std::string& message) {
    std::string query = "INSERT INTO messages (sender, message) VALUES ('" + sender + "', '" + message + "');";
    sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr);
}

std::vector<std::string> getMessages() {
    std::vector<std::string> messages;
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT sender, message, timestamp FROM messages ORDER BY id DESC LIMIT 10;", -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string sender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string message = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        messages.push_back(sender + ": " + message + " (" + timestamp + ")");
    }
    sqlite3_finalize(stmt);
    return messages;
}

int main() {
    initDatabase();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Server started, waiting for connections..." << std::endl;

    while (true) {
        int new_socket = accept(server_fd, nullptr, nullptr);
        // Обработка клиента (можно в отдельном потоке)
        // Для простоты — однопоточный пример
        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);
        std::string request(buffer);

        // Обработка команд (регистрация, вход, сообщение)
        // Пример: "REGISTER login password"
        // Или "LOGIN login password"
        // Или "MSG message"

        // Реализация логики пропущена для краткости
        // В реальности нужно реализовать парсинг и обработку команд

        close(new_socket);
    }

    sqlite3_close(db);
    return 0;
}
