<?php
    require("config.inc");
    $app = new Ant\Application();
    $app->display();

    class IndexController extends Ant\AppController {
        public function indexAction() {
            $this->redirect('index');
        }
    }

    $index = new IndexController();
    $index->indexAction();