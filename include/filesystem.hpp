#ifndef FILESYSTEM_H
#define FILESYSTEM_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class FileSystem {
    public:
        FileSystem();

        Folder* get_current_path();

        Folder* create_folder(const std::string& full_path);

        Folder* update_folder(const std::string& full_path, const std::string& new_name);

        Folder* retrieve_folder(const std::String& full_path);

        void delete_folder(const std::string& full_path);

        File* create_file(const std::string& full_path, const std::string& author, uint32_t size, uint16_t owner_1, uint16_t owner_2 = 0);

        File* update_file(const std::string& full_path, const std::string& new_name, uint16_t new_owner_1, uint16_t new_owner_2);

        File* retrieve_file(const std::string& full_path);

        void delete_file(const std::string& full_path);

        /// Tamanho total do sistema
        uint64_t get_total_size();

        /// Numero de arquivos no sistema
        uint32_t get_files_no();

        /// Numero de pastas no sistema
        uint32_t get_folders_no();


    private:

        Folder root;
        Folder* current_path;
        uint32_t nextID;

}

class Folder {
    public:
        std::string name;
        std::map<std::string, Folder> subfolders;
        std::map<std::string, File> files;

        /// Tamanho total do diretorio
        uint64_t get_total_size();

        /// Numero de arquivos no diretorio
        uint32_t get_files_no();

        /// Numero de pastas no diretorio
        uint32_t get_folders_no();

        /// Preenche peer_files com os arquivos de cada peer da rede
        void fill_peer_files(std::map<uint16_t, std::set<uint32_t>>& peer_files);

        /// @return Conjunto com os IDs de todos os arquivos com apenas um dono.
        std::set<uint32_t> single_files();

        /// Remove os peers offline dos arquivos que era dono anteriormente
        void remove_offline_peers(const std::set<uint16_t> peers);
}

class File {
    public:
        std::string name;
        std::string author;
        uint32_t id;
        uint16_t owner_1;
        uint16_t owner_2;
        uint32_t size;
}

#endif