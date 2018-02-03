<?php
/**
 * box.phar installer.
 *
 * Installs box.phar on behalf of the original installer.
 * You better download the installer and install it you own, if you feel
 * insecure about the code below.
 */

$url_installer = 'https://box-project.github.io/box2/installer.php';

if (! file_exists('./box.phar')) {
    eval(trim(file_get_contents($url_installer), '<?php'));
} else {
    echo `php ./box.phar update`;
}
