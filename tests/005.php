<?php
$database = new AntDatabase('127.0.0.1:3306', 'root', 'root', 'qx_lotcode');
$ret = $database->query('select count(*) as num from lot_company_approval where _co_name like "test%"')->getField('num');
var_dump($ret);
?>
