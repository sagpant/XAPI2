<?php
session_start();
require_once('config.php');
include_once('CheckSignin.php');
?><!DOCTYPE HTML>
<html>
<head>
<meta charset="UTF-8">
<title>主页</title>
</head>
<body>
当前用户：<?php echo $_SESSION['UserID']; ?> 当前IP: <?php echo $_SERVER['REMOTE_ADDR']; ?> <a href="Logout.php">退出</a>  <a href="aboutus.php"  target="_blank">关于我们</a>
<hr/>
<h1>用户信息</h1>
<a href="UserInfoView.php" target="_blank">查看个人信息</a>
<hr/>
<h1>产品信息</h1>
<a href="ProductInfoListView.php" target="_blank">查看产品列表</a>
<hr/>
<h1>授权信息</h1>
<a href="LicenseInfoListView.php" target="_blank">查看授权列表</a>
<?php if($_SESSION['Right'] >= 2) {?>
<hr/>
<h1>管理</h1>
<a href="LicenseInfoListView2.php" target="_blank">审核用户</a>
<hr/>
<?php }?>
<?php if($_SESSION['Right'] == 99) {?>
<hr/>
<h1>管理</h1>
<a href="LicenseInfoListView99.php" target="_blank">审核用户</a>
<a href="User2ListView.php" target="_blank">查看审核人员列表</a>
<a href="Signup.php" target="_blank">注册用户</a>
<a href="SwitchUserID.php" target="_blank">切换用户ID</a>
<hr/>
<?php }?>
</body>
</html>