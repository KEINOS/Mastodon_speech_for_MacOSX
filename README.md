# Qiitedon Speech

A Mastodon voice reader CLI app for macOS.

I named this app "Qiitedon Speech", since my main Mastodon instance is "[Qiitadon](https://qiitadon.com/)" and "[Qiite](https://en.wiktionary.org/wiki/%E8%81%9E%E3%81%8F#Japanese)" means "listen to" in Japanese.:stuck_out_tongue_winking_eye:  Though it must work in other instances.

It's a simple Phar (PHP archived) CLI application that uses Mastodn API and Mac's `say` command.



## How to build it your own

1. Clone this repo.
1. Change current directory to `build/`
    1. `$ cd /path/to/your/cloned/Mastodon_speech_for_MacOSX`
    1. `$ cd ./build`
1. Download and install "[Box](https://box-project.github.io/box2/)"(Phar archiver app) in `build` dir, or just run [`box_installer.php`](https://github.com/KEINOS/Mastodon_speech_for_MacOSX/blob/master/build/box_installer.php) to do it for you as below.
    1. First of all, be sure that you have set your `php.ini` settings to enable create Phar files as below.

       ```php.ini
        phar.readonly = 0
        phar.require_hash = 0
        ```
    1. `$ chmod 0755 ./box_installer.php`
    1. `$ php box_installer.php`
    1. `$ ./box.phar -v` (Current version is v2.7.5, 2018/02/03)
1. Run `$ ./box.phar build` to build/compile your Phar app.
1. Then '**qiitedon.phar**' must be built in the parent directory.
    1. `$ cd ../ && ls`
1. Run test `$ ./qiitedon.phar -t` and if you hear 'Hello mastodon' then it's done. See "How to use" section for detail.