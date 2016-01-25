<?php
$user = 'root';
$pass = '123456';
$host = 'localhost';
$db_name = 'test';

$dsn = "mysql://$user:$pass@$host/$db_name";

require_once('MDB2.php');

$mdb2 =& MDB2::factory($dsn, $options);
if (PEAR::isError($mdb2)) {
	die("{'Error':'".$mdb2->getMessage()."'}");
}
$mdb2->setFetchMode(MDB2_FETCHMODE_ASSOC);
$mdb2->exec("SET NAMES 'utf8'");

?>