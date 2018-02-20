<?php

/* -------------------------------------------------------------- [C] */

function colour_text($string, $color_font = 37, $color_bkgd = '')
{
    $color_bkgd = (empty($color_bkgd)) ? '0;' : "${color_bkgd};";
    return "\e[{$color_bkgd}{$color_font}m{$string}\e[0;40;40m";
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

/* -------------------------------------------------------------- [D] */

function delete_list_oldtoot($array, $hour)
{
    $second    = $hour * 60 * 60;
    $threshold = time() - $second;

    foreach ($array as $key => $time) {
        if ($time < $threshold) {
            //unset($array[$key]);
            $array[$key] = 'deleted';
        }
    }

    return $array;
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

function echo_eol($string = '', $return = false)
{
    if ($return) {
        return (string) $string . PHP_EOL;
    } else {
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

function echo_indent($string, $return = false, $width_indent = WIDTH_INDENT, $title = '')
{
    $string       = trim($string, PHP_EOL);
    $array        = explode(PHP_EOL, $string);
    $margin_left  = str_repeat(' ', $width_indent);
    $margin_right = ' '; // $title と$value の余白
    $has_title    = ! empty($title);
    $margin_right_len = strlen($margin_right);

    //add title
    $title  = $title . $margin_left;
    $result = mb_strimwidth(
        $title,
        0,
        $width_indent - $margin_right_len,
        '',
        'utf-8'
    );

    if ($has_title) {
        $result  = colour_text($result, FONT_BLACK, BKGD_ORANGE);
        $result .= $margin_right;
    }

    //title trigger
    $indent  = empty($title) ? $margin_left : '';
    $result .= "\e[0;m";

    foreach ($array as $value) {
        $result .= $indent . $value . PHP_EOL;
        $indent = (empty($indent)) ? $margin_left : $indent;
    }

    if ($return) {
        return $result;
    } else {
        echo $result;
    }
}

function echo_menu($array, $return = false)
{
    $result          = '';
    $trimmarker      = '';
    $width_key_max   = get_length_key_max($array) + 2;
    $width_full      = get_width_screen();
    $margin_left_max = MARGIN_LEFT_MAX; //半角MARGIN_LEFT_MAX文字ぶん右にインデント

    if (1 == count($array)) {
        $trimmarker    = MARKER_TRIM;
        $width_key_max = $margin_left_max - mb_strlen($trimmarker);
    }

    $width_value   = $width_full - $width_key_max;
    $padding_right = str_repeat(' ', $width_full);

    foreach ($array as $key => $value) {
        $value  = trim($value);
        $marker = (strlen($key) > $margin_left_max) ? $trimmarker : '';

        // add space for pad right to $key and trim to $width_key_max
        $key = $key . $padding_right;
        $key = mb_strimwidth($key, 0, $width_key_max-1, $marker, 'utf-8');

        $value   = fix_width($value, $width_value);
        $result .= echo_indent($value, true, $width_key_max, $key);
    }

    if ($return) {
        return $result;
    }

    echo $result;
}

function explode_type_voice_and_lang($string)
{
    return explode(' ', $string);
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
function get_accent_list()
{
    include('accent_dictionary.php.inc');
    
    return $accent_dictionary;
}

function get_command_input($stdin)
{
    $stdin = strtolower(trim($stdin));

    return (empty($stdin)) ? 'h' : $stdin;
}

function get_current_toot($mode)
{
    $random   = uniqid(md5(time()));
    $schema   = 'https';
    $host     = 'qiitadon.com';
    $endpoint = '/api/v1/timelines/public';
    $method   = 'GET';

    switch ($mode) {
        case 'ltl':
            $args = "local=true&r=${random}";
            break;
        default:
            $args = "&r=${random}";
            break;
    }

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
        $account   = $toot['raw']['account']['acct'];

        if (! is_omit_account($account)) {
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

function get_length_key_max($array)
{
    $max_len = 0;
    foreach ($array as $key => $value) {
        $max_len = (mb_strlen($key) > $max_len ) ? mb_strlen($key) : $max_len;
    }

    return $max_len;
}

function get_path_config()
{
    return PATH_DIR_CURNT . NAME_FILE_CONF;
}

function get_random_voice($voices = array())
{
    if (empty($voices)) {
        $voices = get_type_voice();
    }

    return array_rand($voices);
}

function get_type_voice()
{
    $list_string = trim(`say -v ?`);
    $list_array  = explode(PHP_EOL, $list_string);
    $list        = array();
    foreach ($list_array as $line) {
        // 'Kyoko     ja_JP # こんにちは' -> '['Kyoko    ja_JP','こんにちは']
        $types_and_phrase = explode('#', $line);
        // 'Kyoko     ja_JP' -> ['Kyoko','ja_JP']
        $types_and_phrase[0] = trim(
            preg_replace(
                '/\s+/',
                ' ',
                $types_and_phrase[0]
            )
        );

        $name_and_lang = explode(' ', $types_and_phrase[0]);

        $list[] = [
            'name'   => trim($name_and_lang[0]),
            'lang'   => trim($name_and_lang[1]),
            'phrase' => trim($types_and_phrase[1]),
            'raw'    => trim($line),
        ];
    }

    return $list;
}

function get_pitch_id($name)
{
    global $list_pitch_id;

    $id = md5(trim($name));

    if (isset($list_pitch_id[$id])) {
        $pitch = $list_pitch_id[$id];
    } else {
        $pitch = set_pitch_id($name);
    }

    return $pitch;
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

function is_omit_account($string)
{
    $string = trim((string) $string);
    $list = [
        'qithub',
        'btc@framapiaf.org',
        'usercount@mstdn.io',
        'pawoo_info@pawoo.net',
        'SelfishWhiteBear@mstdn.jp',
        'yasame_pawoo@pawoo.net',
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
    $string = strip_tags_in_toot($string);
    $string = trim($string);

    return $string;
}

function sanitize_toot_to_say($string)
{
    $string = strip_tags_in_toot($string);
    $string = url_cleaner($string);
    $string = escapeshellcmd($string);
    $string = strip_repeated_strings($string);

    return $string;
}

function say($phrase, $type_voice = '', $echo_too = false, $char_id = '')
{
    //https://developer.apple.com/library/content/documentation/UserExperience/Conceptual/SpeechSynthesisProgrammingGuide/FineTuning/FineTuning.html#//apple_ref/doc/uid/TP40004365-CH5-SW11

    $volume = (empty($char_id)) ? '+ 0' : get_pitch_id($char_id);
    $voice  = (string) $type_voice;
    $voice  = empty($type_voice) ? '' : " -v ${type_voice} ";
    $phrase = mb_convert_encoding((string) $phrase, 'utf-8', 'auto');
    $phrase = convert_better_voice_acent($phrase);
    $phrase = $voice . "'[[pbas ${volume}]]${phrase}[[rset 0]]'";

    if ($echo_too) {
        echo_eol($phrase);
    }

    return `say $phrase`;
}

function say_hello()
{
    return say("Hello masto don");
}

function say_name($user_name, $postfix, $voice = 'Kyoko')
{
    say("[[pbas + 1]]${user_name}", $voice);
    say($postfix, $voice);
}

function set_env_utf8_ja($timezone = 'Asia/Tokyo')
{
    if (! function_exists('mb_language')) {
        die('This application requires mb_language.');
    }

    date_default_timezone_set($timezone);
    setlocale(LC_ALL, 'ja_JP');
    mb_language('ja');
    mb_internal_encoding('UTF-8');
    mb_http_output('UTF-8');
}

function set_pitch_id($name)
{
    global $list_pitch_id;

    $id = md5(trim($name));

    $count      = count($list_pitch_id);
    $pos_or_neg = ( $count % 2 ) ? '+' : '-';
    $pitch      = ( string ) ( rand(1, 127) );
    $pitch      = "${pos_or_neg} ${pitch}";

    // set voice pitch as voice id
    $list_pitch_id[$id] = $pitch;

    return $pitch;
}
$list_pitch_id = array();

function set_root_dir()
{
    if (defined('DIR_ROOT_SCRIPT')) {
        return DIR_ROOT_SCRIPT;
    }
    $dir_root = dirname(__FILE__) . DIRECTORY_SEPARATOR;
    define('DIR_ROOT_SCRIPT', $dir_root);

    return DIR_ROOT_SCRIPT;
}

function strip_emoji($string)
{
    mb_substitute_character('none');
    $string = mb_convert_encoding($string, 'SJIS', 'utf-8');
    $string = mb_convert_encoding($string, 'utf-8', 'SJIS');

    return $string;
}

function strip_repeated_strings($string)
{
    $patterns[] = [
        'pattern' => '/[wWｗＷ]{4,}/u',
        'replace' => '爆笑',
    ];
    $patterns[] = [
        'pattern' => '/[8８]{4,}/u',
        'replace' => 'ぱちぱち',
    ];
    $patterns[] = [
        'pattern' => '/([^A-Za-zＡ-Ｚａ-ｚ\s])[wWｗＷ]{1,1}/u',
        'replace' => '$1（わら）',
    ];

    foreach ($patterns as $pattern) {
        $string = preg_replace($pattern['pattern'], $pattern['replace'], $string);
    }

    return $string;
}

function strip_tags_in_toot($string)
{
    $string = sanitize_eol((string) $string);

    $replace = [
        'http'    => PHP_EOL . 'http',
        '`'       => '\'',
        '<br>'    => '<br />',
        '</p>'    => '</p><br />',
        '<br />'  => PHP_EOL,
        '<code>'  => '```' . PHP_EOL,
        '</code>' => PHP_EOL  . '```',
    ];
    $string = strReplaceAssoc($replace, $string);
    $string = strip_tags($string);
    $string = html_entity_decode($string);

    return trim($string);
}

function strReplaceAssoc(array $replace, $subject)
{
    return str_replace(array_keys($replace), array_values($replace), $subject);
}

/* -------------------------------------------------------------- [U] */

function url_cleaner($string,$announce_deletion=true)
{
    $pattern = '/https?:\/\/[-_.!~*\'()a-zA-Z0-9;\/?:@&=+$,%#]+/';

    $string = preg_replace_callback(
        $pattern,
        function ($matches) {
            global $announce_deletion;
            if (isset($matches[1])) {
                return $matches[0];
            }
            return ($announce_deletion) ? '。URLを省略しました。' : '';
        },
        $string
    );

    return $string;
}
