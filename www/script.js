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

function set_nickname() {
    var username = getCookie("nickname");
    while (!username) {
        username = prompt("Qual seu nome de usuario?");
        setCookie("nickname", username);
    }
    if (username) {
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
    update_span("nickname", username);
}

function update() {
    var server = configureBrowserRequest(server);   
    server.onreadystatechange = function() {
        if(server.readyState == 4 && server.status == 200) {
            var parsed = JSON.parse(server.responseText);
            update_span("usersqty", parsed["users_qty"]);
        }
    }
    server.open("GET", "data.php?data=UPDATE_DATA", true);
    server.send();
    setInterval(function() {
        server.open("GET", "data.php?data=UPDATE_DATA", true);
        server.send();
    }, 5000);
}

window.onload = function() {
    
    set_nickname();
    update();

}