<?php
$database = new AntDatabase('127.0.0.1:3306', 'root', 'root', 'test');
$database->insert('admin',
    array('name' => 'lucasho',
        'nick_name' => 'lucas',
        'password' => '32134',
        'status' => 1,
        'last_ip' => 127001,
        'created' => '2017-11-11 13:11:11')
);