<html>
<head>
<style>
body{
background: lightgray url("Smart.png") no-repeat center top;
}
</style>
</head>
</html>
<?php
        $page = $_SERVER['PHP_SELF'];
        $sec = "10";
        header("Refresh: $sec; url=$page");

        $host = 'localhost';
        $dbname = '**********';
        $username = '*********';
        $password = '**********';
        $con = mysqli_connect($host, $username, $password, $dbname);

        if (mysqli_connect_errno())
        {
                echo "Failed to connect to MySQL: " . mysqli_connect_error();
        }

        echo '<a href="/">Index</a><br>';
        echo '<a href="control.php">Control board</a><br>';
        echo "<br>";

        $fan = mysqli_query($con,"call KumulFan();");
        echo "<table border='2'>
        <tr>
        <th>Fan ontime</th>
        </tr>";
        while($row = mysqli_fetch_array($fan))
        {
                echo "<tr>";
                echo "<td>" . $row['cumul'] . "</td>";
                echo "</tr>";
        }
        echo "</table>";

        echo "<br>";
	$con = mysqli_connect($host, $username, $password, $dbname);
        $result = mysqli_query($con,"call KumulLight();");
        echo "<table border='2'>
        <tr>
        <th>Lights ontime</th>
        </tr>";
        while($row = mysqli_fetch_array($result))
        {
                echo "<tr>";
                echo "<td>" . $row['cumul'] . "</td>";
                echo "</tr>";
        }
        echo "</table>";


myscli_close($con);
