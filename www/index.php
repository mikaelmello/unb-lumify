<!DOCTYPE html>
<html>
<head>
	<title>Lumify</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<meta charset="UTF-8">
	<link rel="apple-touch-icon" sizes="57x57" href="favicon/apple-icon-57x57.png">
	<link rel="apple-touch-icon" sizes="60x60" href="favicon/apple-icon-60x60.png">
	<link rel="apple-touch-icon" sizes="72x72" href="favicon/apple-icon-72x72.png">
	<link rel="apple-touch-icon" sizes="76x76" href="favicon/apple-icon-76x76.png">
	<link rel="apple-touch-icon" sizes="114x114" href="favicon/apple-icon-114x114.png">
	<link rel="apple-touch-icon" sizes="120x120" href="favicon/apple-icon-120x120.png">
	<link rel="apple-touch-icon" sizes="144x144" href="favicon/apple-icon-144x144.png">
	<link rel="apple-touch-icon" sizes="152x152" href="favicon/apple-icon-152x152.png">
	<link rel="apple-touch-icon" sizes="180x180" href="favicon/apple-icon-180x180.png">
	<link rel="icon" type="image/png" sizes="192x192"  href="favicon/android-icon-192x192.png">
	<link rel="icon" type="image/png" sizes="32x32" href="favicon/favicon-32x32.png">
	<link rel="icon" type="image/png" sizes="96x96" href="favicon/favicon-96x96.png">
	<link rel="icon" type="image/png" sizes="16x16" href="favicon/favicon-16x16.png">
	<link rel="manifest" href="favicon/manifest.json">
	<meta name="msapplication-TileColor" content="#ffffff">
	<meta name="msapplication-TileImage" content="/ms-icon-144x144.png">
	<meta name="theme-color" content="#007070">
	<link rel="stylesheet" type="text/css" href="style.css">
	<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
	<script src="script.js"></script></head>
	<script>
	    var username = getCookie("nickname");
	    if (!username) {
	        location.href = "/login.php";
	    }
	    window.onload = function() {
	    	update();
	    	update_span("nickname", username);
	    	update_nick();
	    	arquivos();
	    }
	</script>
<body>
	<header class="col-12 col-m-12">
		<a href="\index.php"><h1 class="logo"><i class="fa fa-code"></i>  Lumify</h1></a>
		<nav class="menu">
			<ul>
				<li><i class="fa fa-user-circle-o"></i> Usuário</li>
				<a onClick="arquivos()" href="javascript:void(0)"><li><i class="fa fa-folder"></i> Pastas</li></a>
				<a onClick="logout()" href="javascript:void(0)"><li><i class="fa fa-power-off"></i> Sair</li></a>
			</ul>
		</nav>
	</header>
	<div class="col-12 col-m-12 conteudo">
		<div class="col-3 col-m-12 coluna">
			<div class="col-12 col-m-6 info">
				<h2>Usuário<a onClick="mostra('usuario')" href="javascript:void(0)"><i class="fa fa-angle-right direita"></a></i></h2>
				<ul id="usuario" class="cntrl-1">
					<li>Nome: <span id="nickname"></span></li>
					<?php
						$sock = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
						socket_connect($sock, "8.8.8.8", 53);
						socket_getsockname($sock, $name); // $name passed by reference
						
						$localAddr = $name; 
					?>
					<li>IP: <?php echo $localAddr; ?> <span id="ip"></span></li>
					<li>Status: Online</li>
				</ul>
			</div>
			<div class="col-12 col-m-6 info pasta">
				<h2>Pasta<a onClick="mostra('pasta')" href="javascript:void(0)"><i class="fa fa-angle-right direita"></i></a></h2>
				<ul id="pasta" class="cntrl-1">
					<li><i class="fa fa-folder-open"></i> Subpastas: <span id="current_subfolders"></span></li>
					<li><i class="fa fa-file"></i> Arquivos: <span id="current_files"></span></li>
					<li><i class="fa fa-database"></i> <span id="current_size"></span> bytes</li>
				</ul>
			</div>
			<div class="col-12 col-m-12 info">
				<h2>Sistema<a onClick="mostra('sistema')" href="javascript:void(0)"><i class="fa fa-angle-right direita"></i></a></h2>
				<ul id="sistema" class="cntrl-1">
					<li><i class="fa fa-user"></i> Usuários: <span id="usersqty"></span></li>
					<li><i class="fa fa-folder-open"></i> Pastas: <span id="subfolders"></span></li>
					<li><i class="fa fa-file"></i> Arquivos: <span id="files"></span></li>
					<li><i class="fa fa-database"></i> <span id="size"></span> bytes</li>
				</ul>
			</div>
		</div>
		<div class="col-9 col-m-12 conteudo none" id="pastas" style="color:#000; padding-right: 0px;">
			<div class="tabelas none" id="aviso" style="background-color: #8b0000"><h2>Atenção!</h2>Menos de 3 usuários na rede.</div>
			<div class="tabelas">
				<div class="tabelas"><p>Caminho: <span id="caminho"></span></p></div>
				<div class="tabelas"><h2>Diretórios</h2></div>
				<table id="diretorios">
					<caption class="funcoes">
						<a href="javascript:void(0)" onClick="navegar(-1)" class="btnFuncoes"><i class="fa fa-mail-reply"></i> Voltar </a>
						<a href="javascript:void(0)" onClick="add_folder()" class="btnFuncoes"><i class="fa fa-plus-circle"></i> Adicionar Pasta</a>
					</caption>
					<tr>
						<th>Nome</th>
						<th>Pastas</th>
						<th>Arquivos</th>
						<th>Tamanho</th>
						<th>Tipo</th>
						<th><i class="fa fa-mail-forward"></i></th>
						<th><i class="fa fa-edit"></i></th>
						<th><i class="fa fa-times-circle"></i></th>
					</tr>
				</table>
				<div class="tabelas"><h2>Arquivos</h2></div>
				<table id="arquivos">
					<caption class="funcoes">
						<a href="javascript:void(0)" onClick="add_file()" class="btnFuncoes"><i class="fa fa-plus-circle"></i> Adicionar Arquivo - Primeiro transfira o arquivo para a pasta add-files-here e escolha lá</a>
						<form action="data.php" method="get" target="_blank" class="none" id="file_submit">
							<input type="file" name="file">
							<input type="submit" name="enviar">
							<input type="hidden" name="data" value="CREATE_FILE">
							<input type="hidden" name="fullpath" id="fullpathlol" value="" />
						</form>
					</caption>
					<tr>
						<th>Nome</th>
						<th>Autor</th>
						<th>Tamanho</th>
						<th>Par 1</th>
						<th>Par 2</th>
						<th><i class="fa fa-download"></i></th>
						<th><i class="fa fa-edit"></i></th>
						<th><i class="fa fa-times-circle"></i></th>
					</tr>
				</table>
			</div>
		</div>
		<div class="col-9 col-m-12 conteudo none" id="usuarios" style="color:#000; padding-right: 0px;">
			<p>Caminho: <span id="caminho"></span></p>
			<table id="users">
				<caption>asdf</caption>
				<tr>
					<th>Nome</th>
					<th>Pastas</th>
					<th>Arquivos</th>
					<th>Tamanho</th>
					<th>Tipo</th>
				</tr>
			</table>
		</div>
	</div>
</body>
</html>