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

/* ------------------------------------------------------------------ */
if (ob_get_contents() || ob_get_length()) {
    ob_end_clean();
}

echo_eol('Running app ...');
echo_eol('Press \'control+c(^C)\' to cancel/quit this app.');
echo_hr();

//echo_info_env();
$is_running      = true;
$list_voices     = get_type_voice();
$time_interval   = 15;
$time_lastupdate = 0; //起動時にすぐに読み上げるため time() させない
$data_old        = array();
$fg_color        = FONT_WHITE;
$bg_color        = BKGD_BLACK;

while ($is_running or ($cmd = fgets(STDIN))) {
    $input           = '';
    $time_current    = time();
    $time_nextupdate = $time_lastupdate + $time_interval;
    $time_left       = $time_nextupdate - $time_current;

    if ($time_nextupdate < $time_current) {
        $time_lastupdate = time();
        $input = 'T';
    } else {
        sleep($time_interval);
    }

    if (! empty($cmd)) {
        $input = get_command_input($cmd);
    }

    switch ($input) {
        case 'V':
            foreach ($list_voices as $voice) {
                $type = $voice['name'];
                echo $type . " : " . $voice['phrase'] . PHP_EOL;
                say($voice['phrase'], $type);
                sleep(0.5);
            }
            break;

        case 'T':
            $data_new  = array_reverse(get_current_toot());
            $data_diff = array_diff_key($data_new, $data_old);

            if (empty($data_diff)) {
                break;
            }

            foreach ($data_diff as $toot) {
                $user_name = $toot['display_name'] ?: $toot['user_name'];

                $fg_color = (FONT_WHITE == $fg_color) ? FONT_BLACK : FONT_WHITE;
                $bg_color = (BKGD_BLACK == $bg_color) ? BKGD_WHITE : BKGD_BLACK;
                echo_colored($user_name, $toot['content'], $fg_color, $bg_color);
                say($user_name ."さんの,トゥート", 'Kyoko');
                say($toot['say'], 'Otoya');
            }

            //$data_old = $data_new;
            $data_old = $data_diff;
            $time_lastupdate = time();
            break;

        case 'Q':
            echo_eol('Quit');
            $is_running = false;
            break;

        case 'H':
            echo_eol('Help');
            break;

        case 'S':
            say_hello();
            sleep(1); // sleep
            break;

        default:
    }
}


echo_debug($config_default);
say('終了しました。');
