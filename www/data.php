<?php

$answer = "";
$fp = fsockopen("localhost", 44777, $errno, $errstr, 30);
if (!$fp) {
    echo '{"error":"true"}';
    exit;
} 

$message = $_GET["data"];

fwrite($fp, $message . ".:.:.");
while (!feof($fp)) {
    $answer .= fgets($fp, 128);
}
fclose($fp);

echo '{"error":"false", "' . $_GET["data"] . '": "' . $answer .'"}';


?>