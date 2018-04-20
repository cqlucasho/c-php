<?php
namespace Ant {
    class Config {
        public static function initConfig(array $configs) {}
        public static function getConfig($key) {}
        public static function initDb(array $configs) {}
        public static function getDb($key) {}
        /**
         * 'open': 是否开启权限认证
         * 'mode': 是否使用白名单还是黑名单, 0:白名单|1:黑名单.
         * 'current_user_id': 存放到$_SESSION['current_user']的数组用户id键值
         *      比如: $_SESSION['current_user'] = array('user_id' => 1, 'name' => 'hahh', ...), 那么current_user_id值就是user_id.
         * 'fail_redirect': '/user/login/index'或'/?module=user&controller=login&action=index'
         * 'ignore_list': array('user/login/index', 'user/login/register', ...)
         *
         * @param array $configs
         */
        public static function initAuth(array $configs) {}
        public static function getAuth($key) {}

        private $_configs;
        private $_dbs;
        private $_authes;
    }

    class Application {
        public function __construct() {}
        public function getAllSession() {}
        public function getSession($name) {}
        public function isGet() {}
        public function isPost() {}
        public function module() {}
        public function controller() {}
        public function action() {}
        public function getRoute() {}
        public function display() {}

        protected $_header;
        protected $_content;
        protected $_route_default_obj;
        protected $_route_pathinfo_obj;
        protected $_module;
        protected $_controller;
        protected $_action;
    }

    class Controller {
        public function getUserInfo() {}
        public function render($file_name = '', $path = 'path_apps', $error_page = '') {}
        public function closeRender() {}
        /**
         * define("ALERT_INFO",     "alert-info");
         * define("ALERT_SUCCESS",  "alert-success");
         * define("ALERT_WARNING",  "alert-warning");
         * define("ALERT_DANGER",   "alert-danger");
         *
         * @param string $message
         * @param string $tag
         */
        public function message($message, $tag = ALERT_SUCCESS) {}
        public function smessage($message, $tag = ALERT_SUCCESS) {}
        public function query($name) {}
        public function form($name) {}
        public function isGet() {}
        public function isPost() {}
        public function module() {}
        public function controller() {}
        public function action() {}
        public function setResponeHeader() {}
        public function redirect($url, array $params = array()) {}
        public function assign($key, $value) {}
        /**
         * limit开始数根据$_GET['page']判断, 未找到从0开始, 找到则从$_GET['page']数开始.
         *
         * @param int $number 查询记录数
         */
        public function paginate($number) {}

        protected $_view_obj;
        protected $_app_obj;
    }

    class AppController extends Controller {
        public function __construct($application, $view) {}

        /**
         * 注册权限
         * @example
         *  $priviliges = array(
         *      'user/login/index',
         *      'user/login/register',
         *      'user/login/password'
         *      ...
         *  );
         *
         * @param int $userId
         * @param array $priviliges
         */
        public function registerPriviliges($userId, array $priviliges) {}
    }

    abstract class AService {
        public static function db() {}
        public function setDB($db) {}

        protected static $_db;
    }

    class Core {
        /**
         * 路径为空则表示默认使用配置: path_vendors路径
         *
         * @param string $fileName 文件名
         * @param string $path 路径
         */
        public static function import($fileName, $path = '') {}
    }
}

namespace {
    class AntView {
        public function render($path) {}
        public function viewVars($key) {}
        public function inflateFile($file_name, $module_path) {}
        public function showMessage() {}
        public function pages($showNumber, $currPage, $extParams, $class) {}
        public function url($module = '', $controller = '', $action = '', $params = array()) {}
        public function urlPathinfo($module = '', $controller = '', $action = '', $params = array()) {}
    }

    class AntDatabase {
        public function __construct($host, $user, $password, $dbname, $charset) {}
        public function query($sql) {}
        /**
         * 插入成功返回db对象, 否则返回false.
         *
         * @return object|false
         */
        public function insert() {}
        public function update() {}
        public function getRow($type = MYSQLI_ASSOC) {}
        public function getRows($type = MYSQLI_ASSOC) {}
        public function getRowsNum() {}
        public function getField($type = MYSQLI_ASSOC) {}
        public function getLastId() {}
        public function begin_transaction() {}
        public function rollback_transaction() {}
        public function commit_transaction() {}
    }
}