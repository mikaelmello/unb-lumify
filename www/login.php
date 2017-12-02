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
	<meta name="theme-color" content="#000">
	<link rel="stylesheet" type="text/css" href="style.css">
	<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">
	<script src="script.js"></script>
	<script>
	    var username = getCookie("nickname");
	    if (username) {
	        location.href = "/index.php";
	    }
	</script>
</head>
<style>
	html {
		background-color: #008080;
		text-align: center;
	}
</style>
<body>
	<div class="col-12 col-m-12">
		<a href="/index.php"><h1 class="logo"><i class="fa fa-code"></i>  Lumify</h1></a>
	</div>
	<div class="login col-4 col-m-12">
		<i class="fa fa-user-circle-o" style="color: #008080; font-size: 15em;"></i>
		<form action="javascript:login()" name="login">
			<h2 style="color: #000">Usuário:<br></h2>
			<input name="user" class="col-12 col-m-12" type="text" placeholder="Digite seu nome de usuário." autofocus>
			<input class="col-12 col-m-12" type="submit" value="Entrar"></input>
		</form>
	</div>
</body>
</html>