# To-Do List:
    
### Sockets
- [ ] Portar send() e receive() para enviar dados com Template.
- [x] Verificar se há alternativas melhores para o retorno de TCPSocket::accept()
- [ ] Modificar exceções lançadas por: BaseSocket::bind(), TCPSocket::connect(), TCPSocket::listen()

### HTTP
- [ ] Melhorar gerenciamento de erros em Server::handle()
- [ ] Ao implementar Server::stop() usar Mutex na condição de parada do loop de Server::accept()