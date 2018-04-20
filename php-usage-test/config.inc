<?php
session_start();
date_default_timezone_set('Asia/Shanghai');
ini_set('display_errors', 'On');

# all basic configs
define('DI', DIRECTORY_SEPARATOR);
define('PATH_ROOT', str_replace('\\', '/', dirname(__FILE__).DI));
define('PATH_APPS', PATH_ROOT.'apps'.DI);
define('PATH_PUBLICS', '/publics/');
define('APP_PATH_IMAGE', PATH_PUBLICS.'images/');

// initialize config
Ant\Config::initConfig(array(
    'debug_mode'    => 1,
    'route_style'   => 'default',
    'path_root'     => PATH_ROOT,
    'path_apps'     => PATH_ROOT.'apps'.DI,
    'path_apis'     => PATH_ROOT.'apis'.DI,
    'path_publics'  => PATH_ROOT.'publics'.DI,
    'path_vendors'  => PATH_ROOT.'vendors'.DI,
    'path_logs'     => PATH_ROOT.'logs'.DI)
);

// initialize set of database
Ant\Config::initDb(array(
    'host'      => '127.0.0.1',
    'user'      => 'root',
    'password'  => 'root',
    'select_db' => 'qx_lotcode',
    'charset'   => 'utf8')
);


// initialize authentication configs
Ant\Config::initAuth(
    array(
        'open'              => 1,
        'mode'              => 0,
        'current_user_id'   => 'id',
        'fail_redirect'     => '/?module=user&controller=index&action=register',
        'ignore_list'       => array(
            'user/login/index', 'user/login/admin_index', 'user/index/register', 'user/login/logout', 'user/login/validate_code'
        )
    )
);

Ant\Core::import('proxy/proxy');
Ant\Core::import('function');
