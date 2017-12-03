#include "filesystem.hpp"
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

std::string FileSystem::get_json() {
    return this->root.get_json();
}