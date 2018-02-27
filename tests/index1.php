<?php
require("config.inc");

try {
    $app = new Ant\Application();
    $app->display();
} catch (Exception $ex) {
    $ex->getMessage();
}