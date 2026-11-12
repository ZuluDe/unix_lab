#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>
#include <unistd.h>
#include <openssl/sha.h>

using namespace std;
using namespace std::filesystem;

string get_sha1(const string& file_path) {
    ifstream file(file_path, ios::binary);
    string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)content.data(), content.size(), hash);

    return string((char*)hash, SHA_DIGEST_LENGTH);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Укажите путь к каталогу\n";
        return 1;
    }

    map<string, string> hashes;

    for (const auto& entry : recursive_directory_iterator(argv[1])) {
        if (entry.is_symlink() || !entry.is_regular_file()) {
            continue;
        }

        string path = entry.path().string();
        string h = get_sha1(path);

        if (hashes.count(h) == 0) {
            hashes[h] = path;
        } else {
            unlink(path.c_str());
            link(hashes[h].c_str(), path.c_str());
            cout << "Заменён: " << path << " на " << hashes[h] << "\n";
        }
    }

    return 0;
}
