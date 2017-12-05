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

function aviso(users_qty) {
    var x = document.getElementById("aviso");
    if(users_qty < 3) {
        if (x.className.indexOf("block") == -1) {
            x.className += " block";
            x.className = x.className.replace(" none", "");
        }
    }
    else {
        if (x.className.indexOf("none") == -1) {
            x.className += " none";
            x.className = x.className.replace(" block", "");
        }
    }
}

function bytes(tamanho) {
    if(tamanho == 0) return "0 B";
    var i = 0;
    var str;
    var aux = tamanho;
    while((aux / 1024) > 1) {
        aux /= 1024;
        i++;
    }
    aux = aux.toFixed(1);
    switch (i) {
        case 0:
            return aux + " B";
            break;
        case 1:
            return aux + " KB";
            break;
        case 2:
            return aux + " MB";
            break;
        case 3:
            return aux + " GB";
            break;
        case 4:
            return aux + " TB";
            break;
        default:
            return tamanho.toFixed(1) + " B";
    }
}

function update() {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            var parsed = JSON.parse(server.responseText);
            update_span("usersqty", parsed["users_qty"]);
            update_span("subfolders", parsed["total_folders_qty"]);
            update_span("files", parsed["total_files_qty"]);
            update_span("size", bytes(parsed["total_size"]));
            aviso(parsed["users_qty"]);
        }
    }
    server.open("GET", "data.php?data=UPDATE_DATA", true);
    server.send();
    setInterval(function() {
        server.open("GET", "data.php?data=UPDATE_DATA", true);
        server.send();
        get_fs();
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
    folders();
}

function download_file(file) {
    var fullpath = caminho() + get_json().files[file].name;
    if(window.confirm("Deseja baixar o arquivo?") == true) {
        console.log(fullpath);
        var server = configureBrowserRequest(server);
        server.onreadystatechange = function() {
            if(server.readyState == 4 && server.status == 200) {
                console.log(server.responseText);
                var rrr = JSON.parse(server.responseText);
                if (rrr["error"] == "false") alert("Download feito com sucesso. Acesse seu arquivo na pasta get-files-here");
                else alert("Download falhou: " + rrr["reason"]);
                get_fs();
            }
        }
        server.open("GET", "data.php?data=DOWNLOAD_FILE&fullpath="+fullpath, true);
        server.send();
    }
}

function add_folder() {
    var fullpath = prompt("Digite o nome da pasta:", "Nome");
    if(fullpath == null || fullpath == "") {
        window.alert("Digite um nome válido!");
    }
    else {
        fullpath = caminho() + fullpath;
        console.log(fullpath);
        create_folder(fullpath);
    }
}

function add_file() {
    var x = document.getElementById("file_submit");

    if (x.className.indexOf("block") == -1) {
        x.className += "block";
        x.className = x.className.replace("none", "");
    }
    else {
        x.className += "none";
        x.className = x.className.replace("block", "");
    }

    var fullpath = caminho();
    console.log(fullpath);
}

function edit_folder(id) {
    var fullpath_edit = prompt("Digite o novo nome da pasta:", get_json().subfolders[id].name);
    if(fullpath_edit == null || fullpath_edit == "") {
        window.alert("Digite um nome válido!");
    }
    else {
        fullpath = caminho() + get_json().subfolders[id].name;
        console.log("Editando " + fullpath + " para novo nome " + fullpath_edit);
        update_folder(fullpath, fullpath_edit);
    }
}

function edit_file(id) {
    var fullpath_edit = prompt("Digite o novo nome do arquivo:", get_json().files[id].name);
    if(fullpath_edit == null || fullpath_edit == "") {
        window.alert("Digite um nome válido!");
    }
    else {
        fullpath = caminho() + get_json().files[id].name;
        console.log(fullpath);
        console.log(fullpath_edit);
        update_file(fullpath, fullpath_edit);
    }
}


function update_file(fullpath, new_name) {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            console.log(server.responseText);
            get_fs();
        }
    }
    server.open("GET", "data.php?data=UPDATE_FILE&fullpath="+fullpath+"&newname="+new_name, true);
    server.send();    

}

function remove_folder(id) {
    var fullpath = caminho() + get_json().subfolders[id].name;
    if(window.confirm("Deseja apagar esta pasta?") == true) {
        console.log(fullpath);
        delete_folder(fullpath);
    }
}

function remove_file(id) {
    var fullpath = caminho() + get_json().files[id].name;
    if(window.confirm("Deseja apagar este arquivo?") == true) {
        console.log(fullpath);
        var server = configureBrowserRequest(server);
        server.onreadystatechange = function() {
            if(server.readyState == 4 && server.status == 200) {
                console.log(server.responseText);
                get_fs();
            }
        }
        server.open("GET", "data.php?data=DELETE_FILE&fullpath="+fullpath, true);
        server.send();
    }
}

function preencher_tabela(parse) {
    caminho[no] = parse.name;

    document.getElementById("caminho").innerHTML = caminho();
    delete_rows();

    if(parse.folders_no == 0) {
        var obj = document.getElementById("diretorios").insertRow(1);
        obj.insertCell(0).id = "diretorio_vazio";
        obj = document.getElementById("diretorio_vazio")
        obj.innerHTML = "Não há nada para mostrar!";
        obj.setAttribute("colspan", 8);
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
        obj.setAttribute("colspan", 8);
    }
    else {
        obj = parse.files;
        for(var i = obj.length - 1; i >= 0; i--) {
            add_arquivo(obj[i].name, obj[i].author, obj[i].size, obj[i].owner1, obj[i].owner2, i);
        }
    }
}

function caminho() {
    var str = "";
    for(var i = 0; i <= no; i++) {
        str += caminho[i] + "/";
    }
    document.getElementById("fullpathlol").setAttribute("value", str);
    return str;
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

function add_diretorio(nome, pasta, arquivo, tamanho, tipo, file) {
    var row = document.getElementById("diretorios").insertRow(1);
    row.insertCell(0).innerHTML = nome;
    row.insertCell(1).innerHTML = pasta;
    row.insertCell(2).innerHTML = arquivo;
    row.insertCell(3).innerHTML = bytes(tamanho);
    row.insertCell(4).innerHTML = tipo;
    if(file != -1) {
        row.insertCell(5).innerHTML = '<a href="javascript:void(0)" onClick="navegar(' + file + ')" class="especial"><i class="fa fa-mail-forward"></i></a>';
        row.insertCell(6).innerHTML = '<a href="javascript:void(0)" onClick="edit_folder(' + file + ')" class="editar"><i class="fa fa-edit"></i></a>';
        row.insertCell(7).innerHTML = '<a href="javascript:void(0)" onClick="remove_folder(' + file + ')" class="apagar"><i class="fa fa-times-circle"></i></a>';
    }
    else {
        row.insertCell(5);
        row.insertCell(6);
        row.insertCell(7);
    }
}

function add_arquivo(nome, autor, tamanho, par1, par2, funcao) {
    var row = document.getElementById("arquivos").insertRow(1);
    row.insertCell(0).innerHTML = nome;
    row.insertCell(1).innerHTML = autor;
    row.insertCell(2).innerHTML = bytes(tamanho);
    row.insertCell(3).innerHTML = par1;
    row.insertCell(4).innerHTML = par2;
    row.insertCell(5).innerHTML = '<a href="javascript:void(0)" onClick="download_file(' + funcao + ')" class="especial"><i class="fa fa-download"></i></a>';
    row.insertCell(6).innerHTML = '<a href="javascript:void(0)" onClick="edit_file(' + funcao + ')" class="editar"><i class="fa fa-edit"></i></a>';
    row.insertCell(7).innerHTML = '<a href="javascript:void(0)" onClick="remove_file(' + funcao + ')" class="apagar"><i class="fa fa-times-circle"></i></a>';
}

function get_fs() {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            parsed = JSON.parse(server.responseText);
            folders();
            preencher_tabela(get_json());
            console.log(server.responseText);
        }
    }
    server.open("GET", "data.php?data=GET_FS", true);
    server.send();
}

function folders() {
    var json = get_json();
    update_span("current_subfolders", json["folders_no"]);
    update_span("current_files", json["files_no"]);
    update_span("current_size", bytes(json["size"]));
}

function create_folder(fullpath) {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            console.log(server.responseText);
            get_fs();
        }
    }
    server.open("GET", "data.php?data=CREATE_FOLDER&fullpath="+fullpath, true);
    server.send();    

}

function update_folder(fullpath, new_name) {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            console.log(server.responseText);
            get_fs();
        }
    }
    server.open("GET", "data.php?data=UPDATE_FOLDER&fullpath="+fullpath+"&newname="+new_name, true);
    server.send();    

}

function delete_folder(fullpath) {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            console.log(server.responseText);
            get_fs();
        }
    }
    server.open("GET", "data.php?data=DELETE_FOLDER&fullpath="+fullpath, true);
    server.send();        
}