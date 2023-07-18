<?php

// Set the cookie

// Check if the cookie is set
echo "hello";
if (isset($_COOKIE["name"])) {
	// The cookie is set, so show the page with different content
	echo "<h1>Welcome back, " . $_COOKIE['name'] . "!</h1>\n\n";
} else {
	// The cookie is not set, so show the welcome page
	setcookie("name", "John", time() + 30); // 86400 = 1 day
	  echo "<h1>Welcome!</h1>\r\n\r\n";
	exit();
}
?>