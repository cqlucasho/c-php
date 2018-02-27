--TEST--
Check for ant presence
--SKIPIF--
<?php if (!extension_loaded("ant")) print "skip"; ?>
--FILE--
<?php
define('DI', DIRECTORY_SEPARATOR);
define('PATH_ROOT', str_replace('\\', '/', dirname(__FILE__).DI));

Ant\Config::initConfig(array(
    'debug_mode'    => 1,
    'route_style'   => 'default',
    'path_root'     => PATH_ROOT,
    'path_apps'     => PATH_ROOT.'apps'.DI,
    'path_modules'  => PATH_ROOT.'apps'.DI.'modules'.DI,
    'path_apis'     => PATH_ROOT.'apis'.DI,
    'path_publics'  => PATH_ROOT.'publics'.DI,
    'path_vendors'  => PATH_ROOT.'vendors'.DI,
    'path_logs'     => PATH_ROOT.'logs'.DI)
);

Ant\Config::initDb(array(
    'host'      => '121.41.6.126',
    'user'      => 'qixin',
    'password'  => 'IFUV1KqLYT^Rh$20',
    'select_db' => 'qx_lotcode_1_5_dev',
    'charset'   => 'utf-8')
);

Ant\Config::initAuth(
    array(
        'open'              => 1,
        'mode'              => 0,
        'current_user_id'   => 'id',
        'fail_redirect'     => '/?module=user&controller=login&action=index',
        'ignore_list'       => array('user/login/index')
    )
);

echo Ant\Config::getConfig("path_root"),"\n";
echo Ant\Config::getAuth("open");
?>
--EXPECT--
ant extension is available
