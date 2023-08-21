#pragma warning(disable:4996)
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <filesystem>
#include <algorithm>

DWORD fs;
std::vector<char> file_data;

LPCWSTR stoLPCWSTR(const std::string& s) {
    std::wstring stemp = std::wstring(s.begin(), s.end());
    LPCWSTR sw = stemp.c_str();
    return sw;
}

std::string convertToAbsolutePath(const std::string& inputPath) {
    std::string resultPath = inputPath;
    std::replace(resultPath.begin(), resultPath.end(), '\\', '/');

    if (resultPath.size() >= 2 && (resultPath.substr(0, 2) == "./" || resultPath.substr(0, 2) == ".\\")) {
        resultPath.erase(0, 2);
    }

    return resultPath;
}

std::vector<char> generateRandomKey(int length) {
    std::vector<char> key;
    key.reserve(length);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 255);

    for (int i = 0; i < length; ++i) {
        key.push_back(static_cast<char>(distr(gen)));
    }

    return key;
}

void printline() {
    for (int i = 0; i < 40; i++) std::cout << "=";
    std::cout << "\n";
}

bool READ(std::string inputPath, std::string outputPath) {
    DWORD bt;

    CopyFileW(stoLPCWSTR("stub.exe"), stoLPCWSTR(outputPath), FALSE);
    std::cout << "Getting the HANDLE of the file to be crypted\n";
    HANDLE efile = CreateFileA(inputPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (efile == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening file: " << inputPath;
        return 0;
    }
    std::cout << "Getting the File size\n";
    fs = GetFileSize(efile, NULL);
    std::cout << "The File Size is: " << fs << " Bytes\n";
    std::cout << "Allocating Memory for the ReadFile function\n";
    file_data.resize(fs);
    std::cout << "Reading the file\n";
    ReadFile(efile, (LPVOID)(file_data.data()), fs, &bt, NULL);

    CloseHandle(efile);

    if (fs != bt) {
        std::cerr << "Error reading file!";
        return 0;
    }
    return 1;
}

void xor_crypt(const std::vector<char>& key, std::vector<char>& data) {
    for (size_t i = 0; i != data.size(); i++)
        data[i] ^= key[i % key.size()];
}

void enc(std::vector<char> key)
{
    std::cout << "Encrypting the Data\n";
    xor_crypt(key, file_data);
}

void WriteToResources(LPCWSTR szTargetPE, int id, LPBYTE lpBytes, DWORD dwSize)
{
    std::cout << "Writing Encrypted data to stub's resources\n";
    HANDLE hResource = NULL;
    hResource = hResource = BeginUpdateResourceW(szTargetPE, FALSE);
    UpdateResource(hResource, RT_RCDATA, MAKEINTRESOURCE(id), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPVOID)lpBytes, dwSize);
    EndUpdateResource(hResource, FALSE);
}

void WriteKeyToResources(LPCWSTR szTargetPE, int id, const std::vector<char>& keyData) {
    std::cout << "Writing Key data to stub's resources\n";
    HANDLE hResource = BeginUpdateResourceW(szTargetPE, FALSE);
    UpdateResource(hResource, RT_RCDATA, MAKEINTRESOURCE(id), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPVOID)(keyData.data()), keyData.size());
    EndUpdateResource(hResource, FALSE);
}

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        std::cout << "Example: xor.exe <input_path> -o <output_path> -k <key>";
        return 0;
    }

    std::string inputPath = convertToAbsolutePath(argv[1]);
    std::string outputPath = "Encrypted_" + inputPath;
    std::vector<char> key = generateRandomKey(20);

    if (not std::filesystem::exists(inputPath)) return std::cerr << "Input Path " << "\"" << inputPath << "\"" << " does not exists !", 0;
    printline();
    std::cout << "Input Path    : " << inputPath << "\n";
    std::cout << "Output Path   : " << outputPath << "\n";
    std::cout << "Encryption key: "; for (char c : key) std::cout << c; std::cout << "\n";
    printline();
    if (not READ(inputPath, outputPath)) return 0;
    printline();
    enc(key);
    WriteKeyToResources(stoLPCWSTR(outputPath), 20, key);
    WriteToResources(stoLPCWSTR(outputPath), 10, (BYTE *)file_data.data(), file_data.size());
    printline();
    std::cout << "Finished\n";
    std::cout << "Encrypted file size: " << std::filesystem::file_size(outputPath) << " bytes";
    return 0;
}