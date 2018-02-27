--TEST--
Check for ant presence
--SKIPIF--
<?php if (!extension_loaded("ant")) print "skip"; ?>
--FILE--
<?php
$database = new AntDatabase('127.0.0.1:3306', 'root', 'root', 'test');
$ret = $database->query('select * from admin')->getRows();
var_dump($ret);
$success = $database->insert('admin',
    array('name' => 'lucasho',
        'nick_name' => 'lucas',
        'password' => '32134',
        'status' => 1,
        'last_ip' => 127001,
        'created' => '2017-11-11 13:11:11'));
echo 'insert:',$success,"\n";

$id = $success->getLastId();
echo 'last id:',$id,"\n";
?>
--EXPECT--
ant extension is available
