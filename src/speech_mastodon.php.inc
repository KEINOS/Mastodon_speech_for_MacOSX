<?php
$width_max       = get_width_screen() -1;
$blank_line      = str_repeat(' ', $width_max);
$has_been_tooted = false;
$pause           = (integer) $time_interval / 2;

echo("\r$blank_line\r"); echo("Fetching new toots...");
$toots = array_reverse(get_current_toot($input));

foreach ($toots as $toot) {
    $toot_id   = $toot['toot_id'];
    $toot_lang = $toot['language'];

    if (array_key_exists($toot_id, $list_id_tooted)) {
        sleep($pause);
        echo("\r$blank_line\r"); echo 'No new toot found... ';

        continue;
    }

    echo("\r$blank_line\r");

    /* common info of toot user */
    $user_name = $toot['display_name'] ?: $toot['user_name'];
    $user_name = strip_emoji($user_name);
    $id_voice  = $toot['user_name'];
    $account   = $toot['raw']['account']['acct'];
    $account   = colour_text("@${account} (${toot_id}) $toot_lang", GRAYOUT);

    /* echo */
    $toot_echo = $toot['content'];
    $toot_echo = mb_convert_kana($toot_echo, 'asKH', 'utf-8');
    $toot_echo = url_breakline($toot_echo);
    $toot_echo = html_entity_decode($toot_echo);
    $toot_echo = trim($toot_echo);
    
    /* say  */
    $toot_say   = $toot['say'];
    $toot_say   = strip_url($toot_say, ANNOUNCE_DELETION);
    $type_voice = ('ja' == $toot_lang) ? 'Otoya' : 'Ava';

    //表示
    echo_menu([
        $user_name => $toot_echo,
    ]);
    echo_indent(
        $account,
        DO_NOT_RETURN,
        MARGIN_LEFT_MAX - mb_strlen(MARKER_TRIM) + 1
    );        

    //読み上げ
    say_name($user_name, 'さんの,トゥート', 'Kyoko');

    //get_pitch_id($id_char)=$id_voice    
    say([
        'phrase'     => $toot_say,
        'type_voice' => $type_voice,
        'echo_too'   => DO_NOT_ECHO,
        'pitch'      => get_pitch_id($id_voice),
    ]);

    $list_id_tooted[$toot_id] = time();
    $has_been_tooted = true;
}

$time_lastupdate = time();
if ($has_been_tooted) {
    echo("\r$blank_line\r");
    echo 'Waiting for updates... ';
}

sleep($time_interval-$pause);
// Delete toots older than 2hour.
$list_id_tooted = delete_list_oldtoot($list_id_tooted, 2); //2時間より
