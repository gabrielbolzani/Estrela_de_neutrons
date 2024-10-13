# Estrela_de_neutrons
Maquina de cura para peças feitas em impressora 3d de resina

Em Algum Momento Vou escrever mais sobre esse projeto, Mas a ideia central aqui era desenvolver uma maquina de cura UV Para peças de resina fotossensivel.

O primeiro protótipo foi dentro de um forno microondas. Infelizmente, depois de horas e horas desmontando o forno microondas. pintando o seu interior com tinta cromada reflexiva e tentando encaixar as coisas. Descobri que a placa principal estava com defeito. Com isso, juntei todas as peças e minha diginidade e atirei direto no lixo.

O Segundo prototipo é melhor do que parece cá entre nós, e inclusive é o que eu uso até hoje. Ela é feita dentro de um baú de madeira na vertical. Com um led Cegante UV de 50 fucking watts, um motor que roda a base para ter uma distribuição por igual da incidencia de luz UV ( Fiz com o motor do prato do microondas). Alguns dissipadores e o circuito que controla isso tudo.

Tudo parece meio frankstain mas funciona melhor do que parece.

O projeto usa um arduino micro, e um LCD 16x2 que veio de uma impressora velha a alguns anos. O processo consiste em definir um tempo no encoder, que soma de 1 em 1 minuto e mostra no display quanto tempo levara o processo. Ao apertar o encoder, o preocesso é iniciado.
Caso a porta seja aberta, o processo pausa e segura o cronometro naquele exato momento, voltando ao fechar a porta novamente. Quando o processo finaliza, ele exibi quanto tempo a peça ficou em cura.

Estou usando des de o mês de julho de 2024 e até agora nenhum problema. 

Tenho algumas ideias para icrementar como o uso de uma fonte menos suspeita para melhorar a confiabilidade do circuito. Colocar um knob no encoder, Incluir um buzzer para avisar quando a cura está concluída.
