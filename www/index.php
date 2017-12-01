<!DOCTYPE html>
<html>
<head>
	<title>Lumify</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<meta http-equiv="Content-type" content="text/html; charset=utf-8" />
	<link rel="stylesheet" type="text/css" href="style.css">
	<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
	<script src="script.js"></script>
</head>
<body>
	<header class="col-12 col-m-12">
		<h1 class="logo"><i class="fa fa-code"></i>  Lumify</h1>
		<nav class="menu">
			<ul>
				<li><i class="fa fa-user-circle-o"></i> Usuário</li>
				<li><i class="fa fa-folder"></i> Pastas</li>
				<li><i class="fa fa-power-off"></i> Sair</li>
			</ul>
		</nav>
	</header>
	<div class="col-12 col-m-12 conteudo">
		<div class="col-3 col-m-12 coluna">
			<div class="col-12 col-m-6 info">
				<h2>Usuário<a onClick="mostra('usuario')" href="javascript:void(0)"><i class="fa fa-angle-right direita"></a></i></h2>
				<ul id="usuario" class="cntrl-1">
					<li>Nome: <span id="nickname"></span></li>
					<?php $sock = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
						  socket_connect($sock, "8.8.8.8", 53);
						  socket_getsockname($sock, $name); // $name passed by reference

     					  $localAddr = $name; ?>
					<li>IP: <?php echo $localAddr; ?> <span id="ip"></span></li>
					<li>Status: Online</li>
				</ul>
			</div>
			<div class="col-12 col-m-6 info pasta">
				<h2>Pasta<a onClick="mostra('pasta')" href="javascript:void(0)"><i class="fa fa-angle-right direita"></i></a></h2>
				<ul id="pasta" class="cntrl-1">
					<li><i class="fa fa-folder-open"></i> Subpastas: -</li>
					<li><i class="fa fa-file"></i> Arquivos: -</li>
					<li><i class="fa fa-database"></i> - bytes</li>
				</ul>
			</div>
			<div class="col-12 col-m-12 info">
				<h2>Sistema<a onClick="mostra('sistema')" href="javascript:void(0)"><i class="fa fa-angle-right direita"></i></a></h2>
				<ul id="sistema" class="cntrl-1">
					<li><i class="fa fa-user"></i> Usuários: <span id="usersqty"></span></li>
					<li><i class="fa fa-folder-open"></i> Pastas: -</li>
					<li><i class="fa fa-file"></i> Arquivos: -</li>
					<li><i class="fa fa-database"></i> - bytes</li>
				</ul>
			</div>
		</div>
		<div class="col-9 col-m-12 conteudo" id="lista" style="color:#000"></div>
	</div>
</body>
</html>