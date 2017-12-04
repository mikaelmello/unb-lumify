#include "filesystem.hpp"
#include "helpers.hpp"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <thread>
#include <chrono>


File::File(const std::string& name, const std::string& author, uint32_t size, uint32_t id,
    uint16_t owner_1, uint16_t owner_2) : name(name), author(author),
    size(size), owner_1(owner_1), owner_2(owner_2), id(id) {
}

std::string File::get_json() {
    std::string js = "{\"name\":\""   + this->name + "\", \
                       \"author\":\"" + this->author + "\",\
                       \"size\":\""   + std::to_string(this->size) + "\",\
                       \"owner1\":\"" + std::to_string(this->owner_1) + "\",\
                       \"owner2\":\"" + std::to_string(this->owner_2) + "\"}";

    return js;
}

Folder::Folder(const std::string& name) : name(name) {
}

std::string Folder::get_json() {
    std::string js = "{\"name\":\""   + this->name + "\", \
                       \"folders_no\":\"" + std::to_string(this->get_folders_no()) + "\",\
                       \"files_no\":\""   + std::to_string(this->get_files_no()) + "\",\
                       \"size\":\"" + std::to_string(this->get_total_size()) + "\",\
                       \"files\": [";

    for (auto& file : files) {
        js += file.second.get_json();
        js += ",";
    }
    if (js.back() == ',') js.pop_back();
    js += "], \"subfolders\": [";
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

FileSystem::FileSystem() : root("root"), current_path(&root), nextID(1) {

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

    current->subfolders[tokens[n]] = Folder(tokens[n]);

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
        current = &current->subfolders[tokens[i]];
        if (current->name == "") {
            last->subfolders.erase(tokens[i]);
            throw std::invalid_argument("INVALID_PATH");
        }
    }

    current->name = new_name;

    return current;
}

std::string FileSystem::get_json() {
    return this->root.get_json();
}

uint64_t FileSystem::get_total_size() {
    return root.get_total_size();
}

uint32_t FileSystem::get_files_no() {
    return root.get_files_no();
}

uint32_t FileSystem::get_folders_no() {
    return root.get_folders_no();
}