<?php
require_once('constants.php.inc');
require_once('config.default.php.inc');
require_once('accent_dictionary.php.inc');
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

echo_eol('Running app ...');
echo_hr();
echo_eol('Press \'control+c(^C)\' to cancel/quit this app.');
echo_eol('INPUT MENU:');
echo_menu([
    'ltl'   => 'Qiitadon のローカルタイムラインの読み上げを開始します。Qiitadon のローカルタイムラインの読み上げを開始します。',
    'ftl'   => 'Qiitadon の連合タイムラインの読み上げを開始します。Qiitadon の連合タイムラインの読み上げを開始します。',
    'trend' => 'Qiita のトレンドを読み上げます。Qiita のトレンドを読み上げます。Qiita のトレンドを読み上げます。Qiita のトレンドを読み上げます。',
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

        case 't':
        case 'ltl':
        case 'ftl':
            if ($has_been_tooted) {
                echo_eol('Fetching new toots...');
                echo_eol('');
            }

            $has_been_tooted = false;
            $toots = array_reverse(get_current_toot($input));

            foreach ($toots as $toot) {
                $toot_id   = $toot['toot_id'];
                $toot_lang = $toot['raw']['language'];
                if (array_key_exists($toot_id, $list_id_tooted)) {
                    continue;
                }
                $user_name = $toot['display_name'] ?: $toot['user_name'];
                $user_name = strip_emoji($user_name);
                $content   = trim($toot['content']);
                $account   = $toot['raw']['account']['acct'];
                $account   = colour_text("@${account} (${toot_id})", GRAYOUT);

                //表示
                echo_menu([$user_name => $content]);
                echo_indent($account, false, MARGIN_LEFT_MAX - mb_strlen(MARKER_TRIM) + 1);
                //読み上げ who
                say_name($user_name,'さんの,トゥート','Kyoko');

                //読み上げ what
                $who = ('ja' == $toot_lang) ? 'Otoya' : 'Daniel';
                say($toot['say'], $who, false, $user_name);
             
                $list_id_tooted[$toot_id] = time();
                $has_been_tooted = true;
            }

            $time_lastupdate = time();
            if ($has_been_tooted) {
                //echo_eol('Done. Waiting to update...');
                echo 'Waiting for updates... ';
            }
            sleep($time_interval);
            // Delete toots older than 2hour.
            $list_id_tooted = delete_list_oldtoot($list_id_tooted, 2); //2時間より
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
