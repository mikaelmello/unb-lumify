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

function get_fs() {
    var server = configureBrowserRequest(server);
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            var parsed = JSON.parse(server.responseText);
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
