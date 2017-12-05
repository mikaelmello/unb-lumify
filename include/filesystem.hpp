#ifndef FILESYSTEM_H
#define FILESYSTEM_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <map>
#include <set>
#include <atomic>

class File {
    public:
        File(const std::string& name, const std::string& author, uint32_t size, uint32_t id,
            std::string owner_1, std::string owner_2);
        File();
        std::string name;
        std::string author;
        uint32_t id;
        std::string owner_1;
        std::string owner_2;
        uint32_t size;

        void erase();

        std::string get_json();
};

class Folder {
    public:
        Folder(const std::string& name);
        Folder() {}
        std::string name;
        std::map<std::string, Folder> subfolders;
        std::map<std::string, File> files;

        /// Tamanho total do diretorio
        uint64_t get_total_size();

        /// Numero de arquivos no diretorio
        uint32_t get_files_no();

        /// Numero de pastas no diretorio
        uint32_t get_folders_no();

        void erase();

        std::string get_json();

        /// Preenche peer_files com os arquivos de cada peer da rede
        void fill_peer_files(std::map<uint16_t, std::set<uint32_t>>& peer_files);

        /// @return Conjunto com os IDs de todos os arquivos com apenas um dono.
        std::set<uint32_t> single_files();

        /// Remove os peers offline dos arquivos que era dono anteriormente
        void remove_offline_peers(const std::set<uint16_t> peers);
};

class FileSystem {
    public:
        FileSystem();

        Folder* create_folder(const std::string& full_path);

        Folder* update_folder(const std::string& full_path, const std::string& new_name);

        Folder* retrieve_folder(const std::string& full_path);

        void delete_folder(const std::string& full_path);

        File* create_file(const std::string& full_path, const std::string& author, uint32_t size, std::string owner_1, std::string owner_2 = 0);

        File* update_file(const std::string& full_path, const std::string& new_name, uint16_t new_owner_1, uint16_t new_owner_2);

        File* retrieve_file(const std::string& full_path);

        void delete_file(const std::string& full_path);

        void sync(const std::string& json);

        std::string get_json();

        std::string _get_json();

        void update_json();

        /// Tamanho total do sistema
        uint64_t get_total_size();

        /// Numero de arquivos no sistema
        uint32_t get_files_no();

        /// Numero de pastas no sistema
        uint32_t get_folders_no();

        Folder root;
        Folder* current_path;

        std::string json;


    private:
        std::atomic<uint32_t> nextID;

};

#endif