<?php
if(file_exists('parsedown.php')){
    include_once('parsedown.php');
}else{
    echo_eol('parsedown.php がありません。');
    die();
}

class SayExtension extends Parsedown
{
    protected function blockFencedCodeComplete($Block)
    {
        $Block = Parsedown::blockFencedCodeComplete($Block);
        $text  = $Block['element']['text']['text'];
        $Block['element']['text']['text'] = "[hoge]${text}[/hoge]";
        return $Block;
    }

}

class EchoExtension extends Parsedown
{
    protected function blockHeader($Line)
    {
        $Block = Parsedown::blockHeader($Line);
        return $Block;
    }
}