<?php
include_once('parsedown_extension.php.inc');

$SayExtension  = new SayExtension();
$EchoExtension = new EchoExtension();

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

shuffle($data_diff);

foreach ($data_diff as $item) {
    $url        = $item['url_md'];
    $user_name  = $item['author'];
    $title      = $item['title'];
    $contents   = file_get_contents($url);
    $contents   = sanitize_eol($contents, PHP_EOL);

    $contents = $EchoExtension->text($contents);
    $contents = str_replace_endtag_with_eol($contents);
    $contents = explode(PHP_EOL, $contents);

    $width_full = get_width_screen();

    echo_menu([$user_name => $title]);
    say_name($user_name . 'さんのキータ記事', $title, 'Kyoko');
    echo PHP_EOL;
    foreach ($contents as $line) {

        $line = mb_convert_kana($line, 'asKH', 'utf-8');

        //Say
        $line_say = $line;
        $line_say = sanitize_toot_to_say($line_say);
        $line_say = strip_tags($line_say);
        $line_say = html_entity_decode($line_say);
        $line_say = url_cleaner($line_say, DO_NOT_ANNOUNCE_DELETION);

        //Echo
        $line_echo = $line;

        $color_font  = colour_font(FONT_BLUE);
        $color_reset = colour_reset();
        $line_echo   = str_replace('<h', "${color_font}<h", $line_echo);

        $line_echo = strip_tags($line);
        $line_echo = html_entity_decode($line_echo);

        //Format line width
        $width     = $width_full - MARGIN_LEFT_MAX;
        $line_echo = fix_width($line_echo, $width - 3); /* */
        $line_echo = echo_indent($line_echo, true, MARGIN_LEFT_MAX-3);
        //echo(' ' . $line_echo);
        echo $line_echo;

        say([
            'phrase'     => trim($line_say),
            'type_voice' => 'Otoya',
            'echo_too'   => DO_NOT_ECHO, /* ECHO_SAY_CNTNT */
        ]);
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
