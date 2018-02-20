<?php
require_once('constants.php.inc');
require_once('config.default.php.inc');
require_once('functions.php.inc');

// Check if its Mac OSX(Darwin)
// Read / create application config file.
// Setup application from config.
// Set Lines To Speech as blank.
// Read Mastodon Local Time Line
// Compare LTL with LTS and get diffs
// Configure diff lines to speech
// `say` each lines
// Goto Read Mastodon LTL

/*
for($i=0;$i<200;$i++){
    $cmd = "\e[${i}m${i} SAMPLE TEXT\e[0m";
    echo_eol($cmd);
}
*/

/* ------------------------------------------------------------------ */
set_env_utf8_ja();
if (ob_get_contents() || ob_get_length()) {
    ob_end_clean();
}

echo str_repeat(PHP_EOL, get_height_screen());
echo_eol('Running app ...');
echo_hr();
echo_eol('Press \'control+c(^C)\' to cancel/quit this app.');
echo_eol('INPUT MENU:');
echo_menu([
    'ltl'   => 'Qiitadon のローカルタイムラインの読み上げを開始します。',
    'ftl'   => 'Qiitadon の連合タイムラインの読み上げを開始します。',
    'trend' => 'Qiita のトレンドを読み上げます。 ',
]);

echo_hr();

//echo_info_env();
$is_running      = true;
$list_voices     = get_type_voice();
$time_interval   = 5;
$time_lastupdate = 0; //起動時にすぐに読み上げるため time() させない
$data_old        = array();
$list_id_tooted  = array();
$fg_color        = FONT_WHITE;
$bg_color        = BKGD_BLACK;

//INPUT
$cmd = '';
$has_been_tooted = false;

while ($is_running) {
    $input           = '';
    $time_current    = time();
    $time_nextupdate = $time_lastupdate + $time_interval;
    $time_left       = $time_nextupdate - $time_current;

    //$cmd = fgets(STDIN)

    // time interval
    if ($time_nextupdate < $time_current) {
        $time_lastupdate = time();
        $input = 't';
    } else {
        sleep(1);
    }

    if (empty($cmd)) {
        echo 'INPUT: ';
        $cmd = fgets(STDIN);
        echo_hr();
    }

    if (! empty($cmd)) {
        $input = get_command_input($cmd);
    }

    switch ($input) {
        case 'v':
            foreach ($list_voices as $voice) {
                $type = $voice['name'];
                echo_menu([
                    $type => $voice['lang'] . "-". $voice['phrase'],
                ]);
                say($voice['raw'], $type);
                sleep(0.5);
            }
            $cmd = '';
            break;
        case 'trend':
            include('speech_trend.php.inc');
            $cmd = '';
            break;
        case 't':
        case 'ltl':
        case 'ftl':
            include('speech_mastodon.php.inc');
            break;

        case 'q':
            echo_eol('Quit');
            $is_running = false;
            break;

        case 'h':
            echo_eol('Help');
            $cmd = '';
            break;

        case 's':
            say_hello();
            sleep(1); // sleep
            $cmd = '';
            break;

        default:
    }
}


echo_debug($config_default);
say('終了しました。');
