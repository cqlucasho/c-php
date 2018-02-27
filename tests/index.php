<?php
require("config.inc");

try {
    $_SESSION['current_user'] = array('user' => 'lucas', 'flag' => 1);
    $_SESSION['user'] = 'test';
    $_SESSION['user1'] = 1;
    $_SESSION['user2'] = false;
    $_SESSION['user3'] = array('number' => 'sss');
    $app = new Ant\Application();
    print_r($app->getRoute());
    echo "<br/>";
    print_r($app->getAllSession());
    echo "<br/>";
    var_dump($app->getSession('user'));
    echo "<br/>";
    var_dump($app->getSession('user1'));
    echo "<br/>";
    var_dump($app->getSession('user2'));
    echo "<br/>";
    var_dump($app->getSession('user3'));
    //echo "<br/>";
    //var_dump($app->isGet());
    //echo "<br/>query: ";
    //var_dump($app->query('module'));
    //echo "<br/>module: ";
    //var_dump($app->module());
    //echo "<br/>display: ";
    //echo $app->display();
} catch (Exception $ex) {
    $ex->getMessage();
}