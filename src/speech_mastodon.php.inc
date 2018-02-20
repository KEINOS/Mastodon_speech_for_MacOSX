<?php

if ($has_been_tooted) {
    echo_eol('Fetching new toots...');
    echo_eol();
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
    $id_voice  = $toot['user_name'];
    $content   = trim($toot['content']);
    $account   = $toot['raw']['account']['acct'];
    $account   = colour_text("@${account} (${toot_id})", GRAYOUT);

    //表示
    echo_menu([$user_name => $content]);
    echo_indent($account, DO_NOT_RETURN, MARGIN_LEFT_MAX - mb_strlen(MARKER_TRIM) + 1);

    //読み上げ
    $type_voice = ('ja' == $toot_lang) ? 'Otoya' : 'Daniel';
    say_name($user_name,'さんの,トゥート','Kyoko');
    say($toot['say'], $type_voice, DO_NOT_ECHO, $id_voice);
 
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

