<?php

/* -------------------------------------------------------------- [C] */


function colour_font($color_font = 37, $color_bkgd = '', $color_else = '')
{
    $color_bkgd = (empty($color_bkgd)) ? '0;' : "${color_bkgd};";
    $color_else = (empty($color_else)) ? '0;' : "${color_else};";
    return "\e[${color_else}${color_bkgd}${color_font}m";
}

function colour_reset()
{
    return colour_font(FONT_WHITE, BKGD_BLACK, ATTRB_OFF);
}

function colour_text($string, $color_font = 37, $color_bkgd = '')
{
    $color_reset = colour_reset();
    $color_font  = colour_font($color_font, $color_bkgd);
    return $color_font . $string . $color_reset;
}

function convert_better_voice_accent($string)
{

    $string = replace_with_accent_list($string);
    $string = convert_phrase_with_mecab($string);

    return $string;
}

function convert_phrase_with_mecab($string)
{
    $parsed = parse_mecab($string);
    $result = '';

    foreach ($parsed as $key => $line) {
        $has_post_line   = (isset($parsed[$key+1]));

        $mecab = $line['mecab'];
        $yomi  = $line['phonetic'];

        $pause = function ($val) {
            $value_slnc = $val;
            $value_slnc = "[[slnc ${value_slnc}]]";
            return  $value_slnc;
        };

        $needs_pause = false;
        $mecab_next  = array();

        if ($has_post_line) {
            $mecab_next  = $parsed[$key+1]['mecab'];
            $needs_pause = is_pausable($mecab, $mecab_next);
        }

        if (! empty($yomi)) {
            $result .= $yomi;
            $result .= ($needs_pause) ? "{$pause(1)}" : '';
        }
    }

    return  trim($result);
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

    $lines = explode(PHP_EOL, $contents);

    foreach ($lines as $line) {
        echo fix_width($line, 30);
    }
    echo_hr();
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
    $margin_right = ' '; // $title と$string の余白
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
        $result .= $margin_right . colour_reset();
    } else {
        $result .= $margin_right;
    }

    //title trigger
    $indent  = empty($title) ? $margin_left : '';
    $result .= colour_reset();

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
        $value  = explode(PHP_EOL, $value);
        foreach ($value as $line_num => $line) {
            $marker = (strlen($key) > $margin_left_max) ? $trimmarker : '';
            // add space for pad right to $key and trim to $width_key_max
            $key = $key . $padding_right;

            if (0==$line_num) {
                $key  = mb_strimwidth($key, 0, $width_key_max-1, $marker, 'utf-8');
                $line = fix_width($line, $width_value);
                $result .= echo_indent($line, true, $width_key_max, $key);
            } else {
                $key  = mb_strimwidth($key, 0, $width_key_max, $marker, 'utf-8');
                $line = fix_width($line, $width_value);
                $result .= echo_indent($line, true, $width_key_max);
            }
        }
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
    $a['language']     = 'ja';

    if (preg_match("/[一-龠]+|[ぁ-ん]+|[ァ-ヴー]+|[ａ-ｚＡ-Ｚ０-９]+/u", $a['say'])) {
        $a['lang'] = 'ja';
    } else {
        $a['lang'] = 'en';
    }

    return $a;
}

function fix_width($string, $width)
{
    $start_pt = 0;
    $string   = rtrim($string);
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
        return dirname(Phar::running(false)) . DIR_SEP;
    } else {
        return dirname(__FILE__) . DIR_SEP;
    }
}

function get_height_screen()
{
    $default_width = 30; //デフォルト幅

    if (! is_cli()) {
        return 'n/a';
    }
    $width = trim(`tput lines`); //バッククォートであることに注意

    return (integer) (is_numeric($width) ? $width : $default_width);
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

function get_width_screen()
{
    $default_width = 70; //デフォルト幅

    if (! is_cli()) {
        return 'n/a';
    }
    $width = trim(`tput cols`); //バッククォートであることに注意

    return (integer) (is_numeric($width) ? $width : $default_width);
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

function is_pausable($mecab_pre, $mecab_post)
{
    $needs_pause = false;
    $arg_pre_0   = $mecab_pre[0];
    $arg_post_0  = $mecab_post[0];
    $arg_post_1  = $mecab_post[1];

    if ('助詞'==$arg_pre_0 && '名詞'==$arg_post_0) {
        $needs_pause = true;
    }
    if ('名詞'==$arg_pre_0 && '名詞'==$arg_post_0 && '接尾'==$arg_post_1) {
        $needs_pause = false;
    }
    if ('名詞'==$arg_pre_0 && '名詞'==$arg_post_0 && '固有名詞'==$arg_post_1) {
        $needs_pause = true;
    }
    if ('動詞'==$arg_pre_0 && '非自立'==$arg_post_0 &&'名詞'==$arg_post_1) {
        $needs_pause = true;
    }

    if ('助動詞'==$arg_pre_0 && '連帯詞'==$arg_post_0) {
        $needs_pause = true;
    }

    return $needs_pause;
}

function is_phar()
{
    return strlen(Phar::running()) > 0 ? true : false;
}

/* -------------------------------------------------------------- [P] */

function parse_mecab(string $string)
{
    $mecab = new MeCab\Tagger([
        '-d' => PATH_DIR_DIC_MECAB . DIR_SEP . NAME_FILE_DIC_MECAB,
    ]);

    $parsed = $mecab->parseToString($string);
    $lines  = explode(PHP_EOL, $parsed);
    $result = array();

    foreach ($lines as $key => $line) {
        $tmp   = explode("\t", $line);
        if (! isset($tmp[1])) {
            continue;
        }
        $word  = trim($tmp[0]);
        $mecab = explode(',', $tmp[1]);
        $result[$key] = [
            'word'     => $word,
            'mecab'    => $mecab,
            'phonetic' => (isset($mecab[8])) ? $mecab[8] : $word,
        ];
    }

    return $result;
}

/* -------------------------------------------------------------- [R] */

function replace_with_accent_list($string)
{
    $string  = strtolower($string);
    $replace = get_accent_list();

    // キーの文字列の長さで並び替える（長い文字から置換するため）
    uksort($replace, function ($a, $b) {
        return (mb_strlen($b) > mb_strlen($a));
    });

    return strReplaceAssoc($replace, $string);
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
    $msg = '。コードブロックを省略しました。';

    $string = strip_emoji($string);
    $string = mb_convert_kana($string, 'asKH', 'utf-8');
    $string = str_replace('<code>', "${msg}[[cmnt <code>", $string);
    $string = str_replace('</code>', '</code> ]]', $string);
    $string = strip_tags($string);
    $string = html_entity_decode($string);

    return $string;
}

/**
 * @ref https://developer.apple.com/library/content/documentation/UserExperience/Conceptual/SpeechSynthesisProgrammingGuide/FineTuning/FineTuning.html#//apple_ref/doc/uid/TP40004365-CH5-SW11
 */
function get_arg(array $args, string $key)
{
    return (isset($args[$key]) && ! empty(trim($args[$key]))) ? $args[$key] : false;
}

function say(array $args)
{
    // Argument options (preset and default)
    $phrase     = get_arg($args, 'phrase')     ?: '';
    $type_voice = get_arg($args, 'type_voice') ?: '';
    $echo_too   = get_arg($args, 'echo_too')   ?: false;
    $pitch      = get_arg($args, 'pitch')      ?: '+ 0';

    $voice  = empty($type_voice) ? '' : " -v ${type_voice} ";

    $phrase = str_replace('[[', '[', $phrase);
    $phrase = str_replace(']]', '[', $phrase);
    $phrase = str_replace('(', '', $phrase);
    $phrase = str_replace(')', '', $phrase);

    $phrase = convert_better_voice_accent($phrase);
    $phrase = strip_repeated_strings($phrase);
    $phrase = $voice . "'[[pbas ${pitch}]]${phrase}[[rset 0]]'";

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
    say([
        'phrase' => $user_name,
        'pitch'  => '+ 1',
        'type_voice' => $voice,
    ]);
    say([
        'phrase' => $postfix,
        'type_voice' => $voice,
    ]);
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
        'pattern' => '/[xXｘＸ]{4,}/u',
        'replace' => 'エックス',
    ];
    $patterns[] = [
        'pattern' => '/[0-0]{4,}/u',
        'replace' => '',
    ];
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
        '<br />'  => '<br>',
        '</p>'    => '</p><br>',
        '<br>'    => PHP_EOL,
    ];
    $string = strReplaceAssoc($replace, $string);
    $string = strip_tags($string);
    $string = html_entity_decode($string);

    return trim($string);
}

function strip_url($string, $announce_deletion = false)
{
    return url_cleaner($string, $announce_deletion);
}

function str_replace_endtag_with_eol($string)
{

    $string   = str_replace('</p>', '</p>'  . PHP_EOL, $string);
    $string   = str_replace('</h1>', '</h1>' . PHP_EOL, $string);
    $string   = str_replace('</h2>', '</h2>' . PHP_EOL, $string);
    $string   = str_replace('</h3>', '</h3>' . PHP_EOL, $string);
    $string   = str_replace('</h4>', '</h4>' . PHP_EOL, $string);
    $string   = str_replace('</h5>', '</h5>' . PHP_EOL, $string);
    $string   = str_replace('</h6>', '</h6>' . PHP_EOL, $string);

    return $string;
}

function strReplaceAssoc(array $replace, $subject)
{
    return str_replace(array_keys($replace), array_values($replace), $subject);
}

/* -------------------------------------------------------------- [U] */

function url_breakline($string)
{
    $pattern = '/https?:\/\/[-_.!~*\'()a-zA-Z0-9;\/?:@&=+$,%#]+/';

    $string = preg_replace_callback(
        $pattern,
        function ($matches) {

            if (isset($matches[0])) {
                return $matches[0] . PHP_EOL;
            }
            if (isset($matches[1])) {
                return $matches[0];
            }
            return '';
        },
        $string
    );

    return $string;
}

function url_cleaner($string, $announce_deletion = false)
{
    $pattern = '/https?:\/\/[-_.!~*\'()a-zA-Z0-9;\/?:@&=+$,%#]+/';

    $string = preg_replace_callback(
        $pattern,
        function ($matches) {
            if (isset($matches[1])) {
                echo_eol("match" . $matches[1]);
                return $matches[0];
            }
            return '';
        },
        $string
    );

    $msg_delete = (empty(trim($string))) ? 'URLを省略しました。' : '';
    $string = ($announce_deletion) ? $string . $msg_delete : $string;

    return  $string;
}
