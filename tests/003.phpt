--TEST--
redirect
--SKIPIF--
<?php if (!extension_loaded("ant")) print "skip"; ?>
--FILE--
<?php
    require("config.inc");
    $app = new AntController();
    $app->display();

    class IndexController extends Ant\AppController {
        public function indexAction() {
            $this->redirect('index');
        }
    }

    $index = new IndexController();
    $index->indexAction();
?>
--EXPECT--
ant extension is available
