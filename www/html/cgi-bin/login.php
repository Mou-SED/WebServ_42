<?php
session_start();

if (!isset($_SESSION["visited"])) {
	$_SESSION["visited"] = "true";
	echo "Welcome for first time!";
}
else {
?>

<h1>Welcome back!</h1>
<br>
<a href="home.php">reset</a>

<?php
}