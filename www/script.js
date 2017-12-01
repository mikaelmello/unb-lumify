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
}

function logout() {
    document.cookie = 'nickname=; Max-Age=0';
    location.href = "/login.php";
}

/*window.onload = function() {
    var username = getCookie("nickname");
    if (!username) {
        username = prompt("Qual seu nome de usuario?");
        setCookie("nickname", username);
    }
    update_span("nickname", username);


    setInterval(function() {
        var server = configureBrowserRequest(server);   
        server.onreadystatechange = function() {
            if(server.readyState == 4 && server.status == 200) {
                var parsed = JSON.parse(server.responseText);
                update_span("usersqty", parsed["PEER_QUANTITY"]);
            }
        }
        server.open("GET", "data.php?data=PEER_QUANTITY", true);
        server.send();
    }, 5000);

}*/