<html>
<head>
<style>
body{
background: lightgray url("Smart.png") no-repeat right;
}
</style>
    <form method="post">
    <p>
	<a href="/">Index</a><br>
	<a href="OnTime.php">Statistics</a><br>
	<br>

        <button name="abutton">Fan on</button>
	<button name="bbutton">Fan off</button> 
	<br>
	<br>
        <button name="cbutton">Lights on</button>
        <button name="dbutton">Lights off</button>
	<br>
	<br>
        <button name="ebutton">Alarm on</button>
        <button name="fbutton">Alarm off</button>
        <br>
        <br>
        <button name="gbutton">LDR on</button>
        <button name="hbutton">LDR off</button>
    </p>
    </form>

<?php
	$page = $_SERVER['PHP_SELF'];
        $sec = "5";
        header("Refresh: $sec; url=$page");

        $host = 'localhost';
        $dbname = 'Smarthome';
        $username = 'Pi';
        $password = 'Ytrewq123';
        $con = mysqli_connect($host, $username, $password, $dbname);

        if (mysqli_connect_errno())
        {
                echo "Failed to connect to MySQL: " . mysqli_connect_error();
        }
        $status = mysqli_query($con,"select * from Fan order by DateTime desc limit 1;");
        echo "<table border='1'>
        <tr>
        <th>Fan</th>
        <th>DateTime</th>
        </tr>";
        while($row = mysqli_fetch_array($status))
        {
                echo "<tr>";
                echo "<td>" . $row['OnOff'] . "</td>";
                echo "<td>" . $row['DateTime'] . "</td>";
                echo "</tr>";

        }
	$con = mysqli_connect($host, $username, $password, $dbname);
        $status = mysqli_query($con,"select * from Lights order by DateTime desc limit 1;");
        echo "<table border='1'>
        <tr>
        <th>Lights</th>
        <th>DateTime</th>
        </tr>";
        while($row = mysqli_fetch_array($status))
        {
                echo "<tr>";
                echo "<td>" . $row['OnOff'] . "</td>";
                echo "<td>" . $row['DateTime'] . "</td>";
                echo "</tr>";
        }
        echo "<br>";

	$con = mysqli_connect($host, $username, $password, $dbname);
        $status = mysqli_query($con,"select * from Alarm order by idAlarm desc limit 1;");
        echo "<table border='1'>
        <tr>
        <th>Alarm</th>
        <th>Tripped</th>
        </tr>";
        while($row = mysqli_fetch_array($status))
        {
                echo "<tr>";
                echo "<td>" . $row['idAlarm'] . "</td>";
                echo "<td>" . $row['Tripped'] . "</td>";
                echo "</tr>";
        }
	echo "<br>";
	$con = mysqli_connect($host, $username, $password, $dbname);
        $status = mysqli_query($con,"select * from Temp order by idTemp desc limit 1;");
        echo "<table border='1'>
        <tr>
        <th>Temperature C</th>
        <th>Updated</th>
        </tr>";
        while($row = mysqli_fetch_array($status))
        {
                echo "<tr>";
                echo "<td>" . $row['Cels'] . "</td>";
                echo "<td>" . $row['DateTime'] . "</td>";
                echo "</tr>";
        }
        echo "<br>";

mysqli_close($con);

        if (isset($_POST['abutton']))
	{
	shell_exec('mosquitto_pub -u raspi -P Ytrewq -t fan -m 1');
	}
        if (isset($_POST['bbutton']))
        {
	shell_exec('mosquitto_pub -u raspi -P Ytrewq -t fan -m 0');
        }
        if (isset($_POST['cbutton']))
        {
        shell_exec('mosquitto_pub -u raspi -P Ytrewq -t h1 -m 1');
        }
        if (isset($_POST['dbutton']))
        {
        shell_exec('mosquitto_pub -u raspi -P Ytrewq -t h1 -m 0');
        }
	if (isset($_POST['ebutton']))
        {
        shell_exec('mosquitto_pub -u raspi -P Ytrewq -t alarm -m 1');
        }
        if (isset($_POST['fbutton']))
        {
        shell_exec('mosquitto_pub -u raspi -P Ytrewq -t alarm -m 0');
        }
        if (isset($_POST['gbutton']))
        {
        shell_exec('mosquitto_pub -u raspi -P Ytrewq -t ldr -m 1');
        }
        if (isset($_POST['hbutton']))
        {
        shell_exec('mosquitto_pub -u raspi -P Ytrewq -t ldr -m 0');
        }

