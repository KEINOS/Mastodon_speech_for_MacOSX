<?php

/* -------------------------------------------------------------- [I] */

function is_phar()
{
    return strlen(Phar::running()) > 0 ? true : false;
}

function is_cli()
{
    return PHP_SAPI === 'cli' || empty($_SERVER['REMOTE_ADDR']);
}

/* -------------------------------------------------------------- [S] */

function say($string)
{
    $string = (string) $string;

    return `say $string`;    
}

function say_hello()
{
    say("Hello masto don");    
}

function set_root_dir()
{
    if (defined('DIR_ROOT_SCRIPT')) {
        return DIR_ROOT_SCRIPT;
    }
    $dir_root = dirname(__FILE__) . DIRECTORY_SEPARATOR;
    define('DIR_ROOT_SCRIPT', $dir_root);

    return DIR_ROOT_SCRIPT;
}

/* -------------------------------------------------------------- [G] */

function get_height_screen()
{
    $default_width = 30; //デフォルト幅

    if (! is_cli()) {
        return 'n/a';
    }
    $width = trim(`tput lines`); //バッククォートであることに注意

    return is_numeric($width) ? $width : $default_width;
}

function get_type_voice()
{
    return ''; //use default
}

function get_width_screen()
{
    $default_width = 70; //デフォルト幅

    if (! is_cli()) {
        return 'n/a';
    }
    $width = trim(`tput cols`); //バッククォートであることに注意

    return is_numeric($width) ? $width : $default_width;
}

