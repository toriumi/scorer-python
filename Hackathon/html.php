<?php
$html = file_get_contents('http://www.books.or.jp/ResultList.aspx?searchtype=1&isbn=9784873117393&showcount=20&startindex=0');
$html = mb_convert_encoding($html, 'HTML-ENTITIES', 'ASCII, JIS, UTF-8, EUC-JP, SJIS');
$domDocument = new DOMDocument();
$domDocument->loadHTML($html);
$xmlString = $domDocument->saveXML();
$xmlObject = simplexml_load_string($xmlString);

$array = json_decode(json_encode($xmlObject), true);
var_dump($array["body"]["div"][0]["div"]["section"]["table"]["tr"][1]["td"][1]["a"]);
?>