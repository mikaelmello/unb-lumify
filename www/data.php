<?php

function send_msg($message) {

    $answer = "";
    $fp = fsockopen("localhost", 44777, $errno, $errstr, 30);
    if (!$fp) {
        return '{"error":"true", "before":"true"}';
    }

    fwrite($fp, "PHP:" . $message . ":[end]");
    while (!feof($fp)) {
        $answer .= fgets($fp, 128);
    }
    fclose($fp);
    return $answer;
}

switch($_GET["data"]) {
    case "UPDATE_NICK":
        echo send_msg("NEW_NICK:" . $_GET["nickname"]);
        break;
    case "UPDATE_DATA":
        echo send_msg("UPDATE");
        break;
}


?>