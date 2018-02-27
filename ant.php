<?php
namespace Ant {
    class Config {
        public static function initConfig($configs) {}
        public static function getConfig($name) {}
        public static function setConfig($name) {}
        public static function initDb($configs) {}
        public static function setDb($name, $value) {}
        public static function getDb($name) {}

        private $_configs;
        private $_dbs;
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
        public function render($file_name, $path = 'path_apps', $error_page = '') {}
        public function closeRender() {}
        public function message($message, $tag) {}
        public function query($name) {}
        public function form($name) {}
        public function isGet() {}
        public function isPost() {}
        public function module() {}
        public function controller() {}
        public function action() {}
        public function setResponeHeader() {}
        public function redirect($url, $full) {}
        public function assign($key, $value) {}

        protected $_view_obj;
        protected $_app_obj;
    }

    class AppController extends Controller {
        public function __construct($application, $view) {}
    }

    abstract class AService {
        public static function db() {}
        public function setDB($db) {}

        protected static $_db;
    }

    class Core {
        public function import($fileName, $path) {}
    }
}

namespace {
    class AntView {
        public function render($path) {}
        public function viewVars($key) {}
        public function inflateFile($file_name, $module_path) {}
        public function showMessage() {}
    }

    class AntDatabase {
        public function __construct($host, $user, $password, $dbname, $charset) {}
        public function query($sql) {}
        public function insert() {}
        public function update() {}
        public function getRow($type = MYSQLI_ASSOC) {}
        public function getRows($type = MYSQLI_ASSOC) {}
        public function getField($type = MYSQLI_ASSOC) {}
        public function getLastId() {}
        public function begin_transaction() {}
        public function rollback_transaction() {}
        public function commit_transaction() {}
    }
}