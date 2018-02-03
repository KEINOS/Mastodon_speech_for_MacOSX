<?php
require_once('constants.php.inc');
require_once('functions.php.inc');

$path_json_qiitedon = './qiitedon.json';

echo '__FILE__       :' . __FILE__        . PHP_EOL;
echo 'Dir __FILE__   :' . dirname(__FILE__) . PHP_EOL;
echo 'Phar path true :' . Phar::running() . PHP_EOL;
echo 'Phar path false:' . Phar::running(false) . PHP_EOL;
echo 'Phar dir       :' . dirname(Phar::running(false)) . PHP_EOL;
echo 'Real path:' . realpath('./')  . PHP_EOL;

if (!file_exists($path_json_qiitedon)) {
    echo 'JSON NOT found' . PHP_EOL;
} else {
    echo 'JSON FOUND!' . PHP_EOL;
}

/* ------------------------------------------------------------------ */
echo 'Hello World' . PHP_EOL;
say_hello();