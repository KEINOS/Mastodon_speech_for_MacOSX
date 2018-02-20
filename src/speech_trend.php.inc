<?php

// 人気のQiita記事を取得
$feed_url   = 'https://qiita.com/popular-items/feed';
$feed_xml   = simplexml_load_file($feed_url);
$feed_json  = json_encode($feed_xml);
$feed_array = json_decode($feed_json, true);

foreach ($feed_array['entry'] as $item) {
    $x              = array();
    $x['id_raw']    = (string)  $item['id'];
    $x['id']        = (integer) get_num_from_id($x['id_raw']);
    $x['title']     = (string)  $item['title'];
    $x['link']      = (string)  $item['link']['@attributes']['href'];
    $x['author']    = (string)  $item['author']['name'];
    $x['published'] = (string)  $item['published'];
    $x['updated']   = (string)  $item['updated'];
    $x['url_md']    = (string)  get_url_md($x['link']);
    $data_new[$x['id']] = $x;
}

$data_diff = array_diff_key($data_new, $data_old);
$data_new  = $data_diff + $data_old;

foreach ($data_diff as $item) {
    $url       = $item['url_md'];
    $user_name = $item['author'];
    $title     = $item['title'];
    $contents  = file_get_contents($url);
    $contents  = sanitize_eol($contents, PHP_EOL);
    $contents  = explode(PHP_EOL, $contents);
    $width_full = get_width_screen();

    echo_menu([$user_name => $title]);
    say_name($user_name . 'さんのキータ記事', $title, 'Kyoko');
    echo PHP_EOL;
    foreach ($contents as $line) {
        $line  = mb_convert_kana($line, 'asKH', 'utf-8');
        $line  = trim($line);
        $line  = url_cleaner($line, false);
        $width = $width_full - MARGIN_LEFT_MAX;
        $line_echo = fix_width($line, $width - 3);
        $line_echo = echo_indent($line_echo, true, MARGIN_LEFT_MAX-3);
        echo(' ' . $line_echo);
        say(trim($line), 'Otoya');
    }
}

/*
------------------------------------------------------------------------
    Function
------------------------------------------------------------------------
*/

function get_num_from_id($string)
{
    if (preg_match('/[0-9]+$/', $string, $result)) {
        return $result[0];
    }
}

function get_url_md($url)
{
    $parse = parse_url($url);
    $url   = $parse['scheme'] . '://' . $parse['host'] . $parse['path'];

    return $url . ".md";
}
