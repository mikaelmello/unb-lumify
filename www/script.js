var caminho;
var sequencia = [ 0 ];
var no = 0;
var parsed;

function setCookie(cname, cvalue) {
    document.cookie = cname + "=" + cvalue + ";";
}

function getCookie(cname) {
    var name = cname + "=";
    var decodedCookie = decodeURIComponent(document.cookie);
    var ca = decodedCookie.split(';');
    for(var i = 0; i <ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
}

function mostra(id) {
    var x = document.getElementById(id);
    if (x.className.indexOf("cntrl-2") == -1) {
        x.className += " cntrl-2";
    } else {
        x.className = x.className.replace(" cntrl-2", "");
    }
}

function update_span(id, content) {
    document.getElementById(id).textContent = content;
}

function configureBrowserRequest(xmlhttp){
    if (window.XMLHttpRequest) // code for IE7+, Firefox, Chrome, Opera, Safari
        xmlhttp = new XMLHttpRequest();
    else // code for IE6, IE5
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    return xmlhttp;
}

function arquivos() {
    var x = document.getElementById("pastas");

    if (x.className.indexOf("block") == -1) {
        x.className += " block";
        x.className = x.className.replace(" none", "");
    }

    var x = document.getElementById("usuarios");

    if (x.className.indexOf("none") == -1) {
        x.className += " none";
        x.className = x.className.replace(" block", "");
    }

    get_fs();
}

function login() {
    username = document.forms["login"]["user"].value;
    if (username != "") {
        setCookie("nickname", username);
        location.href = "/index.php";
    }
    else {
        alert("Insira um Usuário válido!");
    }
    update_nick();
}

function logout() {
    document.cookie = 'nickname=; Max-Age=0';
    location.href = "/login.php";
}

function update_nick() {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            var parsed = JSON.parse(server.responseText);
            if (parsed["error"] == "true") username = "";
        }
    }
    server.open("GET", "data.php?data=UPDATE_NICK&nickname=" + username, true);
    server.send();
}

function update() {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            var parsed = JSON.parse(server.responseText);
            update_span("usersqty", parsed["users_qty"]);
            update_span("current_subfolders", parsed["cur_folders_qty"]);
            update_span("current_files", parsed["cur_files_qty"]);
            update_span("current_size", parsed["cur_total_size"]);
            update_span("subfolders", parsed["total_folders_qty"]);
            update_span("files", parsed["total_files_qty"]);
            update_span("size", parsed["total_size"]);
        }
    }
    server.open("GET", "data.php?data=UPDATE_DATA", true);
    server.send();
    setInterval(function() {
        server.open("GET", "data.php?data=UPDATE_DATA", true);
        server.send();
    }, 5000);
}

function get_json() {
    var json = parsed;
    for(var i = 0; i < no; i++) {
        json = json.subfolders[sequencia[i]];
    }
    return json;
}

function navegar(funcao) {
    if(funcao > -1) {
        var parse = get_json();
        sequencia[no] = funcao;
        no++;
        preencher_tabela(parse.subfolders[funcao]);
    }
    else {
        if(no > 0) {
            no--;
        }
        var parse = get_json();
        preencher_tabela(parse);
    }
}

function preencher_tabela(parse) {
    caminho[no] = parse.name;

    caminho();
    delete_rows();

    if(parse.folders_no == 0) {
        var obj = document.getElementById("diretorios").insertRow(1);
        obj.insertCell(0).id = "diretorio_vazio";
        obj = document.getElementById("diretorio_vazio")
        obj.innerHTML = "Não há nada para mostrar!";
        obj.setAttribute("colspan", 5);
    }
    else {
        var obj = parse.subfolders;
        for(var i = obj.length - 1; i >= 0; i--) {
            add_diretorio(obj[i].name, obj[i].folders_no, obj[i].files_no, obj[i].size, "Subpasta", i);
        }
    }
    add_diretorio(parse.name, parse.folders_no, parse.files_no, parse.size, "Atual", -1);

    if(parse.files_no == 0) {
        var obj = document.getElementById("arquivos").insertRow(1);
        obj.insertCell(0).id = "arquivos_vazio";
        obj = document.getElementById("arquivos_vazio")
        obj.innerHTML = "Não há nada para mostrar!";
        obj.setAttribute("colspan", 5);
    }
    else {
        obj = parse.files;
        for(var i = obj.length - 1; i >= 0; i--) {
            add_arquivo(obj[i].name, obj[i].author, obj[i].size, obj[i].owner1, obj[i].owner2);
        }
    }
}

function caminho() {
    var str = "";
    for(var i = 0; i <= no; i++) {
        str += caminho[i] + "/";
    }
    document.getElementById("caminho").innerHTML = str;
}

function delete_rows() {
    var table = document.getElementById("diretorios");
    var rows = table.rows.length - 1;
    for (var i = 0; i < rows; i++) {
        table.deleteRow(1);
    }

    table = document.getElementById("arquivos");
    rows = table.rows.length - 1;
    for (var i = 0; i < rows; i++) {
        table.deleteRow(1);
    }
}

function add_diretorio(nome, pasta, arquivo, tamanho, tipo, funcao) {
    var row = document.getElementById("diretorios").insertRow(1);
    row.setAttribute("onClick", "navegar(" + funcao + ");");
    row.insertCell(0).innerHTML = nome;
    row.insertCell(1).innerHTML = pasta;
    row.insertCell(2).innerHTML = arquivo;
    row.insertCell(3).innerHTML = tamanho + " Bytes";
    row.insertCell(4).innerHTML = tipo;
}

function add_arquivo(nome, autor, tamanho, par1, par2) {
    var row = document.getElementById("arquivos").insertRow(1);
    row.insertCell(0).innerHTML = nome;
    row.insertCell(1).innerHTML = autor;
    row.insertCell(2).innerHTML = tamanho + " Bytes";
    row.insertCell(3).innerHTML = par1;
    row.insertCell(4).innerHTML = par2;
}

/*function get_fs() {
    var json = '{  "name":"root",  "folders_no": "2",  "files_no": "4",  "size": "2544",  "files": [    {      "name": "xd.png",      "author": "Fulaninho",      "size": "9213",      "owner1": "Fulaninho",      "owner2": "Joao"    },     {      "name": "abc.txt",      "author": "Fulaninho",      "size": "999",      "owner1": "Fulaninho",      "owner2": "Joao"    }  ],  "subfolders": [    {      "name": "test",      "folders_no": "0",      "files_no": "2",      "size": "1332",      "files": [        {          "name": "dsadd.png",          "author": "Joao",          "size": "9333",          "owner1": "Joao",          "owner2":"Fulaninho"        },         {          "name": "ffaio.txt",          "author": "Joao",          "size": "999",          "owner1": "Joao",          "owner2":"Fulaninho"        }      ],      "subfolders": [        {        }      ]    },    {      "name": "test2",      "folders_no": "0",      "files_no": "0",      "size": "0",      "files": [      ],      "subfolders": [      ]    }  ],  "error": "false"}';
    parsed = JSON.parse(json);
    preencher_tabela(parsed, 0);
}*/

function get_fs() {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            parsed = JSON.parse(server.responseText);
            no = 0;
            preencher_tabela(parsed);
            console.log(server.responseText);
        }
    }
    server.open("GET", "data.php?data=GET_FS", true);
    server.send();
}

function create_folder(fullpath) {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            var parsed = JSON.parse(server.responseText);
            console.log(server.responseText);
        }
    }
    server.open("GET", "data.php?data=CREATE_FOLDER&fullpath="+fullpath, true);
    server.send();    

}

function update_folder(fullpath, new_name) {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            var parsed = JSON.parse(server.responseText);
            console.log(server.responseText);
        }
    }
    server.open("GET", "data.php?data=UPDATE_FOLDER&fullpath="+fullpath+"&newname="+new_name, true);
    server.send();    

}
