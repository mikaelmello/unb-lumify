# Protocolo

Protocolo usado para a comunicação entre peers.

---

## Identificação dos Pares

Para descobrir outros pares na rede, todos os pares fazem uma tranmissão broadcast periódica de um datagrama (UDP) contendo a string "DISCOVER". Esta transmissão é realizada na porta 44599 e tem um intervalo de 30 segundos, logo após ela ser feita, respostas são aguardadas por 5 segundos.

Todos os pares também estão constantemente ouvindo na porta 44599 esperando por mensagens DISCOVER, eles então respondem diretamente para o remetente na porta 44600 com um datagrama (UDP) contendo uma string no formato "FOUND:<nickname>", onde <nickname> é substituído pelo nome de usuário escolhido pelo par ao navegar na sua própria interface web.

Quando uma resposta é recebida, se ela estiver no formato correto, o par irá adicionar o novo par descoberto à sua lista de pares conhecidos e responder com uma mensagem chamada "SUCCESS", se falhar, responderá com "FAIL:<reason>", onde <reason> é o motivo da falha.

Entre os motivos para falha, pode ocorrer "NICK", onde um par tenta utilizar um nome de usuário já utilizado por outro par na rede em algum momento anterior da execução do programa.