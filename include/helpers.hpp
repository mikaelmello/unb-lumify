#ifndef HELPERS_H
#define HELPERS_H

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                  INCLUDES E DEFINES
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string>
#include <vector>

/// Estrutura de um Peer
struct Peer {
    Peer(uint16_t id, std::string name, std::string host) :
        id(id), name(name), host(host) {}
    Peer() : id(0) {}

    /// ID do peer especificamente neste nó da rede.
    uint16_t id;

    /// Nome do peer
    std::string name;

    /// Endereço IP do peer.
    std::string host;
};

namespace Helpers {
    
// Divide uma string em um vetor de strings de acordo um caractere delimitador
// Criado por Evan Teran
// Disponível em: https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string
template<typename Out>
void split(const std::string &s, char delim, Out result);

// Divide uma string em um vetor de strings de acordo um caractere delimitador
// Criado por Evan Teran
// Disponível em: https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string
std::vector<std::string> split(const std::string &s, char delim);

std::vector<std::string> split(const std::string& text, const std::string& delims);

} // end namespace Helpers

#endif