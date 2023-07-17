<?php

// Set the cookie

// Check if the cookie is set
if (isset($_COOKIE["name"])) {
	// The cookie is set, so show the page with different content
	echo "<h1>Welcome back, " . $_COOKIE['name'] . "!</h1>";
} else {
	// The cookie is not set, so show the welcome page
	setcookie("name", "John Doe", time() + (86400 * 30), "/"); // 86400 = 1 day
	  echo "<h1>Welcome!</h1>";
}

?>