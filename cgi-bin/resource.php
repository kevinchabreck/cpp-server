<?php
	echo "content:\n";
	// reverses all of its inputs
	for ($x = 1; $x < $argc; $x++) {
		echo strrev($argv[$x]." ");
	}
	echo "\n";
?>