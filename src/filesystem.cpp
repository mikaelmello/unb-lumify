#include "filesystem.hpp"
#include "helpers.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <chrono>


File::File(const std::string& name, const std::string& author, uint32_t size, uint32_t id,
    std::string owner_1, std::string owner_2) : name(name), author(author),
    size(size), owner_1(owner_1), owner_2(owner_2), id(id) {
}

File::File() : id(0) {}

std::string File::get_json() {
    std::string js = "{\"name\":\""   + this->name + "\","+
                      "\"author\":\"" + this->author + "\","+
                      "\"size\":\""   + std::to_string(this->size) + "\","+
                      "\"owner1\":\"" + this->owner_1 + "\","+
                      "\"owner2\":\"" + this->owner_2 + "\"}";

    return js;
}

void File::erase() {

}

Folder::Folder(const std::string& name) : name(name) {
}

std::string Folder::get_json() {
    std::string js = "{\"name\":\""   + this->name + "\","+
                      "\"folders_no\":\"" + std::to_string(this->get_folders_no()) + "\"," +
                      "\"files_no\":\""   + std::to_string(this->get_files_no()) + "\"," +
                      "\"size\":\"" + std::to_string(this->get_total_size()) + "\"," +
                      "\"files\":[";

    for (auto& file : files) {
        js += file.second.get_json();
        js += ",";
    }
    if (js.back() == ',') js.pop_back();
    js += "], \"subfolders\":[";
    for (auto& folder : subfolders) {
        js += folder.second.get_json();
        js += ",";
    }
    if (js.back() == ',') js.pop_back();
    js += "]}";
    return js;
}

uint64_t Folder::get_total_size() {
    uint64_t size = 0;
    for (auto& file : files) {
        size += file.second.size;
    }
    for (auto& folder : subfolders) {
        size += folder.second.get_total_size();
    }
    return size;
}

uint32_t Folder::get_files_no() {
    uint32_t no = 0;
    no += files.size();
    for (auto& folder : subfolders) {
        no += folder.second.get_files_no();
    }
    return no;
}

uint32_t Folder::get_folders_no() {
    uint32_t no = 0;
    no += subfolders.size();
    for (auto& folder : subfolders) {
        no += folder.second.get_folders_no();
    }
    return no;
}

void Folder::erase() {
    for (auto& file : files) {
        file.second.erase();
    }
    for (auto& folder : subfolders) {
        folder.second.erase();
    }    
}

FileSystem::FileSystem() : root("root"), current_path(&root), nextID(1) {
    update_json();
}

Folder* FileSystem::create_folder(const std::string& full_path) {
    std::vector<std::string> tokens = Helpers::split(full_path, '/');
    if (tokens[0] != "root") throw std::invalid_argument("full_path wrong");

    Folder* last = NULL;
    Folder* current = &root;
    int n = tokens.size() - 1;
    for (int i = 1; i < n; i++) {
        last = current;
        current = &current->subfolders[tokens[i]];
        if (current->name == "") {
            last->subfolders.erase(tokens[i]);
            throw std::invalid_argument("folder in the middle not found");
        }
    }

    if (current->subfolders[tokens[n]].name == "")
        current->subfolders[tokens[n]] = Folder(tokens[n]);

    update_json();
    return current;
}

Folder* FileSystem::retrieve_folder(const std::string& full_path) {
    std::vector<std::string> tokens = Helpers::split(full_path, '/');
    if (tokens[0] != "root") throw std::invalid_argument("full_path wrong");

    Folder* last = NULL;
    Folder* current = &root;
    int n = tokens.size();
    for (int i = 1; i < n; i++) {
        last = current;
        current = &current->subfolders[tokens[i]];
        if (current->name == "") {
            last->subfolders.erase(tokens[i]);
            throw std::invalid_argument("folder in the middle not found");
        }
    }

    return current;
}

Folder* FileSystem::update_folder(const std::string& full_path, const std::string& new_name) {
    std::vector<std::string> tokens = Helpers::split(full_path, '/');
    if (tokens[0] != "root") throw std::invalid_argument("INVALID_PATH");

    Folder* last = NULL;
    Folder* current = &root;
    int n = tokens.size();
    for (int i = 1; i < n; i++) {
        last = current;
        current = &last->subfolders[tokens[i]];
        if (current->name == "") {
            last->subfolders.erase(tokens[i]);
            throw std::invalid_argument("INVALID_PATH");
        }
    }
    current->name = new_name;
    last->subfolders[new_name] = *current;
    std::map<std::string,Folder>::iterator it = last->subfolders.find(tokens[n-1]);
    last->subfolders.erase(it);


    update_json();
    return current;
}

void FileSystem::delete_folder(const std::string& full_path) {
    std::vector<std::string> tokens = Helpers::split(full_path, '/');
    if (tokens[0] != "root") throw std::invalid_argument("INVALID_PATH");

    Folder* last = NULL;
    Folder* current = &root;
    int n = tokens.size();
    for (int i = 1; i < n; i++) {
        last = current;
        current = &last->subfolders[tokens[i]];
        if (current->name == "") {
            last->subfolders.erase(tokens[i]);
            throw std::invalid_argument("INVALID_PATH");
        }
    }
    current->erase();
    std::map<std::string,Folder>::iterator it = last->subfolders.find(tokens[n-1]);
    last->subfolders.erase(it);
    update_json();
}



File* FileSystem::create_file(const std::string& full_path, const std::string& author, 
        uint32_t size, std::string owner_1, std::string owner_2) {
    std::vector<std::string> tokens = Helpers::split(full_path, '/');
    if (tokens[0] != "root") throw std::invalid_argument("full_path wrong");


    std::string filename = full_path.substr(full_path.find_last_of("/") + 1);
    std::string folder_path = full_path.substr(0, full_path.find_last_of("/"));

    Folder* current = retrieve_folder(folder_path);
    if (current->files[filename].id == 0) {
        current->files[filename] = File(filename, author, size, nextID, owner_1, owner_2);
        system(("mv ./add-files-here/" + filename + " ./files/" + std::to_string(nextID++)).c_str());
    }


    update_json();
    return &current->files[filename];
}

void FileSystem::sync(const std::string& json) {

}

void FileSystem::update_json() {
    this->json = this->root.get_json();
}

std::string FileSystem::get_json() {
    return this->json;
}

uint64_t FileSystem::get_total_size() {
    return this->root.get_total_size();
}

uint32_t FileSystem::get_files_no() {
    return this->root.get_files_no();
}

uint32_t FileSystem::get_folders_no() {
    return this->root.get_folders_no();
}