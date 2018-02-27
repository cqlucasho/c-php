--TEST--
Check for ant presence
--SKIPIF--
<?php if (!extension_loaded("ant")) print "skip"; ?>
--FILE--
<?php
echo ant_camel("HelloWordTestAbc"),"\n";
//echo ant_pascal("hello_word", 0),"\n";
//echo ant_pascal("hello_word", 1),"\n";
echo ant_pascal("hello_word_index_abc", 0),"\n";
echo ant_pascal("hello_word_index_abd", 1),"\n";
?>
--EXPECT--
ant extension is available
