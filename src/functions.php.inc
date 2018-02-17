<?php

/* -------------------------------------------------------------- [C] */

function colour_text($string, $color_font = 37, $color_bkgd = '')
{
    $color_bkgd = (empty($color_bkgd)) ?: "${color_bkgd};";
    return "\e[{$color_bkgd}{$color_font}m{$string}\e[m";
}

function convert_better_voice_acent($string)
{
    $string  = strtolower($string);
    $replace = get_accent_list();

    // キーの文字列の長さで並び替える（長い文字から置換するため）
    uksort($replace, function ($a, $b) {
        return (mb_strlen($b) > mb_strlen($a));
    });

    $string = strReplaceAssoc($replace, $string);

    return $string;
}

/* -------------------------------------------------------------- [E] */

function echo_colored($title, $contents, $fg_color, $bg_color)
{
    $title        = (string) $title . ' ';
    $contents     = (string) $contents;
    $result       = '';

    $title_len    = strlen($title);
    $width_screen = get_width_screen();
    //$temp_title   = colour_text($title, FONT_BLACK, BKGD_GREEN);

    $lines = explode(PHP_EOL, $contents);
    //print_r($lines);

    foreach ($lines as $line) {
        echo fix_width($line, 30);
    }
    echo_hr();

    //$result .= colour_text($contents, $fg_color, $bg_color);

    //echo $result;
}

function echo_debug($contents, $title = '')
{

    if (! is_string($contents)) {
        $contents = print_r($contents, true);
    }

    if (! empty($title)) {
        if (is_cli()) {
            $title    =  $title . PHP_EOL;
            $contents = echo_indent($contents, true);
        } else {
            $attr     = "style='padding-left:2em;'";
            $title    = echo_div($title);
            $contents = echo_div($contents, $attr);
        }
    } else {
        $contents = $contents . PHP_EOL;
    }

    echo $title . (string) $contents . PHP_EOL;
}

function echo_div($contents, $attr = '')
{
    $contents = (string) $contents;
    $attr     = sanitize_attr($attr);

    echo "<div $attr>${contents}</div>" . PHP_EOL;
}

function echo_eol($string, $return=false)
{
    if($return){
        return (string) $string . PHP_EOL;
    }else{
        echo $string . PHP_EOL;
    }
}

function echo_hr()
{
    if (is_cli()) {
        $width_screen = get_width_screen();
        echo str_repeat('-', $width_screen);
    }
}

function echo_info_env()
{
    echo_debug(php_uname(), 'php uname');
    echo_debug(PHP_OS, 'PHP OS');
    echo_debug(__FILE__, '__FILE__');
    echo_debug(dirname(__FILE__), 'Dir __FILE__');
    echo_debug(Phar::running(), 'Phar path true');
    echo_debug(Phar::running(false), 'Phar path false');
    echo_debug(dirname(Phar::running(false)), 'Phar dir');
    echo_debug(realpath('./'), 'Real path');

    if (! file_exists(get_path_config())) {
        echo_debug('JSON NOT found');
    } else {
        echo_debug('JSON FOUND!');
    }

    echo_debug(get_dir_root(), 'get_dir_root');
    echo_debug(get_dir_index(), 'get_dir_index');
}

function echo_indent($string, $return = false)
{
    $width_screen = get_width_screen();
    $width_indent = mb_strlen(INDENT);
    $width_line   = $width_screen - $width_indent;

    $string = sanitize_eol((string) $string);
    $string = chunk_split($string, $width_line, PHP_EOL . '  ');
    $array  = explode(PHP_EOL, $string);
    $result = '';

    foreach ($array as $value) {
        $result .= INDENT . $value . PHP_EOL;
    }

    if ($return) {
        return trim($result, PHP_EOL);
    } else {
        echo trim($result, PHP_EOL);
    }
}

/* -------------------------------------------------------------- [F] */

function fetch_toot_info($toot)
{
    $a['user_id']      = $toot['account']['id'];
    $a['user_name']    = $toot['account']['username'];
    $a['display_name'] = $toot['account']['display_name'];
    $a['toot_id']      = $toot['id'];
    $a['content']      = sanitize_toot_to_echo($toot['content']);
    $a['say']          = sanitize_toot_to_say($toot['content']);
    $a['raw']          = $toot;
    //$a['language']  = $toot['account']['language'];

    return $a;
}

function fix_width($string, $width)
{
    $start_pt = 0;
    $max_len  = mb_strwidth($string, 'utf-8');
    $str_temp = '';
    $result   = '';

    while ($start_pt < $max_len) {
        $start_pt   = mb_strlen($str_temp, 'utf-8');
        $str_trimed = mb_strimwidth($string, $start_pt, $width, '', 'utf-8');
        $str_temp  .= $str_trimed;
        $result    .= $str_trimed . PHP_EOL;
        if (empty($str_trimed)) {
            break;
        }
    }

    return $result;
}


/* -------------------------------------------------------------- [G] */

function get_command_input($stdin)
{
    return strtoupper(trim($stdin));
}

function get_current_toot()
{
    $random   = uniqid(md5(time()));
    $schema   = 'https';
    $host     = 'qiitadon.com';
    $endpoint = '/api/v1/timelines/public';
    $method   = 'GET';
    //$args     = "local=true&r=${random}";
    $args     = "&r=${random}";

    $query  = "curl -X ${method}";
    $query .= " -sS ${schema}://${host}${endpoint}?${args};";

    /* 送信実行 */
    $toots = `$query`; //JSONで結果が返ってくる
    $toots = json_decode(trim($toots), JSON_OBJECT_AS_ARRAY);

    $result = array();

    foreach ($toots as $toot) {
        $toot      = fetch_toot_info($toot);
        $user_name = $toot['user_name'] . "\t";
        $toot_id   = $toot['toot_id'];

        if (! is_omit_user($user_name)) {
            $result[$toot_id] = $toot;
        }
    }

    return $result;
}


function get_dir_index()
{
    return dirname(__FILE__) . DIR_SEP;
}

function get_dir_root()
{
    if (is_phar()) {
        return dirname(Phar::running(false)) . DIR_SEP . PHP_EOL;
    } else {
        return dirname(__FILE__) . DIR_SEP . PHP_EOL;
    }
}

function get_height_screen()
{
    $default_width = 30; //デフォルト幅

    if (! is_cli()) {
        return 'n/a';
    }
    $width = trim(`tput lines`); //バッククォートであることに注意

    return is_numeric($width) ? $width : $default_width;
}

function get_path_config()
{
    return PATH_DIR_CURNT . NAME_FILE_CONF;
}

function explode_type_voice_and_lang($string)
{
    return explode(' ', $string);
}

function get_random_voice($voices = array())
{
    if(empty($voices)){
        $voices = get_type_voice();        
    }
    
    return array_rand($voices);
}

function get_type_voice()
{
    $list_string = trim(`say -v ?`);
    $list_array  = explode(PHP_EOL, $list_string);
    $list        = array();
    foreach($list_array as $line){
        // 'Kyoko     ja_JP # こんにちは' -> '['Kyoko    ja_JP','こんにちは'] 
        $types_and_phrase = explode('#', $line);
        // 'Kyoko     ja_JP' -> ['Kyoko','ja_JP']
        $types_and_phrase[0] = trim( preg_replace('/\s+/', ' ', $types_and_phrase[0]));
        $name_and_lang       = explode(' ', $types_and_phrase[0]);
        
        $list[] = [
            'name'   => trim($name_and_lang[0]),
            'lang'   => trim($name_and_lang[1]),
            'phrase' => trim($types_and_phrase[1]),
        ];
    }

    return $list;
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

/* -------------------------------------------------------------- [I] */

function is_cli()
{
    return PHP_SAPI === 'cli' || empty($_SERVER['REMOTE_ADDR']);
}

function is_mac()
{
    $kernel_require = 'DARWIN';
    $kernel_system  = strtoupper(trim(PHP_OS));

    return $kernel_require == $kernel_system;
}

function is_omit_user($string)
{
    $string = trim((string) $string);
    $list = [
        'qithub',
    ];

    return in_array($string, $list);
}

function is_phar()
{
    return strlen(Phar::running()) > 0 ? true : false;
}

/* -------------------------------------------------------------- [S] */

function sanitize_attr($attr)
{
    $attr = (string) $attr;

    return $attr;
}

function sanitize_eol($string, $to = PHP_EOL)
{
    $string = (string) $string;

    return strtr($string, array_fill_keys(array("\r\n", "\r", "\n"), $to));
}

function sanitize_toot_to_echo($string)
{
    $string = trim($string);
    $string = strip_tags_in_toot($string);

    return $string;
}

function sanitize_toot_to_say($string)
{
    $string = strip_tags_in_toot($string);
    $string = url_cleaner($string);

    return $string;
}

function say($phrase, $type_voice='', $echo_too=false)
{
    $voice  = (string) $type_voice;
    $voice  = empty($type_voice) ? '' : " -v ${type_voice} ";
    $phrase = (string) $phrase;
    $phrase = convert_better_voice_acent($phrase);
    $phrase = $voice . "'${phrase}'";
    
    if($echo_too){
        echo_eol($phrase);
    }

    return `say $phrase`;
}

function say_hello()
{
    return say("Hello masto don");
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

function strip_tags_in_toot($string)
{
    $replace = [
        '`'       => '\'',
        '<br>'    => '<br />',
        '</p>'    => '</p><br />',
        '<br />'  => PHP_EOL,
        '<code>'  => '```' . PHP_EOL,
        '</code>' => PHP_EOL . '```',
    ];
    $string = strReplaceAssoc($replace, $string);
    $string = strip_tags($string);
    $string = html_entity_decode($string);

    return $string;
}

function strReplaceAssoc(array $replace, $subject)
{
    return str_replace(array_keys($replace), array_values($replace), $subject);
}

/* -------------------------------------------------------------- [U] */

function url_cleaner($string)
{
    $pattern = '/https?:\/\/[-_.!~*\'()a-zA-Z0-9;\/?:@&=+$,%#]+/';

    $string = preg_replace_callback(
        $pattern,
        function ($matches) {
            if (isset($matches[1])) {
                return $matches[0];
            }
            return "(URLを省略しました)";
        },
        $string
    );

    return $string;
}

